//
// Test Suite for C-API GEOSintersection

#include <tut/tut.hpp>

// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosintersectionprec_data : public capitest::utility {
    test_capigeosintersectionprec_data() {
    }

    static int
    same(GEOSGeometry* g1, GEOSGeometry* g2, double tolerance)
    {
        GEOSNormalize(g1);
        GEOSNormalize(g2);
        return GEOSEqualsExact(g1, g2, tolerance);
    }
};

typedef test_group<test_capigeosintersectionprec_data> group;
typedef group::object object;

group test_capigeosintersectionprec_group("capi::GEOSIntersectionPrec");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
    geom2_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);
    ensure(nullptr != geom3_);
    ensure_equals(toWKT(geom3_), std::string("POLYGON EMPTY"));
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((1 1,1 5,5 5,5 1,1 1))");
    geom2_ = GEOSGeomFromWKT("POINT(2 2)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);
    ensure(nullptr != geom3_);
    ensure_equals(toWKT(geom3_), std::string("POINT (2 2)"));
}

template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOLYGON(((0 0,0 10,10 10,10 0,0 0)))");
    geom2_ = GEOSGeomFromWKT("POLYGON((-1 1,-1 2,2 2,2 1,-1 1))");
    expected_ = GEOSGeomFromWKT("POLYGON ((0 1, 0 2, 2 2, 2 1, 0 1))");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);

    ensure(nullptr != geom3_);
    ensure(same(geom3_, expected_, 0.1));
}

/* See http://trac.osgeo.org/geos/ticket/719 */
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOLYGON(((0 0,5 10,10 0,0 0),(1 1,1 2,2 2,2 1,1 1),(100 100,100 102,102 102,102 100,100 100)))");
    geom2_ = GEOSGeomFromWKT("POLYGON((0 1,0 2,10 2,10 1,0 1))");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);
    GEOSNormalize(geom3_);

    ensure(nullptr != geom3_);

    expected_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (LINESTRING (1 2, 2 2), LINESTRING (2 1, 1 1), POLYGON ((0.5 1, 1 2, 1 1, 0.5 1)), POLYGON ((9 2, 9.5 1, 2 1, 2 2, 9 2)))");
    GEOSNormalize(expected_);
    ensure(GEOSEqualsExact(expected_, geom3_, 0.00001));
}

template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0)");
    geom2_ = GEOSGeomFromWKT("LINESTRING(0 1, 10 1)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);
    ensure(nullptr != geom3_);
    ensure_equals(toWKT(geom3_), std::string("LINESTRING EMPTY"));
}

template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0)");
    geom2_ = GEOSGeomFromWKT("LINESTRING(0 1, 10 1)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 10);
    ensure(nullptr != geom3_);
    ensure_equals(toWKT(geom3_), std::string("LINESTRING (0 0, 10 0)"));
}

template<>
template<>
void object::test<7>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0)");
    geom2_ = GEOSGeomFromWKT("LINESTRING(0 1, 10 0)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 0);
    ensure(nullptr != geom3_);
    ensure_equals(toWKT(geom3_), std::string("POINT (10 0)"));
}

template<>
template<>
void object::test<8>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 0)");
    geom2_ = GEOSGeomFromWKT("LINESTRING(9 0, 12 0, 12 20, 4 0, 2 0, 2 10, 0 10, 0 -10)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    geom3_ = GEOSIntersectionPrec(geom1_, geom2_, 2);
    ensure(nullptr != geom3_);

    ensure_geometry_equals(geom3_, "GEOMETRYCOLLECTION (LINESTRING (2 0, 4 0), POINT (0 0), POINT (10 0))");
}

} // namespace tut

