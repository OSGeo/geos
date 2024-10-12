// Test Suite for C-API BuildArea

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capi_buildarea_data : public capitest::utility {};

typedef test_group<test_capi_buildarea_data> group;
typedef group::object object;

group test_capi_buildarea_group("capi::GEOSBuildArea");

//
// Test Cases
//

template<>
template<>
void object::test<1>()
{
    input_ = fromWKT("GEOMETRYCOLLECTION(LINESTRING(0 0,0 1,1 1),LINESTRING(1 1,1 0,0 0))");
    result_ = GEOSBuildArea(input_);
    expected_ = fromWKT("POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))");
    ensure_geometry_equals(result_, expected_, 0);
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT("MULTICURVE( CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 0 0) )");
    ensure(input_ != nullptr);

    result_ = GEOSBuildArea(input_);
    ensure(result_ == nullptr);
}

} // namespace tut
