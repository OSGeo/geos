// Test Suite for C-API LineString functions

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capilinestring_data : public capitest::utility {};

typedef test_group<test_capilinestring_data> group;
typedef group::object object;

group test_capilinestring_group("capi::GEOSLineString");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 5 5, 10 10)");
    GEOSGeometry* geom2;
    double x, y, z;
    ensure(nullptr != geom1_);

    char const r1 = GEOSisClosed(geom1_);

    ensure_equals(r1, 0);

    geom2 = GEOSGeomGetPointN(geom1_, 0);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 0);
    ensure_equals(y, 0);
    ensure(std::isnan(z));

    GEOSGeom_destroy(geom2);

    ensure(GEOSGeomGetPointN(geom1_, -1) == nullptr);

    geom2 = GEOSGeomGetStartPoint(geom1_);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 0);
    ensure_equals(y, 0);
    ensure(std::isnan(z));

    GEOSGeom_destroy(geom2);

    geom2 = GEOSGeomGetEndPoint(geom1_);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 10);
    ensure_equals(y, 10);
    ensure(std::isnan(z));

    GEOSGeom_destroy(geom2);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 5 5, 10 10)");
    double length;
    ensure(nullptr != geom1_);

    GEOSGeomGetLength(geom1_, &length);
    ensure(length != 0.0);
}

// Check error on invalid input type
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("POINT (0 0)");

    ensure(GEOSGeomGetStartPoint(geom1_) == nullptr);
    ensure(GEOSGeomGetEndPoint(geom1_) == nullptr);
    ensure(GEOSGeomGetPointN(geom1_, 0) == nullptr);
    ensure(GEOSGeomGetPointN(geom1_, 0) == nullptr);

    double length;
    ensure_equals(GEOSGeomGetLength(geom1_, &length), 0);
}

template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT("LINEARRING(0 0, 5 5, 10 10, 0 0)");
    GEOSGeometry* geom2;
    double x, y, z;
    ensure(nullptr != geom1_);

    char const r1 = GEOSisClosed(geom1_);

    ensure_equals(r1, 1);

    geom2 = GEOSGeomGetPointN(geom1_, 0);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 0);
    ensure_equals(y, 0);
    ensure(std::isnan(z));

    GEOSGeom_destroy(geom2);

    ensure(GEOSGeomGetPointN(geom1_, -1) == nullptr);

    geom2 = GEOSGeomGetStartPoint(geom1_);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 0);
    ensure_equals(y, 0);
    ensure(std::isnan(z));

    GEOSGeom_destroy(geom2);

    geom2 = GEOSGeomGetEndPoint(geom1_);
    GEOSGeomGetX(geom2, &x);
    GEOSGeomGetY(geom2, &y);
    GEOSGeomGetZ(geom2, &z);

    ensure_equals(x, 0);
    ensure_equals(y, 0);
    ensure(std::isnan(z));

    // GEOSGeom_destroy(geom2);
}

template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("LINEARRING(0 0, 5 5, 10 10, 0 0)");
    double length;
    ensure(nullptr != geom1_);

    GEOSGeomGetLength(geom1_, &length);
    ensure(length > 0.0);
}

} // namespace tut
