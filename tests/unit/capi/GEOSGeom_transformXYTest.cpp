//
// Test Suite for C-API GEOSGeom_transformXY

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
struct test_capi_geosgeom_transformxy : public capitest::utility {};

typedef test_group<test_capi_geosgeom_transformxy> group;
typedef group::object object;

group test_capi_geosgeom_transformxy_group("capi::GEOSGeom_transformXY");

static int SCALE_2_3(double* x, double* y, void* userdata) {
    (*x) *= 2;
    (*y) *= 3;
    (void)(userdata);  // make unused parameter warning go away
    return 1;
}

// callback that doesn't update coordinates should return original values
template <>
template <>
void object::test<1>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT (1 1)");

    GEOSGeometry* out = GEOSGeom_transformXY(
        geom,
        [](double* x, double* y, void* userdata) {
            (void)(x);         // make unused parameter warning go away
            (void)(y);         // make unused parameter warning go away
            (void)(userdata);  // make unused parameter warning go away
            return 1;
        },
        nullptr);

    ensure(out != nullptr);
    ensure_equals(GEOSEqualsExact(out, geom, 0), 1);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// failed callback should return NULL
template <>
template <>
void object::test<2>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT (1 1)");

    GEOSGeometry* out = GEOSGeom_transformXY(
        geom,
        [](double* x, double* y, void* userdata) {
            (void)(x);         // make unused parameter warning go away
            (void)(y);         // make unused parameter warning go away
            (void)(userdata);  // make unused parameter warning go away
            return 0;          // indicates error
        },
        nullptr);

    ensure(out == nullptr);

    GEOSGeom_destroy(geom);
}

// callback should modify point coordinates
template <>
template <>
void object::test<3>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT (1 1)");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out, "POINT (2 3)");

    double xmin, ymin, xmax, ymax;
    GEOSGeom_getExtent(out, &xmin, &ymin, &xmax, &ymax);
    ensure_equals(xmin, 2);
    ensure_equals(ymin, 3);
    ensure_equals(xmax, 2);
    ensure_equals(ymax, 3);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify linestring coordinates
template <>
template <>
void object::test<4>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("LINESTRING (1 1, 2 2)");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out, "LINESTRING (2 3, 4 6)");

    double xmin, ymin, xmax, ymax;
    GEOSGeom_getExtent(out, &xmin, &ymin, &xmax, &ymax);
    ensure_equals(xmin, 2);
    ensure_equals(ymin, 3);
    ensure_equals(xmax, 4);
    ensure_equals(ymax, 6);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify polygon coordinates
template <>
template <>
void object::test<5>() {
    GEOSGeometry* geom = GEOSGeomFromWKT(
        "POLYGON ((1 1, 1 10, 10 10, 10 1, 1 1),  (2 2, 2 4, 4 4, 4 2, 2 2))");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out,
        "POLYGON ((2 3, 2 30, 20 30, 20 3, 2 3), (4 6, 4 12, 8 12, 8 6, 4 6))");

    double xmin, ymin, xmax, ymax;
    GEOSGeom_getExtent(out, &xmin, &ymin, &xmax, &ymax);
    ensure_equals(xmin, 2);
    ensure_equals(ymin, 3);
    ensure_equals(xmax, 20);
    ensure_equals(ymax, 30);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify multi point coordinates
template <>
template <>
void object::test<6>() {
    GEOSGeometry* geom = GEOSGeomFromWKT(
        "MULTIPOINT ((1 1), (2 2))");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out,
        "MULTIPOINT ((2 3), (4 6))");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify multi linestring coordinates
template <>
template <>
void object::test<7>() {
    GEOSGeometry* geom = GEOSGeomFromWKT(
        "MULTILINESTRING ((1 1, 2 2), (3 3, 4 4))");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out,
        "MULTILINESTRING ((2 3, 4 6), (6 9, 8 12 ))");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify multi polygon coordinates
template <>
template <>
void object::test<8>() {
    GEOSGeometry* geom = GEOSGeomFromWKT(
        "MULTIPOLYGON (((1 1, 1 10, 10 10, 10 1, 1 1),  (2 2, 2 4, 4 4, 4 2, 2 2)), ((0 0, 0 100, 100 100, 100 0, 0 0)))");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out,
        "MULTIPOLYGON (((2 3, 2 30, 20 30, 20 3, 2 3), (4 6, 4 12, 8 12, 8 6, 4 6)), ((0 0, 0 300, 200 300, 200 0, 0 0)))");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// callback should modify geometry collection coordinates
template <>
template <>
void object::test<9>() {
    GEOSGeometry* geom = GEOSGeomFromWKT(
        "GEOMETRYCOLLECTION (POINT (1 1), LINESTRING (1 1, 2 2), POLYGON ((1 1, 1 2, 2 2, 2 1, 1 1)))");
    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out, "GEOMETRYCOLLECTION (POINT (2 3), LINESTRING (2 3, 4 6), POLYGON ((2 3, 2 6, 4 6, 4 3, 2 3)))");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// should not fail for empty geometry
template <>
template <>
void object::test<10>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT EMPTY");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out, "POINT EMPTY");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// should retain original coords even if they collapse to same coordinate
template <>
template <>
void object::test<11>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("LINESTRING (1 1, 2 2)");

    GEOSGeometry* out = GEOSGeom_transformXY(
        geom,
        [](double* x, double* y, void* userdata) {
            *x = 0;
            *y = 0;
            (void)(userdata);  // make unused parameter warning go away
            return 1;
        },
        nullptr);

    ensure(out != nullptr);
    ensure_equals(GEOSGetNumCoordinates(out), 2);

    // Cannot construct WKT for this case, must test coords directly
    const GEOSCoordSequence* seq = GEOSGeom_getCoordSeq(out);

    double x, y;
    GEOSCoordSeq_getXY(seq, 0, &x, &y);
    ensure_equals(x, 0);
    ensure_equals(y, 0);

    GEOSCoordSeq_getXY(seq, 1, &x, &y);
    ensure_equals(x, 0);
    ensure_equals(y, 0);

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// should pass through userdata
template <>
template <>
void object::test<12>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("LINESTRING (1 1, 2 2)");

    double userdata_scale = 5.0;

    GEOSGeometry* out = GEOSGeom_transformXY(
        geom,
        [](double* x, double* y, void* userdata) {
            double scale = *(double *)(userdata);
            (*x) *= scale;
            (*y) *= scale;
            return 1;
        },
        (void *)(&userdata_scale));

    ensure(out != nullptr);
    ensure_geometry_equals(out, "LINESTRING (5 5, 10 10)");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

// transform should preserve existing Z coordinate values
template <>
template <>
void object::test<13>() {
    GEOSGeometry* geom = GEOSGeomFromWKT("POINT Z (1 1 4)");

    GEOSGeometry* out = GEOSGeom_transformXY(geom, SCALE_2_3, nullptr);

    ensure(out != nullptr);
    ensure_geometry_equals(out, "POINT Z (2 3 4)");

    GEOSGeom_destroy(geom);
    GEOSGeom_destroy(out);
}

template <>
template <>
void object::test<14>() {
    input_ = GEOSGeomFromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_);

    result_ = GEOSGeom_transformXY(input_, SCALE_2_3, nullptr);

    ensure_equals(toWKT(result_), "CIRCULARSTRING (0 0, 2 3, 4 0)");
}

}  // namespace tut
