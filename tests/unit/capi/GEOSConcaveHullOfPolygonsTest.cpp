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
struct test_capigeoshullofpolygons_data : public capitest::utility {
};

typedef test_group<test_capigeoshullofpolygons_data> group;
typedef group::object object;

group test_capigeoshullofpolygons_group("capi::GEOSConcaveHullOfPolygons");

//
// Test Cases
//


template<>
template<>
void object::test<1>()
{
    input_ = fromWKT("POLYGON EMPTY");
    geom1_ = GEOSConcaveHullOfPolygons(input_, 0.7, false, false);
    ensure(nullptr != geom1_);
    expected_ = fromWKT("POLYGON EMPTY");
    ensure_geometry_equals(geom1_, expected_);
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT("MULTIPOLYGON(((0 0, 0 1, 1 1, 1 0, 0 0)))");
    geom1_ = GEOSConcaveHullOfPolygons(input_, 0.7, false, false);
    ensure(nullptr != geom1_);
    expected_ = fromWKT("MULTIPOLYGON(((0 0, 0 1, 1 1, 1 0, 0 0)))");
    ensure_geometry_equals(geom1_, expected_);
}

template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE( CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 0 0) ))");
    ensure(input_ != nullptr);

    result_ = GEOSConcaveHullOfPolygons(input_, 0.7, false, false);
    ensure("curved geometry not supported", result_ == nullptr);
}
  
template<>
template<>
void object::test<4>()
{
    input_ = fromWKT("POLYGON((0 0, 0 0, 0 0))");
    result_ = GEOSConcaveHullOfPolygons(input_, 0.7, false, false);
    ensure(GEOSisEmpty(result_));
}


} // namespace tut

