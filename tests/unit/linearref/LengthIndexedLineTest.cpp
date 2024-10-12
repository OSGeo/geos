//
// Ported from JTS junit/linearref/AbstractIndexedLineTestCase.java r466
// and  junit/linearref/LengthIndexedLineTestCase.java r466

#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h> // required for use in unique_ptr
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
#include <geos/linearref/LengthIndexedLine.h>

// std
#include <cmath>
#include <string>
#include <memory>

using namespace geos::geom;
using namespace geos::linearref;
using namespace std;

/**
 * Tests the {@link LocationIndexedLine} class
 */
namespace tut {

typedef unique_ptr<Geometry> GeomPtr;
static const double TOLERANCE_DIST = 0.001;

struct test_lengthindexedline_data {
    test_lengthindexedline_data()
        : reader(), writer()
    {
    }

    geos::io::WKTReader reader;
    geos::io::WKTWriter writer;

    void
    checkExpected(Geometry* result, string const& expected)
    {
        GeomPtr subLine(reader.read(expected));
        checkExpected(result, subLine.get());
    }

    void
    checkExpected(Geometry* result, const Geometry* expected)
    {
        bool isEqual = result->equalsExact(expected, 1.0e-5);
        ensure_equals("Expect: " + writer.write(expected) + " Obtained: " + writer.write(result), isEqual, true);
    }

    void
    runIndicesOfThenExtract(string const& inputStr, string const& subLineStr)
    {
        GeomPtr input(reader.read(inputStr));
        GeomPtr subLine(reader.read(subLineStr));
        GeomPtr result(indicesOfThenExtract(input.get(), subLine.get()));

        checkExpected(result.get(), subLine.get());
    }

    bool
    indexOfAfterCheck(Geometry* linearGeom, Coordinate testPt)
    {
        LengthIndexedLine indexedLine(linearGeom);

        // check locations are consecutive
        double loc1 = indexedLine.indexOf(testPt);
        double loc2 = indexedLine.indexOfAfter(testPt, loc1);
        if(loc2 <= loc1) {
            return false;
        }

        // check extracted points are the same as the input
        Coordinate pt1 = indexedLine.extractPoint(loc1);
        Coordinate pt2 = indexedLine.extractPoint(loc2);
        if(! pt1.equals2D(testPt)) {
            return false;
        }
        if(! pt2.equals2D(testPt)) {
            return false;
        }

        return true;
    }

    void
    runIndexOfAfterTest(string const& inputStr, string const& testPtWKT)
    {
        GeomPtr input(reader.read(inputStr));
        GeomPtr testPoint(reader.read(testPtWKT));
        const Coordinate testPt(*testPoint->getCoordinate());
        bool resultOK = indexOfAfterCheck(input.get(), testPt);
        ensure(resultOK);
    }

    void
    runOffsetTest(string const& inputWKT, string const& testPtWKT,
                  double offsetDistance, string const& expectedPtWKT)
    {
        GeomPtr input(reader.read(inputWKT));
        GeomPtr testPoint(reader.read(testPtWKT));
        GeomPtr expectedPoint(reader.read(expectedPtWKT));
        const Coordinate testPt(*testPoint->getCoordinate());
        const Coordinate expectedPt(*expectedPoint->getCoordinate());
        Coordinate offsetPt = extractOffsetAt(input.get(), testPt, offsetDistance);

        bool isOk = offsetPt.distance(expectedPt) < TOLERANCE_DIST;
        if(! isOk) {
            cout << "Expected = " << *expectedPoint << "  Actual = " << offsetPt << endl;
        }
        ensure(isOk);
    }

    Coordinate
    extractOffsetAt(Geometry* linearGeom, Coordinate testPt, double offsetDistance)
    {
        LengthIndexedLine indexedLine(linearGeom);
        double index = indexedLine.indexOf(testPt);
        return indexedLine.extractPoint(index, offsetDistance);
    }

    void
    checkExtractLine(const char* wkt, double start, double end, const char* expected)
    {
        string wktstr(wkt);
        GeomPtr linearGeom(reader.read(wktstr));
        LengthIndexedLine indexedLine(linearGeom.get());
        GeomPtr result(indexedLine.extractLine(start, end));
        checkExpected(result.get(), expected);
    }


    std::unique_ptr<Geometry>
    indicesOfThenExtract(Geometry* linearGeom, Geometry* subLine)
    {
        LengthIndexedLine indexedLine(linearGeom);
        double* loc = indexedLine.indicesOf(subLine);
        auto result = indexedLine.extractLine(loc[0], loc[1]);
        delete [] loc;
        return result;
    }

}; // struct test_lengthindexedline_data

typedef test_group<test_lengthindexedline_data> group;
typedef group::object object;

group test_lengthindexedline_group("geos::linearref::LengthIndexedLine");

// testML
template<>
template<>
void object::test<1>
()
{
    runIndicesOfThenExtract("MULTILINESTRING ((0 0, 10 10), (20 20, 30 30))",
                            "MULTILINESTRING ((1 1, 10 10), (20 20, 25 25))");
}

// testPartOfSegmentNoVertex
template<>
template<>
void object::test<2>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 10 10, 20 20)",
                            "LINESTRING (1 1, 9 9)");
}

// testPartOfSegmentContainingVertex()
template<>
template<>
void object::test<3>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 10 10, 20 20)",
                            "LINESTRING (5 5, 10 10, 15 15)");
}

/**
 * Tests that duplicate coordinates are handled correctly.
 *
 * @throws Exception
 */
// testPartOfSegmentContainingDuplicateCoords
template<>
template<>
void object::test<4>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 10 10, 10 10, 20 20)",
                            "LINESTRING (5 5, 10 10, 10 10, 15 15)");
}

/**
 * Following tests check that correct portion of loop is identified.
 * This requires that the correct vertex for (0,0) is selected.
 */

// testLoopWithStartSubLine
template<>
template<>
void object::test<5>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                            "LINESTRING (0 0, 0 10, 10 10)");
}

// testLoopWithEndingSubLine()
template<>
template<>
void object::test<6>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                            "LINESTRING (10 10, 10 0, 0 0)");
}

// test a subline equal to the parent loop
// testLoopWithIdenticalSubLine()
template<>
template<>
void object::test<7>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                            "LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)");
}

// test a zero-length subline equal to the start point
// testZeroLenSubLineAtStart()
template<>
template<>
void object::test<8>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                            "LINESTRING (0 0, 0 0)");
}

// test a zero-length subline equal to a mid point
// testZeroLenSubLineAtMidVertex()
template<>
template<>
void object::test<9>
()
{
    runIndicesOfThenExtract("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                            "LINESTRING (10 10, 10 10)");
}

// testIndexOfAfterSquare()
template<>
template<>
void object::test<10>
()
{
    runIndexOfAfterTest("LINESTRING (0 0, 0 10, 10 10, 10 0, 0 0)",
                        "POINT (0 0)");
}

// testIndexOfAfterRibbon()
template<>
template<>
void object::test<11>
()
{
    runIndexOfAfterTest("LINESTRING (0 0, 0 60, 50 60, 50 20, -20 20)",
                        "POINT (0 20)");
}

// testOffsetStartPoint()
template<>
template<>
void object::test<12>
()
{
    runOffsetTest("LINESTRING (0 0, 10 10, 10 10, 20 20)", "POINT(0 0)", 1.0,
                  "POINT (-0.7071067811865475 0.7071067811865475)");
    runOffsetTest("LINESTRING (0 0, 10 10, 10 10, 20 20)", "POINT(0 0)", -1.0,
                  "POINT (0.7071067811865475 -0.7071067811865475)");
    runOffsetTest("LINESTRING (0 0, 10 10, 10 10, 20 20)", "POINT(10 10)", 5.0,
                  "POINT (6.464466094067262 13.535533905932738)");
    runOffsetTest("LINESTRING (0 0, 10 10, 10 10, 20 20)", "POINT(10 10)", -5.0,
                  "POINT (13.535533905932738 6.464466094067262)");
}

// testExtractLineBothIndicesAtEndpointXXX()
template<>
template<>
void object::test<13>
()
{
    checkExtractLine(
        "MULTILINESTRING ((0 0, 10 0), (20 0, 25 0, 30 0))",
        -10, 10, "LINESTRING (10 0, 10 0)"
    );
}


// testExtractLineBeyondRange()
template<> template<>
void object::test<14>
()
{
    checkExtractLine("LINESTRING (0 0, 10 10)", -100, 100, "LINESTRING (0 0, 10 10)");
}

// testExtractLineReverse()
template<>
template<>
void object::test<15>
()
{
    checkExtractLine("LINESTRING (0 0, 10 0)", 9, 1, "LINESTRING (9 0, 1 0)");
}

// testExtractLineReverseMulti()
template<>
template<>
void object::test<16>
()
{
    checkExtractLine("MULTILINESTRING ((0 0, 10 0), (20 0, 25 0, 30 0))",
                     19, 1, "MULTILINESTRING ((10 0, 1 0), (29 0, 25 0, 20 0))");
}

// testExtractLineNegative()
template<>
template<>
void object::test<17>
()
{
    checkExtractLine("LINESTRING (0 0, 10 0)", -9, -1, "LINESTRING (1 0, 9 0)");
}

// testExtractLineNegativeReverse()
template<>
template<>
void object::test<18>
()
{
    checkExtractLine("LINESTRING (0 0, 10 0)", -1, -9, "LINESTRING (9 0, 1 0)");
}

// testExtractLineIndexAtEndpoint()
template<>
template<>
void object::test<19>
()
{
    checkExtractLine("MULTILINESTRING ((0 0, 10 0), (20 0, 25 0, 30 0))",
                     10, -1, "LINESTRING (20 0, 25 0, 29 0)");
}

// testExtractLineBothIndicesAtEndpoint()
template<>
template<>
void object::test<20>
()
{
    checkExtractLine("MULTILINESTRING ((0 0, 10 0), (20 0, 25 0, 30 0))",
                     10, 10, "LINESTRING (10 0, 10 0)");
}

// testExtractLineBothIndicesAtEndpointNegative()
template<>
template<>
void object::test<21>
()
{
    checkExtractLine("MULTILINESTRING ((0 0, 10 0), (20 0, 25 0, 30 0))",
                     -10, 10, "LINESTRING (10 0, 10 0)");
}

// testExtractPointBeyondRange()
template<>
template<>
void object::test<22>
()
{
    GeomPtr linearGeom(reader.read("LINESTRING (0 0, 10 10)"));
    LengthIndexedLine indexedLine(linearGeom.get());
    Coordinate pt = indexedLine.extractPoint(100);
    ensure(pt == Coordinate(10, 10));

    Coordinate pt2 = indexedLine.extractPoint(0);
    ensure(pt2 == Coordinate(0, 0));
}

// testProjectPointWithDuplicateCoords()
template<>
template<>
void object::test<23>
()
{
    GeomPtr linearGeom(reader.read("LINESTRING (0 0, 10 0, 10 0, 20 0)"));
    LengthIndexedLine indexedLine(linearGeom.get());
    double projIndex = indexedLine.project(Coordinate(10, 1));
    ensure(projIndex == 10.0);
}

/**
 * Tests that z values are interpolated
 *
 */
// testComputeZ()
template<>
template<>
void object::test<24>
()
{
    GeomPtr linearGeom(reader.read("LINESTRING (0 0 0, 10 10 10)"));
    LengthIndexedLine indexedLine(linearGeom.get());
    double projIndex = indexedLine.project(Coordinate(5, 5));
    Coordinate projPt = indexedLine.extractPoint(projIndex);
    //    System.out.println(projPt);
    ensure(projPt.equals3D(Coordinate(5, 5, 5)));
}

/**
 * Tests that if the input does not have Z ordinates, neither does the output.
 *
 */
// testComputeZNaN()
template<>
template<>
void object::test<25>
()
{

    GeomPtr linearGeom(reader.read("LINESTRING (0 0, 10 10)"));
    LengthIndexedLine indexedLine(linearGeom.get());
    double projIndex = indexedLine.project(Coordinate(5, 5));
    Coordinate projPt = indexedLine.extractPoint(projIndex);
    ensure(0 != std::isnan(projPt.z));
}

/**
 * From GEOS Ticket #323
 */
// testProjectExtractPoint()
template<>
template<>
void object::test<26>
()
{
    GeomPtr linearGeom(reader.read("MULTILINESTRING ((0 2, 0 0), (-1 1, 1 1))"));
    LengthIndexedLine indexedLine(linearGeom.get());
    double index = indexedLine.project(Coordinate(1, 0));
    Coordinate pt = indexedLine.extractPoint(index);
    ensure_equals(pt, Coordinate(0, 0));
}

/**
 * Tests that leading and trailing zero-length sublines are trimmed in
 * the computed result, and that zero-length extracts return the lowest
 * extracted zero-length line
 */
// testExtractLineIndexAtEndpointWithZeroLenComponents()
template<> template<>
void object::test<27>
()
{
    checkExtractLine(
        "MULTILINESTRING ((0 0, 10 0), (10 0, 10 0), (20 0, 25 0, 30 0))",
        10, -1, "LINESTRING (20 0, 25 0, 29 0)");

    checkExtractLine(
        "MULTILINESTRING ((0 0, 10 0), (10 0, 10 0), (20 0, 25 0, 30 0))",
        5, 10, "LINESTRING (5 0, 10 0)");

    checkExtractLine(
        "MULTILINESTRING ((0 0,10 0),(10 0,10 0),(10 0,10 0),(20 0,25 0,30 0))",
        10, 10, "LINESTRING (10 0, 10 0)");

    checkExtractLine(
        "MULTILINESTRING((0 0,10 0),(10 0,10 0),(10 0,10 0),(10 0,10 0),(20 0,25 0,30 0))",
        10, -10, "LINESTRING (10 0, 10 0)");
}

#if 0
template<>
template<>
void object::test<28>()
{
    GeomPtr linearGeom(reader.read(
        "MULTILINESTRING ((0 -2, 0 2),(-2 0, 2 0))"
        ));
    LengthIndexedLine indexedLine(linearGeom.get());

    double projIndex = indexedLine.project(Coordinate(2, 1.9));
    ensure_equals(projIndex, 8);
    Coordinate projPt = indexedLine.extractPoint(projIndex);
    ensure_equals(projPt, Coordinate(2, 0));

    projIndex = indexedLine.project(Coordinate(2, 2.1));
    ensure_equals(projIndex, 4);
    projPt = indexedLine.extractPoint(projIndex);
    ensure_equals(projPt, Coordinate(0, 2));
}
#endif

template<>
template<>
void object::test<29>()
{
    GeomPtr linearGeom(reader.read("LINESTRING EMPTY"));
    LengthIndexedLine indexedLine(linearGeom.get());
    Coordinate pt = indexedLine.extractPoint(100);
    ensure(pt.isNull());
}

// testExtractLineIndexAtEndpointOfTouchingLines()
template<>
template<>
void object::test<30>
()
{
    runIndicesOfThenExtract("MULTILINESTRING((0 0, 0 50), (0 50, 0 100))",
                            "LINESTRING (0 50, 0 60)");
}

} // namespace tut
