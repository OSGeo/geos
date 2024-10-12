//
// Test Suite for C-API GEOSPolygonHullSimplify

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeospolygonhull_data : public capitest::utility {};


typedef test_group<test_capigeospolygonhull_data> group;
typedef group::object object;

group test_capigeospolygonhull_group("capi::GEOSPolygonHullSimplify");

//
// Test Cases
//

template<>
template<>
void object::test<1>()
{
    input_ = GEOSGeomFromWKT("POLYGON ((10 90, 40 60, 20 40, 40 20, 70 50, 40 30, 30 40, 60 70, 50 90, 90 90, 90 10, 10 10, 10 90))");
    ensure(nullptr != input_);
    geom1_ = GEOSPolygonHullSimplify(input_, 1, 0.5);
    ensure(nullptr != geom1_);
    expected_ = GEOSGeomFromWKT("POLYGON ((10 90, 50 90, 90 90, 90 10, 10 10, 10 90))");
    ensure_geometry_equals(geom1_, expected_);
    ensure(0 == GEOSisEmpty(geom1_));
}

template<>
template<>
void object::test<2>()
{
    input_ = GEOSGeomFromWKT("POLYGON ((10 90, 40 60, 20 40, 40 20, 70 50, 40 30, 30 40, 60 70, 50 90, 90 90, 90 10, 10 10, 10 90))");
    ensure(nullptr != input_);
    geom1_ = GEOSPolygonHullSimplify(input_, 1, 0.7);
    ensure(nullptr != geom1_);
    expected_ = GEOSGeomFromWKT("POLYGON ((10 90, 40 60, 30 40, 60 70, 50 90, 90 90, 90 10, 10 10, 10 90))");
    ensure_geometry_equals(geom1_, expected_);
    ensure(0 == GEOSisEmpty(geom1_));
}

template<>
template<>
void object::test<3>()
{
    input_ = GEOSGeomFromWKT("POLYGON EMPTY");
    ensure(nullptr != input_);
    geom1_ = GEOSPolygonHullSimplify(input_, 1, 0.7);
    ensure(nullptr != geom1_);
    expected_ = GEOSGeomFromWKT("POLYGON EMPTY");
    ensure_geometry_equals(geom1_, expected_);
}

template<>
template<>
void object::test<4>()
{
    input_ = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
    ensure(nullptr != input_);
    geom1_ = GEOSPolygonHullSimplify(input_, 1, 0.7);
    ensure(nullptr != geom1_);
    expected_ = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
    ensure_geometry_equals(geom1_, expected_);
}

template<>
template<>
void object::test<5>()
{
    input_ = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
    ensure(nullptr != input_);
    geom1_ = GEOSPolygonHullSimplifyMode(input_, 1, GEOSHULL_PARAM_AREA_RATIO, 0.7);
    ensure(nullptr != geom1_);
    // char *wkt = GEOSWKTWriter_write(wktw_, geom1_);
    // printf("%s\n", wkt);
    expected_ = GEOSGeomFromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
    ensure_geometry_equals(geom1_, expected_);
}

template<>
template<>
void object::test<6>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE( CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 1 0.0001, 0 0)))");
    ensure(input_);

    result_ = GEOSPolygonHullSimplify(input_, 1, 0.8);
    ensure("curved geometries not supported", result_ == nullptr);
}


} // namespace tut

