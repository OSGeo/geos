//
// Test Suite for geos::algorithm::PointLocation
// Ported from JTS junit/algorithm/PointLocationTest.java

#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/PointLocation.h>

// std
#include <string>
#include <memory>

using geos::algorithm::PointLocation;
using geos::geom::CoordinateXY;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_PointLocation_data {

    geos::io::WKTReader r_;

    void
    checkOnLine(double x, double y, const std::string& wktLine, bool isExpected)
    {
        CoordinateXY p(x, y);
        std::unique_ptr<CoordinateSequence> line = readPts(wktLine);
        bool isOnLine = PointLocation::isOnLine(p, line.get());
        ensure(isOnLine == isExpected);
    }

    void
    checkOnSegment(double x, double y, const std::string& wktLine, bool isExpected)
    {
        CoordinateXY p(x, y);
        std::unique_ptr<CoordinateSequence> line = readPts(wktLine);

        bool isOnSeg = PointLocation::isOnSegment(p, line->getAt(0), line->getAt(1));
        ensure(isOnSeg == isExpected);
    }

    std::unique_ptr<CoordinateSequence>
    readPts(const std::string& wkt)
    {
        std::unique_ptr<Geometry> geom = r_.read(wkt);
        const LineString* line = dynamic_cast<LineString*>(geom.get());
        if (line)
            return line->getCoordinatesRO()->clone();
        else
            return nullptr;
    }

};


typedef test_group<test_PointLocation_data> group;
typedef group::object object;

group test_PointLocation_data("geos::algorithm::PointLocation");


//
// Test Cases
//

// testOnLineOnVertex
template<>
template<>
void object::test<1> ()
{
    checkOnLine(20, 20, "LINESTRING (0 00, 20 20, 30 30)", true);
}

// testOnLineInSegment
template<>
template<>
void object::test<2> ()
{
    checkOnLine(10, 10, "LINESTRING (0 0, 20 20, 0 40)", true);
    checkOnLine(10, 30, "LINESTRING (0 0, 20 20, 0 40)", true);
}

// testNotOnLine
template<>
template<>
void object::test<3> ()
{
    checkOnLine(0, 100, "LINESTRING (10 10, 20 10, 30 10)", false);
}

// testOnSegment
template<>
template<>
void object::test<4> ()
{
    checkOnSegment(5, 5, "LINESTRING(0 0, 9 9)", true);
    checkOnSegment(0, 0, "LINESTRING(0 0, 9 9)", true);
    checkOnSegment(9, 9, "LINESTRING(0 0, 9 9)", true);
}

// testNotOnSegment
template<>
template<>
void object::test<5> ()
{
    checkOnSegment(5, 6, "LINESTRING(0 0, 9 9)", false);
    checkOnSegment(10, 10, "LINESTRING(0 0, 9 9)", false);
    checkOnSegment(9, 9.00001, "LINESTRING(0 0, 9 9)", false);
}

// testOnZeroLengthSegment
template<>
template<>
void object::test<6> ()
{
    checkOnSegment(1, 1, "LINESTRING(1 1, 1 1)", true);
    checkOnSegment(1, 2, "LINESTRING(1 1, 1 1)", false);
}


} // namespace tut

