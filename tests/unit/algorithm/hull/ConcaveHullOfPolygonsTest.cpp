//
// Test Suite for geos::algorithm::hull::ConcaveHull

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/hull/ConcaveHullOfPolygons.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/constants.h>
#include <utility.h>
// std
#include <string>
#include <memory>

using geos::algorithm::hull::ConcaveHullOfPolygons;
using geos::io::WKTReader;
using geos::io::WKTWriter;
using geos::geom::Geometry;

namespace tut {

//
// Test Group
//
struct test_concavehullofpolygons_data {
    WKTReader reader_;
    test_concavehullofpolygons_data() {};

    void
    checkHull(const std::string& wkt, double maxLen, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = reader_.read(wkt);
        std::unique_ptr<Geometry> actual = ConcaveHullOfPolygons::concaveHullByLength(geom.get(), maxLen);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        ensure_equals_geometry(expected.get(), actual.get());
    }

    void
    checkHullByLenRatio(const std::string& wkt, double lenRatio, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = reader_.read(wkt);
        std::unique_ptr<Geometry> actual = ConcaveHullOfPolygons::concaveHullByLengthRatio(geom.get(), lenRatio);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        ensure_equals_geometry(expected.get(), actual.get());
    }

    void
    checkHullTight(const std::string& wkt, double maxLen, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = reader_.read(wkt);
        std::unique_ptr<Geometry> actual = ConcaveHullOfPolygons::concaveHullByLength(geom.get(), maxLen, true, false);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        ensure_equals_geometry(expected.get(), actual.get());
    }

    void
    checkHullWithHoles(const std::string& wkt, double maxLen, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = reader_.read(wkt);
        std::unique_ptr<Geometry> actual = ConcaveHullOfPolygons::concaveHullByLength(geom.get(), maxLen, false, true);
        std::unique_ptr<Geometry> expected = reader_.read(wktExpected);
        // std::cout << "ACTUAL" << std::endl << actual->toText() << std::endl;
        // std::cout << "EXPECT" << std::endl << expected->toText() << std::endl;
        ensure_equals_geometry(expected.get(), actual.get());
    }
};


typedef test_group<test_concavehullofpolygons_data> group;
typedef group::object object;

group test_concavehullofpolygons_group("geos::algorithm::hull::ConcaveHullOfPolygons");


//
// testEmpty
//
template<>
template<>
void object::test<1>()
{

    const std::string wkt = "MULTIPOLYGON EMPTY";
    checkHullTight(wkt, 1000,
        "POLYGON EMPTY" );
  }

//
// testPolygon
//
template<>
template<>
void object::test<2>()
{

    const std::string wkt = "POLYGON ((1 9, 5 8, 9 9, 4 4, 7 1, 2 1, 1 9))";
    checkHullTight(wkt, 1000,
        "POLYGON ((1 9, 5 8, 9 9, 4 4, 7 1, 2 1, 1 9))" );
    checkHull(wkt, 1000,
        "POLYGON ((1 9, 9 9, 7 1, 2 1, 1 9))" );
}

//
// testSimple
//
template<>
template<>
void object::test<3>()
{

    const std::string wkt = "MULTIPOLYGON (((100 200, 100 300, 150 250, 200 300, 200 200, 100 200)), ((100 100, 200 100, 150 50, 100 100)))";
    checkHullTight(wkt, 1000,
        "POLYGON ((100 100, 100 200, 100 300, 150 250, 200 300, 200 200, 200 100, 150 50, 100 100))" );
    checkHull(wkt, 1000,
        "POLYGON ((100 100, 100 200, 100 300, 200 300, 200 200, 200 100, 150 50, 100 100))" );
}

//
// testSimpleNeck
//
template<>
template<>
void object::test<4>()
{

    const std::string wkt = "MULTIPOLYGON (((1 9, 5 8, 9 9, 9 6, 6 4, 4 4, 1 6, 1 9)), ((1 1, 4 3, 6 3, 9 1, 1 1)))";
    checkHullTight(wkt, 0, wkt );
    checkHullTight(wkt, 2,
        "POLYGON ((6 3, 9 1, 1 1, 4 3, 4 4, 1 6, 1 9, 5 8, 9 9, 9 6, 6 4, 6 3))" );
    checkHullTight(wkt, 6,
        "POLYGON ((1 1, 1 6, 1 9, 5 8, 9 9, 9 6, 9 1, 1 1))" );
}

//
// testPoly3Concave1
//
template<>
template<>
void object::test<5>()
{

    checkHullTight("MULTIPOLYGON (((1 5, 5 8, 5 5, 1 5)), ((5 1, 1 4, 5 4, 5 1)), ((6 8, 9 6, 7 5, 9 4, 6 1, 6 8)))",
       100, "POLYGON ((6 8, 9 6, 7 5, 9 4, 6 1, 5 1, 1 4, 1 5, 5 8, 6 8))" );
}

//
// testPoly3Concave3
//
template<>
template<>
void object::test<6>()
{

    const std::string wkt = "MULTIPOLYGON (((0 7, 4 10, 3 7, 5 6, 4 5, 0 7)), ((4 0, 0 2, 3 4, 5 3, 4 0)), ((9 10, 8 8, 10 9, 8 5, 10 3, 7 0, 6 3, 7 4, 7 6, 5 9, 9 10)))";

    checkHullTight( wkt, 0, wkt );
    checkHullTight( wkt, 2,
        "POLYGON ((5 3, 4 0, 0 2, 3 4, 4 5, 0 7, 4 10, 5 9, 9 10, 8 8, 10 9, 8 5, 10 3, 7 0, 6 3, 5 3))" );
    checkHullTight( wkt, 4,
        "POLYGON ((4 0, 0 2, 3 4, 4 5, 0 7, 4 10, 5 9, 9 10, 8 8, 10 9, 8 5, 10 3, 7 0, 4 0))" );
    checkHullTight( wkt, 100,
        "POLYGON ((0 7, 4 10, 9 10, 8 8, 10 9, 8 5, 10 3, 7 0, 4 0, 0 2, 0 7))" );

    checkHullByLenRatio( wkt, 0, wkt);
    checkHullByLenRatio( wkt, 0.2,
        "POLYGON ((5 9, 9 10, 10 9, 8 5, 10 3, 7 0, 6 3, 5 3, 4 0, 0 2, 3 4, 4 5, 0 7, 4 10, 5 9))" );
    checkHullByLenRatio( wkt, 0.5,
        "POLYGON ((5 9, 9 10, 10 9, 8 5, 10 3, 7 0, 4 0, 0 2, 3 4, 4 5, 0 7, 4 10, 5 9))" );
    checkHullByLenRatio( wkt, 1,
        "POLYGON ((9 10, 10 9, 10 3, 7 0, 4 0, 0 2, 0 7, 4 10, 9 10))" );
}

//
// testPoly3WithHole
//
template<>
template<>
void object::test<7>()
{
    const std::string wkt = "MULTIPOLYGON (((1 9, 5 9, 5 7, 3 7, 3 5, 1 5, 1 9)), ((1 4, 3 4, 3 2, 5 2, 5 0, 1 0, 1 4)), ((6 9, 8 9, 9 5, 8 0, 6 0, 6 2, 8 5, 6 7, 6 9)))";
    checkHullWithHoles( wkt, 0.99, wkt);
    checkHullWithHoles( wkt, 1,
        "POLYGON ((1 5, 1 9, 5 9, 6 9, 8 9, 9 5, 8 0, 6 0, 5 0, 1 0, 1 4, 1 5), (3 4, 3 2, 5 2, 6 2, 8 5, 6 7, 5 7, 3 7, 3 5, 3 4))");
    checkHullWithHoles( wkt, 2.5,
        "POLYGON ((1 5, 1 9, 5 9, 6 9, 8 9, 9 5, 8 0, 6 0, 5 0, 1 0, 1 4, 1 5), (3 4, 3 2, 5 2, 6 2, 8 5, 6 7, 5 7, 3 7, 3 5, 3 4))");
    checkHullWithHoles( wkt, 4,
        "POLYGON ((1 5, 1 9, 5 9, 6 9, 8 9, 9 5, 8 0, 6 0, 5 0, 1 0, 1 4, 1 5), (5 2, 6 2, 8 5, 6 7, 5 7, 3 5, 5 2))");
    checkHullWithHoles( wkt, 9,
        "POLYGON ((6 9, 8 9, 9 5, 8 0, 6 0, 5 0, 1 0, 1 4, 1 5, 1 9, 5 9, 6 9))");
}

// testPolygonHole
template<>
template<>
void object::test<8>()
{
    checkHullByLenRatio(
        "MULTIPOLYGON (((1 1, 10 3, 19 1, 16 8, 19 7, 19 19, 10 20, 8 17, 1 19, 1 1), (3 4, 5 10, 3 16, 9 14, 14 15, 15 9, 13 5, 3 4)))", 
        0.9, 
        "POLYGON ((10 20, 19 19, 19 7, 19 1, 10 3, 1 1, 1 19, 10 20), (13 5, 15 9, 14 15, 9 14, 3 16, 5 10, 3 4, 13 5))" );    
}


// testPolygonNestedPoly
template<>
template<>
void object::test<9>()
{
    checkHullByLenRatio(
        "MULTIPOLYGON (((1 1, 10 3, 19 1, 16 8, 19 7, 19 19, 10 20, 8 17, 1 19, 1 1), (3 4, 5 10, 3 16, 9 14, 14 15, 15 9, 13 5, 3 4)), ((6 10, 7 13, 10 12, 12 13, 13 11, 11 9, 13 8, 9 6, 6 6, 7 8, 6 10)))", 
        0.9, 
        "MULTIPOLYGON (((10 20, 19 19, 19 7, 19 1, 10 3, 1 1, 1 19, 10 20), (13 5, 15 9, 14 15, 9 14, 3 16, 5 10, 3 4, 13 5)), ((7 13, 10 12, 12 13, 13 11, 11 9, 13 8, 9 6, 6 6, 7 8, 6 10, 7 13)))" );    
}

} // namespace tut
