//
// Test Suite for C-API GEOSDelaunayTriangulation

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosdelaunaytriangulation_data : public capitest::utility {
    test_capigeosdelaunaytriangulation_data() {
    }
};

typedef test_group<test_capigeosdelaunaytriangulation_data> group;
typedef group::object object;

group test_capigeosdelaunaytriangulation_group("capi::GEOSDelaunayTriangulation");

//
// Test Cases
//

// Empty polygon
template<>
template<>
void object::test<1>
()
{
    geom1_ = fromWKT("POLYGON EMPTY");

    ensure_equals(GEOSisEmpty(geom1_), 1);

    geom2_ = GEOSDelaunayTriangulation(geom1_, 0, 0);
    ensure (geom2_ != nullptr);
    ensure_equals(GEOSisEmpty(geom2_), 1);
    ensure_equals(GEOSGeomTypeId(geom2_), GEOS_GEOMETRYCOLLECTION);

    geom3_ = GEOSDelaunayTriangulation(geom1_, 0, 1);
    ensure_equals(GEOSisEmpty(geom3_), 1);
    ensure_equals(GEOSGeomTypeId(geom3_), GEOS_MULTILINESTRING);
}

// Single point
template<>
template<>
void object::test<2>()
{
    geom1_ = fromWKT("POINT(0 0)");

    geom2_ = GEOSDelaunayTriangulation(geom1_, 0, 0);
    ensure_equals(GEOSisEmpty(geom2_), 1);
    ensure_equals(GEOSGeomTypeId(geom2_), GEOS_GEOMETRYCOLLECTION);

    geom3_ = GEOSDelaunayTriangulation(geom1_, 0, 1);
    ensure_equals(GEOSisEmpty(geom3_), 1);
    ensure_equals(GEOSGeomTypeId(geom3_), GEOS_MULTILINESTRING);
}

// Three collinear points
template<>
template<>
void object::test<3>()
{
    geom1_ = fromWKT("MULTIPOINT((0 0), (5 0), (10 0))");

    geom2_ = GEOSDelaunayTriangulation(geom1_, 0, 0);
    ensure_equals(GEOSisEmpty(geom2_), 1);
    ensure_equals(GEOSGeomTypeId(geom2_), GEOS_GEOMETRYCOLLECTION);

    geom3_ = GEOSDelaunayTriangulation(geom1_, 0, 1);
    ensure_geometry_equals(geom3_, "MULTILINESTRING ((5 0, 10 0), (0 0, 5 0))");
}

// Three points
template<>
template<>
void object::test<4>()
{
    geom1_ = fromWKT("MULTIPOINT((0 0), (5 0), (10 10))");
    geom2_ = GEOSDelaunayTriangulation(geom1_, 0, 0);
    ensure(geom2_ != nullptr);
    ensure_geometry_equals(geom2_, "GEOMETRYCOLLECTION (POLYGON ((0 0, 10 10, 5 0, 0 0)))");

    geom3_ = GEOSDelaunayTriangulation(geom1_, 0, 1);
    ensure_geometry_equals(geom3_, "MULTILINESTRING ((5 0, 10 10), (0 0, 10 10), (0 0, 5 0))");
}

// A polygon with a hole
template<>
template<>
void object::test<5>
()
{
    geom1_ = fromWKT("POLYGON((0 0, 8.5 1, 10 10, 0.5 9, 0 0),(2 2, 3 8, 7 8, 8 2, 2 2))");

    geom2_ = GEOSDelaunayTriangulation(geom1_, 0, 0);
    ensure(geom2_ != nullptr);
    ensure_geometry_equals(geom2_,
                      "GEOMETRYCOLLECTION (POLYGON ((8 2, 10 10, 8.5 1, 8 2)), POLYGON ((7 8, 10 10, 8 2, 7 8)), POLYGON ((3 8, 10 10, 7 8, 3 8)), POLYGON ((2 2, 8 2, 8.5 1, 2 2)), POLYGON ((2 2, 7 8, 8 2, 2 2)), POLYGON ((2 2, 3 8, 7 8, 2 2)), POLYGON ((0.5 9, 10 10, 3 8, 0.5 9)), POLYGON ((0.5 9, 3 8, 2 2, 0.5 9)), POLYGON ((0 0, 2 2, 8.5 1, 0 0)), POLYGON ((0 0, 0.5 9, 2 2, 0 0)))");

    geom3_ = GEOSDelaunayTriangulation(geom1_, 0, 1);
    ensure_geometry_equals(geom3_,
                      "MULTILINESTRING ((8.5 1, 10 10), (8 2, 10 10), (8 2, 8.5 1), (7 8, 10 10), (7 8, 8 2), (3 8, 10 10), (3 8, 7 8), (2 2, 8.5 1), (2 2, 8 2), (2 2, 7 8), (2 2, 3 8), (0.5 9, 10 10), (0.5 9, 3 8), (0.5 9, 2 2), (0 0, 8.5 1), (0 0, 2 2), (0 0, 0.5 9))");
}

// Four points with a tolerance making one collapse
template<>
template<>
void object::test<6>
()
{
    geom1_ = fromWKT("MULTIPOINT((0 0), (10 0), (10 10), (11 10))");

    geom2_ = GEOSDelaunayTriangulation(geom1_, 2, 1);
    ensure_geometry_equals(geom2_,
                      "MULTILINESTRING ((10 0, 10 10), (0 0, 10 10), (0 0, 10 0))");
}

template<>
template<>
void object::test<7>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0, 3 -1, 4 0)");
    ensure(input_);

    result_ = GEOSDelaunayTriangulation(input_, 0, 0);
    ensure("curved geometry not supported", result_ == nullptr);
}

} // namespace tut

