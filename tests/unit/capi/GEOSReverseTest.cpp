//
// Test Suite for C-API GEOSReverse
#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosreverse : public capitest::utility {
};

typedef test_group<test_capigeosreverse> group;
typedef group::object object;

group test_capigeosreverse_group("capi::GEOSReverse");

void
testReverse(const std::string& wkt_input,
            const std::string& wkt_output)
{
    GEOSGeometry* input = GEOSGeomFromWKT(wkt_input.c_str());
    GEOSGeometry* expected_result = GEOSGeomFromWKT(wkt_output.c_str());
    GEOSGeometry* result = GEOSReverse(input);

    ensure(result != nullptr);
    ensure_equals(1, GEOSEqualsExact(result, expected_result, 0.0));

    GEOSGeom_destroy(input);
    GEOSGeom_destroy(expected_result);
    GEOSGeom_destroy(result);
}

//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    testReverse("POINT (3 5)", "POINT (3 5)");
}

template<>
template<>
void object::test<2>
()
{
    testReverse("MULTIPOINT ((100 100), (10 100), (30 100))",
                "MULTIPOINT ((100 100), (10 100), (30 100))");
}

template<>
template<>
void object::test<3>
()
{
    testReverse("LINESTRING (200 200, 200 100)",
                "LINESTRING (200 100, 200 200)");
}

template<>
template<>
void object::test<4>
()
{
    testReverse("MULTILINESTRING ((1 1, 2 2), (3 3, 4 4))",
                "MULTILINESTRING ((2 2, 1 1), (4 4, 3 3))");
}

template<>
template<>
void object::test<5>
()
{
    testReverse("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1))",
                "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1))");

}

template<>
template<>
void object::test<6>
()
{
    testReverse("MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1)), ((100 100, 100 200, 200 200, 100 100)))",
                "MULTIPOLYGON (((0 0, 0 10, 10 10, 10 0, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1)), ((100 100, 200 200, 100 200, 100 100)))");

}

template<>
template<>
void object::test<7>
()
{
    testReverse("GEOMETRYCOLLECTION (LINESTRING (1 1, 2 2), GEOMETRYCOLLECTION(LINESTRING (3 5, 2 9)))",
                "GEOMETRYCOLLECTION (LINESTRING (2 2, 1 1), GEOMETRYCOLLECTION(LINESTRING (2 9, 3 5)))");

}

template<>
template<>
void object::test<8>
()
{
    testReverse("POINT EMPTY",              "POINT EMPTY");
    testReverse("LINESTRING EMPTY",         "LINESTRING EMPTY");
    testReverse("LINEARRING EMPTY",         "LINEARRING EMPTY");
    testReverse("POLYGON EMPTY",            "POLYGON EMPTY");
    testReverse("MULTIPOINT EMPTY",         "MULTIPOINT EMPTY");
    testReverse("MULTILINESTRING EMPTY",    "MULTILINESTRING EMPTY");
    testReverse("MULTIPOLYGON EMPTY",       "MULTIPOLYGON EMPTY");
    testReverse("GEOMETRYCOLLECTION EMPTY", "GEOMETRYCOLLECTION EMPTY");
}

template<>
template<>
void object::test<9>()
{
    testReverse("CIRCULARSTRING (0 0, 1 1, 2 0)",
                "CIRCULARSTRING (2 0, 1 1, 0 0)");
}

} // namespace tut
