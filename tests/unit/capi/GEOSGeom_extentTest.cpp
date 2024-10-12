//
// Test Suite for C-API GEOSGeom_getXMin et al

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
struct test_capigeosgeom_extent : public capitest::utility {};

typedef test_group<test_capigeosgeom_extent> group;
typedef group::object object;

group test_capigeosgeom_extent_group("capi::GEOSGeom_extent");

template<>
template<>
void object::test<1>()
{
    geom1_ = fromWKT("LINESTRING (3 8, -12 -4)");

    double xmin, ymin, xmax, ymax;

    ensure(GEOSGeom_getXMin(geom1_, &xmin) != 0);
    ensure(GEOSGeom_getYMin(geom1_, &ymin) != 0);
    ensure(GEOSGeom_getXMax(geom1_, &xmax) != 0);
    ensure(GEOSGeom_getYMax(geom1_, &ymax) != 0);

    ensure_equals(xmin, -12);
    ensure_equals(xmax, 3);
    ensure_equals(ymin, -4);
    ensure_equals(ymax, 8);

    ensure(GEOSGeom_getExtent(geom1_, &xmin, &ymin, &xmax, &ymax) != 0);

    ensure_equals(xmin, -12);
    ensure_equals(xmax, 3);
    ensure_equals(ymin, -4);
    ensure_equals(ymax, 8);
}

template<>
template<>
void object::test<2>()
{
    double d;
    geom1_ = fromWKT("POLYGON EMPTY");

    ensure_equals(GEOSGeom_getXMax(geom1_, &d), 0);
    ensure_equals(GEOSGeom_getYMax(geom1_, &d), 0);
    ensure_equals(GEOSGeom_getXMin(geom1_, &d), 0);
    ensure_equals(GEOSGeom_getYMin(geom1_, &d), 0);

    ensure_equals(GEOSGeom_getExtent(geom1_, &d, &d, &d, &d), 0);
}

} // namespace tut

