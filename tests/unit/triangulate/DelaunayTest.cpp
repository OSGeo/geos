
//
// Test Suite for geos::triangulate::quadedge::QuadEdge
//
// tut
#include <tut/tut.hpp>
// geos
#include <geos/algorithm/ConvexHull.h>
#include <geos/coverage/CoverageUnion.h>
#include <geos/triangulate/quadedge/QuadEdge.h>
#include <geos/triangulate/quadedge/QuadEdgeSubdivision.h>
#include <geos/triangulate/IncrementalDelaunayTriangulator.h>
#include <geos/triangulate/DelaunayTriangulationBuilder.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequence.h>

using namespace geos::triangulate;
using namespace geos::triangulate::quadedge;
using namespace geos::geom;
using namespace geos::io;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_incdelaunaytri_data {
    test_incdelaunaytri_data()
    {
    }
};

typedef test_group<test_incdelaunaytri_data> group;
typedef group::object object;

group test_incdelaunaytri_group("geos::triangulate::Delaunay");

//helper function for running triangulation
void
checkDelaunayHull(const char* sitesWkt)
{
    WKTReader reader;
    auto sites = reader.read(sitesWkt);

    DelaunayTriangulationBuilder builder;
    const GeometryFactory& geomFact(*GeometryFactory::getDefaultInstance());
    builder.setSites(*sites);
    std::unique_ptr<Geometry> tris = builder.getTriangles(geomFact);

    std::unique_ptr<Geometry> hullTris = geos::coverage::CoverageUnion::Union(tris.get());
    std::unique_ptr<Geometry> hullSites = sites->convexHull();

    //std::cout << "hullTris: " << hullTris->toString() << std::endl;
    //std::cout << "hullSites: " << hullSites->toString() << std::endl;

    //-- use topological equality, because there may be collinear vertices in the union
    ensure(hullTris->equals(hullSites.get()));
}

void
runDelaunay(const char* sitesWkt, bool computeTriangles, const char* expectedWkt, double tolerance = 0.0)
{
    WKTReader reader;
    std::unique_ptr<Geometry> results;
    auto sites = reader.read(sitesWkt);
    auto expected = reader.read(expectedWkt);
    DelaunayTriangulationBuilder builder;
    builder.setTolerance(tolerance);
    const GeometryFactory& geomFact(*GeometryFactory::getDefaultInstance());

    builder.setSites(*sites);
    if(computeTriangles) {
        results = builder.getTriangles(geomFact);
    }
    else {
        results = builder.getEdges(geomFact);
    }

    results->normalize();
    expected->normalize();

    ensure(results->toString(), results->equalsExact(expected.get(), 1e-7));
    ensure_equals(results->getCoordinateDimension(), expected->getCoordinateDimension());
}

//
// Test Cases
//

// 1 - Basic function test
template<>
template<>
void object::test<1>
()
{
    //Create a subdivision centered at (0,0)
    QuadEdgeSubdivision sub(Envelope(-100, 100, -100, 100), .00001);
    //make a triangulator to work on sub
    IncrementalDelaunayTriangulator triangulator(&sub);

    triangulator.insertSite(Vertex(0, 0));

    //extract the triangles from the subdivision
    const GeometryFactory& geomFact(*GeometryFactory::getDefaultInstance());
    std::unique_ptr<GeometryCollection> tris = sub.getTriangles(geomFact);
}

// 2 - Test Triangle
template<>
template<>
void object::test<2>
()
{
    const char* wkt = "MULTIPOINT ((10 10), (10 20), (20 20))";
    const char* expectedEdges = "MULTILINESTRING ((10 20, 20 20), (10 10, 10 20), (10 10, 20 20))";
    const char* expectedTri = "GEOMETRYCOLLECTION (POLYGON ((10 20, 10 10, 20 20, 10 20)))";

    runDelaunay(wkt, true, expectedTri);
    runDelaunay(wkt, false, expectedEdges);
}

// 3 - Test Random
template<>
template<>
void object::test<3>
()
{
    const char* wkt = "MULTIPOINT ((50 40), (140 70), (80 100), (130 140), (30 150), (70 180), (190 110), (120 20))";
    const char* expectedEdges =
        "MULTILINESTRING ((70 180, 190 110), (30 150, 70 180), (30 150, 50 40), (50 40, 120 20), (190 110, 120 20), (120 20, 140 70), (190 110, 140 70), (130 140, 140 70), (130 140, 190 110), (70 180, 130 140), (80 100, 130 140), (70 180, 80 100), (30 150, 80 100), (50 40, 80 100), (80 100, 120 20), (80 100, 140 70))";
    const char* expectedTri =
        "GEOMETRYCOLLECTION (POLYGON ((30 150, 50 40, 80 100, 30 150)), POLYGON ((30 150, 80 100, 70 180, 30 150)), POLYGON ((70 180, 80 100, 130 140, 70 180)), POLYGON ((70 180, 130 140, 190 110, 70 180)), POLYGON ((190 110, 130 140, 140 70, 190 110)), POLYGON ((190 110, 140 70, 120 20, 190 110)), POLYGON ((120 20, 140 70, 80 100, 120 20)), POLYGON ((120 20, 80 100, 50 40, 120 20)), POLYGON ((80 100, 140 70, 130 140, 80 100)))";

    runDelaunay(wkt, false, expectedEdges);
    runDelaunay(wkt, true, expectedTri);
}

// 4 - Test grid
template<>
template<>
void object::test<4>
()
{
    const char* wkt = "MULTIPOINT ((10 10), (10 20), (20 20), (20 10), (20 0), (10 0), (0 0), (0 10), (0 20))";
    const char* expectedEdges =
        "MULTILINESTRING ((10 20, 20 20), (0 20, 10 20), (0 10, 0 20), (0 0, 0 10), (0 0, 10 0), (10 0, 20 0), (20 0, 20 10), (20 10, 20 20), (10 20, 20 10), (10 10, 20 10), (10 10, 10 20), (10 10, 0 20), (10 10, 0 10), (10 0, 10 10), (0 10, 10 0), (10 10, 20 0))";
    const char* expectedTri =
        "GEOMETRYCOLLECTION (POLYGON ((0 20, 0 10, 10 10, 0 20)), POLYGON ((0 20, 10 10, 10 20, 0 20)), POLYGON ((10 20, 10 10, 20 10, 10 20)), POLYGON ((10 20, 20 10, 20 20, 10 20)), POLYGON ((10 0, 20 0, 10 10, 10 0)), POLYGON ((10 0, 10 10, 0 10, 10 0)), POLYGON ((10 0, 0 10, 0 0, 10 0)), POLYGON ((10 10, 20 0, 20 10, 10 10)))";

    runDelaunay(wkt, false, expectedEdges);
    runDelaunay(wkt, true, expectedTri);
}

// 5 - Test Circle
// Added a point inside to ensure single possible solution
template<>
template<>
void object::test<5>
()
{
    const char* wkt =
        "GEOMETRYCOLLECTION(POLYGON ((42 30, 41.96 29.61, 41.85 29.23, 41.66 28.89, 41.41 28.59, 41.11 28.34, 40.77 28.15, 40.39 28.04, 40 28, 39.61 28.04, 39.23 28.15, 38.89 28.34, 38.59 28.59, 38.34 28.89, 38.15 29.23, 38.04 29.61, 38 30, 38.04 30.39, 38.15 30.77, 38.34 31.11, 38.59 31.41, 38.89 31.66, 39.23 31.85, 39.61 31.96, 40 32, 40.39 31.96, 40.77 31.85, 41.11 31.66, 41.41 31.41, 41.66 31.11, 41.85 30.77, 41.96 30.39, 42 30)), POINT(38.6 30))";
    const char* expectedEdges =
        "MULTILINESTRING((41.96 30.39,42 30),(41.96 29.61,42 30),(41.85 30.77,41.96 30.39),(41.85 29.23,41.96 29.61),(41.66 31.11,41.85 30.77),(41.66 28.89,41.85 29.23),(41.41 31.41,41.66 31.11),(41.41 28.59,41.66 28.89),(41.11 31.66,41.41 31.41),(41.11 28.34,41.41 28.59),(40.77 31.85,41.11 31.66),(40.77 28.15,41.11 28.34),(40.39 31.96,40.77 31.85),(40.39 28.04,40.77 28.15),(40 32,40.39 31.96),(40 28,40.39 28.04),(39.61 31.96,40 32),(39.61 28.04,40 28),(39.23 31.85,39.61 31.96),(39.23 28.15,39.61 28.04),(38.89 31.66,39.23 31.85),(38.89 28.34,39.23 28.15),(38.6 30,42 30),(38.6 30,41.96 30.39),(38.6 30,41.96 29.61),(38.6 30,41.85 30.77),(38.6 30,41.85 29.23),(38.6 30,41.66 31.11),(38.6 30,41.66 28.89),(38.6 30,41.41 31.41),(38.6 30,41.41 28.59),(38.6 30,41.11 31.66),(38.6 30,41.11 28.34),(38.6 30,40.77 31.85),(38.6 30,40.77 28.15),(38.6 30,40.39 31.96),(38.6 30,40.39 28.04),(38.6 30,40 32),(38.6 30,40 28),(38.6 30,39.61 31.96),(38.6 30,39.61 28.04),(38.6 30,39.23 31.85),(38.6 30,39.23 28.15),(38.6 30,38.89 31.66),(38.6 30,38.89 28.34),(38.59 31.41,38.89 31.66),(38.59 31.41,38.6 30),(38.59 28.59,38.89 28.34),(38.59 28.59,38.6 30),(38.34 31.11,38.6 30),(38.34 31.11,38.59 31.41),(38.34 28.89,38.6 30),(38.34 28.89,38.59 28.59),(38.15 30.77,38.6 30),(38.15 30.77,38.34 31.11),(38.15 29.23,38.6 30),(38.15 29.23,38.34 28.89),(38.04 30.39,38.6 30),(38.04 30.39,38.15 30.77),(38.04 29.61,38.6 30),(38.04 29.61,38.15 29.23),(38 30,38.6 30),(38 30,38.04 30.39),(38 30,38.04 29.61))";

    runDelaunay(wkt, false, expectedEdges);
}

// 6 - Test Polygon With Chevron Holes
template<>
template<>
void object::test<6>
()
{
    const char* wkt =
        "POLYGON ((0 0, 0 200, 180 200, 180 0, 0 0), (20 180, 160 180, 160 20, 152.625 146.75, 20 180), (30 160, 150 30, 70 90, 30 160))";
    const char* expectedEdges =
        "MULTILINESTRING ((0 200, 180 200), (0 0, 0 200), (0 0, 180 0), (180 200, 180 0), (152.625 146.75, 180 0), (152.625 146.75, 180 200), (152.625 146.75, 160 180), (160 180, 180 200), (0 200, 160 180), (20 180, 160 180), (0 200, 20 180), (20 180, 30 160), (30 160, 0 200), (0 0, 30 160), (30 160, 70 90), (0 0, 70 90), (70 90, 150 30), (150 30, 0 0), (150 30, 160 20), (0 0, 160 20), (160 20, 180 0), (152.625 146.75, 160 20), (150 30, 152.625 146.75), (70 90, 152.625 146.75), (30 160, 152.625 146.75), (30 160, 160 180))";

    runDelaunay(wkt, false, expectedEdges);
}

// 7 - Test grid (2.5d)
template<>
template<>
void object::test<7>
()
{
    const char* wkt =
        "MULTIPOINT ((10 10 1), (10 20 2), (20 20 3), (20 10 1.5), (20 0 2.5), (10 0 3.5), (0 0 0), (0 10 .5), (0 20 .25))";
    const char* expectedEdges =
        "MULTILINESTRING ((10 20 2, 20 20 3), (0 20 .25, 10 20 2), (0 10 .5, 0 20 .25), (0 0 0, 0 10 .5), (0 0 0, 10 0 3.5), (10 0 3.5, 20 0 2.5), (20 0 2.5, 20 10 1.5), (20 10 1.5, 20 20 3), (10 20 2, 20 10 1.5), (10 10 1, 20 10 1.6), (10 10 1, 10 20 2), (10 10 1, 0 20 .25), (10 10 1, 0 10 .5), (10 0 3.5, 10 10 1), (0 10 .5, 10 0 3.5), (10 10 1, 20 0 2.5))";
    const char* expectedTri =
        "GEOMETRYCOLLECTION (POLYGON ((0 20 .25, 0 10 .5, 10 10 1, 0 20 .25)), POLYGON ((0 20 .25, 10 10 1, 10 20 2, 0 20 .25)), POLYGON ((10 20 2, 10 10 1, 20 10 1.5, 10 20 2)), POLYGON ((10 20 2, 20 10 1.5, 20 20 3, 10 20 2)), POLYGON ((10 0 3.5, 20 0 2.5, 10 10 1, 10 0 3.5)), POLYGON ((10 0 3.5, 10 10 1, 0 10 .5, 10 0 3.5)), POLYGON ((10 0 3.5, 0 10 .5, 0 0 0, 10 0 3.5)), POLYGON ((10 10 1, 20 0 2.5, 20 10 1.5, 10 10 1)))";

    runDelaunay(wkt, false, expectedEdges);
    runDelaunay(wkt, true, expectedTri);
}

// 8 - Tolerance robustness - http://trac.osgeo.org/geos/ticket/604
template<>
template<>
void object::test<8>
()
{
    const char* wkt = "MULTIPOINT(-118.3964065 56.0557,-118.396406 56.0475,-118.396407 56.04,-118.3968 56)";
    const char* expectedEdges =
        "MULTILINESTRING ((-118.3968 56, -118.3964065 56.0557), (-118.3968 56, -118.396407 56.04), (-118.396407 56.04, -118.396406 56.0475), (-118.3964065 56.0557, -118.396406 56.0475), (-118.3968 56, -118.396406 56.0475))";

    runDelaunay(wkt, false, expectedEdges, 0.001);
}

// 9 - Test for DelaunayTriangulationBuilder::envelope
template<>
template<>
void object::test<9>
()
{
    geos::geom::CoordinateSequence seq;
    seq.add(Coordinate(150, 200));
    seq.add(Coordinate(180, 270));
    seq.add(Coordinate(275, 163));

    Envelope env = DelaunayTriangulationBuilder::envelope(seq);

    ensure_equals(env.getWidth(), 125);
    ensure_equals(env.getHeight(), 107);
}

// 10 - Tolerance robustness
template<>
template<>
void object::test<10>
()
{
    const char* wkt =
        "MULTIPOINT(63.547558624186912368 70.904719023616522122,63.547558624186969212 70.904719023616564755,66.103648384371410884 68.588612471664760051,77.882918707497154287 74.870889977331813725,128.47759065022572145 177.65366864730182783)";
    const char* expectedEdges =
        "GEOMETRYCOLLECTION (POLYGON ((63.5475586241869692 70.9047190236165648, 128.4775906502257214 177.6536686473018278, 77.8829187074971543 74.8708899773318137, 63.5475586241869692 70.9047190236165648)), POLYGON ((63.5475586241869692 70.9047190236165648, 77.8829187074971543 74.8708899773318137, 66.1036483843714109 68.5886124716647601, 63.5475586241869692 70.9047190236165648)), POLYGON ((63.5475586241869124 70.9047190236165221, 128.4775906502257214 177.6536686473018278, 63.5475586241869692 70.9047190236165648, 63.5475586241869124 70.9047190236165221)), POLYGON ((63.5475586241869124 70.9047190236165221, 63.5475586241869692 70.9047190236165648, 66.1036483843714109 68.5886124716647601, 63.5475586241869124 70.9047190236165221)))";

    // inCircle predicate can't handle it on machines with 64-bit long double
    bool realLongDouble = sizeof(long double) > sizeof(double);
    if(realLongDouble) {
        runDelaunay(wkt, true, expectedEdges, 0.0);
    }
}

// 11 - Regular grid of points, tol = 0 (see https://trac.osgeo.org/geos/ticket/1035)
template<>
template<>
void object::test<11>
()
{
    const char* wkt =
"MULTIPOINT ((-10 40), (5 40), (20 40), (-10 55), (5 55), (20 55), (-10 70), (5 70), (20 70))";
    const char* expectedEdges =
        "GEOMETRYCOLLECTION (POLYGON ((5 70, 20 70, 20 55, 5 70)), POLYGON ((5 55, 20 55, 20 40, 5 55)), POLYGON ((5 55, 5 70, 20 55, 5 55)), POLYGON ((5 40, 5 55, 20 40, 5 40)), POLYGON ((-10 70, 5 70, 5 55, -10 70)), POLYGON ((-10 55, 5 55, 5 40, -10 55)), POLYGON ((-10 55, -10 70, 5 55, -10 55)), POLYGON ((-10 40, -10 55, 5 40, -10 40)))";
    bool realLongDouble = sizeof(long double) > sizeof(double);
    if(realLongDouble) {
        runDelaunay(wkt, true, expectedEdges, 0.0);
    }
}

// 12 - Regular grid of points, tol > 0 (see https://trac.osgeo.org/geos/ticket/1035)
template<>
template<>
void object::test<12>
()
{
    const char* wkt =
"MULTIPOINT ((-10 40), (5 40), (20 40), (-10 55), (5 55), (20 55), (-10 70), (5 70), (20 70))";
    const char* expectedEdges =
        "GEOMETRYCOLLECTION (POLYGON ((5 70, 20 70, 20 55, 5 70)), POLYGON ((5 55, 20 55, 20 40, 5 55)), POLYGON ((5 55, 5 70, 20 55, 5 55)), POLYGON ((5 40, 5 55, 20 40, 5 40)), POLYGON ((-10 70, 5 70, 5 55, -10 70)), POLYGON ((-10 55, 5 55, 5 40, -10 55)), POLYGON ((-10 55, -10 70, 5 55, -10 55)), POLYGON ((-10 40, -10 55, 5 40, -10 40)))";
    bool realLongDouble = sizeof(long double) > sizeof(double);
    if(realLongDouble) {
        runDelaunay(wkt, true, expectedEdges, 0.01);
    }
}

// failure case due to initial frame size too small
// see https://github.com/libgeos/geos/issues/719, https://github.com/locationtech/jts/pull/931
template<>
template<>
void object::test<13>
()
{
    const char* wkt =
"MULTIPOINT ((0 194), (66 151), (203 80), (273 43), (340 0))";
    const char* expected =
        "GEOMETRYCOLLECTION (POLYGON ((0 194, 66 151, 203 80, 0 194)), POLYGON ((0 194, 203 80, 273 43, 0 194)), POLYGON ((273 43, 203 80, 340 0, 273 43)), POLYGON ((340 0, 203 80, 66 151, 340 0)))";
    runDelaunay(wkt, true, expected);
}

// see https://github.com/libgeos/geos/issues/719
// testNarrow_GEOS_719()
template<>
template<>
void object::test<14>()
{
    const char* wkt = "MULTIPOINT ((1139294.6389832513 8201313.534695469), (1139360.8549531854 8201271.189805277), (1139497.5995843115 8201199.995542546), (1139567.7837303514 8201163.348533507), (1139635.3942210067 8201119.902527407))";
    const char* expected = "GEOMETRYCOLLECTION (POLYGON ((1139294.6389832513 8201313.534695469, 1139360.8549531854 8201271.189805277, 1139497.5995843115 8201199.995542546, 1139294.6389832513 8201313.534695469)), POLYGON ((1139294.6389832513 8201313.534695469, 1139497.5995843115 8201199.995542546, 1139567.7837303514 8201163.348533507, 1139294.6389832513 8201313.534695469)), POLYGON ((1139567.7837303514 8201163.348533507, 1139497.5995843115 8201199.995542546, 1139635.3942210067 8201119.902527407, 1139567.7837303514 8201163.348533507)), POLYGON ((1139635.3942210067 8201119.902527407, 1139497.5995843115 8201199.995542546, 1139360.8549531854 8201271.189805277, 1139635.3942210067 8201119.902527407)))";
    runDelaunay(wkt, true, expected);
}

// testNarrowTriangle()
template<>
template<>
void object::test<15>()
  {
    const char*  wkt = "MULTIPOINT ((100 200), (200 190), (300 200))";
    const char*  expected = "GEOMETRYCOLLECTION (POLYGON ((100 200, 300 200, 200 190, 100 200)))";
    runDelaunay(wkt, true, expected);
  }

// seee https://github.com/locationtech/jts/issues/477
// testNarrow_GH477_1()
template<>
template<>
void object::test<16>()
{
    const char*  wkt = "MULTIPOINT ((0 0), (1 0), (-1 0.05), (0 0))";
    const char*  expected = "GEOMETRYCOLLECTION (POLYGON ((-1 0.05, 1 0, 0 0, -1 0.05)))";
    runDelaunay(wkt, true, expected);
}

// see https://github.com/locationtech/jts/issues/477
// testNarrow_GH477_2()
template<>
template<>
void object::test<17>()
{
    const char*  wkt = "MULTIPOINT ((0 0), (0 486), (1 486), (1 22), (2 22), (2 0))";
    const char*  expected = "GEOMETRYCOLLECTION (POLYGON ((0 0, 0 486, 1 22, 0 0)), POLYGON ((0 0, 1 22, 2 0, 0 0)), POLYGON ((0 486, 1 486, 1 22, 0 486)), POLYGON ((1 22, 1 486, 2 22, 1 22)), POLYGON ((1 22, 2 22, 2 0, 1 22)))";
    runDelaunay(wkt, true, expected);
}

// see https://github.com/libgeos/geos/issues/946
// testNarrow_GEOS_946()
template<>
template<>
void object::test<18>()
{
    const char*  wkt = "MULTIPOINT ((113.56577197798602 22.80081530883069),(113.565723279387 22.800815316487014),(113.56571548761124 22.80081531771092),(113.56571548780202 22.800815317674463),(113.56577197817877 22.8008153088047),(113.56577197798602 22.80081530883069))";
    const char*  expected = "GEOMETRYCOLLECTION (POLYGON ((113.56571548761124 22.80081531771092, 113.565723279387 22.800815316487014, 113.56571548780202 22.800815317674463, 113.56571548761124 22.80081531771092)), POLYGON ((113.56571548780202 22.800815317674463, 113.565723279387 22.800815316487014, 113.56577197817877 22.8008153088047, 113.56571548780202 22.800815317674463)), POLYGON ((113.565723279387 22.800815316487014, 113.56577197798602 22.80081530883069, 113.56577197817877 22.8008153088047, 113.565723279387 22.800815316487014)))";
    runDelaunay(wkt, true, expected);
}

// see https://github.com/shapely/shapely/issues/1873
// testNarrow_Shapely_1873()
template<>
template<>
void object::test<19>()
{
    const char*  wkt = "MULTIPOINT ((584245.72096874 7549593.72686167), (584251.71398371 7549594.01629478), (584242.72446125 7549593.58214511), (584230.73978847 7549592.9760418), (584233.73581213 7549593.13045099), (584236.7318358 7549593.28486019), (584239.72795377 7549593.43742855), (584227.74314188 7549592.83423486))";
    const char*  expected = "GEOMETRYCOLLECTION (POLYGON ((584227.74314188 7549592.83423486, 584233.73581213 7549593.13045099, 584230.73978847 7549592.9760418, 584227.74314188 7549592.83423486)), POLYGON ((584227.74314188 7549592.83423486, 584236.7318358 7549593.28486019, 584233.73581213 7549593.13045099, 584227.74314188 7549592.83423486)), POLYGON ((584227.74314188 7549592.83423486, 584239.72795377 7549593.43742855, 584236.7318358 7549593.28486019, 584227.74314188 7549592.83423486)), POLYGON ((584230.73978847 7549592.9760418, 584233.73581213 7549593.13045099, 584245.72096874 7549593.72686167, 584230.73978847 7549592.9760418)), POLYGON ((584230.73978847 7549592.9760418, 584245.72096874 7549593.72686167, 584251.71398371 7549594.01629478, 584230.73978847 7549592.9760418)), POLYGON ((584233.73581213 7549593.13045099, 584236.7318358 7549593.28486019, 584242.72446125 7549593.58214511, 584233.73581213 7549593.13045099)), POLYGON ((584233.73581213 7549593.13045099, 584242.72446125 7549593.58214511, 584245.72096874 7549593.72686167, 584233.73581213 7549593.13045099)), POLYGON ((584236.7318358 7549593.28486019, 584239.72795377 7549593.43742855, 584242.72446125 7549593.58214511, 584236.7318358 7549593.28486019)))";
    runDelaunay(wkt, true, expected);
}

// testNarrowPoints()
template<>
template<>
void object::test<20>()
{
    const char*  wkt = "MULTIPOINT ((2 204), (3 66), (1 96), (0 236), (3 173), (2 114), (3 201), (0 46), (1 181))";
    checkDelaunayHull(wkt);
}

} // namespace tut
