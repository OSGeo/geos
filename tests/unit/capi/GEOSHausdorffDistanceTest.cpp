//
// Test Suite for C-API GEOSHausdorffDistance

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeoshausdorffdistance_data : public capitest::utility {
    test_capigeoshausdorffdistance_data() {
    }
};

typedef test_group<test_capigeoshausdorffdistance_data> group;
typedef group::object object;

group test_capigeoshausdorffdistance_group("capi::GEOSHausdorffDistance");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING (130 0, 0 0, 0 150)");
    geom2_ = GEOSGeomFromWKT("LINESTRING (10 10, 10 150, 130 10)");

    double dist;
    int ret = GEOSHausdorffDistance(geom1_, geom2_, &dist);

    ensure_equals(ret, 1);
    ensure_distance(dist, 14.142135623730951, 1e-12);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING (130 0, 0 0, 0 150)");
    geom2_ = GEOSGeomFromWKT("LINESTRING (10 10, 10 150, 130 10)");

    double dist;
    int ret = GEOSHausdorffDistanceDensify(geom1_, geom2_, 0.5, &dist);

    ensure_equals(ret, 1);
    ensure_distance(dist, 70., 1e-12);
}

template<>
template<>
void object::test<3>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geom2_ = fromWKT("LINESTRING (1 2, 2 2)");

    ensure(geom1_);
    ensure(geom2_);

    double dist;
    ensure_equals("curved geometry not supported", GEOSHausdorffDistance(geom1_, geom2_, &dist), 0);
    ensure_equals("curved geometry not supported", GEOSHausdorffDistance(geom2_, geom1_, &dist), 0);
}

} // namespace tut
