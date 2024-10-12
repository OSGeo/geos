//
// Test Suite for geos::algorithm::distance::DiscreteFrechetDistance
//

#include <tut/tut.hpp>
// geos
#include <geos/constants.h>
#include <geos/io/WKTReader.h>
#include <geos/algorithm/distance/DiscreteFrechetDistance.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h> // required for use in unique_ptr
#include <geos/geom/Coordinate.h>
#include <geos/util.h>
// std
#include <cmath>
#include <string>
#include <memory>

namespace geos {
namespace geom {
class Geometry;
}
}

using namespace geos::geom;
using namespace geos::algorithm::distance; // for Location

namespace tut {
//
// Test Group
//

// Test data, not used
struct test_DiscreteFrechetDistance_data {

    typedef std::unique_ptr<Geometry> GeomPtr;

    test_DiscreteFrechetDistance_data()
        :
        pm(),
        gf(GeometryFactory::create(&pm)),
        reader(gf.get())
    {}

    static const double TOLERANCE;

    void
    runTest(const std::string& wkt1, const std::string& wkt2,
            double expectedDistance)
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        double distance = DiscreteFrechetDistance::distance(*g1, *g2);
        double diff = std::fabs(distance - expectedDistance);
        //std::cerr << "expectedDistance:" << expectedDistance << " actual distance:" << distance << std::endl;
        ensure(diff <= TOLERANCE);
    }

    void
    runTest(const std::string& wkt1, const std::string& wkt2,
            double densifyFactor, double expectedDistance)
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        double distance = DiscreteFrechetDistance::distance(*g1,
                          *g2, densifyFactor);
        double diff = std::fabs(distance - expectedDistance);
        //std::cerr << "expectedDistance:" << expectedDistance << " actual distance:" << distance << std::endl;
        ensure(diff <= TOLERANCE);
    }

    PrecisionModel pm;
    GeometryFactory::Ptr gf;
    geos::io::WKTReader reader;

};
const double test_DiscreteFrechetDistance_data::TOLERANCE = 0.00001;

typedef test_group<test_DiscreteFrechetDistance_data> group;
typedef group::object object;

group test_DiscreteFrechetDistance_group("geos::algorithm::distance::DiscreteFrechetDistance");



//
// Test Cases
//

// 1 - testLineSegments
template<>
template<>
void object::test<1>
()
{
    runTest("LINESTRING (0 0, 2 1)", "LINESTRING (0 0, 2 0)", 1.0);

    // zero densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)", "LINESTRING EMPTY", 0.0, 0);
    }
    catch(const geos::util::IllegalArgumentException& e) {
        // We do expect an exception
        ::geos::ignore_unused_variable_warning(e);
    }

    // too big densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)", "LINESTRING EMPTY", 1 + 1e-10, 0);
    }
    catch(const geos::util::IllegalArgumentException& e) {
        // We do expect an exception
        ::geos::ignore_unused_variable_warning(e);
    }

    // too small positive densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)", "LINESTRING EMPTY", 1e-30, 0);
    }
    catch(const geos::util::IllegalArgumentException& e) {
        // We do expect an exception
        ::geos::ignore_unused_variable_warning(e);
    }
}

// 2 - testLineSegments2
template<>
template<>
void object::test<2>
()
{
    runTest("LINESTRING (0 0, 2 0)", "LINESTRING (0 1, 1 2, 2 1)", 2.23606797749979);
}

// 3 - testLinePoints
template<>
template<>
void object::test<3>
()
{
    runTest("LINESTRING (0 0, 2 0)", "MULTIPOINT ((0 1), (1 0), (2 1))", 1.0);
}

// 4 - testLinesShowingDiscretenessEffect
//
// Shows effects of limiting FD to vertices
// Answer is not true Frechet distance.
//
template<>
template<>
void object::test<4>
()
{
    runTest("LINESTRING (0 0, 100 0)", "LINESTRING (0 0, 50 50, 100 0)", 70.7106781186548);
// densifying provides accurate HD
    runTest("LINESTRING (0 0, 100 0)", "LINESTRING (0 0, 50 50, 100 0)", 0.5, 50.0);
}

// 5 - test Line Segments revealing distance initialization bug
template<>
template<>
void object::test<5>
()
{
    runTest("LINESTRING (1 1, 2 2)", "LINESTRING (1 4, 2 3)", 3);
}

template<>
template<>
void object::test<6>
()
{
    auto g1 = reader.read("LINESTRING EMPTY");
    auto g2 = reader.read("POLYGON EMPTY");

    try {
        DiscreteFrechetDistance::distance(*g1, *g2);
    } catch (const geos::util::GEOSException& e) {
        ::geos::ignore_unused_variable_warning(e);
    }
}

} // namespace tut
