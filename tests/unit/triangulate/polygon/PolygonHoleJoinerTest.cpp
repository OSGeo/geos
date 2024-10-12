//
// Test Suite for geos::triangulate::polygon::PolygonHoleJoiner
//
// tut
#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
// #include <geos/io/WKTWriter.h>
#include <geos/triangulate/polygon/PolygonHoleJoiner.h>
#include <geos/util.h>

using geos::triangulate::polygon::PolygonHoleJoiner;
using geos::geom::CoordinateSequence;
using geos::geom::Geometry;

namespace tut {
//
// Test Group
//

struct test_polygonholejoiner_data {

    geos::io::WKTReader r;

    // test_polygonholejoiner_data() {}

    void checkJoin(const std::string& wkt, const std::string& wktExpected) {
        std::unique_ptr<Geometry> geom = r.read(wkt);
        Polygon* polyPtr = dynamic_cast<Polygon*>(geom.get());
        ensure(polyPtr);
        std::unique_ptr<Polygon> actual = PolygonHoleJoiner::joinAsPolygon(polyPtr);
        std::unique_ptr<Geometry> expected = r.read(wktExpected);
        Geometry* actualPtr = dynamic_cast<Geometry*>(actual.get());
        ensure(actualPtr);

        // std::cout << std::endl << "geom" << std::endl << *geom << std::endl;
        // std::cout << std::endl << "actual" << std::endl << *actual << std::endl;
        // std::cout << std::endl << "expected" << std::endl << *expected << std::endl;

        ensure_equals_geometry(expected.get(), actualPtr);
    }
};

typedef test_group<test_polygonholejoiner_data> group;
typedef group::object object;

group test_polygonholejoiner_group("geos::triangulate::polygon::PolygonHoleJoiner");

/**
* A failing case for hole joining with two holes that touch at leftmost vertex
* Fails due to PolygonHoleJoiner joining holes with same leftmost vertex
* so that the result linework self-crosses.
* Note that input is normalized.
*/
/* testTouchingHoles */
template<>
template<>
void object::test<1>()
{
    checkJoin(
        "POLYGON ((0 0, 0 9, 9 9, 9 0, 0 0), (1 4, 5 1, 5 4, 1 4), (1 4, 5 5, 6 8, 1 4))",
        "POLYGON ((0 0, 0 9, 1 4, 5 1, 5 4, 1 4, 5 5, 6 8, 1 4, 0 9, 9 9, 9 0, 0 0))"
    );
}

/* testHolesTouchEdgeVertical */
template<>
template<>
void object::test<2>()
{
    checkJoin(
        "POLYGON ((1 9, 9 9, 9 0, 1 0, 1 9), (1 4, 5 1, 5 4, 1 4), (1 5, 5 5, 6 8, 1 5))",
        "POLYGON ((1 9, 9 9, 9 0, 1 0, 1 4, 5 1, 5 4, 1 4, 1 5, 5 5, 6 8, 1 5, 1 9))"
    );
}

/* testHoleTouchesEdgeVertical */
template<>
template<>
void object::test<3>()
{
    checkJoin(
        "POLYGON ((1 9, 9 9, 9 0, 1 0, 1 9), (1 5, 5 5, 6 8, 1 5))",
        "POLYGON ((1 0, 1 5, 5 5, 6 8, 1 5, 1 9, 9 9, 9 0, 1 0))"
    );
}

/**
* Hole ring is CW, to test ring orientation handling.
*/
/* testHoleTouchesEdgeWithCloserVertex */
template<>
template<>
void object::test<4>()
{
    checkJoin(
        "POLYGON ((1 9, 9 9, 5 6, 9 6, 19 1, 9 1, 1 9), (8 5, 9 3, 5 5, 8 5))",
        "POLYGON ((1 9, 9 9, 5 6, 9 6, 19 1, 9 1, 5 5, 9 3, 8 5, 5 5, 1 9))"
    );
}

/* testHoleTouchesEdge */
template<>
template<>
void object::test<5>()
{
    checkJoin(
        "POLYGON ((5 5, 9 5, 9 0, 0 0, 5 5), (3 3, 6 1, 5 3, 3 3))",
        "POLYGON ((5 5, 9 5, 9 0, 0 0, 3 3, 6 1, 5 3, 3 3, 5 5))"
    );
}

/* testHoleTouchesVertex */
template<>
template<>
void object::test<6>()
{
    checkJoin(
        "POLYGON ((70 10, 0 0, 7 47, 20 90, 70 60, 70 10), (10 40, 30 50, 7 47, 10 40))",
        "POLYGON ((70 10, 0 0, 7 47, 10 40, 30 50, 7 47, 20 90, 70 60, 70 10))"
    );
}

/* testHolesManyVertical */
template<>
template<>
void object::test<7>()
{
    checkJoin(
        "POLYGON ((10 90, 90 90, 90 10, 10 10, 50 20, 10 30, 50 50, 10 70, 53 80, 10 90), (60 70, 60 60, 50 60, 50 70, 60 70), (55 55, 55 50, 50 50, 50 55, 55 55), (61 45, 61 40, 50 40, 50 45, 61 45), (60 30, 50 30, 50 35, 60 35, 60 30), (50 15, 50 18, 60 18, 60 15, 50 15), (60 85, 50 85, 55 80, 50 75, 60 75, 60 85))",
        "POLYGON ((10 10, 50 15, 60 15, 60 18, 50 18, 50 15, 10 10, 50 20, 50 30, 60 30, 60 35, 50 35, 50 40, 61 40, 61 45, 50 45, 50 40, 50 35, 50 30, 50 20, 10 30, 50 50, 55 50, 55 55, 50 55, 50 60, 60 60, 60 70, 50 70, 50 75, 60 75, 60 85, 50 85, 55 80, 50 75, 50 70, 50 60, 50 55, 50 50, 10 70, 53 80, 10 90, 90 90, 90 10, 10 10))"
    );
}

/**
* A failing case revealing that joining a hole by a zero-length cut
* was introducing duplicate vertices.
*/
/* testZeroLenCutDuplicateVertices */
template<>
template<>
void object::test<8>()
{
    checkJoin(
        "POLYGON ((70 10, 0 0, 7 47, 16 94, 70 60, 70 10), (10 40, 30 49, 7 47, 10 40), (13 59, 13 54, 26 53, 13 59))",
        "POLYGON ((0 0, 7 47, 10 40, 30 49, 7 47, 13 54, 26 53, 13 59, 13 54, 7 47, 16 94, 70 60, 70 10, 0 0))"
    );
}

/**
* Revealed a bug due to original use of tolerance for coordinate comparison (now removed)
*/
/* testBugHoleJoinCrosses */
template<>
template<>
void object::test<9>()
{
    checkJoin(
        "POLYGON ((27.182 58.2714, 27.182 58.27, 27.1804 58.27, 27.1804 58.2714, 27.182 58.2714), (27.18097 58.2701, 27.18091 58.27009, 27.18086 58.27007, 27.18086 58.27004, 27.18097 58.2701), (27.18079 58.27099, 27.18074 58.27029, 27.18075 58.27024, 27.18079 58.27019, 27.18151 58.27021, 27.18085 58.27104, 27.18079 58.27099))",
        "POLYGON ((27.1804 58.27, 27.1804 58.2714, 27.18074 58.27029, 27.18075 58.27024, 27.18079 58.27019, 27.18086 58.27004, 27.18097 58.2701, 27.18091 58.27009, 27.18086 58.27007, 27.18086 58.27004, 27.18079 58.27019, 27.18151 58.27021, 27.18085 58.27104, 27.18079 58.27099, 27.18074 58.27029, 27.1804 58.2714, 27.182 58.2714, 27.182 58.27, 27.1804 58.27))"
    );
}



} // namespace tut
