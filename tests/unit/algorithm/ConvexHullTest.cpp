//
// Test Suite for geos::algorithm::ConvexHull
// Ported from JTS junit/algorithm/ConvexHullTest.java

#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/algorithm/ConvexHull.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKTReader.h>
// std
#include <sstream>
#include <memory>

namespace geos {
namespace geom {
class Geometry;
}
}

using namespace geos::geom; // for Location

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_convexhull_data {
    //-- WKT reader with Floating precision
    geos::io::WKTReader rdr;

    test_convexhull_data()
    {
    }

    void
    checkHull(const std::string& wkt, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = rdr.read(wkt);
        std::unique_ptr<Geometry> actual = geom->convexHull();
        std::unique_ptr<Geometry> expected = rdr.read(wktExpected);
        // std::cout << *actual << std::endl;
        // std::cout << *expected << std::endl;
        ensure_equals_geometry(expected.get(), actual.get());
    }

};

typedef test_group<test_convexhull_data> group;
typedef group::object object;

group test_convexhull_group("geos::algorithm::ConvexHull");

//
// Test Cases
//

// 1 - Test convex hull of linestring
template<>
template<>
void object::test<1>
()
{
    checkHull("LINESTRING (30 220, 240 220, 240 220)",
        "LINESTRING (30 220, 240 220)");
}



// 2 - Test convex hull of multipoint
template<>
template<>
void object::test<2>
()
{
    checkHull("MULTIPOINT ((130 240), (130 240), (130 240), (570 240), (570 240), (570 240), (650 240))",
        "LINESTRING (130 240, 650 240)");
}

// 3 - Test convex hull of multipoint
template<>
template<>
void object::test<3>
()
{
    checkHull("MULTIPOINT ((0 0), (0 0), (10 0))",
            "LINESTRING (0 0, 10 0)");
}

// 4 - Test convex hull of multipoint
template<>
template<>
void object::test<4>
()
{
    checkHull("MULTIPOINT ((0 0), (10 0), (10 0))",
        "LINESTRING (0 0, 10 0)");
}

// 5 - Test convex hull of multipoint
template<>
template<>
void object::test<5>
()
{
    checkHull("MULTIPOINT ((0 0), (5 0), (10 0))",
        "LINESTRING (0 0, 10 0)");
}

// 6 - Test convex hull of multipoint exported to string form
template<>
template<>
void object::test<6>
()
{
    checkHull("MULTIPOINT ((0 0), (5 1), (10 0))",
        "POLYGON ((0 0, 5 1, 10 0, 0 0))");
}

// 7 - Test convex hull of multipoint
template<>
template<>
void object::test<7>
()
{
    checkHull("MULTIPOINT ((0 0), (0 0), (5 0), (5 0), (10 0), (10 0))",
        "LINESTRING (0 0, 10 0)");
}

template<>
template<>
void object::test<8>
()
{
    // Test case from https://trac.osgeo.org/geos/ticket/850
    geos::io::WKBReader r;
    std::stringstream wkb("01040000001100000001010000002bd3a24002bcb0417ff59d2051e25c4101010000003"
                          "aebcec70a8b3cbfdb123fe713a2e8be0101000000afa0bb8638b770bf7fc1d77d0dda1c"
                          "bf01010000009519cb944ce070bf1a46cd7df4201dbf010100000079444b4cd1937cbfa"
                          "6ca29ada6a928bf010100000083323f09e16c7cbfd36d07ee0b8828bf01010000009081"
                          "b8f066967ebf915fbc9ebe652abf0101000000134cf280633bc1bf37b754972dbe6dbf0"
                          "101000000ea992c094df585bf1bbabc8a42f332bf0101000000c0a13c7fb31186bf9af7"
                          "b10cc50b33bf0101000000a0bba15a0a7188bf8fba7870e91735bf01010000000fc8701"
                          "903db93bf93bdbe93b52241bf01010000007701a73b29cc90bfb770bc3732fe3cbf0101"
                          "00000036fa45b75b8b8cbf1cfca5bf59a238bf0101000000a54e773f7f287ebf910d462"
                          "1e5062abf01010000004b5b5dc4196f55bfa51f0579717f02bf01010000007e54948951"
                          "3a5fbfa57bacea34f30abf");
    Geometry::Ptr geom(r.readHEX(wkb));
    ensure(nullptr != geom);
    auto result = geom->convexHull();
    ensure(result != nullptr); // No crash!
}

// Test convex hull failure from https://github.com/libgeos/geos/issues/722
template<>
template<>
void object::test<9>
()
{
    checkHull(
        "MULTIPOINT ((-0.2 -0.1), (1.38777878e-17 -0.1), (0.2 -0.1), (-1.38777878e-17 -0.1), (-0.2 0.1), (1.38777878e-17 0.1), (0.2 0.1), (-1.38777878e-17 0.1))",
        "POLYGON ((-0.2 -0.1, -0.2 0.1, 0.2 0.1, 0.2 -0.1, -0.2 -0.1))"
    );
}


template<>
template<>
void object::test<10>
()
{
    checkHull("LINESTRING (30 220, 240 220, 240 220, 240 220, 240 220)",
        "LINESTRING (30 220, 240 220)");
}

template<>
template<>
void object::test<11>
()
{
    checkHull("MULTIPOINT ((0 0), (5 1), (10 0), (5 0))",
        "POLYGON ((0 0, 5 1, 10 0, 0 0))");
}


} // namespace tut
