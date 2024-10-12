//
// Test Suite for C-API GEOSCoverageIsValid

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capicoverageisvalid_data : public capitest::utility {

    test_capicoverageisvalid_data() {
    }

    ~test_capicoverageisvalid_data() {
    }

};


typedef test_group<test_capicoverageisvalid_data> group;
typedef group::object object;

group test_capicoverageisvalid_group("capi::GEOSCoverageIsValid");

//
// Test Cases
//

// GEOSCoverageIsValid - all
template<>
template<> void object::test<1>
()
{
    const char* inputWKT = "GEOMETRYCOLLECTION(POLYGON ((100 100, 200 200, 300 100, 200 101, 100 100)), POLYGON ((150 0, 100 100, 200 101, 300 100, 250 0, 150 0)))";
    int allValid;

    input_ = fromWKT(inputWKT);
    allValid = GEOSCoverageIsValid(input_, 0.1, &result_);

    ensure( "valid check is true", allValid > 0 );
    ensure( "result is not null", result_ != nullptr );
    ensure( "type is geometrycollection", GEOSGeomTypeId(result_) == GEOS_GEOMETRYCOLLECTION );

    const char* expectedWKT = "GEOMETRYCOLLECTION(LINESTRING EMPTY, LINESTRING EMPTY)";

    expected_ = fromWKT(expectedWKT);

    // std::cout << toWKT(result_) << std::endl;
    // std::cout << toWKT(expected_) << std::endl;

    ensure_geometry_equals(result_, expected_, 0.01);
}

// GEOSCoverageIsValid - invalid
template<>
template<> void object::test<2>
()
{
    const char* inputWKT = "GEOMETRYCOLLECTION(POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0)), POLYGON ((1 0, 2 0, 2 1, 1 0.9, 1 0)))";
    int allValid;

    input_ = fromWKT(inputWKT);
    allValid = GEOSCoverageIsValid(input_, 0.1, &result_);

    ensure( "valid check is false", allValid == 0 );
    ensure( "result is not null", result_ != nullptr );
    ensure( "type is geometrycollection", GEOSGeomTypeId(result_) == GEOS_GEOMETRYCOLLECTION );

    const char* expectedWKT = "GEOMETRYCOLLECTION (LINESTRING (1 0, 1 1), LINESTRING (2 1, 1 0.9, 1 0))";

    expected_ = fromWKT(expectedWKT);

    // std::cout << toWKT(result_) << std::endl;
    // std::cout << toWKT(expected_) << std::endl;

    ensure_geometry_equals(result_, expected_, 0.01);
}

template<>
template<> void object::test<3>
()
{
    input_ = fromWKT("GEOMETRYCOLLECTION ( "
        "CURVEPOLYGON (COMPOUNDCURVE ( CIRCULARSTRING (2 0, 1 1, 2 2), (2 2, 0 2, 0 0, 2 0))), "
        "CURVEPOLYGON (COMPOUNDCURVE ( CIRCULARSTRING (2 2, 1 1, 2 0), (2 0, 4 0, 4 2, 2 2))))");
    ensure(input_);

    ensure_equals("curved geometry not supported", GEOSCoverageIsValid(input_, 0, nullptr), 2);
}

} // namespace tut
