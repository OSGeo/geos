//
// Test Suite for C-API GEOSOffsetCurve

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capioffsetcurve_data : public capitest::utility {
    test_capioffsetcurve_data() {
    }

    void debugOutput (const char* label, GEOSGeometry* geom)
    {
        if (geom) {
            char* wkt_c = GEOSWKTWriter_write(wktw_, geom);
            printf("%s: %s\n", label, wkt_c);
            if (wkt_c) GEOSFree(wkt_c);
        }
        else {
            printf("%s: NULL\n", label);
        }
    }

    void checkOffset(
        const char* wkt, const char* expected,
        double width, int quadSegs, int joinStyle, double mitreLimit,
        double checkTolerance)
    {
        static int debug = 0;
        // input
        geom1_ = GEOSGeomFromWKT(wkt);
        ensure(nullptr != geom1_);
        if (debug) debugOutput("Input", geom1_);

        // result
        geom2_ = GEOSOffsetCurve(geom1_, width, quadSegs, joinStyle, mitreLimit);
        ensure(nullptr != geom2_);
        if (debug) debugOutput("Result", geom2_);

        // expected
        if (expected) {
            geom3_ = GEOSGeomFromWKT(expected);
            if (debug) debugOutput("Expected", geom3_);
            ensure(nullptr != geom3_);
            ensure_geometry_equals(geom2_, geom3_, checkTolerance);
        }
    }

};

typedef test_group<test_capioffsetcurve_data> group;
typedef group::object object;

group test_capioffsetcurve_group("capi::GEOSOffsetCurve");

//
// Test Cases
//

// Straight, left
template<>
template<>
void object::test<1>()
{
    checkOffset(
        "LINESTRING(0 0, 10 0)",
        "LINESTRING (0 2, 10 2)",
        2, 0, GEOSBUF_JOIN_ROUND, 2,
        0.000001
        );
}

// Straight, right
template<>
template<>
void object::test<2>()
{
     checkOffset(
        "LINESTRING(0 0, 10 0)",
        "LINESTRING (10 -2, 0 -2)",
        -2, 0, GEOSBUF_JOIN_ROUND, 2,
        0.000001
        );
}

// Outside curve
template<>
template<>
void object::test<3>()
{
     checkOffset(
        "LINESTRING(0 0, 10 0, 10 10)",
        "LINESTRING (0 -2, 10 -2, 10.3901806 -1.9615705, 10.76536686 -1.8477590, 11.11114046 -1.66293922, 11.41421356 -1.41421356, 11.66293922 -1.11114046, 11.84775906 -0.76536686, 11.96157056 -0.3901806, 12 0, 12 10)",
        -2, 1, GEOSBUF_JOIN_ROUND, 2,
        0.000001
        );
}

// Inside curve
template<>
template<>
void object::test<4>()
{
    checkOffset(
        "LINESTRING(0 0, 10 0, 10 10)",
        "LINESTRING (0 2, 8 2, 8 10)",
        2, 1, GEOSBUF_JOIN_ROUND, 2,
        0.000001
        );
}

// See http://trac.osgeo.org/postgis/ticket/413
template<>
template<>
void object::test<5>()
{
    checkOffset(
        "LINESTRING(33282908 6005055,33282900 6005050,33282892 6005042,33282876 6005007,33282863 6004982,33282866 6004971,33282876 6004975,33282967 6005018,33282999 6005031)",
        // Old algorithm
        // "LINESTRING EMPTY",
        "LINESTRING (33282951.601378817 6005059.236579252, 33282982.439409934 6005071.764529393)",
        44, 1, GEOSBUF_JOIN_MITRE, 1,
        0.000001
        );
}

// 0 distance
// See http://trac.osgeo.org/postgis/ticket/454
template<>
template<>
void object::test<6>()
{
    checkOffset(
        "LINESTRING(0 0, 10 0)",
        "LINESTRING (0 0, 10 0)",
        0, 0, GEOSBUF_JOIN_ROUND, 2,
        0.000001
        );
}

// left-side and right-side curve
// See http://trac.osgeo.org/postgis/ticket/633
template<>
template<>
void object::test<7>()
{
    const char* wkt0 = "LINESTRING ("
                     "665.7317504882812500 133.0762634277343700,"
                     "1774.4752197265625000 19.9391822814941410,"
                     "756.2413940429687500 466.8306579589843700,"
                     "626.1337890625000000 1898.0147705078125000,"
                     "433.8007202148437500 404.6052856445312500)";

    double width = 57.164000837203;

    // left-sided
    checkOffset(
        wkt0,
        NULL,
        width, 8, GEOSBUF_JOIN_MITRE, 5.57,
        0.000001
        );

    ensure(GEOSGeomGetNumPoints(geom2_) >= GEOSGeomGetNumPoints(geom1_));
}

// left-side and right-side curve
// See http://trac.osgeo.org/postgis/ticket/633
template<>
template<>
void object::test<8>()
{
    const char* wkt0 = "LINESTRING ("
                     "665.7317504882812500 133.0762634277343700,"
                     "1774.4752197265625000 19.9391822814941410,"
                     "756.2413940429687500 466.8306579589843700,"
                     "626.1337890625000000 1898.0147705078125000,"
                     "433.8007202148437500 404.6052856445312500)";

    double width = 57.164000837203;

    // right-sided
    checkOffset(
        wkt0,
        NULL,
        -1 * width, 8, GEOSBUF_JOIN_MITRE, 5.57,
        0.000001
        );

    ensure(GEOSGeomGetNumPoints(geom2_) >= GEOSGeomGetNumPoints(geom1_));
}

// Test duplicated inner vertex in input
// See http://trac.osgeo.org/postgis/ticket/602
template<>
template<>
void object::test<9>()
{
    checkOffset(
        "LINESTRING(0 0,0 10,0 10,10 10)",
        "LINESTRING (10 9, 1 9, 1 0)",
        -1, 8, GEOSBUF_JOIN_ROUND, 0,
        0.000001
        );
}

// Test duplicated final vertex in input
// See http://trac.osgeo.org/postgis/ticket/602
template<>
template<>
void object::test<10>()
{
    checkOffset(
        "LINESTRING(0 0,0 10,0 10)",
        "LINESTRING (1 10, 1 0)",
        -1, 8, GEOSBUF_JOIN_ROUND, 0,
        0.000001
        );
}

// Test only duplicated vertex in input
// See http://trac.osgeo.org/postgis/ticket/602
template<>
template<>
void object::test<11>()
{
    checkOffset(
        "LINESTRING(0 10, 0 10, 0 10)",
        "LINESTRING EMPTY",
        -1, 8, GEOSBUF_JOIN_ROUND, 0,
        0.000001
        );
}

// Test negative offset
// See https://github.com/libgeos/geos/issues/897
template<>
template<>
void object::test<12>()
{
    checkOffset(
        "LINESTRING (292671.05 7336734.42, 292624.85 7336739.72, 292623.53 7336739.92, 292622.36 7336740.2, 292621.35 7336740.57, 292620.94 7336740.78, 292620.61 7336740.97, 292620.33 7336741.14, 292620.13 7336741.29, 292619.79 7336741.57, 292619.48 7336741.86, 292619.34 7336742.01, 292619.21 7336742.17, 292619.09 7336742.34, 292618.98 7336742.51, 292618.8 7336742.88, 292618.63 7336743.25, 292618.57 7336743.43, 292618.52 7336743.58, 292618.48 7336743.71, 292618.46 7336743.8, 292618.44 7336743.98, 292618.42 7336744.15, 292618.41 7336744.33, 292618.4 7336744.51, 292618.39 7336744.69, 292618.38 7336744.88, 292618.38 7336744.91, 292618.38 7336744.95, 292618.38 7336744.99, 292618.38 7336745.02, 292618.38 7336745.06, 292618.38 7336745.1, 292618.38 7336745.13, 292618.38 7336745.17, 292618.38 7336745.21, 292618.38 7336745.25, 292618.38 7336745.43, 292618.41 7336746.92)",
        "LINESTRING (292672.7595667329 7336749.322260955, 292631.53009245027 7336754.052049131)",
        -15, 8, GEOSBUF_JOIN_ROUND, 0,
        0.000001
        );
}

template<>
template<>
void object::test<13>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_);

    result_ = GEOSOffsetCurve(input_, 1, 8, GEOSBUF_JOIN_ROUND, 0);

    ensure("curved geometries not supported", result_ == nullptr);
}

} // namespace tut

