//
// Test Suite for geos::operation::buffer::BufferOp class.

// tut
#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/operation/buffer/OffsetCurve.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>

// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//
using geos::operation::buffer::OffsetCurve;
using geos::operation::buffer::BufferParameters;

// Common data used by tests
struct test_offsetcurve_data {
    geos::io::WKTReader wktreader;

    test_offsetcurve_data() {};

    void checkOffsetCurve(const std::string& wkt, double distance,
        int quadSegs, int joinStyle, double mitreLimit,
        const std::string& wktExpected)
    {
        checkOffsetCurve(wkt, distance, quadSegs, joinStyle, mitreLimit, wktExpected, 0.05);
    }

    void checkOffsetCurve(const std::string& wkt, double distance,
        int quadSegs, int joinStyle, double mitreLimit,
        const std::string& wktExpected, double tolerance)
    {
        BufferParameters::JoinStyle js = static_cast<BufferParameters::JoinStyle>(joinStyle);
        std::unique_ptr<geos::geom::Geometry> geom = wktreader.read(wkt);
        std::unique_ptr<geos::geom::Geometry> result = OffsetCurve::getCurve(*geom, distance, quadSegs, js, mitreLimit);
        std::unique_ptr<geos::geom::Geometry> expected = wktreader.read(wktExpected);
        ensure_equals_geometry(result.get(), expected.get(), tolerance);
    }

    void checkOffsetCurve(const std::string& wkt, double distance, const std::string& wktExpected)
    {
        checkOffsetCurve(wkt, distance, wktExpected, 0.05);
    }

    void checkOffsetCurve(const std::string& wkt, double distance, const std::string& wktExpected, double tolerance)
    {
        std::unique_ptr<geos::geom::Geometry> geom = wktreader.read(wkt);
        std::unique_ptr<geos::geom::Geometry> result = OffsetCurve::getCurve(*geom, distance);
        std::unique_ptr<geos::geom::Geometry> expected = wktreader.read(wktExpected);

        // geos::io::WKTWriter wktwriter;
        // wktwriter.setRoundingPrecision(2);
        // std::cout << std::endl;
        // std::cout << "Expected: " << wktwriter.write(expected.get()) << std::endl;
        // std::cout << "  Result: " << wktwriter.write(result.get()) << std::endl;

        ensure_equals_geometry(result.get(), expected.get(), tolerance);
    }

    void
    checkOffsetCurveJoined(const std::string& wkt, double distance, const std::string& wktExpected)
    {
        std::unique_ptr<geos::geom::Geometry> geom = wktreader.read(wkt);
        std::unique_ptr<geos::geom::Geometry> result = OffsetCurve::getCurveJoined(*geom, distance);
        std::unique_ptr<geos::geom::Geometry> expected = wktreader.read(wktExpected);
        ensure_equals_geometry(result.get(), expected.get(), 0.05);
    }

};

typedef test_group<test_offsetcurve_data> group;
typedef group::object object;

group test_offsetcurve_group("geos::operation::buffer::OffsetCurve");


// testPoint
template<>
template<>
void object::test<1> ()
{
    checkOffsetCurve(
        "POINT (0 0)", 1,
        "LINESTRING EMPTY"
        );
}

// testEmpty
template<>
template<>
void object::test<2> ()
{
    checkOffsetCurve(
        "LINESTRING EMPTY", 1,
        "LINESTRING EMPTY"
        );
}

// testZeroLenLine
template<>
template<>
void object::test<3> ()
{
    checkOffsetCurve(
        "LINESTRING (1 1, 1 1)", 1,
        "LINESTRING EMPTY"
        );
}

// testZeroOffsetLine
template<>
template<>
void object::test<4> ()
{
    checkOffsetCurve(
        "LINESTRING (0 0, 1 0, 1 1)", 0,
        "LINESTRING (0 0, 1 0, 1 1)"
        );
}

// testZeroOffsetPolygon
template<>
template<>
void object::test<5> ()
{
    checkOffsetCurve(
        "POLYGON ((1 9, 9 1, 1 1, 1 9))", 0,
        "LINESTRING (1 9, 1 1, 9 1, 1 9)"
        );
}

/**
* testRepeatedPoint
* Test bug fix for removing repeated points in input for raw curve.
* See https://github.com/locationtech/jts/issues/957
*/
template<>
template<>
void object::test<6> ()
{
    checkOffsetCurve(
        "LINESTRING (4 9, 1 2, 7 5, 7 5, 4 9)", 1,
        "LINESTRING (4.24 7.02, 2.99 4.12, 5.48 5.36, 4.24 7.02)"
        );
}


// testSegment1Short
template<>
template<>
void object::test<7> ()
{
    checkOffsetCurve(
        "LINESTRING (2 2, 2 2.0000001)", 1,
        "LINESTRING (1 2, 1 2.0000001)",
        0.00000001
    );
}

// testSegment1
template<>
template<>
void object::test<8> ()
{
    checkOffsetCurve(
        "LINESTRING (0 0, 9 9)", 1,
        "LINESTRING (-0.71 0.71, 8.29 9.71)"
    );
}


// testSegment1Neg
template<>
template<>
void object::test<9> ()
{
    checkOffsetCurve(
        "LINESTRING (0 0, 9 9)", -1,
        "LINESTRING (0.71 -0.71, 9.71 8.29)"
    );
}

// testSegments2
template<>
template<>
void object::test<10> ()
{
    checkOffsetCurve(
        "LINESTRING (0 0, 9 9, 25 0)", 1,
        "LINESTRING (-0.707 0.707, 8.293 9.707, 8.435 9.825, 8.597 9.915, 8.773 9.974, 8.956 9.999, 9.141 9.99, 9.321 9.947, 9.49 9.872, 25.49 0.872)"
        );
}

// testSegments3
template<>
template<>
void object::test<11> ()
{
    checkOffsetCurve(
        "LINESTRING (0 0, 9 9, 25 0, 30 15)", 1,
        "LINESTRING (-0.71 0.71, 8.29 9.71, 8.44 9.83, 8.6 9.92, 8.77 9.97, 8.96 10, 9.14 9.99, 9.32 9.95, 9.49 9.87, 24.43 1.47, 29.05 15.32)"
        );
}

// testRightAngle
template<>
template<>
void object::test<12> ()
{
    checkOffsetCurve(
        "LINESTRING (2 8, 8 8, 8 1)", 1,
        "LINESTRING (2 9, 8 9, 8.2 8.98, 8.38 8.92, 8.56 8.83, 8.71 8.71, 8.83 8.56, 8.92 8.38, 8.98 8.2, 9 8, 9 1)"
        );
}

// testZigzagOneEndCurved4
template<>
template<>
void object::test<13> ()
{
    checkOffsetCurve(
        "LINESTRING (1 3, 6 3, 4 5, 9 5)", 1,
        "LINESTRING (1 4, 3.59 4, 3.29 4.29, 3.17 4.44, 3.08 4.62, 3.02 4.8, 3 5, 3.02 5.2, 3.08 5.38, 3.17 5.56, 3.29 5.71, 3.44 5.83, 3.62 5.92, 3.8 5.98, 4 6, 9 6)"
        );
}

// testZigzagOneEndCurved1
template<>
template<>
void object::test<14> ()
{
    checkOffsetCurve(
        "LINESTRING (1 3, 6 3, 4 5, 9 5)", 1,
        "LINESTRING (1 4, 3.59 4, 3.29 4.29, 3.17 4.44, 3.08 4.62, 3.02 4.8, 3 5, 3.02 5.2, 3.08 5.38, 3.17 5.56, 3.29 5.71, 3.44 5.83, 3.62 5.92, 3.8 5.98, 4 6, 9 6)"
        );
}


// testAsymmetricU
template<>
template<>
void object::test<15> ()
{
    checkOffsetCurve(
        "LINESTRING (1 1, 9 1, 9 2, 5 2)", 1,
        "LINESTRING (1 2, 4 2)"
        );

    checkOffsetCurve(
        "LINESTRING (1 1, 9 1, 9 2, 5 2)", -1,
        "LINESTRING (1 0, 9 0, 9.2 0.02, 9.38 0.08, 9.56 0.17, 9.71 0.29, 9.83 0.44, 9.92 0.62, 9.98 0.8, 10 1, 10 2, 9.98 2.2, 9.92 2.38, 9.83 2.56, 9.71 2.71, 9.56 2.83, 9.38 2.92, 9.2 2.98, 9 3, 5 3)"
        );
}

// testSymmetricU
template<>
template<>
void object::test<16> ()
{
    checkOffsetCurve(
        "LINESTRING (1 1, 9 1, 9 2, 1 2)", 1,
        "LINESTRING EMPTY"
        );

    checkOffsetCurve(
        "LINESTRING (1 1, 9 1, 9 2, 1 2)", -1,
        "LINESTRING (1 0, 9 0, 9.2 0.02, 9.38 0.08, 9.56 0.17, 9.71 0.29, 9.83 0.44, 9.92 0.62, 9.98 0.8, 10 1, 10 2, 9.98 2.2, 9.92 2.38, 9.83 2.56, 9.71 2.71, 9.56 2.83, 9.38 2.92, 9.2 2.98, 9 3, 1 3)"
        );
}

// testEmptyResult
template<>
template<>
void object::test<17> ()
{
    checkOffsetCurve(
        "LINESTRING (3 5, 5 7, 7 5)", -4,
        "LINESTRING EMPTY"
        );
}

// testSelfCross
template<>
template<>
void object::test<18> ()
{
    checkOffsetCurve(
        "LINESTRING (50 90, 50 10, 90 50, 10 50)", 10,
        "MULTILINESTRING ((60 90, 60 60), (60 40, 60 34.14, 65.85 40, 60 40), (40 40, 10 40))" );
}

// testSelfCrossNeg
template<>
template<>
void object::test<19> ()
{
    checkOffsetCurve(
        "LINESTRING (50 90, 50 10, 90 50, 10 50)", -10,
        "MULTILINESTRING ((40 90, 40 60, 10 60), (40 40, 40 10, 40.19 8.05, 40.76 6.17, 41.69 4.44, 42.93 2.93, 44.44 1.69, 46.17 0.76, 48.05 0.19, 50 0, 51.95 0.19, 53.83 0.76, 55.56 1.69, 57.07 2.93, 97.07 42.93, 98.31 44.44, 99.24 46.17, 99.81 48.05, 100 50, 99.81 51.95, 99.24 53.83, 98.31 55.56, 97.07 57.07, 95.56 58.31, 93.83 59.24, 91.95 59.81, 90 60, 60 60))" );
}

// testSelfCrossCWNeg
template<>
template<>
void object::test<20> ()
{
    checkOffsetCurve(
        "LINESTRING (0 70, 100 70, 40 0, 40 100)", -10,
        "MULTILINESTRING ((0 60, 30 60), (50 60, 50 27.03, 78.25 60, 50 60), (50 80, 50 100))" );
}

// testSelfCrossDartInside
template<>
template<>
void object::test<21> ()
{
    checkOffsetCurve(
        "LINESTRING (60 50, 10 80, 50 10, 90 80, 40 50)", 10,
        "MULTILINESTRING ((54.86 41.43, 50 44.34, 45.14 41.43), (43.9 40.83, 50 30.16, 56.1 40.83))" );
}


// testSelfCrossDartOutside
template<>
template<>
void object::test<22> ()
{
    checkOffsetCurve(
        "LINESTRING (60 50, 10 80, 50 10, 90 80, 40 50)", -10,
        "LINESTRING (50 67.66, 15.14 88.57, 13.32 89.43, 11.35 89.91, 9.33 89.98, 7.34 89.64, 5.46 88.91, 3.76 87.82, 2.32 86.4, 1.19 84.73, 0.42 82.86, 0.04 80.88, 0.07 78.86, 0.5 76.88, 1.32 75.04, 41.32 5.04, 42.42 3.48, 43.8 2.16, 45.4 1.12, 47.17 0.41, 49.05 0.05, 50.95 0.05, 52.83 0.41, 54.6 1.12, 56.2 2.16, 57.58 3.48, 58.68 5.04, 98.68 75.04, 99.5 76.88, 99.93 78.86, 99.96 80.88, 99.58 82.86, 98.81 84.73, 97.68 86.4, 96.24 87.82, 94.54 88.91, 92.66 89.64, 90.67 89.98, 88.65 89.91, 86.68 89.43, 84.86 88.57, 50 67.66)" );
}


// testSelfCrossDart2Inside
template<>
template<>
void object::test<23> ()
{
    checkOffsetCurve(
        "LINESTRING (64 45, 10 80, 50 10, 90 80, 35 45)", 10,
        "LINESTRING (55.00 38.91, 49.58 42.42, 44.74 39.34, 50 30.15, 55.00 38.91)" );
}

// testRing
template<>
template<>
void object::test<24> ()
{
    checkOffsetCurve(
        "LINESTRING (10 10, 50 90, 90 10, 10 10)", -10,
        "LINESTRING (26.18 20, 50 67.63, 73.81 20, 26.18 20)" );
}

// testClosedCurve
template<>
template<>
void object::test<25> ()
{
    checkOffsetCurve(
        "LINESTRING (70 80, 10 80, 50 10, 90 80, 40 80)", 10,
        "LINESTRING (70 70, 40 70, 27.23 70, 50 30.15, 72.76 70, 70 70)"
        );
}


// testOverlapTriangleInside
template<>
template<>
void object::test<26> ()
{
    checkOffsetCurve(
        "LINESTRING (70 80, 10 80, 50 10, 90 80, 40 80)", 10,
        "LINESTRING (70 70, 40 70, 27.23 70, 50 30.15, 72.76 70, 70 70)" );
}


// testOverlapTriangleOutside
template<>
template<>
void object::test<27> ()
{
    checkOffsetCurve(
        "LINESTRING (70 80, 10 80, 50 10, 90 80, 40 80)", -10,
        "LINESTRING (70 90, 40 90, 10 90, 8.11 89.82, 6.29 89.29, 4.6 88.42, 3.11 87.25, 1.87 85.82, 0.91 84.18, 0.29 82.39, 0.01 80.51, 0.1 78.61, 0.54 76.77, 1.32 75.04, 41.32 5.04, 42.42 3.48, 43.8 2.16, 45.4 1.12, 47.17 0.41, 49.05 0.05, 50.95 0.05, 52.83 0.41, 54.6 1.12, 56.2 2.16, 57.58 3.48, 58.68 5.04, 98.68 75.04, 99.46 76.77, 99.9 78.61, 99.99 80.51, 99.71 82.39, 99.09 84.18, 98.13 85.82, 96.89 87.25, 95.4 88.42, 93.71 89.29, 91.89 89.82, 90 90, 70 90)"
        );
}


// testMultiPoint
template<>
template<>
void object::test<28> ()
{
    checkOffsetCurve(
        "MULTIPOINT ((0 0), (1 1))", 1,
        "LINESTRING EMPTY"
    );
}


// testMultiLine
template<>
template<>
void object::test<29> ()
{
    checkOffsetCurve(
        "MULTILINESTRING ((20 30, 60 10, 80 60), (40 50, 80 30))", 10,
        "MULTILINESTRING ((24.47 38.94, 54.75 23.8, 70.72 63.71), (44.47 58.94, 84.47 38.94))"
    );
}


// testMixedWithPoint
template<>
template<>
void object::test<30> ()
{
    checkOffsetCurve(
        "GEOMETRYCOLLECTION (LINESTRING (20 30, 60 10, 80 60), POINT (0 0))", 10,
        "LINESTRING (24.47 38.94, 54.75 23.8, 70.72 63.71)"
    );
}


// testPolygon1
template<>
template<>
void object::test<31> ()
{
    checkOffsetCurve(
        "POLYGON ((100 200, 200 100, 100 100, 100 200))", 10,
        "LINESTRING (90 200, 90.19 201.95, 90.76 203.83, 91.69 205.56, 92.93 207.07, 94.44 208.31, 96.17 209.24, 98.05 209.81, 100 210, 101.95 209.81, 103.83 209.24, 105.56 208.31, 107.07 207.07, 207.07 107.07, 208.31 105.56, 209.24 103.83, 209.81 101.95, 210 100, 209.81 98.05, 209.24 96.17, 208.31 94.44, 207.07 92.93, 205.56 91.69, 203.83 90.76, 201.95 90.19, 200 90, 100 90, 98.05 90.19, 96.17 90.76, 94.44 91.69, 92.93 92.93, 91.69 94.44, 90.76 96.17, 90.19 98.05, 90 100, 90 200)"
    );
}

// testPolygon2
template<>
template<>
void object::test<32> ()
{
    checkOffsetCurve(
        "POLYGON ((100 200, 200 100, 100 100, 100 200))", -10,
        "LINESTRING (110 175.86, 175.86 110, 110 110, 110 175.86)"
    );
}


// testPolygonWithHole1
template<>
template<>
void object::test<33> ()
{
    checkOffsetCurve(
        "POLYGON ((20 80, 80 80, 80 20, 20 20, 20 80), (30 70, 70 70, 70 30, 30 30, 30 70))", 10,
        "MULTILINESTRING ((10 80, 10.19 81.95, 10.76 83.83, 11.69 85.56, 12.93 87.07, 14.44 88.31, 16.17 89.24, 18.05 89.81, 20 90, 80 90, 81.95 89.81, 83.83 89.24, 85.56 88.31, 87.07 87.07, 88.31 85.56, 89.24 83.83, 89.81 81.95, 90 80, 90 20, 89.81 18.05, 89.24 16.17, 88.31 14.44, 87.07 12.93, 85.56 11.69, 83.83 10.76, 81.95 10.19, 80 10, 20 10, 18.05 10.19, 16.17 10.76, 14.44 11.69, 12.93 12.93, 11.69 14.44, 10.76 16.17, 10.19 18.05, 10 20, 10 80), (40 60, 40 40, 60 40, 60 60, 40 60))"
    );
}

// testPolygonWithHole2
template<>
template<>
void object::test<34> ()
{
    checkOffsetCurve(
        "POLYGON ((20 80, 80 80, 80 20, 20 20, 20 80), (30 70, 70 70, 70 30, 30 30, 30 70))", -10,
        "LINESTRING EMPTY"
    );
}



//-------------------------------------------------

// testJoined1
template<>
template<>
void object::test<35> ()
{
    checkOffsetCurveJoined("LINESTRING (0 50, 100 50, 50 100, 50 0)", 10,
        "LINESTRING (0 60, 75.85 60, 60 75.85, 60 0)"
    );
}

// testJoined2
template<>
template<>
void object::test<36> ()
{
    checkOffsetCurveJoined("LINESTRING (0 50, 100 50, 50 100, 50 0)", -10,
        "LINESTRING (0 40, 100 40, 101.95 40.19, 103.83 40.76, 105.56 41.69, 107.07 42.93, 108.31 44.44, 109.24 46.17, 109.81 48.05, 110 50, 109.81 51.95, 109.24 53.83, 108.31 55.56, 107.07 57.07, 57.07 107.07, 55.56 108.31, 53.83 109.24, 51.95 109.81, 50 110, 48.05 109.81, 46.17 109.24, 44.44 108.31, 42.93 107.07, 41.69 105.56, 40.76 103.83, 40.19 101.95, 40 100, 40 0)"
    );
}


//-------------------------------------------------

// testInfiniteLoop
template<>
template<>
void object::test<37> ()
{
    checkOffsetCurve(
        "LINESTRING (21 101, -1 78, 12 43, 50 112, 73 -5, 19 2, 87 85, -7 38, 105 40)", 4,
        "MULTILINESTRING ((23.89 98.24, 3.62 77.04, 12.71 52.58, 46.50 113.93, 46.96 114.60, 47.55 115.16, 48.24 115.59, 49.00 115.87, 49.80 116.00, 50.62 115.95, 51.40 115.75, 52.13 115.38, 52.77 114.88, 53.30 114.26, 53.69 113.55, 53.92 112.77, 61.06 76.50), (62.66 68.36, 63.73 62.91, 72.03 73.05, 62.66 68.36), (65.73 52.73, 67.58 43.33, 104.93 44.00), (69.14 35.36, 76.92 -4.23, 77.00 -4.98, 76.93 -5.73, 76.72 -6.46, 76.38 -7.14, 75.92 -7.73, 75.35 -8.24, 74.70 -8.62, 73.99 -8.88, 73.24 -8.99, 72.49 -8.97, 18.49 -1.97, 17.73 -1.79, 17.03 -1.48, 16.39 -1.04, 15.86 -0.48, 15.44 0.17, 15.16 0.89, 15.02 1.65, 15.02 2.42, 15.18 3.18, 15.48 3.89, 15.91 4.53, 40.74 34.85), (47.40 42.97, 57.15 54.88), (55.23 64.64, 18.32 46.19), (16.24 42.42, 47.40 42.97), (57.89 43.16, 59.45 43.19))"
    );
}

//---------------------------------------

// testQuadSegs
template<>
template<>
void object::test<38> ()
{
    checkOffsetCurve(
        "LINESTRING (20 20, 50 50, 80 20)",
        10, 2, -1, -1,
        "LINESTRING (12.928932188134524 27.071067811865476, 42.928932188134524 57.071067811865476, 44.44429766980398 58.314696123025456, 46.1731656763491 59.23879532511287, 48.04909677983872 59.80785280403231, 50 60, 51.95090322016128 59.80785280403231, 53.8268343236509 59.23879532511287, 55.55570233019602 58.314696123025456, 57.071067811865476 57.071067811865476, 87.07106781186548 27.071067811865476)"
        );
}

// testJoinBevel
template<>
template<>
void object::test<39> ()
{
    checkOffsetCurve(
        "LINESTRING (20 20, 50 50, 80 20)",
        10, -1, BufferParameters::JOIN_BEVEL, -1,
        "LINESTRING (12.93 27.07, 42.93 57.07, 57.07 57.07, 87.07 27.07)"
        );
}

// testJoinMitre
template<>
template<>
void object::test<40> ()
{
    checkOffsetCurve(
        "LINESTRING (20 20, 50 50, 80 20)",
        10, -1, BufferParameters::JOIN_MITRE, -1,
        "LINESTRING (12.93 27.07, 50 64.14, 87.07 27.07)"
        );
}

// testMinQuadrantSegments
// See https://github.com/qgis/QGIS/issues/53165
template<>
template<>
void object::test<42> ()
{
    checkOffsetCurve(
        "LINESTRING (553772.0645892698 177770.05079236583, 553780.9235869241 177768.99614978794, 553781.8325485934 177768.41771963477)",
        -11, 0, BufferParameters::JOIN_MITRE, -1,
        "LINESTRING (553770.76 177759.13, 553777.54 177758.32)"
    );

    checkOffsetCurve(
        "LINESTRING (417.9785426266025752 432.5653800140134990, 447.6161954912822694 436.0936720217578113, 450.6571140550076962 438.0288020166335627)",
        133.819, // distance
        0, // number of segments
        BufferParameters::JOIN_MITRE,
        2,  // mitre limit
        "LINESTRING (402.15941192825255 565.4460778784833, 418.95730224139794 567.4458267253115)"
    );
}

// testMinQuadrantSegments_QGIS
// See https://github.com/qgis/QGIS/issues/53165#issuecomment-1563214857
template<>
template<>
void object::test<43> ()
{
    checkOffsetCurve(
        "LINESTRING (421 622, 446 625, 449 627)",
        133, 0, BufferParameters::JOIN_MITRE, -1,
        "LINESTRING (405.15 754.05, 416.3 755.39)"
    );
}



} // namespace tut2
