//
// Test Suite for C-API GEOSWithin

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeoswithin_data : public capitest::utility {};

typedef test_group<test_capigeoswithin_data> group;
typedef group::object object;

group test_capigeoswithin_group("capi::GEOSWithin");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
    geom2_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSWithin(geom1_, geom2_);

    ensure_equals(r1, 0);

    char const r2 = GEOSWithin(geom2_, geom1_);

    ensure_equals(r2, 0);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((1 1,1 5,5 5,5 1,1 1))");
    geom2_ = GEOSGeomFromWKT("POINT(2 2)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSWithin(geom1_, geom2_);

    ensure_equals(int(r1), 0);

    char const r2 = GEOSWithin(geom2_, geom1_);

    ensure_equals(int(r2), 1);
}

template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOLYGON(((0 0,0 10,10 10,10 0,0 0)))");
    geom2_ = GEOSGeomFromWKT("POLYGON((1 1,1 2,2 2,2 1,1 1))");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSWithin(geom1_, geom2_);

    ensure_equals(int(r1), 0);

    char const r2 = GEOSWithin(geom2_, geom1_);

    ensure_equals(int(r2), 1);
}

template<>
template<>
void object::test<4>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geom2_ = fromWKT("LINESTRING (1 0, 2 0)");

    ensure(geom1_);
    ensure(geom2_);

    ensure_equals("curved geometry not supported", GEOSWithin(geom2_, geom1_), 2);
}

} // namespace tut

