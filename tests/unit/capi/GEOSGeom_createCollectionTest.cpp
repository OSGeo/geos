//
// Test Suite for C-API GEOSGeom_createCollection

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeom_createcollection_data : public capitest::utility
{
    GEOSGeometry ** m_geoms;
    unsigned int m_ngeoms;

    test_capigeosgeom_createcollection_data()
        : m_geoms(nullptr)
        , m_ngeoms(0)
    {}

    ~test_capigeosgeom_createcollection_data()
    {
        if (m_geoms) GEOSFree(m_geoms);
        m_geoms = nullptr;
    }
};

#define geom_size 3

typedef test_group<test_capigeosgeom_createcollection_data> group;
typedef group::object object;

group test_capigeosgeom_createcollection_group("capi::GEOSGeom_createCollection");

//
// Test Cases
//

// Create collection from constant length C-array
template<>
template<>
void object::test<1>()
{
    GEOSGeometry* geoms[geom_size];
    geoms[0] = GEOSGeom_createEmptyPoint();
    geoms[1] = GEOSGeom_createEmptyPoint();
    geoms[2] = GEOSGeom_createEmptyPoint();

    // takes ownership of individual geometries
    geom1_ = GEOSGeom_createCollection(GEOS_MULTIPOINT, geoms, geom_size);
    ensure_equals(GEOSGetNumGeometries(geom1_), geom_size);
}

// Create collection from constant length std::array
template<>
template<>
void object::test<2>()
{
    std::array<GEOSGeometry*, geom_size> geoms = {{
        GEOSGeom_createEmptyLineString(),
        GEOSGeom_createEmptyLineString(),
        GEOSGeom_createEmptyLineString()
    }};

    // takes ownership of individual geometries
    geom1_ = GEOSGeom_createCollection(
        GEOS_MULTILINESTRING,
        geoms.data(),
        static_cast<unsigned int>(geoms.size()));

    ensure_equals(GEOSGetNumGeometries(geom1_), geom_size);
}

// Create collection from dynamic length std::vector of geometries
template<>
template<>
void object::test<3>()
{
    std::vector<GEOSGeometry*> geoms;
    geoms.push_back(GEOSGeom_createEmptyPolygon());
    geoms.push_back(GEOSGeom_createEmptyPolygon());
    geoms.push_back(GEOSGeom_createEmptyPolygon());
    geoms.push_back(GEOSGeom_createEmptyPolygon());
    geoms.push_back(GEOSGeom_createEmptyPolygon());

    // takes ownership of individual geometries
    geom1_ = GEOSGeom_createCollection(
        GEOS_MULTIPOLYGON,
        geoms.data(),
        static_cast<unsigned int>(geoms.size()));

    ensure_equals(static_cast<size_t>(GEOSGetNumGeometries(geom1_)), geoms.size());
}

// Error on invalid collection type, ownership is still transferred
template<>
template<>
void object::test<4>()
{
    std::vector<GEOSGeometry*> geoms;
    geoms.push_back(GEOSGeom_createEmptyPolygon());
    // takes ownership of individual geometries
    geom1_ = GEOSGeom_createCollection(
        12345,
        geoms.data(),
        static_cast<unsigned int>(geoms.size()));
    ensure(geom1_ == nullptr);

    geom1_ = GEOSGeom_createEmptyCollection(12345);
    ensure(geom1_ == nullptr);
}

// Release empty collection
template<>
template<>
void object::test<5>()
{
    geom1_ = fromWKT("MULTIPOLYGON EMPTY");

    m_geoms = GEOSGeom_releaseCollection(geom1_, &m_ngeoms);
    ensure(m_geoms == nullptr);
    ensure(m_ngeoms == 0);
}


// Release generic collection
template<>
template<>
void object::test<6>
()
{
    geom1_ = fromWKT("GEOMETRYCOLLECTION(POINT(0 0), POINT(1 1))");

    m_geoms = GEOSGeom_releaseCollection(geom1_, &m_ngeoms);
    ensure(m_geoms != nullptr);
    ensure(m_ngeoms == 2);

    for (size_t i = 0 ; i < m_ngeoms; i++) {
        ensure(GEOSGeomTypeId(m_geoms[i]) == GEOS_POINT);
        GEOSGeom_destroy(m_geoms[i]);
    }
}

// Release typed collection
template<>
template<>
void object::test<7>
()
{
    geom1_ = fromWKT("MULTIPOINT((0 0), (1 1))");

    m_geoms = GEOSGeom_releaseCollection(geom1_, &m_ngeoms);
    ensure(m_geoms != nullptr);
    ensure(m_ngeoms == 2);

    for (size_t i = 0 ; i < m_ngeoms; i++) {
        ensure(GEOSGeomTypeId(m_geoms[i]) == GEOS_POINT);
        GEOSGeom_destroy(m_geoms[i]);
    }
}

// Create MultiCurve
template<>
template<>
void object::test<8>
()
{
    GEOSGeometry* geoms[2];
    geoms[0] = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geoms[1] = fromWKT("LINESTRING (2 0, 3 3)");

    result_ = GEOSGeom_createCollection(GEOS_MULTICURVE, geoms, 2);
    expected_ = fromWKT("MULTICURVE (CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 3 3))");

    ensure_geometry_equals_identical(result_, expected_);
}

// Create MultiSurface
template<>
template<>
void object::test<9>
()
{
    GEOSGeometry* geoms[2];
    geoms[0] = fromWKT("POLYGON ((0 0, 1 0, 1 1, 0 0))");
    geoms[1] = fromWKT("CURVEPOLYGON (CIRCULARSTRING (10 10, 20 10, 15 15, 10 10))");

    result_ = GEOSGeom_createCollection(GEOS_MULTISURFACE, geoms, 2);
    expected_ = fromWKT("MULTISURFACE (((0 0, 1 0, 1 1, 0 0)), CURVEPOLYGON (CIRCULARSTRING (10 10, 20 10, 15 15, 10 10)))");

    ensure_geometry_equals_identical(result_, expected_);
}


} // namespace tut

