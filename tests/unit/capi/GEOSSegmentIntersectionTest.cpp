//
// Test Suite for C-API GEOSSegmentIntersection
#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeossegmentintersection : capitest::utility {
};

typedef test_group<test_capigeossegmentintersection> group;
typedef group::object object;

group test_capigeossegmentintersection_group("capi::GEOSSegmentIntersection");

//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    // plain old intersection
    int result;
    double x, y;

    result = GEOSSegmentIntersection(0, 0, 10, 10,
                                     8, 0, 8, 10,
                                     &x, &y);

    ensure_equals(result, 1);
    ensure_equals(x, 8);
    ensure_equals(y, 8);
}

template<>
template<>
void object::test<2>
()
{
    // no intersection
    int result;
    double x, y;

    result = GEOSSegmentIntersection(0, 0, 10, 10,
                                     8, 0, 8,  2,
                                     &x, &y);

    ensure_equals(result, -1);
}

} // namespace tut
