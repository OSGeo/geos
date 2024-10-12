/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2019 Martin Davis <mtnclimb@gmail.com>
 * Copyright (C) 2011      Sandro Santilli <strk@kbt.io>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

//
// Test Suite for geos::algorithm::InteriorPointArea

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/Intersection.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/Geometry.h>
// std
#include <memory>

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_intersection_data {
    typedef geos::geom::Geometry Geometry;
    typedef geos::geom::Coordinate Coordinate;
    typedef geos::algorithm::Intersection Intersection;

    geos::io::WKTReader reader;
    std::unique_ptr<Geometry> geom;

    double MAX_ABS_ERROR = 1e-5;

    void checkIntersectionNull(
        double p1x, double p1y, double p2x, double p2y,
        double q1x, double q1y, double q2x, double q2y)
    {
        Coordinate p1(p1x, p1y);
        Coordinate p2(p2x, p2y);
        Coordinate q1(q1x, q1y);
        Coordinate q2(q2x, q2y);
        auto actual = Intersection::intersection(p1, p2, q1, q2);
        ensure("checkIntersectionNull", actual.isNull());
    }

    void checkIntersection(
        double p1x, double p1y, double p2x, double p2y,
        double q1x, double q1y, double q2x, double q2y,
        double expectedx, double expectedy)
    {
        Coordinate p1(p1x, p1y);
        Coordinate p2(p2x, p2y);
        Coordinate q1(q1x, q1y);
        Coordinate q2(q2x, q2y);
        Coordinate expected(expectedx, expectedy);
        auto actual = Intersection::intersection(p1, p2, q1, q2);
        double dist = actual.distance(expected);
        // std::cout << "Expected: " << expected << "  Actual: " << actual << "  Dist = " << dist << std::endl;
        ensure("checkIntersection", dist <= MAX_ABS_ERROR);
    }

    void checkIntersectionLineSegment(
        double p1x, double p1y, double p2x, double p2y,
        double q1x, double q1y, double q2x, double q2y,
        double expectedx, double expectedy)
    {
        Coordinate p1(p1x, p1y);
        Coordinate p2(p2x, p2y);
        Coordinate q1(q1x, q1y);
        Coordinate q2(q2x, q2y);
        Coordinate actual(Intersection::intersectionLineSegment(p1, p2, q1, q2));
        Coordinate expected(expectedx, expectedy);
        double dist = actual.distance(expected);
        ensure("checkIntersectionLineSegment", dist <= MAX_ABS_ERROR);
    }

    void checkIntersectionLineSegmentNull(
        double p1x, double p1y, double p2x, double p2y,
        double q1x, double q1y, double q2x, double q2y)
    {
        Coordinate p1(p1x, p1y);
        Coordinate p2(p2x, p2y);
        Coordinate q1(q1x, q1y);
        Coordinate q2(q2x, q2y);
        Coordinate actual(Intersection::intersectionLineSegment(p1, p2, q1, q2));
        ensure("checkIntersectionLineSegmentNull", actual.isNull());
    }

    test_intersection_data()
    {}

};

typedef test_group<test_intersection_data> group;
typedef group::object object;

group test_intersection_data("geos::algorithm::Intersection");

// testSimple
template<>
template<>
void object::test<1>()
{
    checkIntersection(0,0, 10,10, 0,10, 10,0, 5,5);
}

// testCollinear
template<>
template<>
void object::test<2>()
{
    checkIntersectionNull(0,0, 10,10, 20,20, 30, 30 );
}

// testParallel
template<>
template<>
void object::test<3>()
{
    checkIntersectionNull(
        0,0,  10,10,
        10,0, 20,10 );
}

// testAlmostCollinear
template<>
template<>
void object::test<4>()
{
    checkIntersection(
        35613471.6165017, 4257145.306132293, 35613477.7705378, 4257160.528222711,
        35613477.77505724, 4257160.539653536, 35613479.85607389, 4257165.92369170,
        35613477.772841461, 4257160.5339209242 );
}

// testAlmostCollinearCond
template<>
template<>
void object::test<5>()
{
    checkIntersection(
        1.6165017, 45.306132293, 7.7705378, 60.528222711,
        7.77505724, 60.539653536, 9.85607389, 65.92369170,
        7.772841461, 60.5339209242 );
}

// testLineSegCross
template<>
template<>
void object::test<6>()
{
    checkIntersectionLineSegment( 0, 0, 0, 1,     -1, 9, 1, 9,     0, 9 );
    checkIntersectionLineSegment( 0, 0, 0, 1,     -1, 2, 1, 4,     0, 3 );
}

// testLineSegTouch
template<>
template<>
void object::test<7>()
{
    checkIntersectionLineSegment( 0, 0, 0, 1,     -1, 9, 0, 9,     0, 9 );
    checkIntersectionLineSegment( 0, 0, 0, 1,      0, 2, 1, 4,     0, 2 );
}

// testLineSegCollinear
template<>
template<>
void object::test<8>()
{
    checkIntersectionLineSegment( 0, 0, 0, 1,     0, 9, 0, 8,     0, 9 );
}

// testLineSegNone
template<>
template<>
void object::test<9>()
{
    checkIntersectionLineSegmentNull( 0, 0, 0, 1,    2, 9,  1, 9 );
    checkIntersectionLineSegmentNull( 0, 0, 0, 1,   -2, 9, -1, 9 );
    checkIntersectionLineSegmentNull( 0, 0, 0, 1,    2, 9,  1, 9 );
}


} // namespace tut

