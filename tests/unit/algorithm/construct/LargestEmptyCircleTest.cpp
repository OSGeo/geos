//
// Test Suite for geos::algorithm::construct::LargestEmptyCircle

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/construct/LargestEmptyCircle.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
#include <geos/constants.h>
// std
#include <string>
#include <memory>

using namespace geos::geom;
using geos::algorithm::construct::LargestEmptyCircle;

namespace tut {
//
// Test Group
//

struct test_data_LargestEmptyCircle {
    PrecisionModel pm_;
    GeometryFactory::Ptr factory_;
    geos::io::WKTReader reader_;

    test_data_LargestEmptyCircle():
        pm_(PrecisionModel::FLOATING),
        factory_(GeometryFactory::create(&pm_, 0)),
        reader_(factory_.get())
    {}

    ~test_data_LargestEmptyCircle()
    {
    }

    void
    ensure_equals_coordinate(const Coordinate &lhs,
                             const Coordinate &rhs, double tolerance)
    {
        ensure_equals("x coordinate does not match", lhs.x, rhs.x, tolerance);
        ensure_equals("y coordinate does not match", lhs.y, rhs.y, tolerance);
    }

    /**
     * A coarse distance check, mainly testing
     * that there is not a huge number of iterations.
     * (This will be revealed by CI taking a very long time!)
     */
    void
    checkCircle(std::string wktObstacles, double tolerance)
    {
        std::unique_ptr<Geometry> geom(reader_.read(wktObstacles));
        LargestEmptyCircle lec(geom.get(), tolerance);
        std::unique_ptr<Point> centerPoint = lec.getCenter();
        double dist = geom->distance(centerPoint.get());
        //std::cout << dist << std::endl;
        std::unique_ptr<LineString> radiusLine = lec.getRadiusLine();
        double actualRadius = radiusLine->getLength();
        ensure(std::abs(actualRadius - dist) < 2 * tolerance);
    }

    void
    checkCircle(const Geometry *obstacles, const Geometry *boundary, double build_tolerance, double x, double y, double expectedRadius)
    {
        double tolerance = 2*build_tolerance;
        LargestEmptyCircle lec(obstacles, boundary, tolerance);
        std::unique_ptr<Point> centerPoint = lec.getCenter();
        Coordinate centerPt(*centerPoint->getCoordinate());
        Coordinate expectedCenter(x, y);
        ensure_equals_coordinate(centerPt, expectedCenter, tolerance);
        std::unique_ptr<LineString> radiusLine = lec.getRadiusLine();
        double actualRadius = radiusLine->getLength();
        ensure_equals("radius", actualRadius, expectedRadius, tolerance);
        const Coordinate& linePt0 = radiusLine->getCoordinateN(0);
        const Coordinate& linePt1 = radiusLine->getCoordinateN(1);

        // std::cout << std::endl;
        // std::cout << writer_.write(geom) << std::endl;
        // std::cout << writer_.write(radiusLine.get()) << std::endl;

        ensure_equals_coordinate(centerPt, linePt0, tolerance);
        Coordinate radiusPt(*lec.getRadiusPoint()->getCoordinate());
        ensure_equals_coordinate(radiusPt, linePt1, tolerance);
    }

    void
    checkCircleZeroRadius(const Geometry *geom, double tolerance)
    {
        LargestEmptyCircle lec(geom, tolerance);
        std::unique_ptr<Point> centerPoint = lec.getCenter();
        Coordinate centerPt(*centerPoint->getCoordinate());
        std::unique_ptr<LineString> radiusLine = lec.getRadiusLine();
        double actualRadius = radiusLine->getLength();
        ensure_equals("radius", actualRadius, 0.0, tolerance);

        const Coordinate& linePt0 = radiusLine->getCoordinateN(0);
        const Coordinate& linePt1 = radiusLine->getCoordinateN(1);

        ensure_equals_coordinate(centerPt, linePt0, tolerance);
        Coordinate radiusPt(*lec.getRadiusPoint()->getCoordinate());
        ensure_equals_coordinate(radiusPt, linePt1, tolerance);
    }

    void
    checkCircleZeroRadius(std::string wkt, double tolerance)
    {
        std::unique_ptr<Geometry> geom(reader_.read(wkt));
        checkCircleZeroRadius(geom.get(), tolerance);
    }

    void
    checkCircle(std::string wkt, double tolerance, double x, double y, double expectedRadius)
    {
        std::unique_ptr<Geometry> geom(reader_.read(wkt));
        checkCircle(geom.get(), nullptr, tolerance, x, y, expectedRadius);
    }

    void
    checkCircle(std::string wktObstacles, std::string wktBoundary, double tolerance, double x, double y, double expectedRadius)
    {
        std::unique_ptr<Geometry> obstacles(reader_.read(wktObstacles));
        std::unique_ptr<Geometry> boundary(reader_.read(wktBoundary));
        checkCircle(obstacles.get(), boundary.get(), tolerance, x, y, expectedRadius);
    }

};

typedef test_group<test_data_LargestEmptyCircle> group;
typedef group::object object;

group test_group_LargestEmptyCircle("geos::algorithm::construct::LargestEmptyCircle");

// testPointsSquare
template<>  
template<>
void object::test<1>()
{
    checkCircle("MULTIPOINT ((100 100), (100 200), (200 200), (200 100))",
       0.01, 150, 150, 70.71 );
}

// testPointsTriangleOnHull
template<>
template<>
void object::test<2>()
{
    checkCircle("MULTIPOINT ((100 100), (300 100), (150 50))",
       0.01, 216.66, 99.99, 83.33 );
}

// testPointsTriangleInterior
template<>
template<>
void object::test<3>()
{
 checkCircle("MULTIPOINT ((100 100), (300 100), (200 250))",
       0.01, 200.00, 141.66, 108.33 );
}

// testLinesOpenDiamond
template<>
template<>
void object::test<4>()
{

    checkCircle("MULTILINESTRING ((50 100, 150 50), (250 50, 350 100), (350 150, 250 200), (50 150, 150 200))",
       0.01, 200, 125, 90.13 );
}

//    testLinesCrossed
template<>
template<>
void object::test<5>()
{
  checkCircle("MULTILINESTRING ((100 100, 300 300), (100 200, 300 0))",
       0.01, 299.99, 150.00, 106.05 );
}

// testLinesZigzag
template<>
template<>
void object::test<6>()
{
    checkCircle("MULTILINESTRING ((100 100, 200 150, 100 200, 250 250, 100 300, 300 350, 100 400), (70 380, 0 350, 50 300, 0 250, 50 200, 0 150, 50 120))",
       0.01, 77.52, 249.99, 54.81 );
}

// testPointsLinesTriangle
template<>
template<>
void object::test<7>()
{
checkCircle("GEOMETRYCOLLECTION (LINESTRING (100 100, 300 100), POINT (250 200))",
       0.01, 196.49, 164.31, 64.31 );
}

// testPointsLinesTriangle
template<>
template<>
void object::test<8>()
{
checkCircleZeroRadius("POINT (100 100)",
       0.01 );
}

// testLineFlat
template<>
template<>
void object::test<9>()
{
 checkCircleZeroRadius("LINESTRING (0 0, 50 50)",
       0.01 );
}

// testThinExtent
template<>
template<>
void object::test<10>()
{
    checkCircle("MULTIPOINT ((100 100), (300 100), (200 100.1))",
       0.01 );
}

//------------ Polygon Obstacles -----------------

// testPolygonConcave
template<>
template<>
void object::test<11>()
{
    checkCircle("POLYGON ((1 9, 9 6, 6 5, 5 3, 8 3, 9 4, 9 1, 1 1, 1 9))", 
        0.01, 7.495, 4.216, 1.21);
} 

// testPolygonsBoxes
template<>
template<>
void object::test<12>()
{
    checkCircle("MULTIPOLYGON (((1 6, 6 6, 6 1, 1 1, 1 6)), ((6 7, 4 7, 4 9, 6 9, 6 7)))", 
        0.01, 2.50, 7.50, 1.50);
} 

// testPolygonLines
template<>
template<>
void object::test<13>()
{
    checkCircle("GEOMETRYCOLLECTION (POLYGON ((1 6, 6 6, 6 1, 1 1, 1 6)), LINESTRING (6 7, 3 9), LINESTRING (1 7, 3 8))", 
        0.01, 3.74, 7.14, 1.14);
} 

//----------  Obstacles and Boundary  --------------

// testBoundaryEmpty
template<>
template<>
void object::test<14>()
{
 checkCircle("MULTIPOINT ((2 2), (8 8), (7 5))",
        "POLYGON EMPTY",
        0.01, 4.127, 4.127, 3 );
}

// testBoundarySquare
template<>
template<>
void object::test<15>()
{
    checkCircle("MULTIPOINT ((2 2), (6 4), (8 8))",
        "POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))",
        0.01, 1.00390625, 8.99609375, 7.065 );
}

//testBoundarySquareObstaclesOutside
template<>
template<>
void object::test<16>()
{
    checkCircle("MULTIPOINT ((10 10), (10 0))",
        "POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))",
        0.01, 1.0044, 4.997, 10.29 );
}

// testBoundaryMultiSquares
template<>
template<>
void object::test<17>()
{
    checkCircle("MULTIPOINT ((10 10), (10 0), (5 5))",
        "MULTIPOLYGON (((1 9, 9 9, 9 1, 1 1, 1 9)), ((15 20, 20 20, 20 15, 15 15, 15 20)))",
        0.01, 19.995, 19.997, 14.137 );
}

// testBoundaryAsObstacle
template<>
template<>
void object::test<18>()
{
    checkCircle("GEOMETRYCOLLECTION (LINESTRING (1 9, 9 9, 9 1, 1 1, 1 9), POINT (4 3), POINT (7 6))",
        "POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))",
        0.01, 4, 6, 3 );
}

// testObstacleEmptyElement
template<>
template<>
void object::test<19>()
{
    checkCircle("GEOMETRYCOLLECTION (LINESTRING EMPTY, POINT (4 3), POINT (7 6), POINT (4 6))", 
        0.01, 5.5, 4.5, 2.12 );
}


// https://trac.osgeo.org/postgis/ticket/5626
template<>
template<>
void object::test<20>()
{
    checkCircle(
        "POINT(-11 40)", // input
        "POLYGON((-71.1 42.3,-71.1 42.4,-71.2 42.3,-71.1 42.3))", // boundary
        0.1, // test tolerance
        -71.15, 42.34, 60.19588 ); // expected x, y, radius
}


// https://trac.osgeo.org/postgis/ticket/5626
template<>
template<>
void object::test<21>()
{
    checkCircle(
        "POINT(-11.1111111 40)", // input
        "POLYGON((-71.0821 42.3036,-71.0821 42.3936,-71.0901 42.3036,-71.0821 42.3036))", // boundary
        1, // test tolerance
        -71.15, 42.34, 60.19588 ); // expected x, y, radius
}

} // namespace tut
