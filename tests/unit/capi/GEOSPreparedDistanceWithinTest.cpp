//
// Test Suite for C-API GEOSPreparedDistanceWithin

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/constants.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosprepareddistancewithin_data : public capitest::utility {
    const GEOSPreparedGeometry* pgeom1_;

    test_capigeosprepareddistancewithin_data()
        : pgeom1_(nullptr)
    {}

    ~test_capigeosprepareddistancewithin_data()
    {
        GEOSPreparedGeom_destroy(pgeom1_);
    }

    void checkDistanceWithin(const char* wkt1, const char* wkt2,
                       double dist, char expectedResult)
    {
        geom1_ = GEOSGeomFromWKT(wkt1);
        ensure(nullptr != geom1_);
        pgeom1_ = GEOSPrepare(geom1_);
        ensure(nullptr != pgeom1_);
        geom2_ = GEOSGeomFromWKT(wkt2);
        ensure(nullptr != geom2_);


        int ret = GEOSPreparedDistanceWithin(pgeom1_, geom2_, dist);
        ensure_equals("return code", (int)ret, (int)expectedResult);

    }


};

typedef test_group<test_capigeosprepareddistancewithin_data> group;
typedef group::object object;

group test_capigeosprepareddistancewithin_group("capi::GEOSPreparedDistanceWithin");

//
// Test Cases
//

// Empty inputs
template<>
template<>
void object::test<1>
()
{
    checkDistanceWithin(
        "POLYGON EMPTY",
        "POLYGON EMPTY",
        geos::DoubleInfinity,
        0
    );
}

// Disjoint polygons
template<>
template<>
void object::test<2>
()
{
    checkDistanceWithin(
        "POLYGON((1 1,1 5,5 5,5 1,1 1))",
        "POLYGON((8 8, 9 9, 9 10, 8 8))",
        4.25,
        1
    );

}

// Point contained in polygon
template<>
template<>
void object::test<3>
()
{
    checkDistanceWithin(
        "POLYGON((1 1,1 5,5 5,5 1,1 1))",
        "POINT(2 2)",
        0,
        1
    );
}

// Disjoint line and point
template<>
template<>
void object::test<4>
()
{
    checkDistanceWithin(
        "LINESTRING(1 5,5 5,5 1,1 1)",
        "POINT(2 2)",
        1,
        1
    );
}

// Intersecting lines
template<>
template<>
void object::test<5>
()
{
    checkDistanceWithin(
        "LINESTRING(0 0,10 10)",
        "LINESTRING(0 10,10 0)",
        0,
        1
    );
}

// Intersecting polygon and line
template<>
template<>
void object::test<6>
()
{
    checkDistanceWithin(
        "POLYGON((0 0,10 0,10 10,0 10,0 0))",
        "LINESTRING(8 5,12 5)",
        0,
        1
    );
}

// Empty geometries
template<>
template<>
void object::test<7>
()
{
    checkDistanceWithin(
        "LINESTRING EMPTY",
        "POINT EMPTY",
        geos::DoubleInfinity,
        0
    );
}

// Empty geometries
template<>
template<>
void object::test<8>
()
{
    checkDistanceWithin(
        "POINT EMPTY",
        "LINESTRING EMPTY",
        geos::DoubleInfinity,
        0
    );
}

// Mixed empty and non-empty
template<>
template<>
void object::test<9>
()
{
    checkDistanceWithin(
        "POINT EMPTY",
        "POINT(0 0)",
        geos::DoubleInfinity,
        0
    );
}

// Mixed empty and non-empty
template<>
template<>
void object::test<10>
()
{
    checkDistanceWithin(
        "LINESTRING(0 0, 10 0)",
        "POLYGON EMPTY",
        geos::DoubleInfinity,
        0
    );
}

// Prepared geometry contained in test geometry
template<>
template<>
void object::test<11>
()
{
    checkDistanceWithin(
        "POLYGON((1 1,1 5,5 5,5 1,1 1))",
        "POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))",
        0,
        1
    );
}

// Prepared line within envelope of test line
// see https://github.com/libgeos/geos/issues/958
template<>
template<>
void object::test<12>
()
{
    checkDistanceWithin(
        "LINESTRING (2 2, 3 3, 4 4, 5 5, 6 6, 7 7)",
        "LINESTRING (0 0, 1 1, 2 2, 3 3, 4 4, 5 5, 6 6, 7 7, 8 8, 9 9)",
        1,
        1
    );
}

// Prepared line within test geometry
// see https://github.com/libgeos/geos/issues/960
template<>
template<>
void object::test<13>
()
{
    checkDistanceWithin(
        "LINESTRING (30 30, 70 70)",
        "POLYGON ((0 100, 100 100, 100 0, 0 0, 0 100))",
        1,
        1
    );
}

// Prepared multiline with one element within Polygon
template<>
template<>
void object::test<14>
()
{
    checkDistanceWithin(
        "MULTILINESTRING ((30 30, 70 70), (170 200, 200 170))",
        "POLYGON ((0 100, 100 100, 100 0, 0 0, 0 100))",
        1,
        1
    );
}

// Prepared multiline with one element within MultiPolygon.
template<>
template<>
void object::test<15>
()
{
    checkDistanceWithin(
        "MULTILINESTRING ((1 6, 1 1), (15 16, 15 14))",
        "MULTIPOLYGON (((10 20, 20 20, 20 10, 10 10, 10 20)), ((30 20, 40 20, 40 10, 30 10, 30 20)))",
        1,
        1
    );
}

// Indexed multiline with one element within line envelope.
template<>
template<>
void object::test<16>()
{
    checkDistanceWithin(
        "MULTILINESTRING ((1 6, 1 1), (11 14, 11 11))",
        "LINESTRING (10 10, 10 20, 30 20)",
        2,
        1
    );
}

} // namespace tut

