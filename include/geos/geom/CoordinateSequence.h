/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2022 ISciences LLC
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/export.h>

#include <geos/geom/Coordinate.h> // for applyCoordinateFilter
#include <geos/geom/CoordinateSequenceIterator.h>
#include <geos/util/Vector.h>

#include <cassert>
#include <vector>
#include <iostream>
#include <iosfwd> // ostream
#include <memory> // for unique_ptr typedef

// Forward declarations
namespace geos {
namespace geom {
class Envelope;
class CoordinateFilter;
}
}

namespace geos {
namespace geom { // geos::geom

/**
 * \class CoordinateSequence geom.h geos.h
 *
 * \brief
 * The internal representation of a list of coordinates inside a Geometry.
 *
 * A CoordinateSequence is capable of storing XY, XYZ, XYM, or XYZM Coordinates. For efficient
 * storage, the dimensionality of the CoordinateSequence should be specified at creation using
 * the constructor with `hasz` and `hasm` arguments. Currently most of the GEOS code base
 * stores 2D Coordinates and accesses using the Coordinate type. Sequences used by these parts
 * of the code must be created with constructors without `hasz` and `hasm` arguments.
 *
 * If a high-dimension Coordinate coordinate is read from a low-dimension CoordinateSequence,
 * the higher dimensions will be populated with incorrect values or a segfault may occur.
 *
 */
class GEOS_DLL CoordinateSequence {

public:

    /// Standard ordinate index values
    enum { X, Y, Z, M };

    using iterator = CoordinateSequenceIterator<CoordinateSequence, Coordinate>;
    using const_iterator = CoordinateSequenceIterator<const CoordinateSequence, const Coordinate>;

    typedef std::unique_ptr<CoordinateSequence> Ptr;

    /// \defgroup construct Constructors
    /// @{

    /**
     *  Create an CoordinateSequence capable of storing XY or XYZ coordinates.
     */
    CoordinateSequence();

    /**
     * Create a CoordinateSequence capable of storing XY, XYZ or XYZM coordinates.
     *
     * @param size size of the sequence to create.
     * @param dim 2 for 2D, 3 for XYZ, 4 for XYZM, or 0 to determine
     *            this based on the first coordinate in the sequence
     */
    CoordinateSequence(std::size_t size, std::size_t dim = 0);

    /**
     * Create a CoordinateSequence that packs coordinates of any dimension.
     * Code using a CoordinateSequence constructed in this way must not
     * attempt to access references to coordinates with dimensions that
     * are not actually stored in the sequence.
     *
     * @param size size of the sequence to create
     * @param hasz true if the sequence should store z values
     * @param hasm true if the sequence should store m values
     * @param initialize true if the sequence should be initialized to default coordinate values
     */
    CoordinateSequence(std::size_t size, bool hasz, bool hasm, bool initialize = true);

    /**
     * Create a CoordinateSequence from an externally-owned buffer
     * of packed Coordinates. If Coordinates are added to the CoordinateSequence
     * or the CoordinateSequence requires repacking, the values will
     * be copied into a new buffer owned by this CoordinateSeuqnce.
     * Code using a CoordinateSequence constructed in this way must not
     * attempt to access references to coordinates with dimensions that
     * are not actually stored in the sequence.
     *
     * @param buf buffer of interleaved coordinates
     * @param size number of coordinates in the buffer
     * @param hasz true if the buffer has z values
     * @param hasm true if the buffer has m values
     */
    CoordinateSequence(double* buf, std::size_t size, bool hasz, bool hasm);

    /**
     * Create a CoordinateSequence from a list of XYZ coordinates.
     * Code using the sequence may only access references to CoordinateXY
     * or Coordinate objects.
     */
    CoordinateSequence(const std::initializer_list<Coordinate>&);

    /**
     * Create a CoordinateSequence from a list of XY coordinates.
     * Code using the sequence may only access references to CoordinateXY objects.
     */
    CoordinateSequence(const std::initializer_list<CoordinateXY>&);

    /**
     * Create a CoordinateSequence from a list of XYM coordinates.
     * Code using the sequence may only access references to CoordinateXY
     * or CoordinateXYM objects.
     */
    CoordinateSequence(const std::initializer_list<CoordinateXYM>&);

    /**
     * Create a CoordinateSequence from a list of XYZM coordinates.
     */
    CoordinateSequence(const std::initializer_list<CoordinateXYZM>&);

    /**
     * Create a CoordinateSequence storing XY values only.
     *
     * @param size size of the sequence to create
     */
    static CoordinateSequence XY(std::size_t size)  {
        return CoordinateSequence(size, false, false);
    }

    /**
     * Create a CoordinateSequence storing XYZ values only.
     *
     * @param size size of the sequence to create
     */
    static CoordinateSequence XYZ(std::size_t size)  {
        return CoordinateSequence(size, true, false);
    }

    /**
     * Create a CoordinateSequence storing XYZM values only.
     *
     * @param size size of the sequence to create
     */
    static CoordinateSequence XYZM(std::size_t size)  {
        return CoordinateSequence(size, true, true);
    }

    /**
     * Create a CoordinateSequence storing XYM values only.
     *
     * @param size size of the sequence to create
     */
    static CoordinateSequence XYM(std::size_t size)  {
        return CoordinateSequence(size, false, true);
    }

    /** \brief
     * Returns a heap-allocated deep copy of this CoordinateSequence.
     */
    std::unique_ptr<CoordinateSequence> clone() const;

    /// @}
    /// \defgroup prop Properties
    /// @{

    /**
     * Return the Envelope containing all points in this sequence.
     * The Envelope is not cached and is computed each time the
     * method is called.
     */
    Envelope getEnvelope() const;

    /** \brief
     * Returns the number of Coordinates
     */
    std::size_t getSize() const {
        return size();
    }

    /** \brief
     * Returns the number of Coordinates
     */
    size_t size() const
    {
        assert(stride() == 2 || stride() == 3 || stride() == 4);
        switch(stride()) {
            case 2: return m_vect.size() / 2;
            case 4: return m_vect.size() / 4;
            default : return m_vect.size() / 3;
        }
    }

    /// Returns <code>true</code> if list contains no coordinates.
    bool isEmpty() const {
        return m_vect.empty();
    }

    /** \brief
    * Tests whether an a {@link CoordinateSequence} forms a ring,
    * by checking length and closure. Self-intersection is not checked.
    *
    * @return true if the coordinate form a ring.
    */
    bool isRing() const;

    /**
     * Returns the dimension (number of ordinates in each coordinate)
     * for this sequence.
     *
     * @return the dimension of the sequence.
     */
    std::size_t getDimension() const;

    bool hasZ() const {
        return m_hasdim ? m_hasz : (m_vect.empty() || !std::isnan(m_vect[2]));
    }

    bool hasM() const {
        return m_hasm;
    }

    /// Returns true if contains any two consecutive points
    bool hasRepeatedPoints() const;

    /// Returns true if contains any NaN/Inf coordinates
    bool hasRepeatedOrInvalidPoints() const;

    /// Get the backing type of this CoordinateSequence. This is not necessarily
    /// consistent with the dimensionality of the stored Coordinates; 2D Coordinates
    /// may be stored as a XYZ coordinates.
    CoordinateType getCoordinateType() const {
        switch(stride()) {
            case 4: return CoordinateType::XYZM;
            case 2: return CoordinateType::XY;
            default: return hasM() ? CoordinateType::XYM : CoordinateType::XYZ;
        }
    }

    /// @}
    /// \defgroup access Accessors
    /// @{

    /** \brief
     * Returns a read-only reference to Coordinate at position i.
     */
    template<typename T=Coordinate>
    const T& getAt(std::size_t i) const {
        static_assert(std::is_base_of<CoordinateXY, T>::value, "Must be a Coordinate class");
        assert(sizeof(T) <= sizeof(double) * stride());
        assert(i*stride() < m_vect.size());
        const T* orig = reinterpret_cast<const T*>(&m_vect[i*stride()]);
        return *orig;
    }

    /** \brief
     * Returns a reference to Coordinate at position i.
     */
    template<typename T=Coordinate>
    T& getAt(std::size_t i) {
        static_assert(std::is_base_of<CoordinateXY, T>::value, "Must be a Coordinate class");
        assert(sizeof(T) <= sizeof(double) * stride());
        assert(i*stride() < m_vect.size());
        T* orig = reinterpret_cast<T*>(&m_vect[i*stride()]);
        return *orig;
    }

    /** \brief
     * Write Coordinate at position i to given Coordinate.
     */
    template<typename T>
    void getAt(std::size_t i, T& c) const {
        switch(getCoordinateType()) {
            case CoordinateType::XY: c = getAt<CoordinateXY>(i); break;
            case CoordinateType::XYZ: c = getAt<Coordinate>(i); break;
            case CoordinateType::XYZM: c = getAt<CoordinateXYZM>(i); break;
            case CoordinateType::XYM: c = getAt<CoordinateXYM>(i); break;
            default: getAt<Coordinate>(i);
        }
    }

    void getAt(std::size_t i, CoordinateXY& c) const {
        c = getAt<CoordinateXY>(i);
    }

    // TODO: change to return CoordinateXY
    /**
     * Returns a read-only reference to Coordinate at i
     */
    const Coordinate& operator[](std::size_t i) const
    {
        return getAt(i);
    }

    // TODO: change to return CoordinateXY
    /**
     * Returns a reference to Coordinate at i
     */
    Coordinate&
    operator[](std::size_t i)
    {
        return getAt(i);
    }

    /**
     * Returns the ordinate of a coordinate in this sequence.
     * Ordinate indices 0 and 1 are assumed to be X and Y.
     * Ordinates indices greater than 1 have user-defined semantics
     * (for instance, they may contain other dimensions or measure values).
     *
     * @param index  the coordinate index in the sequence
     * @param ordinateIndex the ordinate index in the coordinate
     *                      (in range [0, dimension-1])
     */
    double getOrdinate(std::size_t index, std::size_t ordinateIndex) const;

    /**
     * Returns ordinate X (0) of the specified coordinate.
     *
     * @param index
     * @return the value of the X ordinate in the index'th coordinate
     */
    double getX(std::size_t index) const
    {
        return m_vect[index * stride()];
    }

    /**
     * Returns ordinate Y (1) of the specified coordinate.
     *
     * @param index
     * @return the value of the Y ordinate in the index'th coordinate
     */
    double getY(std::size_t index) const
    {
        return m_vect[index * stride() + 1];
    }

    /// Return last Coordinate in the sequence
    template<typename T=Coordinate>
    const T& back() const
    {
        return getAt<T>(size() - 1);
    }

    /// Return last Coordinate in the sequence
    template<typename T=Coordinate>
    T& back()
    {
        return getAt<T>(size() - 1);
    }

    /// Return first Coordinate in the sequence
    template<typename T=Coordinate>
    const T& front() const
    {
        return *(reinterpret_cast<const T*>(m_vect.data()));
    }

    /// Return first Coordinate in the sequence
    template<typename T=Coordinate>
    T& front()
    {
        return *(reinterpret_cast<T*>(m_vect.data()));
    }

    /// Pushes all Coordinates of this sequence into the provided vector.
    void toVector(std::vector<Coordinate>& coords) const;

    void toVector(std::vector<CoordinateXY>& coords) const;


    /// @}
    /// \defgroup mutate Mutators
    /// @{

    /// Copy Coordinate c to position pos
    template<typename T>
    void setAt(const T& c, std::size_t pos) {
        switch(getCoordinateType()) {
            case CoordinateType::XY: setAtImpl<CoordinateXY>(c, pos); break;
            case CoordinateType::XYZ: setAtImpl<Coordinate>(c, pos); break;
            case CoordinateType::XYZM: setAtImpl<CoordinateXYZM>(c, pos); break;
            case CoordinateType::XYM: setAtImpl<CoordinateXYM>(c, pos); break;
            default: setAtImpl<Coordinate>(c, pos);
        }
    }

    /**
     * Sets the value for a given ordinate of a coordinate in this sequence.
     *
     * @param index  the coordinate index in the sequence
     * @param ordinateIndex the ordinate index in the coordinate
     *                      (in range [0, dimension-1])
     * @param value  the new ordinate value
     */
    void setOrdinate(std::size_t index, std::size_t ordinateIndex, double value);

    /// Substitute Coordinate list with a copy of the given vector
    void setPoints(const std::vector<Coordinate>& v);

    /// @}
    /// \defgroup add Adding methods
    /// @{

    /// Adds the specified coordinate to the end of the sequence. Dimensions
    /// present in the coordinate but not in the sequence will be ignored.
    /// If multiple coordinates are to be added, a multiple-insert method should
    /// be used for best performance.
    template<typename T=Coordinate>
    void add(const T& c) {
        add(c, size());
    }

    /// Adds the specified coordinate to the end of the sequence. Dimensions
    /// present in the coordinate but not in the sequence will be ignored. If
    /// allowRepeated is false, the coordinate will not be added if it is the
    /// same as the last coordinate in the sequence.
    /// If multiple coordinates are to be added, a multiple-insert method should
    /// be used for best performance.
    template<typename T>
    void add(const T& c, bool allowRepeated)
    {
        if(!allowRepeated && !isEmpty()) {
            const CoordinateXY& last = back<CoordinateXY>();
            if(last.equals2D(c)) {
                return;
            }
        }

        add(c);
    }

    /** \brief
     * Inserts the specified coordinate at the specified position in
     * this sequence. If multiple coordinates are to be added, a multiple-
     * insert method should be used for best performance.
     *
     * @param c the coordinate to insert
     * @param pos the position at which to insert
     */
    template<typename T>
    void add(const T& c, std::size_t pos)
    {
        static_assert(std::is_base_of<CoordinateXY, T>::value, "Must be a Coordinate class");

        // c may be a reference inside m_vect, so we make sure it will not
        // grow before adding it
        if (m_vect.size() + stride() <= m_vect.capacity()) {
            make_space(pos, 1);
            setAt(c, static_cast<std::size_t>(pos));
        } else {
            T tmp{c};
            make_space(pos, 1);
            setAt(tmp, static_cast<std::size_t>(pos));
        }
    }

    /** \brief
     * Inserts the specified coordinate at the specified position in
     * this list.
     *
     * @param i the position at which to insert
     * @param coord the coordinate to insert
     * @param allowRepeated if set to false, repeated coordinates are
     *                      collapsed
     */
    template<typename T>
    void add(std::size_t i, const T& coord, bool allowRepeated)
    {
        // don't add duplicate coordinates
        if(! allowRepeated) {
            std::size_t sz = size();
            if(sz > 0) {
                if(i > 0) {
                    const CoordinateXY& prev = getAt<CoordinateXY>(i - 1);
                    if(prev.equals2D(coord)) {
                        return;
                    }
                }
                if(i < sz) {
                    const CoordinateXY& next = getAt<CoordinateXY>(i);
                    if(next.equals2D(coord)) {
                        return;
                    }
                }
            }
        }

        add(coord, i);
    }

    void add(double x, double y) {
        CoordinateXY c(x, y);
        add(c);
    }

    void add(const CoordinateSequence& cs);

    void add(const CoordinateSequence& cs, bool allowRepeated);

    void add(const CoordinateSequence& cl, bool allowRepeated, bool forwardDirection);

    void add(const CoordinateSequence& cs, std::size_t from, std::size_t to);

    void add(const CoordinateSequence& cs, std::size_t from, std::size_t to, bool allowRepeated);

    template<typename T, typename... Args>
    void add(T begin, T end, Args... args) {
        for (auto it = begin; it != end; ++it) {
            add(*it, args...);
        }
    }

    template<typename T>
    void add(std::size_t i, T from, T to) {
        auto npts = static_cast<std::size_t>(std::distance(from, to));
        make_space(i, npts);

        for (auto it = from; it != to; ++it) {
            setAt(*it, i);
            i++;
        }
    }

    /// @}
    /// \defgroup util Utilities
    /// @{

    void clear() {
        m_vect.clear();
    }

    void reserve(std::size_t capacity) {
        m_vect.reserve(capacity * stride());
    }

    void resize(std::size_t capacity) {
        m_vect.resize(capacity * stride());
    }

    void pop_back();

    /// Get a string representation of CoordinateSequence
    std::string toString() const;

    /// Returns lower-left Coordinate in list
    const CoordinateXY* minCoordinate() const;

    /** \brief
     *  Returns either the given CoordinateSequence if its length
     *  is greater than the given amount, or an empty CoordinateSequence.
     */
    static CoordinateSequence* atLeastNCoordinatesOrNothing(std::size_t n,
            CoordinateSequence* c);

    /// Return position of a Coordinate
    //
    /// or numeric_limits<std::size_t>::max() if not found
    ///
    static std::size_t indexOf(const CoordinateXY* coordinate,
                               const CoordinateSequence* cl);

    /**
     * \brief
     * Returns true if the two arrays are identical, both null,
     * or pointwise equal in two dimensions
     */
    static bool equals(const CoordinateSequence* cl1,
                       const CoordinateSequence* cl2);

    /**
     * \brief
     * Returns true if the two sequences are identical (pointwise
     * equal in all dimensions, with NaN == NaN).
     */
    bool equalsIdentical(const CoordinateSequence& other) const;

    /// Scroll given CoordinateSequence so to start with given Coordinate.
    static void scroll(CoordinateSequence* cl, const CoordinateXY* firstCoordinate);

    /** \brief
     * Determines which orientation of the {@link Coordinate} array
     * is (overall) increasing.
     *
     * In other words, determines which end of the array is "smaller"
     * (using the standard ordering on {@link Coordinate}).
     * Returns an integer indicating the increasing direction.
     * If the sequence is a palindrome, it is defined to be
     * oriented in a positive direction.
     *
     * @param pts the array of Coordinates to test
     * @return <code>1</code> if the array is smaller at the start
     * or is a palindrome,
     * <code>-1</code> if smaller at the end
     *
     * NOTE: this method is found in CoordinateArrays class for JTS
     */
    static int increasingDirection(const CoordinateSequence& pts);

    /// Reverse Coordinate order in given CoordinateSequence
    void reverse();

    void sort();


    /**
     * Expands the given Envelope to include the coordinates in the
     * sequence.
     * @param env the envelope to expand
     */
    void expandEnvelope(Envelope& env) const;

    void closeRing(bool allowRepeated = false);

    /// @}
    /// \defgroup iterate Iteration
    /// @{

    template<typename Filter>
    void apply_rw(const Filter* filter) {
        switch(getCoordinateType()) {
            case CoordinateType::XY:    for (auto& c : items<CoordinateXY>())   { filter->filter_rw(&c); } break;
            case CoordinateType::XYZ:   for (auto& c : items<Coordinate>())     { filter->filter_rw(&c); } break;
            case CoordinateType::XYM:   for (auto& c : items<CoordinateXYM>())  { filter->filter_rw(&c); } break;
            case CoordinateType::XYZM:  for (auto& c : items<CoordinateXYZM>()) { filter->filter_rw(&c); } break;
        }
        m_hasdim = m_hasz = false; // re-check (see http://trac.osgeo.org/geos/ticket/435)
    }

    template<typename Filter>
    void apply_ro(Filter* filter) const {
        switch(getCoordinateType()) {
            case CoordinateType::XY:    for (const auto& c : items<CoordinateXY>())   { filter->filter_ro(&c); } break;
            case CoordinateType::XYZ:   for (const auto& c : items<Coordinate>())     { filter->filter_ro(&c); } break;
            case CoordinateType::XYM:   for (const auto& c : items<CoordinateXYM>())  { filter->filter_ro(&c); } break;
            case CoordinateType::XYZM:  for (const auto& c : items<CoordinateXYZM>()) { filter->filter_ro(&c); } break;
        }
    }

    template<typename F>
    void forEach(F&& fun) const {
        switch(getCoordinateType()) {
            case CoordinateType::XY:    for (const auto& c : items<CoordinateXY>())   { fun(c); } break;
            case CoordinateType::XYZ:   for (const auto& c : items<Coordinate>())     { fun(c); } break;
            case CoordinateType::XYM:   for (const auto& c : items<CoordinateXYM>())  { fun(c); } break;
            case CoordinateType::XYZM:  for (const auto& c : items<CoordinateXYZM>()) { fun(c); } break;
        }
    }

    template<typename T, typename F>
    void forEach(F&& fun) const
    {
        for (std::size_t i = 0; i < size(); i++) {
            fun(getAt<T>(i));
        }
    }

    template<typename T, typename F>
    void forEach(std::size_t from, std::size_t to, F&& fun) const
    {
        for (std::size_t i = from; i <= to; i++) {
            fun(getAt<T>(i));
        }
    }

    template<typename T>
    class Coordinates {
    public:
        using SequenceType = typename std::conditional<std::is_const<T>::value, const CoordinateSequence, CoordinateSequence>::type;

        explicit Coordinates(SequenceType* seq) : m_seq(seq) {}

        CoordinateSequenceIterator<SequenceType, T> begin() {
            return {m_seq};
        }

        CoordinateSequenceIterator<SequenceType, T> end() {
            return {m_seq, m_seq->getSize()};
        }

        CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>
        begin() const {
            return CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>{m_seq};
        }

        CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>
        end() const {
            return CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>{m_seq, m_seq->getSize()};
        }

        CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>
        cbegin() const {
            return CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>{m_seq};
        }

        CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>
        cend() const {
            return CoordinateSequenceIterator<const SequenceType, typename std::add_const<T>::type>{m_seq, m_seq->getSize()};
        }

    private:
        SequenceType* m_seq;
    };

    template<typename T>
    Coordinates<typename std::add_const<T>::type> items() const {
        return Coordinates<typename std::add_const<T>::type>(this);
    }

    template<typename T>
    Coordinates<T> items() {
        return Coordinates<T>(this);
    }


    /// @}

    double* data() {
        return m_vect.data();
    }

    const double* data() const {
        return m_vect.data();
    }

private:
    geos::util::Vector<double> m_vect; // Vector to store values

    uint8_t m_stride;           // Stride of stored values, corresponding to underlying type

    mutable bool m_hasdim;      // Has the dimension of this sequence been determined? Or was it created with no
                                // explicit dimensionality, and we're waiting for getDimension() to be called
                                // after some coordinates have been added?
    mutable bool m_hasz;
    bool m_hasm;

    void initialize();

    template<typename T1, typename T2>
    void setAtImpl(const T2& c, std::size_t pos) {
        auto& orig = getAt<T1>(pos);
        orig = c;
    }

    void make_space(std::size_t pos, std::size_t n) {
        m_vect.make_space(m_vect.begin() + pos * stride(), n * stride());
    }

    std::uint8_t stride() const {
        return m_stride;
    }

};

GEOS_DLL std::ostream& operator<< (std::ostream& os, const CoordinateSequence& cs);

GEOS_DLL bool operator== (const CoordinateSequence& s1, const CoordinateSequence& s2);

GEOS_DLL bool operator!= (const CoordinateSequence& s1, const CoordinateSequence& s2);

} // namespace geos::geom
} // namespace geos

