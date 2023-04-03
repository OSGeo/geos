/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2022 ISciences LLC
 * Copyright (C) 2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/profiler.h>
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <geos/util/IllegalArgumentException.h>
#include <geos/util.h>

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iterator>
#include <sstream>

namespace geos {
namespace geom { // geos::geom

#if PROFILE
static Profiler* profiler = Profiler::instance();
#endif

// If GEOS_COORDSEQ_PADZ is defined:
// - XY sequences will be stored as XYZ
// - XYM sequences will be stored as XYZM
// This prevents incorrect results when an XYZ Coordinate is read from
// a sequence storing XY or XYM. When GEOS is changed to check
// coordinate types throughout the libary, this can be undefined to
// store coordinates efficiently.
#define GEOS_COORDSEQ_PADZ

CoordinateSequence::CoordinateSequence() :
    CoordinateSequence(0, 0) {}

CoordinateSequence::CoordinateSequence(std::size_t sz, bool hasz, bool hasm, bool init) :
#ifdef GEOS_COORDSEQ_PADZ
    m_vect(sz * (3u + hasm)),
    m_stride(static_cast<std::uint8_t>(3u + hasm)),
#else
    m_vect(sz * (2u + hasm + hasz)),
    m_stride(static_cast<std::uint8_t>(2u + hasm + hasz)),
#endif
    m_hasdim(true),
    m_hasz(hasz),
    m_hasm(hasm)
{
    if (init) {
        initialize();
    }
}

CoordinateSequence::CoordinateSequence(double* buf, std::size_t sz, bool hasz, bool hasm) :
    m_vect(sz * (2u + hasm + hasz), buf),
    m_stride(static_cast<std::uint8_t>(2 + hasm + hasz)),
    m_hasdim(true),
    m_hasz(hasz),
    m_hasm(hasm)
{
    #ifdef GEOS_COORDSEQ_PADZ
    // Copy the external buffer into an internally-owned buffer
    // with z-padding.
    auto typ = getCoordinateType();
    if (typ == CoordinateType::XY) {
        CoordinateSequence dst(sz, false, false);
        for (std::size_t i =0; i < size(); i++) {
            dst.setAt(getAt<CoordinateXY>(i), i);
        }
        *this = std::move(dst);
    } else if (typ == CoordinateType::XYM) {
        CoordinateSequence dst(sz, false, true);
        for (std::size_t i =0; i < size(); i++) {
            dst.setAt(getAt<CoordinateXYM>(i), i);
        }
        *this = std::move(dst);
    }
    #endif
}

CoordinateSequence::CoordinateSequence(std::size_t sz, std::size_t dim) :
    m_vect(sz * std::max(static_cast<std::uint8_t>(dim), static_cast<std::uint8_t>(3))),
    m_stride(std::max(static_cast<std::uint8_t>(dim), static_cast<std::uint8_t>(3))),
    m_hasdim(dim > 0),
    m_hasz(dim >= 3),
    m_hasm(dim == 4)
{
    if (dim == 1 || dim > 4) {
        throw util::IllegalArgumentException("Declared dimension must be 2, 3, or 4");
    }
    initialize();
}

CoordinateSequence::CoordinateSequence(const std::initializer_list<Coordinate>& list) :
    m_stride(3),
    m_hasdim(false),
    m_hasz(false),
    m_hasm(false)
{
    reserve(list.size());
    add(list.begin(), list.end());
}

CoordinateSequence::CoordinateSequence(const std::initializer_list<CoordinateXY>& list) :
#ifdef GEOS_COORDSEQ_PADZ
    m_stride(3),
#else
    m_stride(2),
#endif
    m_hasdim(true),
    m_hasz(false),
    m_hasm(false)
{
    reserve(list.size());
    add(list.begin(), list.end());
}

CoordinateSequence::CoordinateSequence(const std::initializer_list<CoordinateXYM>& list) :
#ifdef GEOS_COORDSEQ_PADZ
    m_stride(4),
#else
    m_stride(3),
#endif
    m_hasdim(true),
    m_hasz(false),
    m_hasm(true)
{
    reserve(list.size());
    add(list.begin(), list.end());
}

CoordinateSequence::CoordinateSequence(const std::initializer_list<CoordinateXYZM>& list) :
    m_stride(4),
    m_hasdim(true),
    m_hasz(true),
    m_hasm(true)
{
    reserve(list.size());
    add(list.begin(), list.end());
}

template<typename T>
void fillVector(util::Vector<double> & v)
{
    const T c;
    T* from = reinterpret_cast<T*>(v.data());
    T* to = reinterpret_cast<T*>(v.data() + v.size());
    std::fill(from, to, c);
}

void
CoordinateSequence::initialize()
{
    switch(getCoordinateType()) {
        case CoordinateType::XYZ: fillVector<Coordinate>(m_vect); break;
        case CoordinateType::XYZM: fillVector<CoordinateXYZM>(m_vect); break;
        case CoordinateType::XY: fillVector<CoordinateXY>(m_vect); break;
        case CoordinateType::XYM: fillVector<CoordinateXYM>(m_vect); break;
    }
}

void
CoordinateSequence::add(const CoordinateSequence& cs, std::size_t from, std::size_t to)
{
    if (cs.stride() == stride() && cs.hasM() == cs.hasM()) {
        m_vect.insert(m_vect.end(),
                      std::next(cs.m_vect.begin(), static_cast<std::ptrdiff_t>(from * stride())),
                      std::next(cs.m_vect.begin(), static_cast<std::ptrdiff_t>((to + 1u)*stride())));
    } else {
        std::size_t pos = size();
        make_space(pos, to - from + 1);

        switch(cs.getCoordinateType()) {
            case CoordinateType::XY:   cs.forEach<CoordinateXY>(from, to, [this, &pos](const CoordinateXY& c) { setAt(c, pos++); }); break;
            case CoordinateType::XYZ:  cs.forEach<Coordinate>(from, to, [this, &pos](const Coordinate& c) { setAt(c, pos++); }); break;
            case CoordinateType::XYZM: cs.forEach<CoordinateXYZM>(from, to, [this, &pos](const CoordinateXYZM& c) { setAt(c, pos++); }); break;
            case CoordinateType::XYM:  cs.forEach<CoordinateXYM>(from, to, [this, &pos](const CoordinateXYM& c) { setAt(c, pos++); }); break;
        }
    }
}

void
CoordinateSequence::add(const CoordinateSequence& cs)
{
    add(cs, 0, cs.size() - 1);
}

void
CoordinateSequence::add(const CoordinateSequence& cs, std::size_t from, std::size_t to, bool allowRepeated)
{
    if (allowRepeated) {
        add(cs, from, to);
        return;
    }

    std::size_t first = from;

    // Check for case where first point(s) of `cs` duplicate last points of `this`
    if (!isEmpty()) {
        while(first <= to && cs.getAt<CoordinateXY>(first).equals2D(back<CoordinateXY>())) {
            first++;
        }
    }

    std::size_t last = first + 1;
    const CoordinateXY* last_unique = &cs.front<CoordinateXY>();
    while(last <= to) {
        const CoordinateXY* curr = &cs.getAt<CoordinateXY>(last);
        if (curr->equals2D(*last_unique)) {
            // End of block
            add(cs, first, last - 1);
            do {
                last++;
            } while (last <= to && cs.getAt<CoordinateXY>(last).equals2D(*last_unique));

            if (last != (to + 1) ) {
                first = last;
                last_unique = &cs.getAt<CoordinateXY>(first);
            }
            last++;
        } else {
            last_unique = curr;
            last++;
        }
    }

    if (last == (to + 1)) {
        add(cs, first, to);
    }
}

void
CoordinateSequence::add(const CoordinateSequence& cs, bool allowRepeated) {
    if (!cs.isEmpty()) {
        add(cs, 0, cs.size() - 1, allowRepeated);
    }
}

void
CoordinateSequence::add(const CoordinateSequence& cl, bool allowRepeated, bool forwardDirection)
{
    if (forwardDirection) {
        add(cl, allowRepeated);
    } else {
        CoordinateSequence rev(cl);
        rev.reverse();
        add(rev, allowRepeated);
        return;
    }

}

/*public*/

std::unique_ptr<CoordinateSequence>
CoordinateSequence::clone() const
{
    return detail::make_unique<CoordinateSequence>(*this);
}

void
CoordinateSequence::closeRing(bool allowRepeated)
{
    if(!isEmpty() && (allowRepeated || front<CoordinateXY>() != back<CoordinateXY>())) {
        m_vect.insert(m_vect.end(),
                      m_vect.begin(),
                      std::next(m_vect.begin(), stride()));
    }
}

std::size_t
CoordinateSequence::getDimension() const
{
    if (m_hasdim) {
        return static_cast<std::size_t>(2 + hasM() + hasZ());
    }

    if (m_vect.empty()) {
        return 3;
    }

    assert(stride() >= 3);
    m_hasdim = true;
    if (!std::isnan(getAt<Coordinate>(0).z)) {
        m_hasz = true;
    }

    return getDimension();
}


double
CoordinateSequence::getOrdinate(std::size_t index, std::size_t ordinateIndex) const
{
    switch(ordinateIndex) {
        case CoordinateSequence::X:
            return getAt<CoordinateXY>(index).x;
        case CoordinateSequence::Y:
            return getAt<CoordinateXY>(index).y;
        case CoordinateSequence::Z:
            return hasZ() ? getAt<Coordinate>(index).z : DoubleNotANumber;
        case CoordinateSequence::M:
            return hasM() ? (hasZ() ? getAt<CoordinateXYZM>(index).m : getAt<CoordinateXYM>(index).m) : DoubleNotANumber;
        default:
            return DoubleNotANumber;
    }
}

bool
CoordinateSequence::hasRepeatedPoints() const
{
    // Iterate over the array of doubles and check x/y values directly.
    // This is about 30% faster than retrieving/comparing CoordinateXY&.
    for (std::size_t i = stride(); i < m_vect.size(); i += stride()) {
        if (m_vect[i - stride()] == m_vect[i] && m_vect[i + 1 - stride()] == m_vect[i+1]) {
            return true;
        }
    }
    return false;
}

bool
CoordinateSequence::hasRepeatedOrInvalidPoints() const
{
    // Check first points
    if (! (std::isfinite(m_vect[0]) && std::isfinite(m_vect[1]) )) {
        return true;
    }
    // Iterate over the array of doubles and check x/y values directly.
    // This is about 30% faster than retrieving/comparing CoordinateXY&.
    for (std::size_t i = stride(); i < m_vect.size(); i += stride()) {
        if (! (std::isfinite(m_vect[i]) && std::isfinite(m_vect[i+1]) )) {
            return true;
        }
        if (m_vect[i - stride()] == m_vect[i] && m_vect[i + 1 - stride()] == m_vect[i+1]) {
            return true;
        }
    }
    return false;
}


/*
 * Returns either the given coordinate array if its length is greater than the
 * given amount, or an empty coordinate array.
 */
CoordinateSequence*
CoordinateSequence::atLeastNCoordinatesOrNothing(std::size_t n,
        CoordinateSequence* c)
{
    if(c->getSize() >= n) {
        return c;
    }
    else {
        // FIXME: return NULL rather then empty coordinate array
        return new CoordinateSequence(0, c->getDimension());
    }
}

const CoordinateXY*
CoordinateSequence::minCoordinate() const
{
    const CoordinateXY* minCoord = nullptr;
    const std::size_t p_size = getSize();
    for(std::size_t i = 0; i < p_size; i++) {
        if(minCoord == nullptr || minCoord->compareTo(getAt<CoordinateXY>(i)) > 0) {
            minCoord = &getAt<CoordinateXY>(i);
        }
    }
    return minCoord;
}

size_t
CoordinateSequence::indexOf(const CoordinateXY* coordinate,
                            const CoordinateSequence* cl)
{
    std::size_t p_size = cl->size();
    for(std::size_t i = 0; i < p_size; ++i) {
        if((*coordinate) == cl->getAt<CoordinateXY>(i)) {
            return i;
        }
    }
    return std::numeric_limits<std::size_t>::max();
}

void
CoordinateSequence::scroll(CoordinateSequence* cl,
                           const CoordinateXY* firstCoordinate)
{
    std::size_t ind = indexOf(firstCoordinate, cl);
    if(ind == 0 || ind == std::numeric_limits<std::size_t>::max()) {
        return;    // not found or already first
    }

    std::rotate(cl->m_vect.begin(),
        std::next(cl->m_vect.begin(), static_cast<std::ptrdiff_t>(ind * cl->stride())),
        cl->m_vect.end());
}

int
CoordinateSequence::increasingDirection(const CoordinateSequence& pts)
{
    std::size_t ptsize = pts.size();
    for(std::size_t i = 0, n = ptsize / 2; i < n; ++i) {
        std::size_t j = ptsize - 1 - i;
        // skip equal points on both ends
        int comp = pts[i].compareTo(pts[j]);
        if(comp != 0) {
            return comp;
        }
    }
    // array must be a palindrome - defined to be in positive direction
    return 1;
}

/* public */
bool
CoordinateSequence::isRing() const
{
    if (size() < 4)
        return false;

    if (front<CoordinateXY>() != back<CoordinateXY>())
        return false;

    return true;
}

void
CoordinateSequence::reverse()
{
    auto mid = m_vect.size() / 2;
    auto last = m_vect.size() - stride();
    for (std::size_t i = 0; i < mid; i += stride()) {
        switch(stride()) {
            case 4: std::swap(m_vect[i + 3], m_vect[last - i + 3]); // fall-through
            case 3: std::swap(m_vect[i + 2], m_vect[last - i + 2]); // fall-through
            case 2: std::swap(m_vect[i + 1], m_vect[last - i + 1]);
                    std::swap(m_vect[i],     m_vect[last - i]);

        }
    }
}

void
CoordinateSequence::sort()
{
    switch(getCoordinateType()) {
        case CoordinateType::XY:   std::sort(items<CoordinateXY>().begin(), items<CoordinateXY>().end()); return;
        case CoordinateType::XYZ:  std::sort(items<Coordinate>().begin(), items<Coordinate>().end()); return;
        case CoordinateType::XYZM: std::sort(items<CoordinateXYZM>().begin(), items<CoordinateXYZM>().end()); return;
        case CoordinateType::XYM:  std::sort(items<CoordinateXYM>().begin(), items<CoordinateXYM>().end()); return;
    }
}

bool
CoordinateSequence::equals(const CoordinateSequence* cl1,
                           const CoordinateSequence* cl2)
{
    if(cl1 == cl2) {
        return true;
    }

    if(cl1 == nullptr || cl2 == nullptr) {
        return false;
    }

    std::size_t npts1 = cl1->getSize();
    if(npts1 != cl2->getSize()) {
        return false;
    }
    for(std::size_t i = 0; i < npts1; i++) {
        if(!(cl1->getAt<CoordinateXY>(i) == cl2->getAt<CoordinateXY>(i))) {
            return false;
        }
    }
    return true;
}

bool
CoordinateSequence::equalsIdentical(const CoordinateSequence& other) const
{
    if (this == &other) {
        return true;
    }

    if (size() != other.size()) {
        return false;
    }

    if (hasZ() != other.hasZ()) {
        return false;
    }

    if (hasM() != other.hasM()) {
        return false;
    }

    assert(getCoordinateType() == other.getCoordinateType());

    for (std::size_t i = 0; i < m_vect.size(); i++) {
        const double& a = m_vect[i];
        const double& b = other.m_vect[i];
        if (a != b && !(std::isnan(a) && std::isnan(b))) {
            return false;
        }
    }

    return true;
}

void
CoordinateSequence::expandEnvelope(Envelope& env) const
{
    const std::size_t p_size = getSize();
    for(std::size_t i = 0; i < p_size; i++) {
        env.expandToInclude(getAt<CoordinateXY>(i));
    }
}

Envelope
CoordinateSequence::getEnvelope() const {
    if (isEmpty()) {
        return {};
    }

    double xmin = std::numeric_limits<double>::infinity();
    double ymin = std::numeric_limits<double>::infinity();
    double xmax = -std::numeric_limits<double>::infinity();
    double ymax = -std::numeric_limits<double>::infinity();

    for (std::size_t i = 0; i < m_vect.size(); i += stride()) {
        xmin = std::min(xmin, m_vect[i]);
        xmax = std::max(xmax, m_vect[i]);
        ymin = std::min(ymin, m_vect[i+1]);
        ymax = std::max(ymax, m_vect[i+1]);
    }

    return {xmin, xmax, ymin, ymax};
}


void
CoordinateSequence::setOrdinate(std::size_t index, std::size_t ordinateIndex, double value)
{
    switch(ordinateIndex) {
        case CoordinateSequence::X:
        getAt<CoordinateXY>(index).x = value;
        break;
        case CoordinateSequence::Y:
        getAt<CoordinateXY>(index).y = value;
        break;
        case CoordinateSequence::Z:
        getAt<Coordinate>(index).z = value;
        break;
        case CoordinateSequence::M:
        {
            if (getCoordinateType() == CoordinateType::XYZM) {
                getAt<CoordinateXYZM>(index).m = value;
            } else {
                getAt<CoordinateXYM>(index).m = value;
            }
            break;
        }
        default: {
            std::stringstream ss;
            ss << "Unknown ordinate index " << ordinateIndex;
            throw util::IllegalArgumentException(ss.str());
            break;
        }
    }
}

void
CoordinateSequence::setPoints(const std::vector<Coordinate>& v)
{
    m_stride = 3;
    m_hasdim = false;
    m_hasz = false;
    m_hasm = false;

    m_vect.resize(3 * v.size());
    const double* cbuf = reinterpret_cast<const double*>(v.data());
    m_vect.assign(cbuf, cbuf + m_vect.size());
}

void
CoordinateSequence::toVector(std::vector<Coordinate>& out) const
{
    if (getCoordinateType() == CoordinateType::XYZ) {
        const Coordinate* cbuf = reinterpret_cast<const Coordinate*>(m_vect.data());
        out.insert(out.end(), cbuf, cbuf + size());
    } else if (hasZ()) {
        for (const auto& c : items<Coordinate>()) {
            out.emplace_back(c.x, c.y, c.z);
        }
    } else {
        for (const auto& c : items<CoordinateXY>()) {
            out.emplace_back(c.x, c.y);
        }
    }
}

void
CoordinateSequence::toVector(std::vector<CoordinateXY>& out) const
{
    if (stride() == 2) {
        const CoordinateXY* cbuf = reinterpret_cast<const CoordinateXY*>(m_vect.data());
        out.insert(out.end(), cbuf, cbuf + size());
    } else {
        for (const CoordinateXY& c : items<CoordinateXY>()) {
            out.emplace_back(c.x, c.y);
        }
    }
}

void
CoordinateSequence::pop_back()
{
    switch (stride()) {
    case 4: m_vect.pop_back(); // fall through
    case 3: m_vect.pop_back(); // fall through
    case 2: m_vect.pop_back();
            m_vect.pop_back();
        break;
    default:
        assert(0);
    }
}

std::ostream&
operator<< (std::ostream& os, const CoordinateSequence& cs)
{
    os << "(";

    bool writeComma = false;
    auto write = [&os, &writeComma](const auto& coord) {
        if (writeComma) {
            os << ", ";
        } else {
            writeComma = true;
        }

        os << coord;
    };

    cs.forEach(write);
    os << ")";

    return os;
}

std::string
CoordinateSequence::toString() const
{
    std::ostringstream ss;
    ss << *this;
    return ss.str();
}

bool
operator== (const CoordinateSequence& s1, const CoordinateSequence& s2)
{
    return CoordinateSequence::equals(&s1, &s2);
}

bool
operator!= (const CoordinateSequence& s1, const CoordinateSequence& s2)
{
    return ! CoordinateSequence::equals(&s1, &s2);
}

} // namespace geos::geom
} // namespace geos
