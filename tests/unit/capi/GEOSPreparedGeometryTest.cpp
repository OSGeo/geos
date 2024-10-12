//
// Test Suite for C-API GEOSPreparedGeometry

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/io/WKBReader.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
// std
#include <memory>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeospreparedgeometry_data : public capitest::utility {
    const GEOSPreparedGeometry* prepGeom1_;
    const GEOSPreparedGeometry* prepGeom2_;

    test_capigeospreparedgeometry_data()
        : prepGeom1_(nullptr), prepGeom2_(nullptr)
    {}

    ~test_capigeospreparedgeometry_data()
    {
        GEOSPreparedGeom_destroy(prepGeom1_);
        GEOSPreparedGeom_destroy(prepGeom2_);
        prepGeom1_ = nullptr;
        prepGeom2_ = nullptr;
    }

};

typedef test_group<test_capigeospreparedgeometry_data> group;
typedef group::object object;

group test_capigeospreparedgeometry_group("capi::GEOSPreparedGeometry");

//
// Test Cases
//

// Test geometry preparation
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

}

// Test PreparedContainsProperly
// Taken from regress/regress_ogc_prep.sql of postgis
// as of revno 3936
// ref: containsproperly200 (a)
template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))");
    geom2_ = GEOSGeomFromWKT("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
    ensure_equals(ret, 1);

}

// Test PreparedContainsProperly
// Taken from regress/regress_ogc_prep.sql of postgis
// as of revno 3936
// ref: containsproperly200 (b)
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((2 2, 2 3, 3 3, 3 2, 2 2))");
    geom2_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 10, 10 0, 0 0))");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
    ensure_equals(ret, 0);

}

// Test PreparedIntersects
// Also used as a linestring leakage reported
// by http://trac.osgeo.org/geos/ticket/305
//
template<>
template<>
void object::test<4>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 10)");
    geom2_ = GEOSGeomFromWKT("LINESTRING(0 10, 10 0)");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedIntersects(prepGeom1_, geom2_);
    ensure_equals(ret, 1);

}

// Test PreparedCovers
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 11, 10 0, 0 0))");
    geom2_ = GEOSGeomFromWKT("POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedCovers(prepGeom1_, geom2_);
    ensure_equals(ret, 1);

}

// Test PreparedContains
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 10, 10 11, 10 0, 0 0))");
    geom2_ = GEOSGeomFromWKT("POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedContains(prepGeom1_, geom2_);
    ensure_equals(ret, 1);

}

// Test PreparedIntersects: point on segment with FLOAT PM
// X coordinate of 3rd and 4th vertices of the line are not
// float-point exact with X coordinate of the point.
// The X values differ after 14th decimal place:
// POINT (-23.1094689600055080 50.5195368635957180)
// --------------------^^^^^^^------------^^^^^^^^
// LINESTRING 3rd and 4th points
//        -23.1094689600055150 50.5223376452201340,
//        -23.1094689600055010 50.5169177629559480,
// --------------------^^^^^^^------------^^^^^^^^
// So, in float-point precision model, the point does DOES NOT intersect the segment.
// See RobustLineIntersectorTest.cpp for similar test cases.
template<>
template<>
void object::test<7>
()
{
    // POINT located between 3rd and 4th vertex of LINESTRING
    // POINT(-23.1094689600055080 50.5195368635957180)
    std::string point("01010000009a266328061c37c0e21a172f80424940");
    // LINESTRING(-23.122057005539 50.5201976774794,-23.1153476966995 50.5133404815199,-23.1094689600055150 50.5223376452201340,-23.1094689600055010 50.5169177629559480,-23.0961967920942 50.5330464848094,-23.0887991006034 50.5258515213185,-23.0852302622362 50.5264582238409)
    std::string
    line("0102000000070000009909bf203f1f37c05c1d66d6954249404afe386d871d37c0a7eb1124b54149409c266328061c37c056d8bff5db42494098266328061c37c0034f7b5c2a42494060065c5aa01837c08ac001de3a4449408401b189bb1637c0b04e471a4f43494014ef84a6d11537c0b20dabfb62434940");
    geom1_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(line.data()), line.size());
    geom2_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(point.data()), point.size());

    prepGeom1_ = GEOSPrepare(geom1_);
    ensure(nullptr != prepGeom1_);
    int ret = GEOSPreparedIntersects(prepGeom1_, geom2_);
    ensure_equals(ret, 0);
}

// Test PreparedIntersects: point on segment with FIXED PM
// X coordinate of 3rd and 4th vertices of the line are not
// float-point exact with X coordinate of the point.
// The X values differ after 14th decimal place:
// POINT (-23.1094689600055080 50.5195368635957180)
// --------------------^^^^^^^------------^^^^^^^^
// LINESTRING 3rd and 4th points
//        -23.1094689600055150 50.5223376452201340,
//        -23.1094689600055010 50.5169177629559480,
// --------------------^^^^^^^------------^^^^^^^^
// So, if float-point values are trimmed up to 14 decimal digits, the point DOES intersect the segment.
// See RobustLineIntersectorTest.cpp for similar test cases.
template<>
template<>
void object::test<8>
()
{
    geos::geom::PrecisionModel pm(1e+13);
    geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
    geos::io::WKBReader reader(*factory);

    // POINT located between 3rd and 4th vertex of LINESTRING
    // POINT(-23.1094689600055080 50.5195368635957180)
    std::string point("01010000009a266328061c37c0e21a172f80424940");
    // LINESTRING(-23.122057005539 50.5201976774794,-23.1153476966995 50.5133404815199,-23.1094689600055150 50.5223376452201340,-23.1094689600055010 50.5169177629559480,-23.0961967920942 50.5330464848094,-23.0887991006034 50.5258515213185,-23.0852302622362 50.5264582238409)
    std::string
    line("0102000000070000009909bf203f1f37c05c1d66d6954249404afe386d871d37c0a7eb1124b54149409c266328061c37c056d8bff5db42494098266328061c37c0034f7b5c2a42494060065c5aa01837c08ac001de3a4449408401b189bb1637c0b04e471a4f43494014ef84a6d11537c0b20dabfb62434940");
    std::stringstream sPoint(point);
    geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sPoint).release());
    std::stringstream sLine(line);
    geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sLine).release());

    prepGeom1_ = GEOSPrepare(geom1_);
    ensure(nullptr != prepGeom1_);
    int ret = GEOSPreparedIntersects(prepGeom1_, geom2_);
    ensure_equals(ret, 1);
}

// Test PreparedIntersects: point on vertex (default FLOAT PM)
template<>
template<>
void object::test<9>
()
{
    // POINT located on the 3rd vertex of LINESTRING
    // POINT(-23.1094689600055 50.5223376452201)
    std::string point("01010000009c266328061c37c056d8bff5db424940");
    // LINESTRING(-23.122057005539 50.5201976774794,-23.1153476966995 50.5133404815199,-23.1094689600055 50.5223376452201,-23.1094689600055 50.5169177629559,-23.0961967920942 50.5330464848094,-23.0887991006034 50.5258515213185,-23.0852302622362 50.5264582238409)
    std::string
    line("0102000000070000009909bf203f1f37c05c1d66d6954249404afe386d871d37c0a7eb1124b54149409c266328061c37c056d8bff5db42494098266328061c37c0034f7b5c2a42494060065c5aa01837c08ac001de3a4449408401b189bb1637c0b04e471a4f43494014ef84a6d11537c0b20dabfb62434940");
    geom1_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(line.data()), line.size());
    geom2_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(point.data()), point.size());
    prepGeom1_ = GEOSPrepare(geom1_);
    ensure(nullptr != prepGeom1_);

    int ret = GEOSPreparedIntersects(prepGeom1_, geom2_);
    ensure_equals(ret, 1);
}

// Test outer polygon contains inner polygon with two coincident vertices
// with results compared depending on precision used with FIXED PMs.
template<>
template<>
void object::test<10>
()
{
    // Coincident vertices of both polygons at
    // -700.67089999181 93743.4218587986, -713.450135807349 93754.1677576647,
    std::string const
    outer("01030000800100000009000000af9dd0005ee585c0f802efbff6e2f6400000000000000000955acde0994b86c039a922afa2e3f64000000000000000002af6fb4f5d1887c07adb1c4071e3f6400000000000000000e5962b388d4f87c0bd3aeda7bae2f640000000000000000087c61344030887c07d585e6ff6e1f6400000000000000000fc8a31b5166186c0230588b20ae1f640000000000000000034733daf050186c0ed9f3ac98ae1f6400000000000000000f190aef659b385c0df2876538ce2f6400000000000000000af9dd0005ee585c0f802efbff6e2f6400000000000000000");
    std::string const
    inner("0103000080010000000a000000ac21f88bbaff86c05f45d8c7b4e2f6400000000000000000467f1177ebf386c05de1971187e2f6400000000000000000fcf677888fc886c04e855a544be2f6400000000000000000c61226e540b686c0c0662d1fe7e1f640000000000000000042dc1bece8a486c09b85529f8ae1f6400000000000000000891047cde55e86c038cfa59c4ee1f6400000000000000000ae9dd0005ee585c0fa02efbff6e2f6400000000000000000975acde0994b86c038a922afa2e3f6400000000000000000287e339b09f986c01b1a083a10e3f6400000000000000000ac21f88bbaff86c05f45d8c7b4e2f6400000000000000000");

    // A contains B if precision is limited to 1e+10
    {
        geos::geom::PrecisionModel pm(1e+10); // NOTE: higher precision fails this test case
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);
        prepGeom1_ = GEOSPrepare(geom1_);
        ensure(nullptr != prepGeom1_);

        int ret = GEOSPreparedContains(prepGeom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
        ensure_equals(ret, 0);

        GEOSGeom_destroy(geom1_);
        GEOSGeom_destroy(geom2_);
        GEOSPreparedGeom_destroy(prepGeom1_);
    }

    // A does NOT contain B if precision is extended to 1e+11 or beyond
    {
        geos::geom::PrecisionModel pm(1e+11);
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);
        prepGeom1_ = GEOSPrepare(geom1_);
        ensure(nullptr != prepGeom1_);

        int ret = GEOSPreparedContains(prepGeom1_, geom2_);
        ensure_equals(ret, 0);
        ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
        ensure_equals(ret, 0);
    }
}

// Test outer rectangle contains inner rectangle with one coincident vertex
// and two vertices of the inner rectangle are on the boundary (lay on segments)
// of the outer rectangle.
// Precision model should not affect the containment test result.
template<>
template<>
void object::test<11>
()
{
    // Coincident vertex at -753.167968418005 93709.4279185742
    //POLYGON ((-753.167968418005 93754.0955183194,-816.392328351464 93754.0955183194,-816.392328351464 93709.4279185742,-753.167968418005 93709.4279185742,-753.167968418005 93754.0955183194))
    std::string const
    outer("01030000800100000005000000bd70d3ff578987c09e373e87a1e3f6400000000000000000a9f60b7d238389c09e373e87a1e3f6400000000000000000a9f60b7d238389c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09e373e87a1e3f6400000000000000000");
    //POLYGON ((-753.167968418005 93747.6909727677,-799.641978447015 93747.6909727677,-799.641978447015 93709.4279185742,-753.167968418005 93709.4279185742,-753.167968418005 93747.6909727677))
    std::string const
    inner("01030000800100000005000000bd70d3ff578987c0f875390e3be3f6400000000000000000579598c522fd88c0f875390e3be3f6400000000000000000579598c522fd88c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09625c1d8d6e0f6400000000000000000bd70d3ff578987c0f875390e3be3f6400000000000000000");

    // A contains B if precision is limited to 1e+10
    {
        geos::geom::PrecisionModel pm(1e+10);
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);
        prepGeom1_ = GEOSPrepare(geom1_);
        ensure(nullptr != prepGeom1_);

        int ret = GEOSPreparedContains(prepGeom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
        ensure_equals(ret, 0);

        GEOSGeom_destroy(geom1_);
        GEOSGeom_destroy(geom2_);
        GEOSPreparedGeom_destroy(prepGeom1_);
    }

    // A contains B if FLOATING PM is used with extended precision
    {
        geos::geom::PrecisionModel pm;
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);
        prepGeom1_ = GEOSPrepare(geom1_);
        ensure(nullptr != prepGeom1_);

        int ret = GEOSPreparedContains(prepGeom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSPreparedContainsProperly(prepGeom1_, geom2_);
        ensure_equals(ret, 0);
    }
}

// Test PreparedIntersects with Point EMPTY
template<>
template<>
void object::test<12>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    geom2_ = GEOSGeomFromWKT("POINT EMPTY");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);
    ensure(nullptr != geom2_);

    int ret = GEOSPreparedIntersects(prepGeom1_, geom2_);
    ensure_equals(ret, 0);
}

// Test PreparedCovers with Point EMPTY
template<>
template<>
void object::test<13>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    geom2_ = GEOSGeomFromWKT("POINT EMPTY");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);
    ensure(nullptr != geom2_);

    int ret = GEOSPreparedCovers(prepGeom1_, geom2_);
    ensure_equals(ret, 0);
}

// Verify no memory leak on exception (https://github.com/libgeos/geos/issues/505)
// RelateNG does not throw for this case
template<>
template<>
void object::test<14>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 1 NaN)");
    geom2_ = GEOSGeomFromWKT("POINT(0 0)");
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure(nullptr != prepGeom1_);
    ensure(nullptr != geom2_);

    int ret = GEOSPreparedTouches(prepGeom1_, geom2_);
    ensure_equals(ret, 1);
}

// Test XY variants
template<>
template<>
void object::test<15>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))")    ;
    prepGeom1_ = GEOSPrepare(geom1_);

    ensure_equals(GEOSPreparedContainsXY(prepGeom1_, 0.5, 0.5), 1);
    ensure_equals(GEOSPreparedContainsXY(prepGeom1_, 1.5, 0.5), 0);
    ensure_equals(GEOSPreparedContainsXY(prepGeom1_, 0.75, 0.5), 1);

    ensure_equals(GEOSPreparedIntersectsXY(prepGeom1_, 0.5, 0.5), 1);
    ensure_equals(GEOSPreparedIntersectsXY(prepGeom1_, 1.5, 0.5), 0);
    ensure_equals(GEOSPreparedIntersectsXY(prepGeom1_, 0.75, 0.5), 1);
}


template<>
template<>
void object::test<16>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");

    ensure(geom1_);

    prepGeom1_ = GEOSPrepare(geom1_);
    ensure("curved geometries not supporteds", prepGeom1_ == nullptr);
}


template<>
template<>
void object::test<17>()
{
    geom1_ = fromWKT("POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))");
    geom2_ = fromWKT("POLYGON((0.5 0.5, 1.5 0.5, 1.5 1.5, 0.5 1.5, 0.5 0.5))");

    ensure(geom1_);
    ensure(geom2_);

    prepGeom1_ = GEOSPrepare(geom1_);

    // Run each test twice to make sure the "accelerated" second call
    // is the same as the first one.
    ensure_equals("prepTouches1", GEOSPreparedTouches(prepGeom1_, geom2_), GEOSTouches(geom1_, geom2_));
    ensure_equals("prepTouches2", GEOSPreparedTouches(prepGeom1_, geom2_), GEOSTouches(geom1_, geom2_));
    ensure_equals("prepOverlaps1", GEOSPreparedOverlaps(prepGeom1_, geom2_), GEOSOverlaps(geom1_, geom2_));
    ensure_equals("prepOverlaps2", GEOSPreparedOverlaps(prepGeom1_, geom2_), GEOSOverlaps(geom1_, geom2_));
    ensure_equals("prepCrosses1", GEOSPreparedCrosses(prepGeom1_, geom2_), GEOSCrosses(geom1_, geom2_));
    ensure_equals("prepCrosses2", GEOSPreparedCrosses(prepGeom1_, geom2_), GEOSCrosses(geom1_, geom2_));
    ensure_equals("prepDisjoint1", GEOSPreparedDisjoint(prepGeom1_, geom2_), GEOSDisjoint(geom1_, geom2_));
    ensure_equals("prepDisjoint2", GEOSPreparedDisjoint(prepGeom1_, geom2_), GEOSDisjoint(geom1_, geom2_));

    char* r1 = GEOSPreparedRelate(prepGeom1_, geom2_);
    char* r2 = GEOSRelate(geom1_, geom2_);
    ensure_equals("prepRelate1", std::string(r1), std::string(r2));
    GEOSFree(r1);
    GEOSFree(r2);
}

} // namespace tut

