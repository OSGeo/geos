//
// Test Suite for C-API GEOSMinimumClearance
#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/constants.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosminimumclearance_data : public capitest::utility {
    static void
    testClearance(const std::string& wkx_input,
                  const std::string& wkx_expected,
                  double clearance)
    {

        GEOSGeometry* input;
        GEOSGeometry* expected_result;
        if(wkx_input[0] == '0') {
            input = GEOSGeomFromHEX_buf((const unsigned char*) wkx_input.c_str(), wkx_input.length());
        }
        else {
            input = GEOSGeomFromWKT(wkx_input.c_str());
        }

        if(wkx_expected[0] == '0') {
            expected_result = GEOSGeomFromHEX_buf((const unsigned char*) wkx_expected.c_str(), wkx_expected.length());
        }
        else {
            expected_result = GEOSGeomFromWKT(wkx_expected.c_str());
        }

        double d;
        int error = GEOSMinimumClearance(input, &d);

        ensure(!error);
        if(clearance == geos::DoubleInfinity) {
            ensure(d == clearance);
        }
        else {
            ensure_equals("clearance", d, clearance, 1e-12);
        }

        GEOSGeometry* result = GEOSMinimumClearanceLine(input);
        ensure(result != nullptr);
        ensure_equals(GEOSEquals(result, expected_result), 1);

        GEOSGeom_destroy(input);
        GEOSGeom_destroy(expected_result);
        GEOSGeom_destroy(result);
    }

};

typedef test_group<test_capigeosminimumclearance_data> group;
typedef group::object object;

group test_capigeosminimumclearance_group("capi::GEOSMinimumClearance");

//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    testClearance("MULTIPOINT ((100 100), (100 100))",
                  "LINESTRING EMPTY",
                  geos::DoubleInfinity);
}

template<>
template<>
void object::test<2>
()
{
    testClearance("MULTIPOINT ((100 100), (10 100), (30 100))",
                  "LINESTRING (30 100, 10 100)",
                  20);
}

template<>
template<>
void object::test<3>
()
{
    testClearance("POLYGON ((100 100, 300 100, 200 200, 100 100))",
                  "LINESTRING (200 200, 200 100)",
                  100);
}

template<>
template<>
void object::test<4>
()
{
    testClearance("0106000000010000000103000000010000001a00000035d42824992d5cc01b834e081dca404073b9c150872d5cc03465a71fd4c940400ec00644882d5cc03b8a73d4d1c94040376dc669882d5cc0bf9cd9aed0c940401363997e892d5cc002f4fbfecdc94040ca4e3fa88b2d5cc0a487a1d5c9c940408f1ce90c8c2d5cc0698995d1c8c94040fab836548c2d5cc0bd175fb4c7c940409f1f46088f2d5cc0962023a0c2c940407b15191d902d5cc068041bd7bfc940400397c79a912d5cc0287d21e4bcc940403201bf46922d5cc065e3c116bbc940409d9d0c8e922d5cc0060fd3beb9c940400ef7915b932d5cc09012bbb6b7c940404fe61f7d932d5cc0e4a08499b6c94040fc71fbe5932d5cc0ea9106b7b5c94040eaec6470942d5cc0c2323674b3c94040601dc70f952d5cc043588d25acc94040aea06989952d5cc03ecf9f36aac94040307f85cc952d5cc0e5eb32fca7c94040dd0a6135962d5cc01b615111a7c9404048a7ae7c962d5cc00a2aaa7ea5c94040f4328ae5962d5cc05eb87361a4c94040c49448a2972d5cc04d81cccea2c940407c80eecb992d5cc06745d4449fc9404035d42824992d5cc01b834e081dca4040",
                  "LINESTRING (-112.712119 33.575919, -112.712127 33.575885)",
                  3.49284983912134e-05);
}

template<>
template<>
void object::test<5>
()
{
    testClearance("POLYGON EMPTY",
                  "LINESTRING EMPTY",
                  geos::DoubleInfinity);
}

template<>
template<>
void object::test<6>
()
{
    input_ = GEOSGeom_createPointFromXY(std::numeric_limits<double>::quiet_NaN(), 1);
    double d;
    ensure_equals(GEOSMinimumClearance(input_, &d), 2);
}

} // namespace tut
