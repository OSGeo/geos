//
// Test Suite for C-API GEOSBuffer and GEOSBufferWithStyle

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosbuffer_data : public capitest::utility
{
    GEOSBufferParams* bp_;
    double area_;

    test_capigeosbuffer_data()
        : bp_(nullptr)
    {}

    ~test_capigeosbuffer_data()
    {
        GEOSBufferParams_destroy(bp_);
    }

};

typedef test_group<test_capigeosbuffer_data> group;
typedef group::object object;

group test_capigeosbuffer_group("capi::GEOSBuffer");

//
// Test Cases
//


// Buffer against empty point
template<>
template<>
void object::test<1>()
{
    geom1_ = fromWKT("POINT EMPTY");
    geom2_ = GEOSBufferWithStyle(geom1_, 1, 8,
                                 GEOSBUF_CAP_ROUND,
                                 GEOSBUF_JOIN_BEVEL,
                                 5.0);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON EMPTY");
    ensure_geometry_equals(geom2_, geom3_);
}

// Buffer against empty linestring
template<>
template<>
void object::test<2>()
{
    geom1_ = fromWKT("LINESTRING EMPTY");
    geom2_ = GEOSBufferWithStyle(geom1_, 1, 8,
                                 GEOSBUF_CAP_ROUND,
                                 GEOSBUF_JOIN_BEVEL,
                                 5.0);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON EMPTY");
    ensure_geometry_equals(geom2_, geom3_);
}

// Buffer against empty polygon
template<>
template<>
void object::test<3>()
{
    geom1_ = fromWKT("POLYGON EMPTY");
    geom2_ = GEOSBufferWithStyle(geom1_, 1, 8,
                                 GEOSBUF_CAP_ROUND,
                                 GEOSBUF_JOIN_BEVEL,
                                 5.0);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON EMPTY");
    ensure_geometry_equals(geom2_, geom3_);
}

// Simple Buffer on a 2-vertices line (quadSegs: 1)
template<>
template<>
void object::test<4>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 20)");
    geom2_ = GEOSBuffer(geom1_, 5, 1);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 7);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 161.803, 0.001);
}

// Simple Buffer on a 2-vertices line (quadSegs: 2)
template<>
template<>
void object::test<5>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 20)");
    geom2_ = GEOSBuffer(geom1_, 5, 2);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 11);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 182.514, 0.001);
}

// Buffer with square end caps on a 2-vertices line (no matter quadSegs)
template<>
template<>
void object::test<6>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 20)");

    bp_ = GEOSBufferParams_create();
    ensure_equals(GEOSBufferParams_setQuadrantSegments(bp_, 20), 1);
    ensure_equals(GEOSBufferParams_setEndCapStyle(bp_, GEOSBUF_CAP_SQUARE), 1);
    ensure_equals(GEOSBufferParams_setJoinStyle(bp_, GEOSBUF_JOIN_ROUND), 1);
    ensure_equals(GEOSBufferParams_setMitreLimit(bp_, 5.0), 1);

    geom2_ = GEOSBufferWithParams(geom1_, bp_, 5);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 7);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 211.803, 0.001);
}

// Buffer with flat end caps on a 2-vertices line (no matter quadSegs)
template<>
template<>
void object::test<7>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 20)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_FLAT,
                                 GEOSBUF_JOIN_ROUND, 5.0);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 5);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 111.803, 0.001);
}

// Buffer with flat end cap on a 2-vertices horizontal line
template<>
template<>
void object::test<8>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_FLAT,
                                 GEOSBUF_JOIN_ROUND, 5.0);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 5);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 50.0, 0.001);

    geom3_ = fromWKT("POLYGON ((10.0 15.0, 10.0 5.0, 5.0 5.0, 5.0 15.0, 10.0 15.0))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with square end cap on a 2-vertices horizontal line
template<>
template<>
void object::test<9>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_SQUARE,
                                 GEOSBUF_JOIN_ROUND, 5.0);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 7);

    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 150.0, 0.001);

    geom3_ = fromWKT("POLYGON ((10.0 15.0, 15.0 15.0, 15.0 5.0, 5.0 5.0, 0.0 5.0, 0.0 15.0, 10.0 15.0))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with flat end cap and round join style
// on an L-shaped simple line
template<>
template<>
void object::test<10>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10, 10 20)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_SQUARE,
                                 GEOSBUF_JOIN_ROUND, 5.0);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 29);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 244.615, 0.001);
}

// Buffer with flat end cap and mitre join style
// on an L-shaped simple line
template<>
template<>
void object::test<11>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10, 10 20)");
    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_SQUARE,
                                 GEOSBUF_JOIN_MITRE, 5.0);

    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 9);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 250.0, 0.001);

    geom3_ = fromWKT("POLYGON ((5.0 15.0, 5.0 20.0, 5.0 25.0, 15.0 25.0, 15.0 5.0, 5.0 5.0, 0.0 5.0, 0.0 15.0, 5.0 15.0))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with flat end cap and bevel join style
// on an L-shaped simple line
template<>
template<>
void object::test<12>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10, 10 20)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 20, GEOSBUF_CAP_SQUARE,
                                 GEOSBUF_JOIN_BEVEL, 5.0);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 10);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 237.5, 0.001);

    geom3_ = fromWKT("POLYGON ((5.0 15.0, 5.0 20.0, 5.0 25.0, 15.0 25.0, 15.0 10.0, 10.0 5.0, 5.0 5.0, 0.0 5.0, 0.0 15.0, 5.0 15.0))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with flat end cap and bevel join style
// on an L-shaped simple line with different quadSegs and mitreLimit
// (result unaffected)
template<>
template<>
void object::test<13>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10, 10 20)");

    geom2_ = GEOSBufferWithStyle(geom1_, 5, 200, GEOSBUF_CAP_SQUARE,
                                 GEOSBUF_JOIN_BEVEL, 10.0);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 10);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 237.5, 0.001);

    geom3_ = fromWKT("POLYGON ((5.0 15.0, 5.0 20.0, 5.0 25.0, 15.0 25.0, 15.0 10.0, 10.0 5.0, 5.0 5.0, 0.0 5.0, 0.0 15.0, 5.0 15.0))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with limited mitre (1)
template<>
template<>
void object::test<14>()
{
    geom1_ = fromWKT("POLYGON((0 0, 10 0, 10 10, 0 0))");

    geom2_ = GEOSBufferWithStyle(geom1_, 2, 200, GEOSBUF_CAP_FLAT,
                                 GEOSBUF_JOIN_MITRE, 1);
    ensure(nullptr != geom2_);

    ensure_equals(GEOSGetNumCoordinates(geom2_), 7);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 132.289, 0.001);
}

// Buffer with limited mitre (2)
template<>
template<>
void object::test<15>()
{
    geom1_ = fromWKT("POLYGON((0 0, 10 0, 10 10, 0 0))");

    geom2_ = GEOSBufferWithStyle(geom1_, 2, 200, GEOSBUF_CAP_FLAT,
                                 GEOSBUF_JOIN_MITRE, 2);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 6);

    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 140.352, 0.001);
}

// Buffer with limited mitre  (3)
template<>
template<>
void object::test<16>()
{
    geom1_ = fromWKT("POLYGON((0 0, 10 0, 10 10, 0 0))");

    geom2_ = GEOSBufferWithStyle(geom1_, 2, 200, GEOSBUF_CAP_FLAT,
                                 GEOSBUF_JOIN_MITRE, 3);
    ensure(nullptr != geom2_);
    ensure_equals(GEOSGetNumCoordinates(geom2_), 4);
    ensure(0 != GEOSArea(geom2_, &area_));
    ensure_area(area_, 141.598, 0.001);
}

// Buffer with params:
// flat end cap on a straight line
template<>
template<>
void object::test<17>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10)");

    bp_ = GEOSBufferParams_create();
    GEOSBufferParams_setEndCapStyle(bp_, GEOSBUF_CAP_SQUARE);
    geom2_ = GEOSBufferWithParams(geom1_, bp_, 2);

    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON ((10 12, 12 12, 12 8, 5 8, 3 8, 3 12, 10 12))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with params:
// flat end cap on a straight line
// Single sided (left)
template<>
template<>
void object::test<18>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10)");

    bp_ = GEOSBufferParams_create();
    GEOSBufferParams_setEndCapStyle(bp_, GEOSBUF_CAP_SQUARE);
    GEOSBufferParams_setSingleSided(bp_, 1);
    geom2_ = GEOSBufferWithParams(geom1_, bp_, 2);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON ((10 10, 5 10, 5 12, 10 12, 10 10))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Buffer with params:
// flat end cap on a straight line
// Single sided (right)
template<>
template<>
void object::test<19>()
{
    geom1_ = fromWKT("LINESTRING(5 10, 10 10)");

    bp_ = GEOSBufferParams_create();
    GEOSBufferParams_setEndCapStyle(bp_, GEOSBUF_CAP_SQUARE);
    GEOSBufferParams_setSingleSided(bp_, 1);
    geom2_ = GEOSBufferWithParams(geom1_, bp_, -2);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("POLYGON ((5 10, 10 10, 10 8, 5 8, 5 10))");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

// Single-sided buffer  (3)
// http://trac.osgeo.org/geos/ticket/455
template<>
template<>
void object::test<20>()
{
    geom1_ = fromWKT("LINESTRING(0 0, 10 0, 10 10)");

    geom2_ = GEOSSingleSidedBuffer(geom1_, 10, 8, GEOSBUF_JOIN_BEVEL, 0, 0);
    ensure(nullptr != geom2_);

    geom3_ = fromWKT("LINESTRING (20.0 10.0, 20.0 0.0, 10.0 -10.0, 0.0 -10.0)");
    ensure_geometry_equals(geom2_, geom3_, 0.00001);
}

/*
// Invalid result polygon with full precision, fall back on lower precision
// https://trac.osgeo.org/geos/ticket/1131
template<>
template<>
void object::test<21>()
{
    geom1_ = fromWKT("POLYGON((-6503873.862740669 -3656747.43316935, -6481859.9985945 -3656747.43316935, -6481859.9985945 -3688545.2369360398, -6506319.8476458 -3688545.2369360398, -6506319.8476458 -3664085.38788474, -6501427.87783554 -3664085.38788474, -6501427.87783554 -3661639.40297961, -6498981.89293041 -3661639.40297961, -6498981.89293041 -3659193.41807448, -6503873.862740669 -3659193.41807448, -6503873.862740669 -3656747.43316935))");
    ensure(nullptr != geom1_);

    bp_ = GEOSBufferParams_create();
    GEOSBufferParams_setQuadrantSegments(bp_, 1);
    geom2_ = GEOSBufferWithParams(geom1_, bp_, 2445.98490513);

    wkt_ = GEOSGeomToWKT(geom2_);
    std::cout << wkt_ << "\n";

    ensure(nullptr != geom2_);
    ensure("Buffer result polygon is not valid", GEOSisValid(geom2_));
}
*/

// https://trac.osgeo.org/geos/ticket/590
template<>
template<>
void object::test<22>()
{
    geom1_ = fromWKT("LINEARRING(38.7066196617741550 -28.8266827415760860, -48.9228243285119790 100.6496977731573000, 54.4799195800256510 129.8110447359351000, 108.8101748540030500 45.8263654831350490, 86.7372079193139310 22.3209346883718070, 71.8793256882949690 36.0080540867567290, 55.2741306329362700 34.2630391674088840, 52.0696193064635370 19.4304123529519610, 62.0890652576763390 -3.9267923737325212, 38.7066196617741550 -28.8266827415760860)");

    geom2_ = GEOSBufferWithStyle(geom1_, 22.532378519833863, 6, GEOSBUF_CAP_FLAT, GEOSBUF_JOIN_MITRE, 5);
    ensure(nullptr != geom2_);
    ensure(GEOSisValid(geom2_));
}

// Error raised on invalid value of buffer params
template<>
template<>
void object::test<23>()
{
    bp_ = GEOSBufferParams_create();
    ensure_equals(GEOSBufferParams_setEndCapStyle(bp_, 500), 0);
    ensure_equals(GEOSBufferParams_setJoinStyle(bp_, 500), 0);
}

// Segfault with Inf coords
// https://github.com/libgeos/geos/issues/822
template<>
template<>
void object::test<24>()
{
    std::string wkb("0106000020E61000000100000001030000000100000005000000000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F000000000000F07F");
    geom1_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(wkb.c_str()), wkb.size());
    ensure(geom1_ != nullptr);
    geom2_ = GEOSBuffer(geom1_, 20, 8);
    ensure(geom2_ == nullptr);
    geom3_ = GEOSBuffer(geom1_, -20, 8);
    ensure(geom3_ == nullptr);
}


template<>
template<>
void object::test<25>()
{
    geom1_ = fromWKT("POLYGON ((4.6664239253667485 4.9470840685113275, 4.666423925366749 4.947084068511328, 3.569508914897422 -10.739531408188364, -9.082056557097435 19.893317266250286, 5.639581102785941 18.86388007810711, 4.6664239253667485 4.9470840685113275))");
    geom2_ = GEOSBufferWithStyle(geom1_, -1, 8,
                                 GEOSBUF_CAP_ROUND,
                                 GEOSBUF_JOIN_MITRE,
                                 5);
    geom3_ = fromWKT("POLYGON ((3.3225774291798533 0.0647708524944821, 3.3225774291798555 0.0647708524944812, 2.8688758567150883 -6.4234639154696263, -7.5416226086581215 18.7831577331451953, 4.5722605787819921 17.9360725015914078, 3.3225774291798533 0.0647708524944821))");
    ensure_geometry_equals(geom3_, geom2_, 0.001);
}

template<>
template<>
void object::test<26>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_ != nullptr);

    result_ = GEOSBuffer(input_, 1, 8);
    ensure(result_ == nullptr);
}

} // namespace tut
