// Test Suite for C-API MakeValid

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstring>
#include <cstdint>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capimakevalid_data : public capitest::utility {
    test_capimakevalid_data() {
    }
};

typedef test_group<test_capimakevalid_data> group;
typedef group::object object;

group test_capimakevalid_group("capi::GEOSMakeValid");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0,1 1,0 1,1 0,0 0))");
    geom2_ = GEOSMakeValid(geom1_);
    expected_ = GEOSGeomFromWKT("MULTIPOLYGON (((0 0, 0.5 0.5, 1 0, 0 0)), ((0.5 0.5, 0 1, 1 1, 0.5 0.5)))");
    GEOSNormalize(geom2_);
    GEOSNormalize(expected_);
    ensure(GEOSEqualsExact(geom2_, expected_, 0.01));
}

template<>
template<>
void object::test<2>
()
{
    const char* hex = "0103000000010000000900000062105839207df640378941e09d491c41ced67431387df640c667e7d398491c4179e92631387df640d9cef7d398491c41fa7e6abcf87df640cdcccc4c70491c41e3a59bc4527df64052b81e053f491c41cdcccccc5a7ef640e3a59bc407491c4104560e2da27df640aaf1d24dd3481c41e9263108c67bf64048e17a1437491c4162105839207df640378941e09d491c41";
    geom1_ = GEOSGeomFromHEX_buf((uint8_t*)hex, std::strlen(hex));
    geom2_ = GEOSMakeValid(geom1_);
    // std::cout << toWKT(geom2_) << std::endl;
    expected_ = GEOSGeomFromWKT("POLYGON ((92127.546 463452.075, 92117.173 463439.755, 92133.675 463425.942, 92122.136 463412.826, 92092.37699999999 463437.77, 92114.014 463463.469, 92115.512 463462.207, 92115.51207431706 463462.2069374289, 92127.546 463452.075))");
    GEOSNormalize(geom2_);
    GEOSNormalize(expected_);
    ensure(GEOSEqualsExact(geom2_, expected_, 0.01));
}


template<>
template<>
void object::test<3>
()
{
    const char* wkt = "LINESTRING(0 0, 0 0)";
    GEOSMakeValidParams *params = GEOSMakeValidParams_create();
    GEOSMakeValidParams_setKeepCollapsed(params, 1);
    geom1_ = GEOSGeomFromWKT(wkt);
    geom2_ = GEOSMakeValidWithParams(geom1_, params);
    GEOSMakeValidParams_destroy(params);
    // std::cout << toWKT(geom2_) << std::endl;
    expected_ = GEOSGeomFromWKT("POINT(0 0)");
    ensure(GEOSEqualsExact(geom2_, expected_, 0.01));
}

template<>
template<>
void object::test<4>
()
{
    const char* wkt = "LINESTRING(0 0, 0 0)";
    GEOSMakeValidParams *params = GEOSMakeValidParams_create();
    GEOSMakeValidParams_setKeepCollapsed(params, 0);
    geom1_ = GEOSGeomFromWKT(wkt);
    geom2_ = GEOSMakeValidWithParams(geom1_, params);
    GEOSMakeValidParams_destroy(params);
    // std::cout << toWKT(geom2_) << std::endl;
    expected_ = GEOSGeomFromWKT("POINT(0 0)");
    ensure(GEOSEqualsExact(geom2_, expected_, 0.01));
}

template<>
template<>
void object::test<5>()
{
    // ring outside shell
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING (0 0, 10 10, 20 0), (20 0, 0 0)), (10 10, 30 10, 30 30, 10 10))");
    ensure(input_);

    result_ = GEOSMakeValid(input_);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut
