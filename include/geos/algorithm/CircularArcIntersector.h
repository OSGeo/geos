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

#include <array>
#include <cstdint>

#include <geos/export.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CircularArc.h>
#include <geos/geom/LineSegment.h>

namespace geos::algorithm {

class GEOS_DLL CircularArcIntersector {
public:
    using CoordinateXY = geom::CoordinateXY;
    using CircularArc = geom::CircularArc;
    using Envelope = geom::Envelope;

    enum intersection_type : uint8_t {
        NO_INTERSECTION = 0,
        ONE_POINT_INTERSECTION = 1,
        TWO_POINT_INTERSECTION = 2,
        COCIRCULAR_INTERSECTION = 3,
    };

    intersection_type getResult() const
    {
        return result;
    }

    const CoordinateXY& getPoint(std::uint8_t i) const
    {
        return intPt[i];
    }

    const std::array<CoordinateXY, 3>& getArc(std::uint8_t i) const
    {
        return intArc[i];
    }

    std::uint8_t getNumPoints() const
    {
        return nPt;
    }

    std::uint8_t getNumArcs() const
    {
        return nArc;
    }

    /// Determines whether and where a circular arc intersects a line segment.
    ///
    /// Sets the appropriate value of intersection_type and stores the intersection
    /// points, if any.
    void intersects(const CircularArc& arc, const CoordinateXY& p0, const CoordinateXY& p1);

    void intersects(const CircularArc& arc, const geom::LineSegment& seg)
    {
        intersects(arc, seg.p0, seg.p1);
    }

    /// Determines whether and where two circular arcs intersect.
    ///
    ///	Sets the appropriate value of intesection_type and stores the intersection
    /// points and/or arcs, if any.
    void intersects(const CircularArc& arc1, const CircularArc& arc2);

    static int
    circleIntersects(const CoordinateXY& center, double r, const CoordinateXY& p0, const CoordinateXY& p1, CoordinateXY& isect0, CoordinateXY& isect1);

private:

    void intersects(const CoordinateXY& p0, const CoordinateXY& p1, const CoordinateXY& q0, const CoordinateXY& q1);

    std::array<CoordinateXY, 2> intPt;
    std::array<std::array<CoordinateXY, 3>, 2> intArc;
    intersection_type result;
    std::uint8_t nPt = 0;
    std::uint8_t nArc = 0;

};

}
