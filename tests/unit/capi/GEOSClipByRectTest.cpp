//
// Test Suite for C-API GEOSClipByRect

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
struct test_capigeosclipbyrect_data : public capitest::utility {

    void
    checkClipByRect(
        const char* wktInput,
        double xmin, double ymin,
        double xmax, double ymax,
        const char* wktExpected)
    {
        input_ = fromWKT(wktInput);
        result_ = GEOSClipByRect(input_, xmin, ymin, xmax, ymax);
        expected_ = fromWKT(wktExpected);

        bool equal = GEOSEquals(result_, expected_) == 1;
        if (!equal) {
            wkt_ = GEOSWKTWriter_write(wktw_, expected_);
            std::printf("EXP: %s\n", wkt_);
            GEOSFree(wkt_);
            wkt_ = GEOSWKTWriter_write(wktw_, result_);
            std::printf("OBT: %s\n", wkt_);
        }
        ensure(equal);
    }

    void
    checkClipByRectIdentical(
        const char* wktInput,
        double xmin, double ymin,
        double xmax, double ymax,
        const char* wktExpected)
    {
        input_ = fromWKT(wktInput);
        result_ = GEOSClipByRect(input_, xmin, ymin, xmax, ymax);
        expected_ = fromWKT(wktExpected);

        bool equal =  GEOSEqualsIdentical(result_, expected_) == 1;
        if (!equal) {
            wkt_ = GEOSWKTWriter_write(wktw_, expected_);
            std::printf("EXP: %s\n", wkt_);
            GEOSFree(wkt_);
            wkt_ = GEOSWKTWriter_write(wktw_, result_);
            std::printf("OBT: %s\n", wkt_);
        }
        ensure(equal);
    }
};

typedef test_group<test_capigeosclipbyrect_data> group;
typedef group::object object;

group test_capigeosclipbyrect_group("capi::GEOSClipByRect");

//
// Test Cases
//

/// Point outside
template<>
template<>
void object::test<1>()
{
    checkClipByRect(
        "POINT(0 0)",
        10, 10, 20, 20,
        "POINT EMPTY"
        );
}

/// Point inside
template<>
template<>
void object::test<2>()
{
    checkClipByRectIdentical(
        "POINT(15 15)",
        10, 10, 20, 20,
        "POINT(15 15)"
        );
}

/// Point on boundary
template<>
template<>
void object::test<3>()
{
    checkClipByRect(
        "POINT(15 10)",
        10, 10, 20, 20,
        "POINT EMPTY"
        );
}

/// Line outside
template<>
template<>
void object::test<4>()
{
    checkClipByRect(
        "LINESTRING(0 0, -5 5)",
        10, 10, 20, 20,
        "LINESTRING EMPTY"
        );
}

/// Line inside
template<>
template<>
void object::test<5>()
{
    checkClipByRectIdentical(
        "LINESTRING(15 15, 16 15)",
        10, 10, 20, 20,
        "LINESTRING(15 15, 16 15)"
        );
}

/// Line on boundary
template<>
template<>
void object::test<6>()
{
    checkClipByRect(
        "LINESTRING(10 15, 10 10, 15 10)",
        10, 10, 20, 20,
        "LINESTRING EMPTY"
        );
}

/// Line splitting rectangle
template<>
template<>
void object::test<7>()
{
    checkClipByRectIdentical(
        "LINESTRING(10 5, 25 20)",
        10, 10, 20, 20,
        "LINESTRING (15 10, 20 15)"
        );
}

/// Polygon shell (CCW) fully on rectangle boundary
template<>
template<>
void object::test<8>()
{
    checkClipByRect(
        "POLYGON((10 10, 20 10, 20 20, 10 20, 10 10))",
        10, 10, 20, 20,
        "POLYGON((10 10, 20 10, 20 20, 10 20, 10 10))"
        );
}

/// Polygon shell (CW) fully on rectangle boundary
template<>
template<>
void object::test<9>()
{
    checkClipByRect(
        "POLYGON((10 10, 10 20, 20 20, 20 10, 10 10))",
        10, 10, 20, 20,
        "POLYGON((10 10, 20 10, 20 20, 10 20, 10 10))"
        );
}

/// Polygon hole (CCW) fully on rectangle boundary
template<>
template<>
void object::test<10>()
{
    checkClipByRect(
        "POLYGON((0 0, 0 30, 30 30, 30 0, 0 0),(10 10, 20 10, 20 20, 10 20, 10 10))",
        10, 10, 20, 20,
        "POLYGON EMPTY"
        );
}

/// Polygon hole (CW) fully on rectangle boundary
template<>
template<>
void object::test<11>()
{
    checkClipByRect(
        "POLYGON((0 0, 0 30, 30 30, 30 0, 0 0),(10 10, 10 20, 20 20, 20 10, 10 10))",
        10, 10, 20, 20,
        "POLYGON EMPTY"
        );
}

/// Polygon fully within rectangle
template<>
template<>
void object::test<12>()
{
    const char* wkt = "POLYGON((1 1, 1 30, 30 30, 30 1, 1 1),(10 10, 20 10, 20 20, 10 20, 10 10))";
    checkClipByRectIdentical(
        wkt,
        0, 0, 40, 40,
        wkt);
}

/// Polygon overlapping rectangle
template<>
template<>
void object::test<13>()
{
    checkClipByRectIdentical(
        "POLYGON((0 0, 0 30, 30 30, 30 0, 0 0),(10 10, 20 10, 20 20, 10 20, 10 10))",
        5, 5, 15, 15,
        "POLYGON ((5 5, 5 15, 10 15, 10 10, 15 10, 15 5, 5 5))"
        );
}

/// Clipping invalid polygon
template<>
template<>
void object::test<14>
()
{
    geom1_ = fromWKT("POLYGON((1410 2055, 1410 2056, 1410 2057, 1410 2055))");
    geom2_ = GEOSClipByRect(geom1_, -8, -8, 2056, 2056);
    if (geom2_ != nullptr) {
        wkt_ = GEOSWKTWriter_write(wktw_, geom2_);
        std::printf("OBT: %s\n", wkt_);
    }
    ensure(nullptr == geom2_);
}

// Polygon fully covering rectangle
// https://trac.osgeo.org/postgis/ticket/4904
template<>
template<>
void object::test<15>
()
{
    //  POLYGON((0 0,10 0,10 10,0 10))
    //  Clip by ST_MakeEnvelope(2,2,5,5)
    GEOSCoordSequence *cs = GEOSCoordSeq_create(5, 2);
    GEOSCoordSeq_setXY(cs, 0,  0,  0);
    GEOSCoordSeq_setXY(cs, 1, 10,  0);
    GEOSCoordSeq_setXY(cs, 2, 10, 10);
    GEOSCoordSeq_setXY(cs, 3,  0, 10);
    GEOSCoordSeq_setXY(cs, 4,  0,  0);
    GEOSGeometry *shell = GEOSGeom_createLinearRing(cs); // take ownership of cs
    input_ = GEOSGeom_createPolygon(shell, NULL, 0); // take ownership of shell
    result_ = GEOSClipByRect(input_, 2, 2, 5, 5);
    expected_ = GEOSGeomFromWKT("POLYGON ((2 2, 2 5, 5 5, 5 2, 2 2))");
    ensure_geometry_equals(result_, expected_);
}

/// Empty combinations - always return GEOMETRYCOLLECTION EMPTY
template<>
template<>
void object::test<16>
()
{
    std::vector<const char*> variants{
        "POINT EMPTY",
        "LINESTRING EMPTY",
        "POLYGON EMPTY",
        "MULTIPOINT EMPTY",
        "MULTILINESTRING EMPTY",
        "MULTIPOLYGON EMPTY",
        "GEOMETRYCOLLECTION EMPTY",
        "LINEARRING EMPTY",
    };
    for (const auto& wkt : variants) {
        GEOSGeometry* a = fromWKT(wkt);
        GEOSGeometry* b = GEOSClipByRect(a, 0, 0, 1, 1);
        ensure("GEOS_GEOMETRYCOLLECTION", GEOSGeomTypeId(b) == GEOS_GEOMETRYCOLLECTION);
        GEOSGeom_destroy(a);
        GEOSGeom_destroy(b);
    }
}

template<>
template<>
void object::test<17>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_ != nullptr);

    result_ = GEOSClipByRect(input_, 0, 0, 1, 1);
    ensure(result_ == nullptr);
}

/// Point Z inside
template<>
template<>
void object::test<18>()
{
    checkClipByRectIdentical(
        "POINT Z (15 15 100)",
        10, 10, 20, 20,
        "POINT Z (15 15 100)"
        );
}

/// Line Z inside
template<>
template<>
void object::test<19>()
{
    checkClipByRectIdentical(
        "LINESTRING Z (15 15 0, 16 15 100)",
        10, 10, 20, 20,
        "LINESTRING Z (15 15 0, 16 15 100)"
        );
}

/// Line Z splitting rectangle
template<>
template<>
void object::test<20>()
{
    checkClipByRectIdentical(
        "LINESTRING Z (0 15 0, 100 15 100)",
        10, 10, 20, 20,
        "LINESTRING Z (10 15 10, 20 15 20)"
        );
}

/// Polygon Z overlapping rectangle
template<>
template<>
void object::test<21>()
{
    checkClipByRectIdentical(
        "POLYGON Z ((0 0 100, 0 30 100, 30 30 100, 30 0 100, 0 0 100),(10 10 100, 20 10 100, 20 20 100, 10 20 100, 10 10 100))",
        5, 5, 15, 15,
        "POLYGON Z ((5 5 100, 5 15 100, 10 15 100, 10 10 100, 15 10 100, 15 5 100, 5 5 100))"
        );
}

/// Polygon Z enclosing rectangle
template<>
template<>
void object::test<22>()
{
    checkClipByRectIdentical(
        "POLYGON Z ((0 0 100, 0 30 100, 30 30 100, 30 0 100, 0 0 100))",
        5, 5, 15, 15,
        "POLYGON Z ((5 5 100, 5 15 100, 15 15 100, 15 5 100, 5 5 100))"
        );
}


} // namespace tut

