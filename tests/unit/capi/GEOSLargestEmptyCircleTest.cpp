// $Id$
//
// Test Suite for C-API GEOSLargestEmptyCircle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_data_GEOSLargestEmptyCircle : public capitest::utility {

    test_data_GEOSLargestEmptyCircle()
    {
    }

    ~test_data_GEOSLargestEmptyCircle()
    {
    }

};

typedef test_group<test_data_GEOSLargestEmptyCircle> group;
typedef group::object object;

group test_capi_largestemptycircle_group("capi::GEOSLargestEmptyCircle");

//
// Test Cases
//

// Points of a square
template<>
template<>
void object::test<1>
()
{
    input_ = GEOSGeomFromWKT("MULTIPOINT ((100 100), (100 200), (200 200), (200 100))");
    result_ = GEOSLargestEmptyCircle(input_, nullptr, 0.001);
    expected_ = GEOSGeomFromWKT("LINESTRING (150 150, 100 100)");
    ensure_geometry_equals_exact(result_, expected_, 0.0001);
}

// Line obstacles with square boundary
template<>
template<>
void object::test<2>
()
{
    input_ = GEOSGeomFromWKT("MULTILINESTRING ((40 90, 90 60), (90 40, 40 10))");
    geom2_ = GEOSGeomFromWKT("POLYGON ((0 100, 100 100, 100 0, 0 0, 0 100))");
    result_ = GEOSLargestEmptyCircle(input_, geom2_, 0.001);
    expected_ = GEOSGeomFromWKT("LINESTRING (0.00038147 49.99961853, 40 10)");
    ensure_geometry_equals_exact(result_, expected_, 0.0001);
}

template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("MULTICURVE (CIRCULARSTRING (0 0, 1 1, 2 0), (0 3, 2 3))");
    ensure(input_);

    result_ = GEOSLargestEmptyCircle(input_, nullptr, 0.001);

    ensure("curved geometries not supported", result_ == nullptr);
}

template<>
template<>
void object::test<4>()
{
    input_ = GEOSGeomFromWKT("MULTILINESTRING ((40 90, 90 60), (90 40, 40 10))");
    geom2_ = GEOSGeomFromWKT("CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING(0 100, 50 150, 100 100), (100 100, 100 0, 0 0, 0 100)))");
    ensure(input_);
    ensure(geom2_);

    result_ = GEOSLargestEmptyCircle(input_, geom2_, 0.001);

    ensure("curved geometries not supported", result_ == nullptr);
}


} // namespace tut

