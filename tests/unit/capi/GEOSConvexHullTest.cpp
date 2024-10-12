//
// Test Suite for C-API GEOSConvexHull

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
struct test_capigeosconvexhull_data : public capitest::utility {
};

typedef test_group<test_capigeosconvexhull_data> group;
typedef group::object object;

group test_capigeosconvexhull_group("capi::GEOSConvexHull");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    input_ = fromWKT("MULTIPOINT ((130 240), (130 240), (570 240), (570 290), (650 240))");
    expected_ = fromWKT("POLYGON ((130 240, 570 290, 650 240, 130 240))");
    result_ = GEOSConvexHull(input_);
    ensure(nullptr != result_);
    ensure(0 == GEOSisEmpty(result_));
    ensure_geometry_equals(result_, expected_);
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_ != nullptr);

    result_ = GEOSConvexHull(input_);
    ensure(result_ == nullptr);
}

} // namespace tut

