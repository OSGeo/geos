//
// Test Suite for geos::precision::SimpleGeometryPrecisionReducer class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/precision/SimpleGeometryPrecisionReducer.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
// std
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_sgpr_data {
    typedef std::unique_ptr<geos::geom::Geometry> GeometryPtr;
    typedef geos::geom::GeometryFactory GeometryFactory;

    geos::geom::PrecisionModel pm_float_;
    geos::geom::PrecisionModel pm_fixed_;
    GeometryFactory::Ptr factory_;
    geos::io::WKTReader reader_;
    geos::precision::SimpleGeometryPrecisionReducer reducer_;
    geos::precision::SimpleGeometryPrecisionReducer reducer2_; // keep collapse

    test_sgpr_data() :
        pm_float_(),
        pm_fixed_(1),
        factory_(GeometryFactory::create(&pm_float_, 0)),
        reader_(factory_.get()),
        reducer_(&pm_fixed_),
        reducer2_(&pm_fixed_)
    {
        reducer2_.setRemoveCollapsedComponents(false);
    }
};

typedef test_group<test_sgpr_data> group;
typedef group::object object;

group test_sgpr_group("geos::precision::SimpleGeometryPrecisionReducer");

//
// Test Cases
//

// Test square
template<>
template<>
void object::test<1>
()
{
    GeometryPtr g1(reader_.read("POLYGON (( 0 0, 0 1.4, 1.4 1.4, 1.4 0, 0 0 ))"));
    GeometryPtr g2(reader_.read("POLYGON (( 0 0, 0 1, 1 1, 1 0, 0 0 ))"));

    GeometryPtr result(reducer_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test tiny square collapse
template<>
template<>
void object::test<2>
()
{
    GeometryPtr g1(reader_.read("POLYGON (( 0 0, 0 .4, .4 .4, .4 0, 0 0 ))"));
    GeometryPtr g2(reader_.read("POLYGON EMPTY"));

    GeometryPtr result(reducer_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test square collapse
template<>
template<>
void object::test<3>
()
{
    GeometryPtr g1(reader_.read("POLYGON (( 0 0, 0 1.4, .4 .4, .4 0, 0 0 ))"));
    GeometryPtr g2(reader_.read("POLYGON EMPTY"));

    GeometryPtr result(reducer_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test square keep collapse
template<>
template<>
void object::test<4>
()
{
    GeometryPtr g1(reader_.read("POLYGON (( 0 0, 0 1.4, .4 .4, .4 0, 0 0 ))"));
    GeometryPtr g2(reader_.read("POLYGON (( 0 0, 0 1, 0 0, 0 0, 0 0 ))"));

    GeometryPtr result(reducer2_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test line
template<>
template<>
void object::test<5>
()
{
    GeometryPtr g1(reader_.read("LINESTRING ( 0 0, 0 1.4 )"));
    GeometryPtr g2(reader_.read("LINESTRING (0 0, 0 1)"));

    GeometryPtr result(reducer_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test line remove collapse
template<>
template<>
void object::test<6>
()
{
    GeometryPtr g1(reader_.read("LINESTRING ( 0 0, 0 .4 )"));
    GeometryPtr g2(reader_.read("LINESTRING EMPTY"));

    GeometryPtr result(reducer_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

// Test line keep collapse
template<>
template<>
void object::test<7>
()
{
    GeometryPtr g1(reader_.read("LINESTRING ( 0 0, 0 .4 )"));
    GeometryPtr g2(reader_.read("LINESTRING ( 0 0, 0 0 )"));

    GeometryPtr result(reducer2_.reduce(g1.get()));

    ensure(result->equalsExact(g2.get()));
}

} // namespace tut

