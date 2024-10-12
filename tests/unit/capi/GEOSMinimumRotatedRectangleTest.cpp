//
// Test Suite for C-API GEOSMinimumRotatedRectangle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_minimumrotatedrectangle_data : public capitest::utility {
    test_minimumrotatedrectangle_data() {
        GEOSWKTWriter_setRoundingPrecision(wktw_, 8);
    }

    void checkMinRectangle(const char* wkt, const char* expected)
    {
        // input
        geom1_ = GEOSGeomFromWKT(wkt);
        ensure(nullptr != geom1_);
        GEOSSetSRID(geom1_, 1234);

        // result
        geom2_ = GEOSMinimumRotatedRectangle(geom1_);
        ensure(nullptr != geom2_);
        ensure("SRID equal", GEOSGetSRID(geom2_) == GEOSGetSRID(geom1_));

        // expected
        if (expected) {
            geom3_ = GEOSGeomFromWKT(expected);
            ensure(nullptr != geom3_);
            ensure_geometry_equals(geom2_, geom3_, 0.0001);
        }
    }
};

typedef test_group<test_minimumrotatedrectangle_data> group;
typedef group::object object;

group test_capigeosminimumrotatedrectangle_group("capi::GEOSMinimumRotatedRectangle");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    checkMinRectangle(  "POLYGON ((1 6, 6 11, 11 6, 6 1, 1 6))",
                        "POLYGON ((1 6, 6 11, 11 6, 6 1, 1 6))");
}

// zero-length
template<>
template<>
void object::test<2>
()
{
    checkMinRectangle("LINESTRING (1 1, 1 1)", "POINT (1 1)");
}

// Horizontal
template<>
template<>
void object::test<3>
()
{
    checkMinRectangle("LINESTRING (1 1, 3 1, 5 1, 7 1)", "LINESTRING (1 1, 7 1)");
}

// Vertical
template<>
template<>
void object::test<4>
()
{
    checkMinRectangle("LINESTRING (1 1, 1 4, 1 7, 1 9)", "LINESTRING (1 1, 1 9)");
}

// Bent Line
template<>
template<>
void object::test<5>
()
{
    checkMinRectangle(
        "LINESTRING (1 2, 3 8, 9 6)",
        "POLYGON ((1 2, 3 8, 9 6, 7 0, 1 2))");
}

// Failure case from https://trac.osgeo.org/postgis/ticket/5163
template<>
template<>
void object::test<6>
()
{
    checkMinRectangle(
        "LINESTRING(-99.48710639268086 34.79029839231914,-99.48370699999998 34.78689899963806,-99.48152167568102 34.784713675318976)",
        "POLYGON ((-99.48710639 34.79029839, -99.48710639 34.79029839, -99.48152168 34.78471368, -99.48152168 34.78471368, -99.48710639 34.79029839))"
        );
}

// Collection Input
template<>
template<>
void object::test<7>
()
{
    checkMinRectangle(
        "MULTILINESTRING ((1 2, 3 8, 9 6))",
        "POLYGON ((1 2, 3 8, 9 6, 7 0, 1 2))");
}

template<>
template<>
void object::test<8>
()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_);

    result_ = GEOSMinimumRotatedRectangle(input_);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut
