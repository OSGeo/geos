//
// Test Suite for C-API GEOSGeom_extractUniquePoints_r

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosextractuniquepoints_data : public capitest::utility {};

typedef test_group<test_capigeosextractuniquepoints_data> group;
typedef group::object object;

group test_capigeosextractuniquepoints_group("capi::GEOSGeom_extractUniquePoints");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = fromWKT("POLYGON EMPTY");
    // ensure_equals(GEOSGetNumGeometries(geom2_), 0);
    geom3_ = GEOSGeom_extractUniquePoints(geom1_);
    ensure(geom3_ != nullptr);
    ensure(0 != GEOSisEmpty(geom3_));
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = fromWKT("MULTIPOINT((0 0), (0 0), (1 1))");
    geom2_ = fromWKT("MULTIPOINT((0 0), (1 1))");
    /* ensure_equals(GEOSGetNumGeometries(geom2_), 0); */
    geom3_ = GEOSGeom_extractUniquePoints(geom1_);
    ensure(geom3_);
    ensure_geometry_equals(geom3_, geom2_);
}

template<>
template<>
void object::test<3>
()
{
    geom1_ = fromWKT("GEOMETRYCOLLECTION(MULTIPOINT((0 0), (0 0), (1 1)),LINESTRING(1 1, 2 2, 2 2, 0 0),POLYGON((5 5, 0 0, 0 2, 2 2, 5 5)))");
    geom2_ = fromWKT("MULTIPOINT((0 0), (1 1), (2 2), (5 5), (0 2))");
    geom3_ = GEOSGeom_extractUniquePoints(geom1_);
    /* ensure_equals(GEOSGetNumGeometries(geom2_), 0); */
    ensure_geometry_equals(geom3_, geom2_);
}

template <>
template <>
void object::test<4>() {
    input_ = GEOSGeomFromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_);

    result_ = GEOSGeom_extractUniquePoints(input_);

    ensure_geometry_equals(result_, "MULTIPOINT ((0 0), (1 1), (2 0))");
}


} // namespace tut

