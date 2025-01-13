/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2024 ISciences, LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/export.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Quadrant.h>
#include <geos/algorithm/CircularArcs.h>
#include <geos/algorithm/Orientation.h>
#include <geos/triangulate/quadedge/TrianglePredicate.h>

namespace geos {
namespace geom {

/// A CircularArc is a reference to three points that define a circular arc.
/// It provides for the lazy calculation of various arc properties such as the center, radius, and orientation
class GEOS_DLL CircularArc {
public:

    using CoordinateXY = geom::CoordinateXY;

    CircularArc(const CoordinateXY& q0, const CoordinateXY& q1, const CoordinateXY& q2)
        : p0(q0)
        , p1(q1)
        , p2(q2)
        , m_center_known(false)
        , m_radius_known(false)
        , m_orientation_known(false)
    {}

    const CoordinateXY& p0;
    const CoordinateXY& p1;
    const CoordinateXY& p2;

    /// Return the orientation of the arc as one of:
    /// - algorithm::Orientation::CLOCKWISE,
    /// - algorithm::Orientation::COUNTERCLOCKWISE
    /// - algorithm::Orientation::COLLINEAR
    int orientation() const {
        if (!m_orientation_known) {
            m_orientation = algorithm::Orientation::index(p0, p1, p2);
            m_orientation_known = true;
        }
        return m_orientation;
    }

    /// Return the center point of the circle associated with this arc
    const CoordinateXY& getCenter() const {
        if (!m_center_known) {
            m_center = algorithm::CircularArcs::getCenter(p0, p1, p2);
            m_center_known = true;
        }

        return m_center;
    }

    /// Return the radius of the circle associated with this arc
    double getRadius() const {
        if (!m_radius_known) {
            m_radius = getCenter().distance(p0);
            m_radius_known = true;
        }

        return m_radius;
    }

    /// Return whether this arc forms a complete circle
    bool isCircle() const {
        return p0.equals(p2);
    }

    /// Returns whether this arc forms a straight line (p0, p1, and p2 are collinear)
    bool isLinear() const {
        return !std::isfinite(getRadius());
    }

    /// Return the inner angle of the sector associated with this arc
    double getAngle() const {
        if (isCircle()) {
            return 2*MATH_PI;
        }

        /// Even Rouault:
        /// potential optimization?: using crossproduct(p0 - center, p2 - center) = radius * radius * sin(angle)
        /// could yield the result by just doing a single asin(), instead of 2 atan2()
        /// actually one should also likely compute dotproduct(p0 - center, p2 - center) = radius * radius * cos(angle),
        /// and thus angle = atan2(crossproduct(p0 - center, p2 - center) , dotproduct(p0 - center, p2 - center) )
        auto t0 = theta0();
        auto t2 = theta2();

        if (orientation() == algorithm::Orientation::COUNTERCLOCKWISE) {
            std::swap(t0, t2);
        }

        if (t0 < t2) {
            t0 += 2*MATH_PI;
        }

        auto diff = t0-t2;

        return diff;
    }

    /// Return the length of the arc
    double getLength() const {
        if (isLinear()) {
            return p0.distance(p2);
        }

        return getAngle()*getRadius();
    }

    /// Return the area enclosed by the arc p0-p1-p2 and the line segment p2-p0
    double getArea() const {
        if (isLinear()) {
            return 0;
        }

        auto R = getRadius();
        auto theta = getAngle();
        return R*R/2*(theta - std::sin(theta));
    }

    /// Return the angle of p0
    double theta0() const {
        return std::atan2(p0.y - getCenter().y, p0.x - getCenter().x);
    }

    /// Return the angle of p2
    double theta2() const {
        return std::atan2(p2.y - getCenter().y, p2.x - getCenter().x);
    }

    /// Check to see if a coordinate lies on the arc
    /// Only the angle is checked, so it is assumed that the point lies on
    /// the circle of which this arc is a part.
    bool containsPointOnCircle(const CoordinateXY& q) const {
        double theta = std::atan2(q.y - getCenter().y, q.x - getCenter().x);
        return containsAngle(theta);
    }

    /// Check to see if a coordinate lies on the arc, after testing whether
    /// it lies on the circle.
    bool containsPoint(const CoordinateXY& q) {
        if (q == p0 || q == p1 || q == p2) {
            return true;
        }

        auto dist = std::abs(q.distance(getCenter()) - getRadius());

        if (dist > 1e-8) {
            return false;
        }

        if (triangulate::quadedge::TrianglePredicate::isInCircleNormalized(p0, p1, p2, q) != geom::Location::BOUNDARY) {
            return false;
        }

        return containsPointOnCircle(q);
    }

    /// Check to see if a given angle lies on this arc
    bool containsAngle(double theta) const {
        auto t0 = theta0();
        auto t2 = theta2();

        if (theta == t0 || theta == t2) {
            return true;
        }

        if (orientation() == algorithm::Orientation::COUNTERCLOCKWISE) {
            std::swap(t0, t2);
        }

        t2 -= t0;
        theta -= t0;

        if (t2 < 0) {
            t2 += 2*MATH_PI;
        }
        if (theta < 0) {
            theta += 2*MATH_PI;
        }

        return theta >= t2;
    }

    /// Return true if the arc is pointing positive in the y direction
    /// at the location of a specified point. The point is assumed to
    /// be on the arc.
    bool isUpwardAtPoint(const CoordinateXY& q) const {
        auto quad = geom::Quadrant::quadrant(getCenter(), q);
        bool isUpward;

        if (orientation() == algorithm::Orientation::CLOCKWISE) {
            isUpward = (quad == geom::Quadrant::SW || quad == geom::Quadrant::NW);
        } else {
            isUpward = (quad == geom::Quadrant::SE || quad == geom::Quadrant::NE);
        }

        return isUpward;
    }

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = geom::CoordinateXY;
        using pointer = const geom::CoordinateXY*;
        using reference = const geom::CoordinateXY&;

        Iterator(const CircularArc& arc, int i) : m_arc(arc), m_i(i) {}

        reference operator*() const {
            return m_i == 0 ? m_arc.p0 : (m_i == 1 ? m_arc.p1 : m_arc.p2);
        }

        Iterator& operator++() {
            m_i++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator ret = *this;
            m_i++;
            return ret;
        }

        bool operator==(const Iterator& other) const {
            return m_i == other.m_i;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        const CircularArc& m_arc;
        int m_i;

    };

    Iterator begin() const {
        return Iterator(*this, 0);
    }

    Iterator end() const {
        return Iterator(*this, 3);
    }

private:
    mutable CoordinateXY m_center;
    mutable double m_radius;
    mutable int m_orientation;
    mutable bool m_center_known = false;
    mutable bool m_radius_known = false;
    mutable bool m_orientation_known = false;
};

}
}
