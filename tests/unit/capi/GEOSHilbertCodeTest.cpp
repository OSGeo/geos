//
// Test Suite for C-API GEOSHilbertCode

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
struct test_capi_geoshilbertcode : public capitest::utility {};

typedef test_group<test_capi_geoshilbertcode> group;
typedef group::object object;

group test_capi_geoshilbertcode_group("capi::GEOSHilbertCode");


// Test various points on the Hilbert curve
template<>
template<>
void object::test<1>
()
{
    GEOSGeometry* geom1 = GEOSGeomFromWKT("POINT (0 0)");
    GEOSGeometry* geom2 = GEOSGeomFromWKT("POINT (1 1)");
    GEOSGeometry* extent = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");

    unsigned int code;
    int ret;

    ret = GEOSHilbertCode(geom1, extent, 1, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 0u);

    ret = GEOSHilbertCode(geom1, extent, 16, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 0u);

    ret = GEOSHilbertCode(geom2, extent, 2, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 10u);

    ret = GEOSHilbertCode(geom2, extent, 8, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 43690u);

    ret = GEOSHilbertCode(geom2, extent, 16, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 2863311530u);

    GEOSGeom_destroy(geom1);
    GEOSGeom_destroy(geom2);
    GEOSGeom_destroy(extent);
}

// Test that midpoint is calculated
template<>
template<>
void object::test<2>
()
{
    GEOSGeometry* extent = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");

    unsigned int code;
    int ret;

    ret = GEOSHilbertCode(extent, extent, 2, &code);
    ensure_equals(ret, 1);
    ensure_equals(code, 2u);

    GEOSGeom_destroy(extent);
}

// Test level out of bounds
template<>
template<>
void object::test<3>
()
{
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT (0 0)");
    GEOSGeometry* extent = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");

    unsigned int code;
    int ret;

    ret = GEOSHilbertCode(geom, extent, 17, &code);
    ensure_equals(ret, 0);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(extent);
}

} // namespace tut

