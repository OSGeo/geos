//
// Test Suite for C-API GEOSPolygonize*

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeospolygonize_data : public capitest::utility {
};

typedef test_group<test_capigeospolygonize_data> group;
typedef group::object object;

group test_capigeospolygonize_group("capi::GEOSPolygonize");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    constexpr int size = 2;
    GEOSGeometry* geoms[size] = { nullptr };

    geoms[0] = GEOSGeomFromWKT("LINESTRING(1 3, 3 3, 3 1, 1 1, 1 3)");
    geoms[1] = GEOSGeomFromWKT("LINESTRING(1 3, 3 3, 3 1, 1 1, 1 3)");

    GEOSGeometry* g = GEOSPolygonizer_getCutEdges(geoms, size);

    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), size);

    GEOSGeom_destroy(g);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

template<>
template<>
void object::test<2>
()
{
    constexpr int size = 6;
    GEOSGeometry* geoms[size] = { nullptr };

    // Example from JTS Developer's Guide, Chapter 6 - Polygonization
    geoms[0] = GEOSGeomFromWKT("LINESTRING(0 0, 10 10)"); // isolated edge
    geoms[1] = GEOSGeomFromWKT("LINESTRING(185 221, 100 100)"); // dangling edge
    geoms[2] = GEOSGeomFromWKT("LINESTRING(185 221, 88 275, 180 316)");
    geoms[3] = GEOSGeomFromWKT("LINESTRING(185 221, 292 281, 180 316)");
    geoms[4] = GEOSGeomFromWKT("LINESTRING(189 98, 83 187, 185 221)");
    geoms[5] = GEOSGeomFromWKT("LINESTRING(189 98, 325 168, 185 221)");

    GEOSGeometry* g = GEOSPolygonizer_getCutEdges(geoms, size);

    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), 0);

    GEOSGeom_destroy(g);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

template<>
template<>
void object::test<3>
()
{
    constexpr int size = 2;
    GEOSGeometry* geoms[size];
    geoms[0] = GEOSGeomFromWKT("LINESTRING (100 100, 100 300, 300 300, 300 100, 100 100)");
    geoms[1] = GEOSGeomFromWKT("LINESTRING (150 150, 150 250, 250 250, 250 150, 150 150)");

    // GEOSPolygonize gives us a collection of two polygons
    GEOSGeometry* g = GEOSPolygonize(geoms, size);
    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), 2);
    ensure_equals(GEOSGeomTypeId(g), GEOS_GEOMETRYCOLLECTION);
    GEOSGeom_destroy(g);

    // GEOSPolygonize_valid gives us a single polygon with a hole
    g = GEOSPolygonize_valid(geoms, 2);

    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), 1);
    ensure_equals(GEOSGeomTypeId(g), GEOS_POLYGON);
    GEOSGeom_destroy(g);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

template<>
template<>
void object::test<4>
()
{
    constexpr int size = 1;
    GEOSGeometry* geoms[size];
    geoms[0] = GEOSGeomFromWKT("LINESTRING (0 0, 1 1)");

    GEOSGeometry* g = GEOSPolygonize_valid(geoms, size);

    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), 0);
    ensure_equals(GEOSGeomTypeId(g), GEOS_GEOMETRYCOLLECTION);
    GEOSGeom_destroy(g);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

template<>
template<>
void object::test<5>
()
{
    constexpr int size = 2;
    GEOSGeometry* geoms[size];
    geoms[0] = GEOSGeomFromWKT("LINESTRING (0 0, 1 0, 1 1, 0 1, 0 0)");
    geoms[1] = GEOSGeomFromWKT("LINESTRING (1 1, 2 1, 2 2, 1 2, 1 1)");

    GEOSGeometry* g = GEOSPolygonize_valid(geoms, size);

    ensure(nullptr != g);
    ensure_equals(GEOSGetNumGeometries(g), 2);
    ensure_equals(GEOSGeomTypeId(g), GEOS_MULTIPOLYGON);
    GEOSGeom_destroy(g);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

// Test GEOSPolygonize_full
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("MULTILINESTRING ((0 0, 1 0, 1 1, 0 1, 0 0),  (0 0, 0.5 0.5),  (1 1, 2 2, 1 2, 2 1, 1 1))");

    GEOSGeometry* cuts;
    GEOSGeometry* dangles;
    GEOSGeometry* invalidRings;

    result_ = GEOSPolygonize_full(geom1_, &cuts, &dangles, &invalidRings);

    expected_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION(POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0)))");
    GEOSGeometry* expected_cuts = GEOSGeomFromWKT("GEOMETRYCOLLECTION EMPTY");
    GEOSGeometry* expected_dangles = GEOSGeomFromWKT("GEOMETRYCOLLECTION(LINESTRING (0 0, 0.5 0.5))");
    GEOSGeometry* expected_invalidRings = GEOSGeomFromWKT("GEOMETRYCOLLECTION(LINESTRING (1 1, 2 2, 1 2, 2 1, 1 1))");

    ensure_geometry_equals(result_, expected_);
    ensure_geometry_equals(cuts, expected_cuts);
    ensure_geometry_equals(dangles, expected_dangles);
    ensure_geometry_equals(invalidRings, expected_invalidRings);

    GEOSGeom_destroy(cuts);
    GEOSGeom_destroy(dangles);
    GEOSGeom_destroy(invalidRings);

    GEOSGeom_destroy(expected_cuts);
    GEOSGeom_destroy(expected_dangles);
    GEOSGeom_destroy(expected_invalidRings);
}

template<>
template<>
void object::test<7>
()
{
    constexpr int size = 2;
    GEOSGeometry* geoms[size];
    geoms[0] = GEOSGeomFromWKT("LINESTRING (0 0, 2 0)");
    geoms[1] = GEOSGeomFromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");

    for (auto& geom : geoms) {
        ensure(geom != nullptr);
    }

    GEOSGeometry* g = GEOSPolygonize(geoms, size);

    ensure("curved geometries not supported", g == nullptr);

    for(auto& input : geoms) {
        GEOSGeom_destroy(input);
    }
}

} // namespace tut

