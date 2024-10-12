//
// Test Suite for geos::algorithm::Length
// Ported from JTS junit/algorithm/LengthTest.java

#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/algorithm/PolygonNodeTopology.h>

// std
#include <string>
#include <memory>

using geos::algorithm::PolygonNodeTopology;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_polygonnodetopology_data {

    geos::io::WKTReader r_;

    void
    checkCrossing(const std::string& wktA, const std::string& wktB)
    {
        checkCrossing(wktA, wktB, true);
    }

    void
    checkNonCrossing(const std::string& wktA, const std::string& wktB)
    {
        checkCrossing(wktA, wktB, false);
    }

    void
    checkCrossing(const std::string& wktA, const std::string& wktB, bool isExpected)
    {
        std::unique_ptr<CoordinateSequence> a = readPts(wktA);
        std::unique_ptr<CoordinateSequence> b = readPts(wktB);
        // assert: a[1] = b[1]
        bool isCrossing = PolygonNodeTopology::isCrossing(
            &a->getAt(1), &a->getAt(0), &a->getAt(2), &b->getAt(0), &b->getAt(2));
        ensure(isCrossing == isExpected);
    }

    void
    checkInterior(const std::string& wktA, const std::string& wktB)
    {
        checkInteriorSegment(wktA, wktB, true);
    }

    void
    checkExterior(const std::string& wktA, const std::string& wktB)
    {
        checkInteriorSegment(wktA, wktB, false);
    }

    void
    checkInteriorSegment(const std::string& wktA, const std::string& wktB, bool isExpected)
    {
        std::unique_ptr<CoordinateSequence> a = readPts(wktA);
        std::unique_ptr<CoordinateSequence> b = readPts(wktB);
        // assert: a[1] = b[1]
        bool isInterior = PolygonNodeTopology::isInteriorSegment(
            &a->getAt(1), &a->getAt(0), &a->getAt(2), &b->getAt(1));
        ensure(isInterior == isExpected);
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


typedef test_group<test_polygonnodetopology_data> group;
typedef group::object object;

group test_polygonnodetopology_data("geos::algorithm::PolygonNodeTopology");


//
// Test Cases
//
template<>
template<>
void object::test<1> ()
{
    checkCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (1000 500, 1000 1000, 500 1500)");
}


//
// testNonCrossingQuadrant2
//
template<>
template<>
void object::test<2> ()
{
    checkNonCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (300 1200, 1000 1000, 500 1500)");
}

//
// testNonCrossingQuadrant4
//
template<>
template<>
void object::test<3> ()
{
    checkNonCrossing(
        "LINESTRING (500 1000, 1000 1000, 1000 1500)",
        "LINESTRING (1000 500, 1000 1000, 1500 1000)");
}

//
// testInteriorSegment
//
template<>
template<>
void object::test<4> ()
{
    checkInterior(
        "LINESTRING (5 9, 5 5, 9 5)",
        "LINESTRING (5 5, 0 0)");
}

//
// testExteriorSegment
//
template<>
template<>
void object::test<5> ()
{
    checkExterior(
        "LINESTRING (5 9, 5 5, 9 5)",
        "LINESTRING (5 5, 9 9)");
}

} // namespace tut

