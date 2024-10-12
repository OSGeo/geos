// $Id$
//
// Test Suite for C-API GEOSGetCentroid

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capicentroid_data : public capitest::utility {
    test_capicentroid_data() {
        GEOSWKTWriter_setRoundingPrecision(wktw_, 6);
    }
};

typedef test_group<test_capicentroid_data> group;
typedef group::object object;

group test_capicentroid_group("capi::GEOSGetCentroid");

//
// Test Cases
//

// Single point
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POINT(10 0)");

    ensure(nullptr != geom1_);

    geom2_ = GEOSGetCentroid(geom1_);

    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("POINT (10 0)"));

}

// line
template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0)");

    ensure(nullptr != geom1_);

    geom2_ = GEOSGetCentroid(geom1_);

    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("POINT (5 0)"));

}

// polygon
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");

    ensure(nullptr != geom1_);

    geom2_ = GEOSGetCentroid(geom1_);

    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("POINT (5 5)"));

}

// Tiny triangle, see http://trac.osgeo.org/geos/ticket/559
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT(
                 "POLYGON(( \
56.528666666700 25.2101666667, \
56.529000000000 25.2105000000, \
56.528833333300 25.2103333333, \
56.528666666700 25.2101666667))");

    ensure(nullptr != geom1_);

    geom2_ = GEOSGetCentroid(geom1_);

    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("POINT (56.528833 25.210333)"));

}

// Empty geometry -- see http://trac.osgeo.org/geos/ticket/560
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING EMPTY");

    ensure(nullptr != geom1_);

    geom2_ = GEOSGetCentroid(geom1_);

    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("POINT EMPTY"));

}

template<>
template<>
void object::test<6>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 0 0)))");
    ensure(input_ != nullptr);

    result_ = GEOSGetCentroid(input_);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut

