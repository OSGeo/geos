// $Id$
//
// Test Suite for geos::precision::GeometryPrecisionReducer class.

// tut
#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/precision/GeometryPrecisionReducer.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
// std
#include <string>

using namespace geos::geom;
using namespace geos::precision;

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_gpr_data {

    PrecisionModel pm_float_;
    PrecisionModel pm_fixed_;
    GeometryFactory::Ptr factory_;
    GeometryFactory::Ptr factory_fixed_;
    geos::io::WKTReader reader_;
    GeometryPrecisionReducer reducer_;
    GeometryPrecisionReducer reducerKeepCollapse_; // keep collapse
    GeometryPrecisionReducer reducerChangePM_; // change precision model

    test_gpr_data() :
        pm_float_(),
        pm_fixed_(1),
        factory_(GeometryFactory::create(&pm_float_, 0)),
        factory_fixed_(GeometryFactory::create(&pm_fixed_, 0)),
        reader_(factory_.get()),
        reducer_(pm_fixed_),
        reducerKeepCollapse_(pm_fixed_),
        reducerChangePM_(*factory_fixed_)
    {
        reducerKeepCollapse_.setRemoveCollapsedComponents(false);
        reducerChangePM_.setChangePrecisionModel(true);
        reducer_.setRemoveCollapsedComponents(true);
        reducer_.setChangePrecisionModel(false);
    }

    void checkReducePointwise(
        const std::string& wkt,
        const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> g = reader_.read(wkt);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        std::unique_ptr<Geometry> actual = GeometryPrecisionReducer::reducePointwise(*g, pm_fixed_);
        ensure_equals_geometry(expected.get(), actual.get());
        ensure("Factories are not the same", expected->getFactory() == actual->getFactory());
    }

    void checkReduceKeepCollapse(
        const std::string& wkt,
        const std::string& wktExpected)
    {
        checkReduceAny(wkt, wktExpected, reducerKeepCollapse_);
    }

    void checkReduceNewPM(
        const std::string& wkt,
        const std::string& wktExpected)
    {
        checkReduceAny(wkt, wktExpected, reducerChangePM_, false);
    }

    void checkReduce(
        double scaleFactor,
        const std::string& wkt,
        const std::string& wktExpected)
    {
        PrecisionModel pm(scaleFactor);
        GeometryPrecisionReducer reducer(pm);
        reducer.setRemoveCollapsedComponents(true);
        reducer.setChangePrecisionModel(false);
        checkReduceAny(wkt, wktExpected, reducer);
    }

    void checkReduce(
        const std::string& wkt,
        const std::string& wktExpected)
    {
        checkReduceAny(wkt, wktExpected, reducer_);
    }

    void checkReduceAny(
        const std::string& wkt,
        const std::string& wktExpected,
        GeometryPrecisionReducer& reducerAny,
        bool samePM = true)
    {
        std::unique_ptr<Geometry> g = reader_.read(wkt);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        std::unique_ptr<Geometry> actual = reducerAny.reduce(*g);

        // std::cout << *actual << std::endl;

        ensure_equals_geometry(expected.get(), actual.get());
        if (samePM)
            ensure("Factories are not the same", actual->getFactory() == g->getFactory());
        else
            ensure("Factories are the same", actual->getFactory() != g->getFactory());
    }

};

typedef test_group<test_gpr_data> group;
typedef group::object object;

group test_gpr_group("geos::precision::GeometryPrecisionReducer");

//
// Test Cases
//

// Test square
template<>
template<>
void object::test<1> ()
{
    checkReduce(
        "POLYGON (( 0 0, 0 1.4, 1.4 1.4, 1.4 0, 0 0 ))",
        "POLYGON (( 0 0, 0 1, 1 1, 1 0, 0 0 ))"
        );
}

// Test tiny square collapse
template<>
template<>
void object::test<2> ()
{
    checkReduce(
        "POLYGON (( 0 0, 0 .4, .4 .4, .4 0, 0 0 ))",
        "POLYGON EMPTY"
        );
}

// Test square collapse
template<>
template<>
void object::test<3> ()
{
    checkReduce(
        "POLYGON (( 0 0, 0 1.4, .4 .4, .4 0, 0 0 ))",
        "POLYGON EMPTY"
        );
}

// Test square keep collapse
template<>
template<>
void object::test<4> ()
{
    checkReduceKeepCollapse(
        "POLYGON (( 0 0, 0 1.4, .4 .4, .4 0, 0 0 ))",
        "POLYGON EMPTY"
        );
}

// Test line
template<>
template<>
void object::test<5> ()
{
    checkReduce(
        "LINESTRING ( 0 0, 0 1.4 )",
        "LINESTRING (0 0, 0 1)"
        );
}

// testLineNotNoded
template<>
template<>
void object::test<6> ()
{
    checkReduce(
        "LINESTRING(1 1, 3 3, 9 9, 5.1 5, 2.1 2)",
        "LINESTRING(1 1, 3 3, 9 9, 5 5, 2 2)"
        );
}


// Test line remove collapse
template<>
template<>
void object::test<7> ()
{
    checkReduce(
        "LINESTRING ( 0 0, 0 .4 )",
        "LINESTRING EMPTY"
        );
}

// /* Test line keep collapse */
template<>
template<>
void object::test<8>
()
{
    checkReduceKeepCollapse(
        "LINESTRING (0 0, 0 .4)",
        "LINESTRING (0 0, 0 0)"
        );
}

// Test square with changed PM
template<>
template<>
void object::test<9> ()
{
    checkReduceNewPM(
        "POLYGON (( 0 0, 0 1.4, 1.4 1.4, 1.4 0, 0 0 ))",
        "POLYGON (( 0 0, 0 1, 1 1, 1 0, 0 0 ))"
        );
}

// Test points with changed PM
template<>
template<>
void object::test<10> ()
{
    checkReduceNewPM(
        "MULTIPOINT ((0 0), (0 1.4), (1.4 1.4), (1.4 0), (0.9 0))",
        "MULTIPOINT ((0 0), (0 1), (1 1), (1 0), (1 0))"
    );
}

// testPoint
template<>
template<>
void object::test<11> ()
{
    checkReduce(
        "POINT(1.1 4.9)",
        "POINT(1 5)");
}

// testMultiPoint
template<>
template<>
void object::test<12> ()
{
    checkReduce(
        "MULTIPOINT( (1.1 4.9),(1.2 4.8), (3.3 6.6))",
        "MULTIPOINT((1 5), (1 5), (3 7))");
}

// testPolgonWithCollapsedLine
template<>
template<>
void object::test<13> ()
{
    checkReduce(
        "POLYGON ((10 10, 100 100, 200 10.1, 300 10, 10 10))",
        "POLYGON ((10 10, 100 100, 200 10, 10 10))");
}

// testPolgonWithCollapsedPoint
template<>
template<>
void object::test<14> ()
{
    checkReduce(
        "POLYGON ((10 10, 100 100, 200 10.1, 300 100, 400 10, 10 10))",
        "MULTIPOLYGON (((10 10, 100 100, 200 10, 10 10)), ((200 10, 300 100, 400 10, 200 10)))"
        );
}

// testMultiPolgonCollapse
template<>
template<>
void object::test<15> ()
{
    checkReduce(
        "MULTIPOLYGON (((1 9, 5 9, 5 1, 1 1, 1 9)), ((5.2 8.7, 9 8.7, 9 1, 5.2 1, 5.2 8.7)))",
        "MULTIPOLYGON (((1 1, 1 9, 5 9, 9 9, 9 1, 5 1, 1 1)))");
}

// testGC
template<>
template<>
void object::test<16> ()
{
    checkReduce(
        "GEOMETRYCOLLECTION (POINT (1.1 2.2), MULTIPOINT ((1.1 2), (3.1 3.9)), LINESTRING (1 2.1, 3 3.9), MULTILINESTRING ((1 2, 3 4), (5 6, 7 8)), POLYGON ((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), MULTIPOLYGON (((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), ((7 2, 3 2, 3 -2, 7 -2, 7 2))))",
        "GEOMETRYCOLLECTION (POINT (1 2),     MULTIPOINT ((1 2), (3 4)),       LINESTRING (1 2, 3 4),     MULTILINESTRING ((1 2, 3 4), (5 6, 7 8)), POLYGON ((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), MULTIPOLYGON (((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), ((7 2, 3 2, 3 -2, 7 -2, 7 2))))"
    );
}

// testGCPolygonCollapse
template<>
template<>
void object::test<17> ()
{
    checkReduce(
        "GEOMETRYCOLLECTION (POINT (1.1 2.2), POLYGON ((10 10, 100 100, 200 10.1, 300 100, 400 10, 10 10)) )",
        "GEOMETRYCOLLECTION (POINT (1 2),     MULTIPOLYGON (((10 10, 100 100, 200 10, 10 10)), ((200 10, 300 100, 400 10, 200 10))) )"
    );
}

// testGCNested
template<>
template<>
void object::test<18> ()
{
    checkReduce(
        "GEOMETRYCOLLECTION (POINT (1.1 2.2), GEOMETRYCOLLECTION( POINT (1.1 2.2), LINESTRING (1 2.1, 3 3.9) ) )",
        "GEOMETRYCOLLECTION (POINT (1 2),     GEOMETRYCOLLECTION( POINT (1 2),     LINESTRING (1 2, 3 4) ) )"
    );
}

// testPolgonWithCollapsedLinePointwise
template<>
template<>
void object::test<19> ()
{
    checkReducePointwise(
        "POLYGON ((10 10, 100 100, 200 10.1, 300 10, 10 10))",
        "POLYGON ((10 10, 100 100, 200 10,   300 10, 10 10))"
        );
}

// testPolgonWithCollapsedPointPointwise
template<>
template<>
void object::test<20> ()
{
    checkReducePointwise(
        "POLYGON ((10 10, 100 100, 200 10.1, 300 100, 400 10, 10 10))",
        "POLYGON ((10 10, 100 100, 200 10,   300 100, 400 10, 10 10))"
        );
}

// testGridsize
template<>
template<>
void object::test<21> ()
{
    checkReduce(-100,
        "POLYGON ((100 120, 190 400, 485 398, 250 380, 400 100, 100 120))",
        "POLYGON ((200 400, 300 400, 400 100, 100 100, 200 400))");
}

/**
* Test issue showing bug in SnapRoundingNoder not passing tolerance to MCIndexNoder.
*
* See https://trac.osgeo.org/geos/ticket/1127
*/
template<>
template<>
void object::test<22> ()
{
    checkReduce(-612,
        "POLYGON((3670939.6336634574 3396937.3777869204, 3670995.4715200397 3396926.0316904164, 3671077.280213823 3396905.4302639295, 3671203.8838707027 3396908.120176068, 3671334.962571111 3396904.8310892633, 3670037.299066126 3396904.8310892633, 3670037.299066126 3398075.9808747065, 3670939.6336634574 3396937.3777869204))",
        "POLYGON ((3670164 3396600, 3670164 3397824, 3670776 3397212, 3670776 3396600, 3670164 3396600))");
}

// https://github.com/libgeos/geos/issues/748
template<>
template<>
void object::test<23> ()
{
    checkReduce(1, "POINT EMPTY", "POINT EMPTY");
}

// https://github.com/libgeos/geos/issues/811
// template<>
// template<>
// void object::test<24> ()
// {
//     std::string wkt("POLYGON ((127.11125000000001 34.55639, 127.11125000000001 34.56839, 127.12325000000001 34.56839, 127.12325000000001 34.559037113624825, 127.123156453 34.5589634680001, 127.122811919 34.5587243150001, 127.122414652 34.5584866740001, 127.122002904 34.5582775410001, 127.121541652 34.5580788670001, 127.121044598 34.5579037090001, 127.12055893 34.5577654690001, 127.120676924 34.557430595, 127.120795189 34.5569197390001, 127.12083733777376 34.55639, 127.11125000000001 34.55639), (127.117461568 34.562519572, 127.117607152 34.562312309, 127.117578583 34.5623224060001, 127.117254733 34.5621607510001, 127.117314277 34.5620469410001, 127.118203396 34.561428344, 127.118190156 34.5613418900001, 127.118384564 34.5613216470001, 127.118737508 34.561522295, 127.119362293 34.561425193, 127.119599297 34.5614524040001, 127.119733456 34.561485114, 127.11992963 34.561673145, 127.120093027 34.5620903450001, 127.120079113 34.562106242, 127.120189288 34.562190672, 127.120516852 34.5621474980001, 127.120628803 34.5621547800001, 127.120618491 34.5622284200001, 127.120774952 34.562272916, 127.12096395 34.5623545190001, 127.121092847 34.5622872980001, 127.121047121 34.562239017, 127.120954155 34.5622026450001, 127.120956522 34.562114165, 127.121016436 34.562116614, 127.121027842 34.5620463710001, 127.121154732 34.561871357, 127.121171039 34.561875641, 127.121187768 34.561831698, 127.121292353 34.5617865670001, 127.12155222 34.5617921530001, 127.121600392 34.561753272, 127.121769634 34.5612906470001, 127.121871731 34.5613190400001, 127.121888898 34.5613071240001, 127.12189454 34.5613286010001, 127.122906822 34.5618186950001, 127.122965601 34.5620002050001, 127.122436258 34.5623006220001, 127.122388016 34.562411323, 127.122163789 34.562609222, 127.122195096 34.5626851820001, 127.122188291 34.5627269080001, 127.122039124 34.562739865, 127.121948872 34.562636766, 127.121954912 34.562624176, 127.12164618 34.56258689, 127.121555526 34.562560648, 127.121529786 34.563089668, 127.12132344 34.5634194470001, 127.121323793 34.5635131510001, 127.12137824 34.563570618, 127.121008002 34.5639598650001, 127.120796029 34.5640138700001, 127.120592147 34.5636423400001, 127.120479325 34.5633902170001, 127.119916637 34.5626650310001, 127.119816205 34.562558858, 127.119573987 34.562359648, 127.119427371 34.5622498590001, 127.119269955 34.562177192, 127.118943752 34.5620657370001, 127.118614649 34.5620455640001, 127.11852493 34.5620550880001, 127.117776763 34.562382928, 127.11769759 34.5623760980001, 127.117683778 34.5624113220001, 127.117601744 34.562384149, 127.117926302 34.5618559370001, 127.117923867 34.5617969850001, 127.117827728 34.5619306070001, 127.117603496 34.5623196400001, 127.117462315 34.562520066, 127.117245225 34.562385186, 127.117461568 34.562519572))");
//     checkReduce(100000, wkt, "POLYGON EMPTY");
// }

// https://github.com/libgeos/geos/issues/662
template<>
template<>
void object::test<25> ()
{
    checkReduce(1, "MULTIPOLYGON EMPTY", "MULTIPOLYGON EMPTY");
}

template<>
template<>
void object::test<26> ()
{
    checkReduce(10,
        "MULTIPOLYGON(((0 0, 1 0, 1 1, 0 1, 0 0)))",
        "MULTIPOLYGON(((0 0, 1 0, 1 1, 0 1, 0 0)))");
}

template<>
template<>
void object::test<28> ()
{
    checkReduce(10,
        "MULTIPOLYGON(((0 0, 1 0, 1 1, 0 1, 0 0)), ((10 10, 11 10, 11 11, 10 11, 10 10)))",
        "MULTIPOLYGON(((0 0, 1 0, 1 1, 0 1, 0 0)), ((10 10, 11 10, 11 11, 10 11, 10 10)))");
}



} // namespace tut

