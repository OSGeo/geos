//
// Test Suite for C-API GEOSLineMergeDirected

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeoslinemergedirected_data : public capitest::utility {
};

typedef test_group<test_capigeoslinemergedirected_data> group;
typedef group::object object;

group test_capigeoslinemergedirected_group("capi::GEOSLineMergeDirected");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    auto input = GEOSGeomFromWKT("MULTILINESTRING((0 0, 0 100),(0 -5, 0 0))");
    auto result = GEOSLineMergeDirected(input);
    auto expected = GEOSGeomFromWKT("LINESTRING(0 -5,0 0,0 100)");

    ensure(GEOSEqualsExact(result, expected, 0));

    GEOSGeom_destroy(input);
    GEOSGeom_destroy(result);
    GEOSGeom_destroy(expected);
}

template<>
template<>
void object::test<2>
()
{
    auto input = GEOSGeomFromWKT("MULTILINESTRING((0 0, 0 100),(0 0, 0 -5))");
    auto result = GEOSLineMergeDirected(input);
    auto expected = GEOSGeomFromWKT("MULTILINESTRING((0 0, 0 100),(0 0, 0 -5))");

    ensure(GEOSEqualsExact(result, expected, 0));

    GEOSGeom_destroy(input);
    GEOSGeom_destroy(result);
    GEOSGeom_destroy(expected);
}


template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("MULTICURVE (CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 3 0))");
    ensure(input_);

    result_ = GEOSLineMergeDirected(input_);

    ensure("curved geometries not supported", result_ == nullptr);
}

} // namesplace tut

