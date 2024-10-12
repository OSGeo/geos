//
// Ported from JTS junit/algorithm/RobustLineIntersectionTest.java r788

#include <tut/tut.hpp>
// geos
#include <geos/io/WKTReader.h>
#include <geos/algorithm/LineIntersector.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h> // required for use in unique_ptr
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
// std
#include <string>
#include <memory>

namespace geos {
namespace geom {
class Geometry;
}
}

using namespace geos::geom; //

namespace tut {
//
// Test Group
//

struct test_robustlineintersection_data {
    typedef std::unique_ptr<Geometry> GeomPtr;

    bool
    equals(const Coordinate& p0, const Coordinate& p1,
           double distanceTolerance)
    {
        return p0.distance(p1) <= distanceTolerance;
    }

    void
    checkIntPoints(const Coordinate& p, const Coordinate& q,
                   double distanceTolerance)
    {
        bool isEqual = equals(p, q, distanceTolerance);
        ensure("checkIntPoints: expected: " + p.toString() + " obtained " + q.toString(), isEqual);
    }

    /**
     *
     * @param pt array of 4 Coordinates
     * @param expectedIntersectionNum
     * @param intPt the expected intersection points
     *              (maybe null if not tested)
     *              must contain at least expectedIntersectionNum
     *              elements
     */
    void
    checkIntersection(const std::vector<Coordinate>& pt,
                      std::size_t expectedIntersectionNum,
                      const std::vector<Coordinate>& intPt,
                      double distanceTolerance)
    {
        geos::algorithm::LineIntersector li;
        li.computeIntersection(pt[0], pt[1], pt[2], pt[3]);

        auto intNum = li.getIntersectionNum();
        ensure_equals(intNum, expectedIntersectionNum);

        if(intPt.empty()) {
            return;
        }

        ensure_equals(intPt.size(), intNum);

        // test that both points are represented here
        //bool isIntPointsCorrect = true;
        if(intNum == 1) {
            checkIntPoints(intPt[0], li.getIntersection(0),
                           distanceTolerance);
        }
        else if(intNum == 2) {
            checkIntPoints(intPt[0], li.getIntersection(0),
                           distanceTolerance);
            checkIntPoints(intPt[1], li.getIntersection(0),
                           distanceTolerance);

            if(!(
                        equals(intPt[0], li.getIntersection(0), distanceTolerance)
                        ||
                        equals(intPt[0], li.getIntersection(1), distanceTolerance))) {
                checkIntPoints(intPt[0], li.getIntersection(0),
                               distanceTolerance);
                checkIntPoints(intPt[0], li.getIntersection(1),
                               distanceTolerance);
            }

            else if(!(
                        equals(intPt[1], li.getIntersection(0), distanceTolerance)
                        ||
                        equals(intPt[1], li.getIntersection(1), distanceTolerance))) {
                checkIntPoints(intPt[1], li.getIntersection(0),
                               distanceTolerance);
                checkIntPoints(intPt[1], li.getIntersection(1),
                               distanceTolerance);
            }
        }
        //assertTrue("Int Pts not equal", isIntPointsCorrect);
    }

    void
    checkIntersection(const std::string& wkt1,
                      const std::string& wkt2,
                      std::size_t expectedIntersectionNum,
                      const std::string& expectedWKT,
                      double distanceTolerance)
    //throws ParseException
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        LineString* l1ptr = dynamic_cast<LineString*>(g1.get());
        LineString* l2ptr = dynamic_cast<LineString*>(g2.get());

        ensure(nullptr != l1ptr);
        ensure(nullptr != l2ptr);

        LineString& l1 = *l1ptr;
        LineString& l2 = *l2ptr;

        std::vector<Coordinate> pt;
        pt.push_back(l1.getCoordinateN(0));
        pt.push_back(l1.getCoordinateN(1));
        pt.push_back(l2.getCoordinateN(0));
        pt.push_back(l2.getCoordinateN(1));

        GeomPtr g(reader.read(expectedWKT));

        std::unique_ptr<CoordinateSequence> cs(g->getCoordinates());

        std::vector<Coordinate> intPt;
        for(std::size_t i = 0; i < cs->size(); ++i) {
            intPt.push_back(cs->getAt(i));
        }

        checkIntersection(pt, expectedIntersectionNum,
                          intPt, distanceTolerance);
    }

    void
    checkIntersection(const std::string& wkt1,
                      const std::string& wkt2,
                      std::size_t expectedIntersectionNum,
                      const std::vector<Coordinate>& intPt,
                      double distanceTolerance)
    // throws ParseException
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        LineString* l1ptr = dynamic_cast<LineString*>(g1.get());
        LineString* l2ptr = dynamic_cast<LineString*>(g2.get());

        ensure(nullptr != l1ptr);
        ensure(nullptr != l2ptr);

        LineString& l1 = *l1ptr;
        LineString& l2 = *l2ptr;

        std::vector<Coordinate> pt;
        pt.push_back(l1.getCoordinateN(0));
        pt.push_back(l1.getCoordinateN(1));
        pt.push_back(l2.getCoordinateN(0));
        pt.push_back(l2.getCoordinateN(1));

        checkIntersection(pt, expectedIntersectionNum,
                          intPt, distanceTolerance);
    }

    void
    checkIntersectionNone(const std::string& wkt1,
                          const std::string& wkt2)
    // throws ParseException
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        LineString* l1ptr = dynamic_cast<LineString*>(g1.get());
        LineString* l2ptr = dynamic_cast<LineString*>(g2.get());

        ensure(nullptr != l1ptr);
        ensure(nullptr != l2ptr);

        LineString& l1 = *l1ptr;
        LineString& l2 = *l2ptr;

        std::vector<Coordinate> pt;
        pt.push_back(l1.getCoordinateN(0));
        pt.push_back(l1.getCoordinateN(1));
        pt.push_back(l2.getCoordinateN(0));
        pt.push_back(l2.getCoordinateN(1));

        std::vector<Coordinate> intPt;
        checkIntersection(pt, 0, intPt, 0);
    }

    void
    checkInputNotAltered(const std::string& wkt1, const std::string& wkt2, int scaleFactor)
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        LineString* l1ptr = dynamic_cast<LineString*>(g1.get());
        LineString* l2ptr = dynamic_cast<LineString*>(g2.get());

        ensure(nullptr != l1ptr);
        ensure(nullptr != l2ptr);

        LineString& l1 = *l1ptr;
        LineString& l2 = *l2ptr;

        std::vector<Coordinate> pt;
        pt.push_back(l1.getCoordinateN(0));
        pt.push_back(l1.getCoordinateN(1));
        pt.push_back(l2.getCoordinateN(0));
        pt.push_back(l2.getCoordinateN(1));
        checkInputNotAltered(pt, scaleFactor);
    }

    void
    checkInputNotAltered(const std::vector<Coordinate>& pt, int scaleFactor)
    {
        // save input points
        std::vector<Coordinate> savePt = pt;

        geos::algorithm::LineIntersector li;
        PrecisionModel lpm(scaleFactor);
        li.setPrecisionModel(&lpm);
        li.computeIntersection(pt[0], pt[1], pt[2], pt[3]);

        // check that input points are unchanged
        for(std::size_t i = 0; i < 4; i++) {
            ensure_equals(savePt[i], pt[i]);
        }
    }



    test_robustlineintersection_data()
        :
        pm(),
        gf(GeometryFactory::create(&pm)),
        reader(gf.get())
    {
    }

    PrecisionModel pm;
    GeometryFactory::Ptr gf;
    geos::io::WKTReader reader;

};

typedef test_group<test_robustlineintersection_data> group;
typedef group::object object;

group test_robustlineintersection_group(
    "geos::algorithm::RobustLineIntersection");


//
// Test Cases
//

// 1 - Test from strk which is bad in GEOS (2009-04-14).
template<>
template<>
void object::test<1>
()
{
    checkIntersection(
        "LINESTRING (588750.7429703881 4518950.493668233, 588748.2060409798 4518933.9452804085)",
        "LINESTRING (588745.824857241 4518940.742239175, 588748.2060437313 4518933.9452791475)",
        1,
        "POINT (588748.2060416829 4518933.945284994)",
        0);
}

// 2 - Test from strk which is bad in GEOS (2009-04-14).
template<>
template<>
void object::test<2>
()
{
    checkIntersection(
        "LINESTRING (588743.626135934 4518924.610969561, 588732.2822865889 4518925.4314047815)",
        "LINESTRING (588739.1191384895 4518927.235700594, 588731.7854614238 4518924.578370095)",
        1,
        "POINT (588733.8306132929 4518925.319423238)",
        0);
}

// 3 - DaveSkeaCase
//
// This used to be a failure case (exception),
// but apparently works now.
// Possibly normalization has fixed this?
//
template<>
template<>
void object::test<3>
()
{
    std::vector<Coordinate> intPt;
    intPt.push_back(Coordinate(2087600.4716727887, 1187639.7426241424));

    checkIntersection(
        "LINESTRING ( 2089426.5233462777 1180182.3877339689, 2085646.6891757075 1195618.7333999649 )",
        "LINESTRING ( 1889281.8148903656 1997547.0560044837, 2259977.3672235999 483675.17050843034 )",
        1,
        intPt, 0);
}

// 4 - Outside envelope using HCoordinate method (testCmp5CaseWKT)
template<>
template<>
void object::test<4>
()
{
    std::vector<Coordinate> intPt;
    intPt.push_back(Coordinate(4348440.8493873989, 5552599.2720221197));

    checkIntersection(
        "LINESTRING (4348433.262114629 5552595.478385733, 4348440.849387404 5552599.272022122 )",
        "LINESTRING (4348433.26211463  5552595.47838573,  4348440.8493874   5552599.27202212  )",
        1,
        intPt, 0);
}

// 5 - Result of this test should be the same as the WKT one!
//     (testCmp5CaseRaw)
template<>
template<>
void object::test<5>
()
{
    std::vector<Coordinate> pt;
    pt.push_back(Coordinate(4348433.262114629, 5552595.478385733));
    pt.push_back(Coordinate(4348440.849387404, 5552599.272022122));
    pt.push_back(Coordinate(4348433.26211463,  5552595.47838573));
    pt.push_back(Coordinate(4348440.8493874,   5552599.27202212));

    std::vector<Coordinate> intPt;
    intPt.push_back(Coordinate(4348440.8493873989, 5552599.2720221197));

    checkIntersection(pt, 1, intPt, 0);
}

/**
 * Test involving two non-almost-parallel lines.
 * Does not seem to cause problems with basic line intersection algorithm.
 *
 */
//     (testLeduc_1)
template<>
template<>
void object::test<6>
()
{
    checkIntersection(
        "LINESTRING (305690.0434123494 254176.46578338774, 305601.9999843455 254243.19999846347)",
        "LINESTRING (305689.6153764265 254177.33102743194, 305692.4999844298 254171.4999983967)",
        1,
        "POINT (305690.0434123494 254176.46578338774)",
        0);
}

/**
 * Test from Tomas Fa - JTS list 6/13/2012
 *
 * Fails using original JTS DeVillers determine orientation test.
 * Succeeds using DD and Shewchuk orientation
 *
 */
// testTomasFa_2
template<>
template<>
void object::test<7>
()
{
    checkIntersectionNone(
        "LINESTRING (-5.9 163.1, 76.1 250.7)",
        "LINESTRING (14.6 185.0, 96.6 272.6)");
}

/**
 * Test from Tomas Fa - JTS list 6/13/2012
 *
 * Fails using original JTS DeVillers determine orientation test.
 * Succeeds using DD and Shewchuk orientation
 *
 */
// testTomasFa_1
template<>
template<>
void object::test<8>
()
{
    checkIntersectionNone(
        "LINESTRING (-42.0 163.2, 21.2 265.2)",
        "LINESTRING (-26.2 188.7, 37.0 290.7)");
}


/**
 * Following cases were failures when using the CentralEndpointIntersector heuristic.
 * This is because one segment lies at a significant angle to the other,
 * with only one endpoint is close to the other segment.
 * The CE heuristic chose the wrong endpoint to return.
 * The fix is to use a new heuristic which out of the 4 endpoints
 * chooses the one which is closest to the other segment.
 * This works in all known failure cases.
 *
 */
// public void testCentralEndpointHeuristicFailure()
template<>
template<>
void object::test<9>
()
{
    checkIntersection(
        "LINESTRING (163.81867067 -211.31840378, 165.9174252 -214.1665075)",
        "LINESTRING (2.84139601 -57.95412726, 469.59990601 -502.63851732)",
        1,
        "POINT (163.81867067 -211.31840378)",
        0);
}

// public void testCentralEndpointHeuristicFailure2()
template<>
template<>
void object::test<10>
()
{
    checkIntersection(
        "LINESTRING (-58.00593335955 -1.43739086465, -513.86101637525 -457.29247388035)",
        "LINESTRING (-215.22279674875 -158.65425425385, -218.1208801283 -160.68343590235)",
        1,
        "POINT ( -215.22279674875 -158.65425425385 )",
        1.0e-10);
}

/**
 * Tests a case where intersection point is rounded,
 * and it is computed as a nearest endpoint.
 * Exposed a bug due to aliasing of endpoint.
 *
 * MD 8 Mar 2013
 *
 */
// testRoundedPointsNotAltered()
template<>
template<>
void object::test<11>
()
{
    checkInputNotAltered(
        "LINESTRING (-58.00593335955 -1.43739086465, -513.86101637525 -457.29247388035)",
        "LINESTRING (-215.22279674875 -158.65425425385, -218.1208801283 -160.68343590235)",
        100000);
}




} // namespace tut

