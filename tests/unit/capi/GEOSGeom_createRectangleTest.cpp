// Test Suite for C-API GEOSGeom_createRectangle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
struct test_capiGEOSGeom_createRectangle : public capitest::utility {};

typedef test_group<test_capiGEOSGeom_createRectangle> group;
typedef group::object object;

group test_capiGEOSGeom_createRectangle_group("capi::GEOSGeom_createRectangle");

template <>
template <>
void object::test<1>() {
    GEOSGeometry* expected = GEOSGeomFromWKT("POLYGON ((0 0, 1 0, 1 2, 0 2, 0 0))");
    GEOSGeometry* geom = GEOSGeom_createRectangle(0, 0, 1, 2);

    ensure(geom != nullptr);
    ensure_equals(GEOSEqualsExact(geom, expected, 0), 1);

    GEOSGeom_destroy(expected);
    GEOSGeom_destroy(geom);
}

// verify that bounds with width and height of 0 returns point
template <>
template <>
void object::test<2>() {
    GEOSGeometry* expected = GEOSGeomFromWKT("POINT (1 1)");
    GEOSGeometry* geom = GEOSGeom_createRectangle(1,1,1,1);

    ensure(geom != nullptr);
    ensure_equals(GEOSEqualsExact(geom, expected, 0), 1);

    GEOSGeom_destroy(expected);
    GEOSGeom_destroy(geom);
}

// verify that a rectangle is returned if either but not both width and height are 0
template <>
template <>
void object::test<3>() {
    // no height
    GEOSGeometry* expected = GEOSGeomFromWKT("POLYGON ((0 0, 1 0, 1 0, 0 0, 0 0))");
    GEOSGeometry* geom = GEOSGeom_createRectangle(0,0,1,0);

    ensure(geom != nullptr);
    ensure_equals(GEOSEqualsExact(geom, expected, 0), 1);

    GEOSGeom_destroy(expected);
    GEOSGeom_destroy(geom);

    // no width
    expected = GEOSGeomFromWKT("POLYGON ((0 0, 0 0, 0 1, 0 1, 0 0))");
    geom = GEOSGeom_createRectangle(0,0,0,1);

    ensure(geom != nullptr);
    ensure_equals(GEOSEqualsExact(geom, expected, 0), 1);

    GEOSGeom_destroy(expected);
    GEOSGeom_destroy(geom);
}

// verify that mismatched bounds are reversed correctly
template <>
template <>
void object::test<4>() {
    GEOSGeometry* expected = GEOSGeomFromWKT("POLYGON ((0 0, 1 0, 1 2, 0 2, 0 0))");
    GEOSGeometry* geom = GEOSGeom_createRectangle(1, 2, 0, 0);

    ensure(geom != nullptr);
    ensure_equals(GEOSEqualsExact(geom, expected, 0), 1);

    GEOSGeom_destroy(expected);
    GEOSGeom_destroy(geom);
}

}  // namespace tut
