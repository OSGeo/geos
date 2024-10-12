//
// Test Suite for C-API GEOSisValidDetail

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cctype>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capiisvaliddetail_data : public capitest::utility {
    GEOSGeometry* loc_;
    char* reason_;

    test_capiisvaliddetail_data() : loc_(nullptr), reason_(nullptr)
    {
    }

    void
    strToUpper(std::string& str)
    {
        using std::toupper;
        using std::string;

        for(string::size_type i = 0, len = str.size(); i < len; ++i) {
            str[i] = static_cast<string::value_type>(toupper(str[i]));
        }
    }

    ~test_capiisvaliddetail_data()
    {
        GEOSGeom_destroy(loc_);
        GEOSFree(reason_);
    }

};

typedef test_group<test_capiisvaliddetail_data> group;
typedef group::object object;

group test_capiisvaliddetail_group("capi::GEOSisValidDetail");

//
// Test Cases
//


// Flag values
template<>
template<>
void object::test<1>
()
{
    ensure_equals(GEOSVALID_ALLOW_SELFTOUCHING_RING_FORMING_HOLE, 1);
}

// Valid case
template<>
template<>
void object::test<2>
()
{
    // Looks invalid (self-intersecting) but isn't
    // (is non-simple though)
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0, 5 -5, 5 5)");
    int r = GEOSisValidDetail(geom1_, 0, &reason_, &loc_);
    ensure_equals(r, 1); // valid
    ensure_equals(reason_, (void*)nullptr);
    ensure_equals(loc_, (void*)nullptr);
}

// Invalid coordinate
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0, NaN -5)");
    ensure(nullptr != geom1_);
    int r = GEOSisValidDetail(geom1_, 0, &reason_, &loc_);
    std::string wkt = toWKT(loc_);
    strToUpper(wkt);
    ensure_equals(r, 0); // invalid
    ensure_equals(std::string(reason_), std::string("Invalid Coordinate"));
    std::string exp1 = "POINT (NAN -5)";
    std::string exp2 = "POINT (-1#IND -5)";
    // http://trac.osgeo.org/geos/ticket/656
    std::string exp3 = "POINT (1.#QNAN -5)";
    std::stringstream ss;
    ss << "Expected '" << exp1 << "' or '" << exp2 << "' or '" << exp3 << "', Obtained '" << wkt;
    ensure(ss.str(),
           wkt == exp1 ||
           wkt == exp2 ||
           wkt == exp3);
}

// Self intersecting ring forming hole
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 1, -10 10, 10 10, 0 1, 4 6, -4 6, 0 1))");
    int r = GEOSisValidDetail(geom1_, 0, &reason_, &loc_);
    ensure_equals(r, 0); // invalid
    ensure_equals(std::string(reason_), std::string("Ring Self-intersection"));
    ensure_equals(toWKT(loc_), "POINT (0 1)");
}

// Self intersecting ring forming hole (with ESRI flag)
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 1, -10 10, 10 10, 0 1, 4 6, -4 6, 0 1))");
    int flags = GEOSVALID_ALLOW_SELFTOUCHING_RING_FORMING_HOLE;

    int r = GEOSisValidDetail(geom1_, flags, &reason_, &loc_);
    ensure_equals(r, 1); // valid
    ensure_equals(reason_, (void*)nullptr);
    ensure_equals(loc_, (void*)nullptr);
}

// Check it is possible to not request details
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 1, -10 10, 10 10, 0 1, 4 6, -4 6, 0 1))");
    int r = GEOSisValidDetail(geom1_, 0, nullptr, nullptr);
    ensure_equals(r, 0); // invalid
}

template<>
template<>
void object::test<7>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE( CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 1 1)))");
    ensure(input_ != nullptr);

    char ret = GEOSisValidDetail(input_, 0, nullptr, nullptr);
    ensure_equals("error raised on curved geometry", ret, 2);
}

} // namespace tut

