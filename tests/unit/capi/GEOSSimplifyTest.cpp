//
// Test Suite for C-API GEOSSimplify

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeossimplify_data : public capitest::utility {};

typedef test_group<test_capigeossimplify_data> group;
typedef group::object object;

group test_capigeossimplify_group("capi::GEOSSimplify");

//
// Test Cases
//

// Test GEOSSimplify
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(0 != GEOSisEmpty(geom1_));

    geom2_ = GEOSSimplify(geom1_, 43.2);

    ensure(0 != GEOSisEmpty(geom2_));
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_ != nullptr);

    result_ = GEOSSimplify(input_, 2);
    ensure(result_ == nullptr);
}

} // namespace tut

