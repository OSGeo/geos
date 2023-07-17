// $Id$
//
// Test Suite for C-API GEOSLargestEmptyCircle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
    ensure(nullptr != result_);
    expected_ = GEOSGeomFromWKT("LINESTRING (150 150, 100 100)");
    ensure(GEOSEqualsExact(result_, expected_, 0.0001) == 1);
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
    ensure(nullptr != result_);
    expected_ = GEOSGeomFromWKT("LINESTRING (0.00038147 49.99961853, 40 10)");
    ensure(GEOSEqualsExact(result_, expected_, 0.0001) == 1);
}

} // namespace tut

