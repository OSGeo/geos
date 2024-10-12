//
// Test Suite for C-API GEOSUnaryUnion

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capiunaryunionprec_data : public capitest::utility {

    test_capiunaryunionprec_data() {
    }

};

typedef test_group<test_capiunaryunionprec_data> group;
typedef group::object object;

group test_capiunaryunionprec_group("capi::GEOSUnaryUnionPrec");

//
// Test Cases
//


// Self-union an empty point
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOINT ((4 5), (6 7), (4 5), (6 5), (6 7))");
    ensure(nullptr != geom1_);

    geom2_ = GEOSUnaryUnionPrec(geom1_, 2);
    ensure(nullptr != geom2_);

    ensure_equals(toWKT(geom2_), std::string("MULTIPOINT ((4 6), (6 6), (6 8))"));
}


} // namespace tut
