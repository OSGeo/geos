//
// Test Suite for geos::triangulate::polygon::PolygonTriangulator
//
// tut
#include <tut/tut.hpp>
#include <utility.h>

// geos
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
// #include <geos/io/WKTWriter.h>
#include <geos/triangulate/polygon/PolygonTriangulator.h>
#include <geos/util.h>

using geos::triangulate::polygon::PolygonTriangulator;
using geos::geom::CoordinateSequence;
using geos::geom::Geometry;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_polygontriangulator_data {

    geos::io::WKTReader r;

    test_polygontriangulator_data() {}

    void checkTri(const std::string& wkt, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = r.read(wkt);
        std::unique_ptr<Geometry> actual = PolygonTriangulator::triangulate(geom.get());
        std::unique_ptr<Geometry> expected = r.read(wktExpected);
        ensure_equals_geometry(expected.get(), actual.get());
    }

    /**
    * Check union of result equals original geom
    * @param wkt
    */
    void checkTri(const std::string& wkt)
    {
        std::unique_ptr<Geometry> geom = r.read(wkt);
        std::unique_ptr<Geometry> actual = PolygonTriangulator::triangulate(geom.get());
        std::unique_ptr<Geometry> actualUnion = actual->Union();
        // std::cout << std::endl << "actual" << std::endl << *actual << std::endl;
        // std::cout << std::endl << "actualUnion" << std::endl << *actualUnion << std::endl;
        // std::cout << std::endl << "geom" << std::endl << *geom << std::endl;

        // compare to fully noded verstion of input polygon
        std::unique_ptr<Geometry> nodedGeom = geom->Union(geom.get());
        ensure_equals_geometry(nodedGeom.get(), actualUnion.get());
    }

};


typedef test_group<test_polygontriangulator_data> group;
typedef group::object object;

group test_polygontriangulator_group("geos::triangulate::polygon::PolygonTriangulator");


// testQuad
template<>
template<>
void object::test<1>()
{
    checkTri(
        "POLYGON ((10 10, 20 40, 90 90, 90 10, 10 10))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 90, 10 10)), POLYGON ((90 90, 90 10, 10 10, 90 90)))"
        );
}

// testPent
template<>
template<>
void object::test<2>()
{
    checkTri(
        "POLYGON ((10 10, 20 40, 90 90, 100 50, 90 10, 10 10))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 90, 10 10)), POLYGON ((90 90, 100 50, 90 10, 90 90)), POLYGON ((90 10, 10 10, 90 90, 90 10)))"
        );
}

// testHoleCW
template<>
template<>
void object::test<3>()
{
    checkTri(
        "POLYGON ((10 90, 90 90, 90 20, 10 10, 10 90), (30 70, 80 70, 50 30, 30 70))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 10 90, 50 30, 10 10)), POLYGON ((10 10, 50 30, 90 20, 10 10)), POLYGON ((10 90, 30 70, 50 30, 10 90)), POLYGON ((10 90, 80 70, 30 70, 10 90)), POLYGON ((10 90, 90 90, 80 70, 10 90)), POLYGON ((50 30, 80 70, 90 20, 50 30)), POLYGON ((80 70, 90 90, 90 20, 80 70)))"
        );
}

// testTouchingHoles
template<>
template<>
void object::test<4>()
{
    checkTri(
        "POLYGON ((10 10, 10 90, 90 90, 90 10, 10 10), (20 80, 30 30, 50 70, 20 80), (50 70, 70 20, 80 80, 50 70))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 10 90, 20 80, 10 10)), POLYGON ((30 30, 50 70, 70 20, 30 30)), POLYGON ((80 80, 50 70, 20 80, 80 80)), POLYGON ((20 80, 10 90, 90 90, 20 80)), POLYGON ((10 10, 20 80, 30 30, 10 10)), POLYGON ((80 80, 20 80, 90 90, 80 80)), POLYGON ((90 10, 10 10, 30 30, 90 10)), POLYGON ((70 20, 80 80, 90 90, 70 20)), POLYGON ((90 10, 30 30, 70 20, 90 10)), POLYGON ((70 20, 90 90, 90 10, 70 20)))"
        );
}

// testRepeatedPoints
template<>
template<>
void object::test<5>()
{
    checkTri(
        "POLYGON ((71 195, 178 335, 178 335, 239 185, 380 210, 290 60, 110 70, 71 195))",
        "GEOMETRYCOLLECTION (POLYGON ((71 195, 178 335, 239 185, 71 195)), POLYGON ((71 195, 239 185, 290 60, 71 195)), POLYGON ((71 195, 290 60, 110 70, 71 195)), POLYGON ((239 185, 380 210, 290 60, 239 185)))"
        );
}

// testMultiPolygon
template<>
template<>
void object::test<6>()
{
    checkTri(
        "MULTIPOLYGON (((10 10, 20 50, 50 50, 40 20, 10 10)), ((10 70, 10 90, 50 90, 40 70, 10 70)), ((20 60, 90 90, 90 20, 60 60, 20 60)))",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 50, 50 50, 10 10)), POLYGON ((50 50, 40 20, 10 10, 50 50)), POLYGON ((90 90, 90 20, 60 60, 90 90)), POLYGON ((60 60, 20 60, 90 90, 60 60)), POLYGON ((10 70, 10 90, 50 90, 10 70)), POLYGON ((50 90, 40 70, 10 70, 50 90)))"
        );
}

// testCeeShape
template<>
template<>
void object::test<7>()
{
    checkTri(
        "POLYGON ((110 170, 138 272, 145 286, 152 296, 160 307, 303 307, 314 301, 332 287, 343 278, 352 270, 385 99, 374 89, 359 79, 178 89, 167 91, 153 99, 146 107, 173 157, 182 163, 191 170, 199 176, 208 184, 218 194, 226 203, 198 252, 188 247, 182 239, 175 231, 167 223, 161 213, 156 203, 155 198, 110 170))"
        );
}

// Linestring input
template<>
template<>
void object::test<8>()
{
    checkTri(
        "LINESTRING (110 170, 138 272, 145 286, 152 296, 160 307, 303 307, 314 301, 332 287, 343 278, 352 270, 385 99, 374 89, 359 79, 178 89, 167 91, 153 99, 146 107, 173 157, 182 163, 191 170, 199 176, 208 184, 218 194, 226 203, 198 252, 188 247, 182 239, 175 231, 167 223, 161 213, 156 203, 155 198, 110 170)",
        "GEOMETRYCOLLECTION EMPTY"
        );
}

// Empty input
template<>
template<>
void object::test<9>()
{
    checkTri(
        "POLYGON EMPTY",
        "GEOMETRYCOLLECTION EMPTY"
        );
}

// Empty input in collection
template<>
template<>
void object::test<10>()
{
    checkTri(
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 90, 90 10, 10 10)), POLYGON EMPTY)",
        "GEOMETRYCOLLECTION (POLYGON ((10 10, 20 40, 90 90, 10 10)), POLYGON ((90 90, 90 10, 10 10, 90 90)))"
        );
}

// Empty mistyped input
template<>
template<>
void object::test<11>()
{
    checkTri(
        "POINT EMPTY",
        "GEOMETRYCOLLECTION EMPTY"
        );
}

//
template<>
template<>
void object::test<12>()
{
    auto shell_seq = geos::detail::make_unique<CoordinateSequence>(CoordinateSequence{Coordinate{0, 0}, Coordinate{0, 10}, Coordinate{3, 10}, Coordinate{3, 0}, Coordinate{0, 0}});
    auto hole_1_seq = geos::detail::make_unique<CoordinateSequence>(CoordinateSequence{Coordinate{1, 1}, Coordinate{2, 1}, Coordinate{2, 2}, Coordinate{1, 2}, Coordinate{1, 1}});
    auto hole_2_seq = geos::detail::make_unique<CoordinateSequence>(CoordinateSequence{Coordinate{1, 8}, Coordinate{1, 9}, Coordinate{2, 9}, Coordinate{2, 8}, Coordinate{1, 8}});

    auto* geom_factory = geos::geom::GeometryFactory::getDefaultInstance();

    auto shell_ring  = geom_factory->createLinearRing(std::move(shell_seq));
    auto hole_1_ring = geom_factory->createLinearRing(std::move(hole_1_seq));
    auto hole_2_ring = geom_factory->createLinearRing(std::move(hole_2_seq));

    std::vector<std::unique_ptr<geos::geom::LinearRing>> holes;
    holes.emplace_back(std::move(hole_1_ring));
    holes.emplace_back(std::move(hole_2_ring));

    auto polygon = geom_factory->createPolygon(std::move(shell_ring), std::move(holes));

    // std::cout << "Before triangulate()" << std::endl;

    auto triangles = geos::triangulate::polygon::PolygonTriangulator::triangulate(polygon.get());

    // std::cout << "After triangulate()" << std::endl;
    // geos::io::WKTWriter writer;
    // std::cout << writer.write(triangles.get()) << std::endl;

    ensure(true);
}


/**
 * A failing case revealing that joining holes by a zero-length cut
 * was introducing duplicate vertices.
 */
template<>
template<>
void object::test<13>()
{
    checkTri(
        "POLYGON ((71 12, 0 0, 7 47, 16 94, 71 52, 71 12), (7 38, 25 48, 7 47, 7 38), (13 59, 13 54, 26 53, 13 59))"
    );
}


/**
* A failing case for hole joining with two touching holes.
* Fails due to PolygonHoleJoiner not handling holes which have same leftmost vertex.
* Note that input is normalized.
*/
// testBadHoleJoinTouchingHoles
template<>
template<>
void object::test<14>()
{
    checkTri(
        "POLYGON ((0 0, 0 9, 9 9, 9 0, 0 0), (1 4, 5 1, 5 4, 1 4), (1 4, 5 5, 6 8, 1 4))"
    );
}

// testBadHoleJoinHolesTouchVertical
template<>
template<>
void object::test<15>()
{
    checkTri(
        "POLYGON ((1 9, 9 9, 9 0, 1 0, 1 9), (1 4, 5 1, 5 4, 1 4), (1 5, 5 5, 6 8, 1 5))"
    );
}

// testBadHoleJoinHoleTouchesShellVertical
template<>
template<>
void object::test<16>()
{
    checkTri(
        "POLYGON ((1 9, 9 9, 9 0, 1 0, 1 9), (1 5, 5 5, 6 8, 1 5))",
        "GEOMETRYCOLLECTION (POLYGON ((1 0, 1 5, 5 5, 1 0)), POLYGON ((6 8, 1 5, 1 9, 6 8)), POLYGON ((9 9, 9 0, 1 0, 9 9)), POLYGON ((6 8, 1 9, 9 9, 6 8)), POLYGON ((9 9, 1 0, 5 5, 9 9)), POLYGON ((5 5, 6 8, 9 9, 5 5)))"
    );
}

// testBadHoleJoinHoleTouchesShell
template<>
template<>
void object::test<17>()
{
    checkTri(
        "POLYGON ((5 5, 9 5, 9 0, 0 0, 5 5), (3 3, 6 1, 5 3, 3 3))",
        "GEOMETRYCOLLECTION (POLYGON ((0 0, 3 3, 6 1, 0 0)), POLYGON ((5 3, 3 3, 5 5, 5 3)), POLYGON ((5 5, 9 5, 9 0, 5 5)), POLYGON ((9 0, 0 0, 6 1, 9 0)), POLYGON ((6 1, 5 3, 5 5, 6 1)), POLYGON ((5 5, 9 0, 6 1, 5 5)))"
    );
}


} // namespace tut
