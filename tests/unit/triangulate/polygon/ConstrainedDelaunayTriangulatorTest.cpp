//
// Test Suite for geos::triangulate::polygon::ConstrainedDelaunayTriangulator
//
// tut
#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/triangulate/polygon/ConstrainedDelaunayTriangulator.h>

using geos::triangulate::polygon::ConstrainedDelaunayTriangulator;
using geos::geom::Geometry;


namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_constraineddelaunay_data {

    geos::io::WKTReader r;

    test_constraineddelaunay_data() {}

    void checkTri(const std::string& wkt, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = r.read(wkt);
        std::unique_ptr<Geometry> actual = ConstrainedDelaunayTriangulator::triangulate(geom.get());
        std::unique_ptr<Geometry> expected = r.read(wktExpected);
        // std::cout << std::endl << "actual" << std::endl << *actual << std::endl;
        // std::cout << std::endl << "expected" << std::endl << *expected << std::endl;
        ensure_equals_geometry(expected.get(), actual.get());
    }

    /**
    * Check union of result equals original geom
    * @param wkt
    */
    void checkTri(const std::string& wkt)
    {
        std::unique_ptr<Geometry> geom = r.read(wkt);
        std::unique_ptr<Geometry> actual = ConstrainedDelaunayTriangulator::triangulate(geom.get());
        std::unique_ptr<Geometry> actualUnion = actual->Union();
        // std::cout << std::endl << "actual" << std::endl << *actual << std::endl;
        // std::cout << std::endl << "actualUnion" << std::endl << *actualUnion << std::endl;
        // std::cout << std::endl << "geom" << std::endl << *geom << std::endl;
        ensure_equals_geometry(geom.get(), actualUnion.get());
    }

};


typedef test_group<test_constraineddelaunay_data> group;
typedef group::object object;

group test_constraineddelaunay_group("geos::triangulate::polygon::ConstrainedDelaunayTriangulator");


// testQuad
template<>
template<>
void object::test<1>()
{
    checkTri(
        "POLYGON ((10 10, 20 40, 90 90, 90 10, 10 10))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 10, 10 10)), POLYGON ((90 90, 20 40, 90 10, 90 90)))"
        );
}

// testPent
template<>
template<>
void object::test<2>()
{
    checkTri(
        "POLYGON ((10 10, 20 40, 90 90, 100 50, 90 10, 10 10))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 10, 10 10)), POLYGON ((90 90, 20 40, 100 50, 90 90)), POLYGON ((100 50, 20 40, 90 10, 100 50)))"
        );
}

// testHoleCW
template<>
template<>
void object::test<3>()
{
    checkTri(
        "POLYGON ((10 90, 90 90, 90 20, 10 10, 10 90), (30 70, 80 70, 50 30, 30 70))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 10 90, 30 70, 10 10)), POLYGON ((10 10, 30 70, 50 30, 10 10)), POLYGON ((80 70, 30 70, 90 90, 80 70)), POLYGON ((10 90, 30 70, 90 90, 10 90)), POLYGON ((80 70, 90 90, 90 20, 80 70)), POLYGON ((90 20, 10 10, 50 30, 90 20)), POLYGON ((90 20, 50 30, 80 70, 90 20)))"
        );
}

// testMultiPolygon
template<>
template<>
void object::test<4>()
{
    checkTri(
        "MULTIPOLYGON (((10 10, 20 50, 50 50, 40 20, 10 10)), ((20 60, 60 60, 90 20, 90 90, 20 60)), ((10 90, 10 70, 40 70, 50 90, 10 90)))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 50, 40 20, 10 10)), POLYGON ((50 50, 20 50, 40 20, 50 50)), POLYGON ((90 90, 90 20, 60 60, 90 90)), POLYGON ((90 90, 60 60, 20 60, 90 90)), POLYGON ((10 70, 10 90, 40 70, 10 70)), POLYGON ((50 90, 10 90, 40 70, 50 90)))"
        );
}

// testFail
template<>
template<>
void object::test<5>()
{
    checkTri(
        "POLYGON ((110 170, 138 272, 145 286, 152 296, 160 307, 303 307, 314 301, 332 287, 343 278, 352 270, 385 99, 374 89, 359 79, 178 89, 167 91, 153 99, 146 107, 173 157, 182 163, 191 170, 199 176, 208 184, 218 194, 226 203, 198 252, 188 247, 182 239, 175 231, 167 223, 161 213, 156 203, 155 198, 110 170))"
        );
}

// testTouchingHoles
template<>
template<>
void object::test<6>()
{
    checkTri("POLYGON ((10 90, 90 90, 90 10, 10 10, 10 90), (20 80, 50 70, 30 30, 20 80), (70 20, 50 70, 80 80, 70 20))"
        ,"GEOMETRYCOLLECTION (POLYGON ((10 10, 10 90, 20 80, 10 10)), POLYGON ((30 30, 50 70, 70 20, 30 30)), POLYGON ((80 80, 50 70, 20 80, 80 80)), POLYGON ((20 80, 10 90, 90 90, 20 80)), POLYGON ((10 10, 20 80, 30 30, 10 10)), POLYGON ((80 80, 20 80, 90 90, 80 80)), POLYGON ((70 20, 10 10, 30 30, 70 20)), POLYGON ((90 10, 80 80, 90 90, 90 10)), POLYGON ((10 10, 70 20, 90 10, 10 10)), POLYGON ((80 80, 90 10, 70 20, 80 80)))"
        );
}


} // namespace tut
