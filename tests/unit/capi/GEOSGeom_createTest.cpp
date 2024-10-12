//
// Test Suite for C-API GEOSGeom_createPolygon

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdio>
#include <cstdlib>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeom_create_data  : public capitest::utility
{
    GEOSGeometry* geom1_;

    test_capigeosgeom_create_data()
        : geom1_(nullptr)
    {
    }

    ~test_capigeosgeom_create_data()
    {
        GEOSGeom_destroy(geom1_);
        geom1_ = nullptr;
    }

};

typedef test_group<test_capigeosgeom_create_data> group;
typedef group::object object;

group test_capigeosgeom_create_group("capi::GEOSGeom_create");

//
// Test Cases
//

// EMPTY point
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeom_createEmptyPoint();
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_POINT);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

// EMPTY linestring
template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeom_createEmptyLineString();
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_LINESTRING);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}


// EMPTY polygon
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeom_createEmptyPolygon();
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_POLYGON);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

// EMPTY multipoint
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeom_createEmptyCollection(GEOS_MULTIPOINT);
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_MULTIPOINT);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

// EMPTY multilinestring
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeom_createEmptyCollection(GEOS_MULTILINESTRING);
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_MULTILINESTRING);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

// EMPTY multipolygon
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeom_createEmptyCollection(GEOS_MULTIPOLYGON);
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_MULTIPOLYGON);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

// EMPTY collection
template<>
template<>
void object::test<7>
()
{
    geom1_ = GEOSGeom_createEmptyCollection(GEOS_GEOMETRYCOLLECTION);
    ensure(0 != GEOSisEmpty(geom1_));
    ensure_equals(GEOSGeomTypeId(geom1_), GEOS_GEOMETRYCOLLECTION);

    ensure_equals(GEOSHasZ(geom1_), 0);
    ensure_equals(GEOSHasM(geom1_), 0);

    GEOSGeom_destroy(geom1_);
    geom1_ = nullptr;
}

template<>
template<>
void object::test<8>()
{
    input_ = GEOSGeom_createLineString(NULL);
    ensure_equals(GEOSHasZ(input_), 0);
    ensure_equals(GEOSHasM(input_), 0);
}

} // namespace tut

