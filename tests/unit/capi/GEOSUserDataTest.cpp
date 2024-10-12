//
// Test Suite for C-API GEOSGeom_setUserData

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeouserdata_data : public capitest::utility {};

typedef test_group<test_capigeouserdata_data> group;
typedef group::object object;

group test_capigeosuserdata_group("capi::GEOSUserData");

//
// Test Cases
//

// Test GEOSUserData
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(0 != GEOSisEmpty(geom1_));

    int in = 1;
    GEOSGeom_setUserData(geom1_, &in);

    int* out = (int*)GEOSGeom_getUserData(geom1_);

    ensure_equals(*out, 1);
}

struct UserData {
    int a;
    int b;
} ;

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(0 != GEOSisEmpty(geom1_));

    UserData in;
    in.a = 1;
    in.b = 2;

    GEOSGeom_setUserData(geom1_, &in);

    struct UserData* out = (struct UserData*)GEOSGeom_getUserData(geom1_);

    ensure_equals(in.a, out->a);
    ensure_equals(in.b, out->b);
    ensure_equals(&in, out);
}

} // namespace tut

