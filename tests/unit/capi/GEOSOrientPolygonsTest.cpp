//
// Test Suite for C-API GEOSOrientationPolygons

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosorientpolygons_data : public capitest::utility {};

typedef test_group<test_capigeosorientpolygons_data> group;
typedef group::object object;

group test_capigeosorientpolygons_group("capi::GEOSOrientPolygons");

// empty polygon is passed through
template<>
template<>
void object::test<1>()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
    ensure_equals(GEOSOrientPolygons(geom1_, 1), 0);

    ensure_equals(toWKT(geom1_), "POLYGON EMPTY");
}

// hole orientation is opposite to shell
template<>
template<>
void object::test<2>()
{
    geom1_ = GEOSGeomFromWKT("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1))");

    ensure_equals(GEOSOrientPolygons(geom1_, 0), 0);
    ensure_equals(toWKT(geom1_), "POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1))");

    ensure_equals(GEOSOrientPolygons(geom1_, 1), 0);
    ensure_equals(toWKT(geom1_), "POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1))");
}

// all polygons in collection are processed
template<>
template<>
void object::test<3>()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1)), ((100 100, 200 100, 200 200, 100 100)))");

    ensure_equals(GEOSOrientPolygons(geom1_, 0), 0);
    ensure_equals(toWKT(geom1_), "MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1)), ((100 100, 200 100, 200 200, 100 100)))");

    ensure_equals(GEOSOrientPolygons(geom1_, 1), 0);
    ensure_equals(toWKT(geom1_), "MULTIPOLYGON (((0 0, 0 10, 10 10, 10 0, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1)), ((100 100, 200 200, 200 100, 100 100)))");
}

// polygons in collection are oriented, closed linestring unchanged
template<>
template<>
void object::test<4>()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1)), LINESTRING (100 100, 200 100, 200 200, 100 100))");

    ensure_equals(GEOSOrientPolygons(geom1_, 1), 0);
    ensure_equals(toWKT(geom1_), "GEOMETRYCOLLECTION (POLYGON ((0 0, 0 10, 10 10, 10 0, 0 0), (1 1, 2 1, 2 2, 1 2, 1 1)), LINESTRING (100 100, 200 100, 200 200, 100 100))");
}

// nested collection handled correctly
template<>
template<>
void object::test<5>()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (GEOMETRYCOLLECTION (MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0)))))");

    ensure_equals(GEOSOrientPolygons(geom1_, 0), 0);
    ensure_equals(toWKT(geom1_), "GEOMETRYCOLLECTION (GEOMETRYCOLLECTION (MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0)))))");

    ensure_equals(GEOSOrientPolygons(geom1_, 1), 0);
    ensure_equals(toWKT(geom1_), "GEOMETRYCOLLECTION (GEOMETRYCOLLECTION (MULTIPOLYGON (((0 0, 0 10, 10 10, 10 0, 0 0)))))");
}

template<>
template<>
void object::test<6>()
{
    input_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE( CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 0 0)))");
    ensure(input_);

    auto ret = GEOSOrientPolygons(input_, 0);
    ensure_equals("curved geometries not supported", ret, -1);
}

} // namespace tut

