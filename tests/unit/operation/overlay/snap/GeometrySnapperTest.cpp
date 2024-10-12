//
// Test Suite for geos::operation::overlay::snap::GeometrySnapper class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/io/WKTReader.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_geometrysnapper_data {
    typedef std::unique_ptr<geos::geom::Geometry> GeomPtr;

    geos::io::WKTReader reader;

    typedef geos::operation::overlay::snap::GeometrySnapper GeometrySnapper;

    test_geometrysnapper_data()
        :
        reader()
    {
    }
};

typedef test_group<test_geometrysnapper_data> group;
typedef group::object object;

group test_geometrysnapper_group("geos::operation::overlay::snap::GeometrySnapper");

//
// Test Cases
//

// Test vertices snapping
template<>
template<>
void object::test<1>
()
{
    GeomPtr src(reader.read(
                    "POLYGON ((0 0, 0 100, 100 100, 100 0, 0 0))"
                ));

    GeometrySnapper snapper(*(src.get()));

    GeomPtr snap(reader.read(
                     "MULTIPOINT ((0 0), (0 100.0000001), (100 100), (100 0))"
                 ));

    GeomPtr expected(reader.read(
                         "POLYGON ((0 0, 0 100.0000001, 100 100, 100 0, 0 0))"
                     ));

    GeomPtr ret(snapper.snapTo(*(snap.get()), 0.000001));

    ensure(ret->equalsExact(expected.get(), 0));

}

// Test vertices snapping
template<>
template<>
void object::test<2>
()
{
    GeomPtr src(reader.read(
                    "POLYGON ((0 0, 0 100, 100 100, 100 0, 0 0))"
                ));

    GeometrySnapper snapper(*(src.get()));

    GeomPtr snap(reader.read(
                     "MULTIPOINT ((0.0000001 50))"
                 ));

    GeomPtr expected(reader.read(
                         "POLYGON ((0 0, 0.0000001 50, 0 100, 100 100, 100 0, 0 0))"
                     ));

    GeomPtr ret(snapper.snapTo(*(snap.get()), 0.000001));

    ensure(ret->equalsExact(expected.get(), 0));
}



} // namespace tut
