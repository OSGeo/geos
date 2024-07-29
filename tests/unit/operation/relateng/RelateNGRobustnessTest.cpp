//
// Test Suite for geos::operation::relateng::RelateNG class.

#include <tut/tut.hpp>
#include <utility.h>

// geos
#include "RelateNGTest.h"

using namespace geos::geom;
using namespace geos::operation::relateng;
using geos::io::WKTReader;
using geos::io::WKTWriter;

namespace tut {
//
// Test Group
//

// Common data used by all tests
struct test_relatengrobuestness_data : test_relateng_support {};


typedef test_group<test_relatengrobuestness_data> group;
typedef group::object object;

group test_relatengrobuestness_group("geos::operation::relateng::RelateNGRobustness");


//--------------------------------------------------------
//  GeometryCollection semantics
//--------------------------------------------------------

// see https://github.com/libgeos/geos/issues/1033
// testGEOS_1033
template<>
template<>
void object::test<1> ()
{
    checkContainsWithin("POLYGON((1 0,0 4,2 2,1 0))",
        "GEOMETRYCOLLECTION(POINT(2 2),POINT(1 0),LINESTRING(1 2,1 1))",
        true);
}

// https://github.com/libgeos/geos/issues/1027
// testGEOS_1027
template<>
template<>
void object::test<2> ()
{
    checkCoversCoveredBy("MULTIPOLYGON (((0 0, 3 0, 3 3, 0 3, 0 0)))",
        "GEOMETRYCOLLECTION ( LINESTRING (1 2, 1 1), POINT (0 0))",
        true);
}

// https://github.com/libgeos/geos/issues/1022
// testGEOS_1022
template<>
template<>
void object::test<3> ()
{
    checkCrosses("GEOMETRYCOLLECTION (POINT (7 1), LINESTRING (6 5, 6 4))",
        "POLYGON ((7 1, 1 3, 3 9, 7 1))",
        false);
}

// https://github.com/libgeos/geos/issues/1011
// testGEOS_1011
template<>
template<>
void object::test<4> ()
{
    std::string a = "LINESTRING(75 15,55 43)";
    std::string b = "GEOMETRYCOLLECTION(POLYGON EMPTY,LINESTRING(75 15,55 43))";
    checkCoversCoveredBy(a, b, true);
    checkEquals(a, b, true);
}

// https://github.com/libgeos/geos/issues/983
// testGEOS_983
template<>
template<>
void object::test<5> ()
{
    std::string a = "POINT(0 0)";
    std::string b = "GEOMETRYCOLLECTION(POINT (1 1), LINESTRING (1 1, 2 2))";
    checkIntersectsDisjoint(a, b, false);
}

// https://github.com/libgeos/geos/issues/982
// testGEOS_982
template<>
template<>
void object::test<6> ()
{
    std::string a = "POINT(0 0)";
    std::string b1 = "GEOMETRYCOLLECTION(POINT(0 0), LINESTRING(0 0, 1 0))";
    checkContainsWithin(b1, a, false);
    checkCoversCoveredBy(b1, a, true);

    std::string b2 = "GEOMETRYCOLLECTION(LINESTRING(0 0, 1 0), POINT(0 0))";
    checkContainsWithin(b2, a, false);
    checkCoversCoveredBy(b2, a, true);
}

// https://github.com/libgeos/geos/issues/981
// testGEOS_981
template<>
template<>
void object::test<7> ()
{
    std::string a = "POINT(0 0)";
    std::string b = "GEOMETRYCOLLECTION(LINESTRING(0 1, 0 0), POINT(0 0))";
    checkRelateMatches(b, a, IntersectionMatrixPattern::CONTAINS_PROPERLY, false);
}


//--------------------------------------------------------
//  Noding robustness problems
//--------------------------------------------------------

// https://github.com/libgeos/geos/issues/1053
// testGEOS_1053
template<>
template<>
void object::test<8> ()
{
    std::string a = "MULTILINESTRING((2 4, 10 10),(15 10,10 5,5 10))";
    std::string b = "MULTILINESTRING((2 4, 10 10))";
    checkRelate(a, b, "1F1F00FF2");
}

// https://github.com/libgeos/geos/issues/968
// testGEOS_968
template<>
template<>
void object::test<9> ()
{
    std::string a2 = "LINESTRING(10 0, 0 20)";
    std::string b2 = "POINT (9 2)";
    checkCoversCoveredBy(a2, b2, true);
}

// xtestGEOS_968_2
// commented out, related to deeper DD vs FP differences

// template<>
// template<>
// void object::test<10> ()
// {
//     std::string a = "LINESTRING(1 0, 0 2)";
//     std::string b = "POINT (0.9 0.2)";
//     //-- this case doesn't work due to numeric rounding for Orientation test
//     checkCoversCoveredBy(a, b,true);
// }

// https://github.com/libgeos/geos/issues/933
// testGEOS_933
template<>
template<>
void object::test<11> ()
{
    std::string a = "LINESTRING (0 0, 1 1)";
    std::string b = "LINESTRING (0.2 0.2, 0.5 0.5)";
    checkCoversCoveredBy(a, b, true);
}

// https://github.com/libgeos/geos/issues/740
// testGEOS_740
template<>
template<>
void object::test<12> ()
{
    std::string a = "POLYGON ((1454700 -331500, 1455100 -330700, 1455466.6191038645 -331281.94727476506, 1455467.8182005754 -331293.26796732045, 1454700 -331500))";
    std::string b = "LINESTRING (1455389.376551584 -331255.3803222172, 1455467.2422460222 -331287.83037053316)";
    checkContainsWithin(a, b, false);
}

//--------------------------------------------------------
//  Robustness failures (TopologyException in old code)
//--------------------------------------------------------

// https://github.com/libgeos/geos/issues/766
// testGEOS_766
template<>
template<>
void object::test<13> ()
{
    std::string a = "POLYGON ((26639.240191093646 6039.3615818717535, 26639.240191093646 5889.361620883223, 28000.000095100608 5889.362081553552, 28000.000095100608 6039.361620882992, 28700.00019021402 6039.361620882992, 28700.00019021402 5889.361822800367, 29899.538842431968 5889.362160452064, 32465.59665091549 5889.362882757903, 32969.2837182586 -1313.697771558439, 31715.832811969216 -1489.87008918589, 31681.039836323587 -1242.3030298361555, 32279.3890331618 -1158.210534269224, 32237.63710287376 -861.1301136466199, 32682.89764107368 -802.0828534499739, 32247.445200905553 5439.292852892075, 31797.06861513178 5439.292852892075, 31797.06861513178 5639.36178850523, 29899.538849750803 5639.361268079038, 26167.69458275995 5639.3602445643955, 26379.03654594742 2617.0293071870683, 26778.062167926924 2644.9318977193907, 26792.01346261031 2445.419086759444, 26193.472956813417 2403.5650586598513, 25939.238114175267 6039.361685403233, 26639.240191093646 6039.3615818717535), (32682.89764107368 -802.0828534499738, 32682.89764107378 -802.0828534499669, 32247.445200905655 5439.292852892082, 32247.445200905553 5439.292852892075, 32682.89764107368 -802.0828534499738))";
    std::string b = "POLYGON ((32450.100392347143 5889.362314133216, 32050.104955691 5891.272957209961, 32100.021071878822 16341.272221116333, 32500.016508656867 16339.361578039587, 32450.100392347143 5889.362314133216))";
    checkIntersectsDisjoint(a, b, true);
}

// https://github.com/libgeos/geos/issues/1026
// testGEOS_1026
template<>
template<>
void object::test<14> ()
{
    std::string a = "POLYGON((335645.7810000004 5677846.65,335648.6579999998 5677845.801999999,335650.8630842535 5677845.143617179,335650.77673334075 5677844.7250704905,335642.90299999993 5677847.498,335645.7810000004 5677846.65))";
    std::string b = "POLYGON((335642.903 5677847.498,335642.894 5677847.459,335645.92 5677846.69,335647.378 5677852.523,335644.403 5677853.285,335644.374 5677853.293,335642.903 5677847.498))";
    checkTouches(a, b, false);
}

// https://github.com/libgeos/geos/issues/1069 =- too large to reproduce here
// https://trac.osgeo.org/postgis/ticket/5583 =- too large to reproduce here
// https://github.com/locationtech/jts/issues/1051
// testJTS_1051
template<>
template<>
void object::test<15> ()
{
    std::string a = "POLYGON ((414188.5999999999 6422867.1, 414193.7 6422866.5, 414205.1 6422859.4, 414223.7 6422846.8, 414229.6 6422843.2, 414235.2 6422835.4, 414224.7 6422837.9, 414219.4 6422842.1, 414210.9 6422849, 414199.2 6422857.6, 414191.1 6422863.4, 414188.5999999999 6422867.1))";
    std::string b = "LINESTRING (414187.2 6422831.6, 414179 6422836.1, 414182.2 6422841.8, 414176.7 6422844, 414184.5 6422859.5, 414188.6 6422867.1)";
    checkIntersectsDisjoint(a, b, true);
}

// https://trac.osgeo.org/postgis/ticket/5362
// testPostGIS_5362
template<>
template<>
void object::test<16> ()
{
    std::string a = "POLYGON ((-707259.66 -1121493.36, -707205.9 -1121605.808, -707310.5388 -1121540.5446, -707318.8200000001 -1121533.21, -707259.66 -1121493.36))";
    std::string b = "POLYGON ((-707356.18 -1121550.69, -707332.82 -1121536.63, -707318.82 -1121533.21, -707321.72 -1121535.08, -707327.4 -1121539.21, -707356.18 -1121550.69))";
    checkRelate(a, b, "2F2101212");
    checkIntersectsDisjoint(a, b, true);
}

//--------------------------------------------------------
//  Topological Inconsistency
//--------------------------------------------------------

// https://github.com/libgeos/geos/issues/1064
// testGEOS_1064
template<>
template<>
void object::test<17> ()
{
    std::string a = "LINESTRING (16.330791631988802 68.75635661578073, 16.332533372319826 68.75496886016562)";
    std::string b = "LINESTRING (16.30641253121884 68.75189557630306, 16.33167771310482 68.75565061843871)";
    checkRelate(a, b, "F01FF0102");
}

// https://github.com/locationtech/jts/issues/396
// testJTS_396
template<>
template<>
void object::test<18> ()
{
    std::string a = "LINESTRING (1 0, 0 2, 0 0, 2 2)";
    std::string b = "LINESTRING (0 0, 2 2)";
    checkRelate(a, b, "101F00FF2");
    checkCoversCoveredBy(a, b, true);
}

//https://github.com/locationtech/jts/issues/270
// testJTS_270
template<>
template<>
void object::test<19> ()
{
    std::string a = "LINESTRING(0.0 0.0, -10.0 1.2246467991473533E-15)";
    std::string b = "LINESTRING(-9.999143275740073 -0.13089595571333978, -10.0 1.0535676356486768E-13)";
    checkRelate(a, b, "FF10F0102");
    checkIntersectsDisjoint(a, b, true);
}



} // namespace tut
