//
// Test Suite for C-API GEOSFrechetDistance

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosfrechetdistance_data : public capitest::utility {};

typedef test_group<test_capigeosfrechetdistance_data> group;
typedef group::object object;

group test_capigeosfrechetdistance_group("capi::GEOSFrechetDistance");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = fromWKT("LINESTRING (0 0, 100 0)");
    geom2_ = fromWKT("LINESTRING (0 0, 50 50, 100 0)");

    double dist;
    int ret = GEOSFrechetDistance(geom1_, geom2_, &dist);

    ensure_equals(ret, 1);
    ensure_distance(dist, 70.7106781186548, 1e-12);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = fromWKT("LINESTRING (0 0, 100 0)");
    geom2_ = fromWKT("LINESTRING (0 0, 50 50, 100 0)");

    double dist;
    int ret = GEOSFrechetDistanceDensify(geom1_, geom2_, 0.5, &dist);

    ensure_equals(ret, 1);
    ensure_distance(dist, 50., 1e-12);
}

// No crash with tiny densify fraction
// https://trac.osgeo.org/geos/ticket/1086
template<>
template<>
void object::test<3>
()
{
    geom1_ = fromWKT("LINESTRING (0 0, 3 7, 5 5)");
    geom2_ = fromWKT("LINESTRING (0 0, 9 1, 2 2)");

    double dist = 0;
    GEOSFrechetDistanceDensify(geom1_, geom2_, 1e-40, &dist);

    ensure(dist >= 0); // no crash
}

// No crash with tiny densify fraction
// https://trac.osgeo.org/geos/ticket/1086
template<>
template<>
void object::test<4>
()
{
    geom1_ = fromWKT("LINESTRING (0 0, 3 7, 5 5)");
    geom2_ = fromWKT("LINESTRING (0 0, 9 1, 2 2)");

    double dist = 0;
    GEOSFrechetDistanceDensify(geom1_, geom2_, 1e-19, &dist);

    ensure(dist >= 0); // no crash
}

template<>
template<>
void object::test<5>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geom2_ = fromWKT("LINESTRING (1 2, 2 2)");

    ensure(geom1_);
    ensure(geom2_);

    double dist;
    ensure_equals("curved geometry not supported", GEOSFrechetDistance(geom1_, geom2_, &dist), 0);
    ensure_equals("curved geometry not supported", GEOSFrechetDistance(geom2_, geom1_, &dist), 0);
}

} // namespace tut
