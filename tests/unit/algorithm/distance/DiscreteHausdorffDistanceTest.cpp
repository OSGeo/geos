//
// Test Suite for geos::algorithm::distance::DiscreteHausdorffDistance
// Ported from JTS junit/algorithm/distance/DiscreteHausdorffDistanceTest.java rev. 1.2


#include <tut/tut.hpp>
// geos
#include <geos/constants.h>
#include <geos/io/WKTReader.h>
#include <geos/algorithm/distance/DiscreteHausdorffDistance.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h> // required for use in unique_ptr
#include <geos/geom/Coordinate.h>
// std
#include <cmath>
#include <string>
#include <memory>
#include <cfenv>

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
struct test_DiscreteHausdorffDistance_data {

    typedef std::unique_ptr<Geometry> GeomPtr;

    test_DiscreteHausdorffDistance_data()
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

        double distance = DiscreteHausdorffDistance::distance(*g1, *g2);
        double diff = std::fabs(distance - expectedDistance);
        // std::cerr << "expectedDistance:" << expectedDistance << " actual distance:" << distance << std::endl;
        ensure(diff <= TOLERANCE);
    }

    void
    runTest(const std::string& wkt1, const std::string& wkt2,
            double densifyFactor, double expectedDistance)
    {
        GeomPtr g1(reader.read(wkt1));
        GeomPtr g2(reader.read(wkt2));

        double distance = DiscreteHausdorffDistance::distance(*g1,
                          *g2, densifyFactor);
        double diff = std::fabs(distance - expectedDistance);
        // std::cerr << "expectedDistance:" << expectedDistance << " actual distance:" << distance << std::endl;
        ensure(diff <= TOLERANCE);
    }

    PrecisionModel pm;
    GeometryFactory::Ptr gf;
    geos::io::WKTReader reader;

};
const double test_DiscreteHausdorffDistance_data::TOLERANCE = 0.00001;

typedef test_group<test_DiscreteHausdorffDistance_data> group;
typedef group::object object;

group test_DiscreteHausdorffDistance_group("geos::algorithm::distance::DiscreteHausdorffDistance");



//
// Test Cases
//

// 1 - testLineSegments
template<>
template<>
void object::test<1>
()
{
    runTest("LINESTRING (0 0, 2 1)",
            "LINESTRING (0 0, 2 0)", 1.0);

    // zero densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)",
                "LINESTRING EMPTY", 0.0, 0);
    }
    catch(const geos::util::IllegalArgumentException& ) {
        // We do expect an exception
    }

    // too big densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)",
                "LINESTRING EMPTY", 1 + 1e-10, 0);
    }
    catch(const geos::util::IllegalArgumentException& ) {
        // We do expect an exception
    }

    // too small positive densify factor
    try {
        runTest("LINESTRING (0 0, 2 1)",
                "LINESTRING EMPTY", 1e-30, 0);
    }
    catch(const geos::util::IllegalArgumentException& ) {
        // We do expect an exception
    }
}

// 2 - testLineSegments2
template<>
template<>
void object::test<2>
()
{
    runTest("LINESTRING (0 0, 2 0)",
            "LINESTRING (0 1, 1 2, 2 1)", 2.0);
}

// 3 - testLinePoints
template<>
template<>
void object::test<3>
()
{
    runTest("LINESTRING (0 0, 2 0)",
            "MULTIPOINT ((0 1), (1 0), (2 1))", 1.0);
}

// 4 - testLinesShowingDiscretenessEffect
//
// Shows effects of limiting HD to vertices
// Answer is not true Hausdorff distance.
//
template<>
template<>
void object::test<4>
()
{
    runTest("LINESTRING (130 0, 0 0, 0 150)",
            "LINESTRING (10 10, 10 150, 130 10)", 14.142135623730951);
    // densifying provides accurate HD
    runTest("LINESTRING (130 0, 0 0, 0 150)",
            "LINESTRING (10 10, 10 150, 130 10)", 0.5, 70.0);
}


// https://github.com/libgeos/geos/issues/569
//
// Segfault in Hausdorff distance with empty geometries
//
template<>
template<>
void object::test<5>
()
{
    GeomPtr g1(reader.read("POINT (1 1)"));
    GeomPtr g2(reader.read("POINT EMPTY"));
    double distance = DiscreteHausdorffDistance::distance(*g1, *g2);
    ensure(std::isnan(distance));
}


// https://github.com/libgeos/geos/issues/515
//
// Avoid FE_INVALID floating point errors being raised
//
template<>
template<>
void object::test<6>
()
{
    std::feclearexcept(FE_ALL_EXCEPT);

    runTest(
        "LINESTRING (0 0, 100 0, 10 100, 10 100)",
        "LINESTRING (0 100, 0 10, 80 10)", 0.001, 47.89);

    ensure("FE_INVALID raised", !std::fetestexcept(FE_INVALID));
}

// Crash on collection with empty components
// https://github.com/libgeos/geos/issues/840
template<>
template<>
void object::test<7>
()
{
    auto g1 = reader.read("GEOMETRYCOLLECTION (POINT EMPTY, LINESTRING (0 0, 1 1))");
    auto g2 = reader.read("POINT (1 2)");
    auto g3 = reader.read("LINESTRING (0 0, 1 1)");

    ensure_equals(DiscreteHausdorffDistance::distance(*g1, *g2),
                  DiscreteHausdorffDistance::distance(*g2, *g3));
}

// see https://github.com/libgeos/geos/issues/987
template<>
template<>
void object::test<8>
()
{
    runTest("LINEARRING (0 0, 0 10, 10 10, 10 0, 0 0)",
            "LINEARRING (1 1, 1 9, 8 8, 9 1, 1 1)", 
            2.8284271247461903);
}

} // namespace tut

