//
// Test Suite for C-API GEOSIntersects

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/constants.h>
// std
#include <cstring>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosintersects_data : public capitest::utility {};

typedef test_group<test_capigeosintersects_data> group;
typedef group::object object;

group test_capigeosintersects_group("capi::GEOSIntersects");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON EMPTY");
    geom2_ = GEOSGeomFromWKT("POLYGON EMPTY");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSIntersects(geom1_, geom2_);

    ensure_equals(r1, 0);

    char const r2 = GEOSIntersects(geom2_, geom1_);

    ensure_equals(r2, 0);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON((1 1,1 5,5 5,5 1,1 1))");
    geom2_ = GEOSGeomFromWKT("POINT(2 2)");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSIntersects(geom1_, geom2_);

    ensure_equals(int(r1), 1);

    char const r2 = GEOSIntersects(geom2_, geom1_);

    ensure_equals(int(r2), 1);
}

template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("MULTIPOLYGON(((0 0,0 10,10 10,10 0,0 0)))");
    geom2_ = GEOSGeomFromWKT("POLYGON((1 1,1 2,2 2,2 1,1 1))");

    ensure(nullptr != geom1_);
    ensure(nullptr != geom2_);

    char const r1 = GEOSIntersects(geom1_, geom2_);

    ensure_equals(int(r1), 1);

    char const r2 = GEOSIntersects(geom2_, geom1_);

    ensure_equals(int(r2), 1);
}

// This is a test for bug #357 (GEOSIntersects with nan coords)
template<>
template<>
void object::test<4>
()
{
    GEOSCoordSequence* cs = GEOSCoordSeq_create(5, 2);

    constexpr double nan = geos::DoubleNotANumber;
    GEOSCoordSeq_setX(cs, 0, 1);
    GEOSCoordSeq_setY(cs, 0, 1);
    for(unsigned int i = 1; i < 4; ++i) {
        GEOSCoordSeq_setX(cs, i, nan);
        GEOSCoordSeq_setY(cs, i, nan);
    }
    GEOSCoordSeq_setX(cs, 4, 1);
    GEOSCoordSeq_setY(cs, 4, 1);

    geom1_ = GEOSGeom_createPolygon(GEOSGeom_createLinearRing(cs),
                                    nullptr, 0);

    char const r1 = GEOSIntersects(geom1_, geom1_);

    (void) r1;

    // The specific result here is not important -- the point is that
    // we shouldn't crash or enter an infinite loop.
    // ensure_equals(int(r1), 2);
}

// This is a test for bug #357 (GEOSIntersects with inf coords)
template<>
template<>
void object::test<5>
()
{
    const char* hex =
        "0103000020E61000000100000005000000737979F3DDCC2CC0F92154F9E7534540000000000000F07F000000000000F07F8F806E993F7E55C0304B29FFEA8554400634E8D1DD424540B5FEE6A37FCD4540737979F3DDCC2CC0F92154F9E7534540";

    geom1_ = GEOSGeomFromHEX_buf((unsigned char*)hex, std::strlen(hex));

    ensure(nullptr != geom1_);

    char const r1 = GEOSIntersects(geom1_, geom1_);

    (void) r1;

    // The specific result here is not important -- the point is that
    // we shouldn't crash or enter an infinite loop.
    // ensure_equals(int(r1), 2);
}

// Test for #782 (collection with empty components)
template<>
template<>
void object::test<6>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(10 0, 0 0, 0 10)");
    geom2_ = GEOSGeomFromWKT("MULTILINESTRING((10 -1,-1 10),EMPTY)");

    char r1 = GEOSIntersects(geom1_, geom2_);

    ensure_equals(r1, 1);
}

// https://trac.osgeo.org/geos/ticket/1110
template<>
template<>
void object::test<7>
()
{
    std::string wkb1 = "0103000080010000000C000000D76CFA7C7CF3FFC1B982482636CDCBC15EC31B0E65B6C1C12E877662F4E700C2103B2D82C78DCFC1A29779FE5D09E1C123CB6770DEF3FFC1FCDADA768882CBC17A8FA0B9C080CBC1482374FCF95200C26C3E6539D010EB41502415824E2BDCC1F846A740265CFCC1849036017A69EC41C0AE6BA9836DB6C1B03E5F6E25BBEFC16CB0BE5B0C05E54156D5BAAB3988FDC1887692F5A0B5E1C14E5950599AB8C6413600EDEF3251F3C1483B4D575A32FF417C70666B8C74EB410A04298CBF8AE7C1AEEB2DC41939E1C16D2B823AAC97C441FCE7F04C7BBDE0C184D82356622AEA4161A19916C828F5C1414C7E9080C9FAC12A178C59F5F0FEC1668C89B9F61CFCC1E8964B49B6FEEC41D76CFA7C7CF3FFC1B982482636CDCBC15EC31B0E65B6C1C1";
    std::string wkb2 = "01070000C00400000001040000C00500000001010000C0A0BB9ACC8B0DC1C1B8602E666477FD4121296E95037BF4C1DA74378FB4D3F84101010000C0C0B97F00099AB9C1AE10AA1E9448014288E51AA082CDDB41E679A87E43F7F4C101010000C0F48DC1F85AB100C2F4D0E9475F2AEBC14E5BA1AFD2C6F641D864680FF688FD4101010000C04C4627DB46B6D8C164D47813315AF44128DAB8593D46D241225B75BFD679F6C101010000C0000000000000F87F000000000000F87F000000000000F87F000000000000F87F01020000C00200000092BFA968BC02F5411E5BEFB89D0EEEC12C469C1FABF2FA419C8A9B2C549AFEC1544F6C35B75E02420C6A71564AAEF2415015ABBA4B92CBC1AB4F97AF8015F5C101060000C00000000001040000C00300000001010000C0000000000000F87F000000000000F87F000000000000F87F000000000000F87F01010000C0B2F20191DDB1F0412162A5B662D7FEC1FE5B50FDD7E8F9413EA6AFAAADDAF14101010000C0000000000000F87F000000000000F87F000000000000F87F000000000000F87F";

    geom1_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(wkb1.c_str()), wkb1.size());
    ensure(geom1_);

    geom2_ = GEOSGeomFromHEX_buf(reinterpret_cast<const unsigned char*>(wkb2.c_str()), wkb2.size());
    ensure(geom2_);

    char r1 = GEOSIntersects(geom1_, geom2_);
    ensure(r1 != 2); // no exception
}

// Simplified test for https://trac.osgeo.org/geos/ticket/1110 (collection with empty points)
template<>
template<>
void object::test<8>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 10)");
    geom2_ = GEOSGeomFromWKT("MULTIPOINT ((5 5),EMPTY)");

    char r1 = GEOSIntersects(geom1_, geom2_);

    ensure_equals(r1, 1);
}

template<>
template<>
void object::test<9>
()
{
    geom1_ = GEOSGeomFromWKT("GEOMETRYCOLLECTION (LINESTRING (0 0, 1 1), POINT (2 2))");
    geom2_ = GEOSGeomFromWKT("LINESTRING (2 2, 3 3)");

    char r1 = GEOSIntersects(geom1_, geom2_);
    ensure_equals(r1, 1);

    char r2 = GEOSIntersects(geom2_, geom1_);
    ensure_equals(r2, 1);
}

// https://github.com/libgeos/geos/issues/766
template<>
template<>
void object::test<10>
()
{
    geom1_ = GEOSGeomFromWKT("POLYGON ((26639.240191093646 6039.3615818717535, 26639.240191093646 5889.361620883223,28000.000095100608 5889.362081553552, 28000.000095100608 6039.361620882992, 28700.00019021402 6039.361620882992, 28700.00019021402 5889.361822800367, 29899.538842431968 5889.362160452064,32465.59665091549 5889.362882757903, 32969.2837182586 -1313.697771558439, 31715.832811969216 -1489.87008918589, 31681.039836323587 -1242.3030298361555, 32279.3890331618 -1158.210534269224, 32237.63710287376 -861.1301136466199, 32682.89764107368 -802.0828534499739, 32247.445200905553 5439.292852892075, 31797.06861513178 5439.292852892075, 31797.06861513178 5639.36178850523, 29899.538849750803 5639.361268079038, 26167.69458275995 5639.3602445643955, 26379.03654594742 2617.0293071870683, 26778.062167926924 2644.9318977193907, 26792.01346261031 2445.419086759444, 26193.472956813417 2403.5650586598513, 25939.238114175267 6039.361685403233, 26639.240191093646 6039.3615818717535), (32682.89764107368 -802.0828534499738, 32682.89764107378 -802.0828534499669, 32247.445200905655 5439.292852892082, 32247.445200905553 5439.292852892075, 32682.89764107368 -802.0828534499738))");
    geom2_ = GEOSGeomFromWKT("POLYGON ((32450.100392347143 5889.362314133216, 32050.1049555691 5891.272957209961, 32100.021071878822 16341.272221116333, 32500.016508656867 16339.361578039587, 32450.100392347143 5889.362314133216))");

    char r1 = GEOSIntersects(geom1_, geom2_);
    ensure_equals(r1, 1);

    char r2 = GEOSIntersects(geom2_, geom1_);
    ensure_equals(r2, 1);
}

template<>
template<>
void object::test<11>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geom2_ = fromWKT("LINESTRING (1 0, 2 1)");

    ensure(geom1_);
    ensure(geom2_);

    ensure_equals("curved geometry not supported", GEOSIntersects(geom1_, geom2_), 2);
    ensure_equals("curved geometry not supported", GEOSIntersects(geom2_, geom1_), 2);
}

// test PIP special case for CurvePolygon
template<>
template<>
void object::test<12>()
{
    geom1_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE(CIRCULARSTRING(0 0, 1 1, 2 0), (2 0, 0 0)))");
    geom2_ = fromWKT("POINT (0.1556955 0.5355459)");

    // PostGIS would return false here because geom2 is inside geom1
    // but outside the linearized form of geom1
    ensure_equals(GEOSIntersects(geom1_, geom2_), 1);
    ensure_equals(GEOSIntersects(geom2_, geom1_), 1);
}

template<>
template<>
void object::test<13>()
{
    geom1_ = fromWKT("CURVEPOLYGON (COMPOUNDCURVE(CIRCULARSTRING(0 0, 1 1, 2 0), (2 0, 0 0)))");
    geom2_ = fromWKT("POINT EMPTY");

    ensure_equals(GEOSIntersects(geom1_, geom2_), 0);
    ensure_equals(GEOSIntersects(geom2_, geom1_), 0);
}

} // namespace tut

