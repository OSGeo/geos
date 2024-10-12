// $Id$
//
// Test Suite for C-API GEOSMaximumInscribedCircle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capimaximuminscribedcircle_data : public capitest::utility {
    char* wkt_;

    test_capimaximuminscribedcircle_data()
        : wkt_(nullptr)
    {
        GEOSWKTWriter_setRoundingPrecision(wktw_, 8);
    }

    ~test_capimaximuminscribedcircle_data()
    {
        GEOSFree(wkt_);
    }

};

typedef test_group<test_capimaximuminscribedcircle_data> group;
typedef group::object object;

group test_capimaximuminscribedcircle_group("capi::GEOSMaximumInscribedCircle");

//
// Test Cases
//

// Square
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON ((100 200, 200 200, 200 100, 100 100, 100 200))");
    ensure(nullptr != geom1_);
    geom2_ = GEOSMaximumInscribedCircle(geom1_, 0.001);
    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);

    ensure_equals(std::string(wkt_), std::string("LINESTRING (150 150, 150 200)"));
}

// Crash with Inf coords
// https://github.com/libgeos/geos/issues/821
template<>
template<>
void object::test<2>
()
{
    std::string wkb("0106000020E61000000100000001030000000100000005000000000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F");
    geom1_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(wkb.c_str()), wkb.size());

    result_ = GEOSMaximumInscribedCircle(geom1_, 1);

    // no crash
}

template<>
template<>
void object::test<3>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING (0 0, 10 10, 20 0), (20 0, 0 0)))");
    ensure(input_);

    result_ = GEOSMaximumInscribedCircle(input_, 1);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut

