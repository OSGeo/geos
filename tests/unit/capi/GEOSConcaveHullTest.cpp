//
// Test Suite for C-API GEOSConcaveHull

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
struct test_capigeosconcavehull_data : public capitest::utility {
};

typedef test_group<test_capigeosconcavehull_data> group;
typedef group::object object;

group test_capigeosconcavehull_group("capi::GEOSConcaveHull");

//
// Test Cases
//

template<>
template<>
void object::test<1>()
{
    input_ = fromWKT("MULTIPOINT ((10 90), (10 10), (90 10), (90 90), (40 40), (60 30), (30 70), (40 60), (60 50), (60 72), (47 66), (90 60))");
    expected_ = fromWKT("POLYGON ((30 70, 10 90, 60 72, 90 90, 90 60, 90 10, 60 30, 10 10, 40 40, 60 50, 47 66, 40 60, 30 70))");
    result_ = GEOSConcaveHull(input_, 0, 0);
    ensure(nullptr != result_);
    ensure(0 == GEOSisEmpty(result_));
    ensure_geometry_equals(result_, expected_);
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT("MULTIPOINT ((10 90), (10 10), (90 10), (90 90), (40 40), (60 30), (30 70), (40 60), (60 50), (60 72), (47 66), (90 60))");
    expected_ = fromWKT("POLYGON ((30 70, 10 90, 60 72, 90 90, 90 60, 90 10, 60 30, 10 10, 40 40, 30 70))");
    result_ = GEOSConcaveHullByLength(input_, 50, 0);
    ensure(nullptr != result_);
    ensure(0 == GEOSisEmpty(result_));
    ensure_geometry_equals(result_, expected_);
}

template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_ != nullptr);

    result_ = GEOSConcaveHull(input_, 0, 0);
    ensure(result_ == nullptr);
}

} // namespace tut
