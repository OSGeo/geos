
#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeomsetprecision_data : public capitest::utility {};

typedef test_group<test_capigeosgeomsetprecision_data> group;
typedef group::object object;

group test_capigeosgeomsetprecision_group("capi::GEOSGeom_setPrecision");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = fromWKT("POLYGON EMPTY");
    ensure_equals(GEOSGeom_getPrecision(geom1_), 0.0);

    geom3_ = GEOSGeom_setPrecision(geom1_, 2.0, 0);
    ensure(geom3_ != 0);
    ensure_geometry_equals(geom3_, "POLYGON EMPTY");
    ensure_equals(GEOSGeom_getPrecision(geom3_), 2.0);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = fromWKT("LINESTRING(-3 6, 9 1)");
    geom3_ = GEOSGeom_setPrecision(geom1_, 2.0, 0);
    ensure(geom3_ != 0);
    ensure_geometry_equals(geom3_, "LINESTRING (-2 6, 10 2)");
}

// See effects of precision reduction on intersection operation
template<>
template<>
void object::test<3>
()
{
    geom1_ = fromWKT("LINESTRING(2 10, 4 30)");
    geom2_ = fromWKT("LINESTRING(4 10, 2 30)");

    geom3_ = GEOSIntersection(geom1_, geom2_);
    ensure(geom3_ != 0);
    ensure_geometry_equals(geom3_, "POINT (3 20)");

    GEOSGeometry* g;

    // Both inputs with precision grid of 2.0
    g = GEOSGeom_setPrecision(geom1_, 2.0, 0);
    ensure(g != 0);
    GEOSGeom_destroy(geom1_);
    geom1_ = g;
    g = GEOSGeom_setPrecision(geom2_, 2.0, 0);
    ensure(g != 0);
    GEOSGeom_destroy(geom2_);
    geom2_ = g;
    GEOSGeom_destroy(geom3_);
    geom3_ = GEOSIntersection(geom1_, geom2_);
    ensure(geom3_ != 0);
    ensure_geometry_equals(geom3_, "POINT (4 20)");

    // One input with precision grid of 0.5, the other of 2.0
    g = GEOSGeom_setPrecision(geom1_, 0.5, 0);
    ensure(g != 0);
    GEOSGeom_destroy(geom1_);
    geom1_ = g;
    GEOSGeom_destroy(geom3_);
    geom3_ = GEOSIntersection(geom1_, geom2_);
    ensure(geom3_ != 0);
    ensure_geometry_equals(geom3_, "POINT (3 20)");

    ensure_equals(GEOSGeom_getPrecision(geom1_), 0.5);
    ensure_equals(GEOSGeom_getPrecision(geom2_), 2.0);
}

// Retain (or not) topology
template<>
template<>
void object::test<4>
()
{
    geom1_ = fromWKT("POLYGON((10 10,20 10,16 15,20 20, 10 20, 14 15, 10 10))");

    geom2_ = GEOSGeom_setPrecision(geom1_, 5.0, 0);
    ensure_geometry_equals(geom2_,
        "MULTIPOLYGON (((10 10, 15 15, 20 10, 10 10)), ((15 15, 10 20, 20 20, 15 15)))");

    geom3_ = GEOSGeom_setPrecision(geom1_, 5.0, GEOS_PREC_NO_TOPO);
    ensure_geometry_equals(geom3_,
        "POLYGON ((10 10, 20 10, 15 15, 20 20, 10 20, 15 15, 10 10))");
}

// Retain (or not) collapsed elements
template<>
template<>
void object::test<5>
()
{
    geom1_ = fromWKT("LINESTRING(1 0, 2 0)");

    geom2_ = GEOSGeom_setPrecision(geom1_, 5.0, 0);
    ensure_geometry_equals(geom2_, "LINESTRING EMPTY");

    geom3_ = GEOSGeom_setPrecision(geom1_, 5.0, GEOS_PREC_KEEP_COLLAPSED);
    ensure_equals(toWKT(geom3_), "LINESTRING (0 0, 0 0)");
}

// Retain (or not) collapsed elements
template<>
template<>
void object::test<6> ()
{
    geom1_ = fromWKT("LINESTRING (0 0, 0.1 0.1)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, 0);
    ensure_geometry_equals(geom2_, "LINESTRING EMPTY");
}

// Retain (or not) collapsed elements
template<>
template<>
void object::test<7> ()
{
    geom1_ = fromWKT("LINESTRING (0 0, 0.1 0.1)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, GEOS_PREC_NO_TOPO);
    ensure_geometry_equals(geom2_, "LINESTRING (0 0, 0 0)");
}

// Retain (or not) collapsed elements
template<>
template<>
void object::test<8> ()
{
    geom1_ = fromWKT("LINESTRING (0 0, 0.1 0.1)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, GEOS_PREC_KEEP_COLLAPSED);
    ensure_geometry_equals(geom2_, "LINESTRING (0 0, 0 0)");
}

// Retain (or not) collapsed elements
template<>
template<>
void object::test<9> ()
{
    geom1_ = fromWKT("LINESTRING (0 0, 0.1 0.1)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, GEOS_PREC_KEEP_COLLAPSED | GEOS_PREC_NO_TOPO);
    ensure_geometry_equals(geom2_, "LINESTRING (0 0, 0 0)");
}


// Collapse a linearRing / Trac #1135
template<>
template<>
void object::test<10> ()
{
    geom1_ = fromWKT("LINEARRING (0 0, 0.1 0, 0.1 0.1, 0 0.1, 0 0)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, 0);
    ensure_geometry_equals(geom2_, "LINEARRING EMPTY");
}

// Reduce polygon precision, corner case / Trac #1127
template<>
template<>
void object::test<11> ()
{
    // POLYGON((
    //   100 49.5, (1)
    //   100 300,  (2)
    //   320 60,   (3)
    //   340 49.9, (4)
    //   360 50.1, (5)
    //   380 49.5, (6)
    //   100 49.5  (7)
    // ))
    // * points 4 and 5 are close (less than 100.0/100) to segment (6, 7);
    // * Y coordinates of points 4 and 5 are rounded to different values, 0 and 100 respectively;
    // * point 4 belongs to monotone chain of size > 1 -- segments (2, 3) and (3, 4)
    geom1_ = fromWKT("POLYGON((100 49.5, 100 300, 320 60, 340 49.9, 360 50.1, 380 49.5, 100 49.5))");
    geom2_ = GEOSGeom_setPrecision(geom1_, 100.0, 0);
    ensure(geom2_ != nullptr); // just check that valid geometry is constructed
}

template<>
template<>
void object::test<12>()
{
    geom1_ = fromWKT("POLYGON ((0 0, 0.1 0, 0.1 0.1, 0 0.1, 0 0))");
    geom2_ = GEOSGeom_setPrecision(geom1_, 1.0, 0);

    ensure_equals(GEOSGeom_getCoordinateDimension(geom2_), 2);
}

//-- test that a large gridsize works
template<>
template<>
void object::test<13>()
{
    geom1_ = fromWKT("LINESTRING (657035.913 6475590.114,657075.57 6475500)");
    geom2_ = GEOSGeom_setPrecision(geom1_, 100, 0);
    ensure_geometry_equals(geom2_,
        "LINESTRING (657000 6475600, 657100 6475500)");
}

// Test more exact rounding for integral scale factors
// see https://trac.osgeo.org/postgis/ticket/5520
template<>
template<>
void object::test<14>()
{
    geom1_ = fromWKT("LINESTRING (657035.913 6475590.114,657075.57 6475500)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .001, 0);
    ensure_geometry_equals(geom2_,
        "LINESTRING (657035.913 6475590.114, 657075.57 6475500)");
}

// see https://trac.osgeo.org/postgis/ticket/5425
template<>
template<>
void object::test<15>()
{
    geom1_ = fromWKT("LINESTRING(674169.89 198051.38,674197.7 198065.55,674200.36 198052.38)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .001, 0);
    ensure_geometry_equals(geom2_,
        "LINESTRING (674169.89 198051.38, 674197.7 198065.55, 674200.36 198052.38)");
}

// see https://trac.osgeo.org/postgis/ticket/3929
template<>
template<>
void object::test<16>()
{
    geom1_ = fromWKT("POINT(311.4 0)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .1, 0);
    ensure_geometry_equals(geom2_,
        "POINT(311.4 0)");
}

// see https://gis.stackexchange.com/questions/465485/postgis-reduce-precision-in-linestring
template<>
template<>
void object::test<17>()
{
    geom1_ = fromWKT("LINESTRING (16.418792399944802 54.24801559999939, 16.4176588 54.248003)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .0000001, 0);
    ensure_geometry_equals(geom2_,
        "LINESTRING (16.4187924 54.2480156, 16.4176588 54.248003)");
}

// see https://gis.stackexchange.com/questions/321814/st-snaptogrid-doesnt-work-properly-e-g-41-94186153740355-41-94186149999999
template<>
template<>
void object::test<18>()
{
    geom1_ = fromWKT("POINT (21.619820510769063 41.94186153740355)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .0000001, 0);
    ensure_geometry_equals(geom2_,
        "POINT (21.6198205 41.9418615)");
}

// see https://gis.stackexchange.com/questions/321814/st-snaptogrid-doesnt-work-properly-e-g-41-94186153740355-41-94186149999999
template<>
template<>
void object::test<19>()
{
    geom1_ = fromWKT("POINT (22.49594094391644 41.20357506925623)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .0000001, 0);
    ensure_geometry_equals(geom2_,
        "POINT (22.4959409 41.2035751)");
}

// see https://lists.osgeo.org/pipermail/postgis-users/2006-January/010861.html
template<>
template<>
void object::test<20>()
{
    geom1_ = fromWKT("POINT(1.23456789 9.87654321)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .000001, 0);
    geom3_ = GEOSGeom_setPrecision(geom2_, .001, 0);
    ensure_geometry_equals(geom3_,
        "POINT(1.235 9.877)");
}

// see https://lists.osgeo.org/pipermail/postgis-users/2023-September/046107.html
template<>
template<>
void object::test<21>()
{
    geom1_ = fromWKT("LINESTRING(334729.13 4103548.88, 334729.12 4103548.53)");
    geom2_ = GEOSGeom_setPrecision(geom1_, .001, 0);
    ensure_geometry_equals(geom2_,
        "LINESTRING(334729.13 4103548.88,334729.12 4103548.53)");
}

} // namespace tut

