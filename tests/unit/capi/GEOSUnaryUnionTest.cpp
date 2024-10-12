//
// Test Suite for C-API GEOSUnaryUnion

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstring>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capiunaryunion_data : public capitest::utility {

    test_capiunaryunion_data() {
    }

};

typedef test_group<test_capiunaryunion_data> group;
typedef group::object object;

group test_capiunaryunion_group("capi::GEOSUnaryUnion");

//
// Test Cases
//


// Self-union an empty point
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POINT EMPTY");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("POINT EMPTY"));
}

// Self-union a 2d point
template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POINT (6 3)");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("POINT (6 3)"));
}

// Self-union a 3d point
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("POINT (4 5 6)");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("POINT Z (4 5 6)"));
}

// Self-union a multipoint with duplicated points
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOINT ((4 5), (6 7), (4 5), (6 5), (6 7))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("MULTIPOINT ((4 5), (6 5), (6 7))"));
}

// Self-union a collection of puntal and lineal geometries
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT(4 5), MULTIPOINT((6 7), (6 5), (6 7)), LINESTRING(0 5, 10 5), LINESTRING(4 -10, 4 10))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    GEOSGeometry* expected = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT (6 7), LINESTRING (4 -10, 4 5), LINESTRING (4 5, 4 10), LINESTRING (0 5, 4 5), LINESTRING (4 5, 10 5))");

    ensure(GEOSEquals(geom2_, expected) != 0);

    GEOSGeom_destroy(expected);
}

// Self-union a collection of puntal and polygonal geometries
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT(4 5), MULTIPOINT((6 7), (6 5), (6 7)), POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(5 6, 7 6, 7 8, 5 8, 5 6)))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    expected_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT (6 7), POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (5 6, 7 6, 7 8, 5 8, 5 6)))");
    ensure(nullptr != expected_);

    ensure(GEOSEquals(geom2_, expected_));
}

// Self-union a collection of lineal and polygonal geometries
template<>
template<>
void object::test<7>
()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (MULTILINESTRING((5 7, 12 7), (4 5, 6 5), (5.5 7.5, 6.5 7.5)), POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(5 6, 7 6, 7 8, 5 8, 5 6)))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    GEOSGeometry* expected = GEOSGeomFromWKT("GEOMETRYCOLLECTION (LINESTRING (5 7, 7 7), LINESTRING (10 7, 12 7), LINESTRING (5.5 7.5, 6.5 7.5), POLYGON ((10 7, 10 0, 0 0, 0 10, 10 10, 10 7), (5 6, 7 6, 7 7, 7 8, 5 8, 5 7, 5 6)))");
    GEOSNormalize(geom2_);
    GEOSNormalize(expected);

    ensure(GEOSEquals(geom2_, expected) != 0);
    GEOSGeom_destroy(expected);
}

// Self-union a collection of puntal, lineal and polygonal geometries
template<>
template<>
void object::test<8>
()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (MULTILINESTRING((5 7, 12 7), (4 5, 6 5), (5.5 7.5, 6.5 7.5)), POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(5 6, 7 6, 7 8, 5 8, 5 6)), MULTIPOINT((6 6.5), (6 1), (12 2), (6 1)))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    GEOSGeometry* expected = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT (6 6.5), POINT (12 2), LINESTRING (5 7, 7 7), LINESTRING (10 7, 12 7), LINESTRING (5.5 7.5, 6.5 7.5), POLYGON ((10 7, 10 0, 0 0, 0 10, 10 10, 10 7), (5 6, 7 6, 7 7, 7 8, 5 8, 5 7, 5 6)))");
    GEOSNormalize(geom2_);
    GEOSNormalize(expected);

    ensure(GEOSEquals(geom2_, expected));
    GEOSGeom_destroy(expected);
}

// Self-union a geometry with NaN coordinates
// See http://trac.osgeo.org/geos/ticket/530
template<>
template<>
void object::test<9>
()
{
    const char* hexwkb = "010200000002000000000000000000F8FF000000000000F8FF0000000000000000000000000000F03F";
    geom1_ = GEOSGeomFromHEX_buf((const unsigned char*)hexwkb, std::strlen(hexwkb));
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr == geom2_);

}

// Self-union an empty linestring
template<>
template<>
void object::test<10>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING EMPTY");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnion(geom1_);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("LINESTRING EMPTY"));
}

// https://github.com/libgeos/geos/issues/830
// Self-union on collection with an empty point
template<>
template<>
void object::test<11>
()
{
    input_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POINT EMPTY, LINESTRING (0 0, 1 1))");

    result_ = GEOSUnaryUnion(input_);
    expected_ = GEOSGeomFromWKT("LINESTRING (0 0, 1 1)");

    ensure_geometry_equals(result_, expected_);
}

template<>
template<>
void object::test<12>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");

    ensure(input_);

    result_ = GEOSUnaryUnion(input_);
    ensure("curved geometry not supported", result_ == nullptr);
}


} // namespace tut


