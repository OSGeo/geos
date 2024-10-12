#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//
struct test_geostopologypreservesimplify_data : public capitest::utility {};

// Common data used in test cases.
typedef test_group<test_geostopologypreservesimplify_data> group;
typedef group::object object;

group test_capigeostopologypreservesimplify_group("capi::GEOSTopologyPreserveSimplify");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    input_ = GEOSGeomFromWKT("LINESTRING (0 0, 1 0.1, 2 0)");
    GEOSSetSRID(input_, 32145);

    result_ = GEOSTopologyPreserveSimplify(input_, 1);
    expected_ = GEOSGeomFromWKT("LINESTRING (0 0, 2 0)");

    ensure_geometry_equals(result_, expected_);
    ensure_equals(GEOSGetSRID(result_), 32145);
}

template<>
template<>
void object::test<2>
()
{
    input_ = GEOSGeomFromWKT("LINESTRING EMPTY");
    result_ = GEOSTopologyPreserveSimplify(input_, 1);
    expected_ = GEOSGeomFromWKT("LINESTRING EMPTY");

    ensure_geometry_equals(result_, expected_);
}

template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING (0 0, 10 10, 20 0), (20 0, 10 0, 0 0)), (10 1, 11 1, 11 2, 10 1))");
    ensure(input_);

    result_ = GEOSTopologyPreserveSimplify(input_, 0.2);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut

