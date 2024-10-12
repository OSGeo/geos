//
// Test Suite for geos::operation::overlay::validate::OverlayResultValidator class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/overlayng/OverlayNG.h>
#include <geos/operation/overlay/validate/OverlayResultValidator.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/algorithm/PointLocator.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/Coordinate.h>
// std
#include <memory>

namespace tut {
using namespace geos::operation::overlay;
using namespace geos::operation::overlay::validate;

//
// Test Group
//

// Common data used by tests
struct test_overlayresultvalidator_data {
    geos::io::WKTReader wktreader;
    geos::algorithm::PointLocator locator;

    typedef geos::geom::Geometry::Ptr GeomPtr;

    GeomPtr g0, g1, gres;

    test_overlayresultvalidator_data()
        :
        wktreader()
    {
    }

};

using geos::operation::overlayng::OverlayNG;


typedef test_group<test_overlayresultvalidator_data> group;
typedef group::object object;

group test_overlayresultvalidator_group("geos::operation::overlay::validate::OverlayResultValidator");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{

    std::string wkt0("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    std::string wktres("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_gres(wktreader.read(wkt1));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(validator.isValid(OverlayNG::UNION));
}

template<>
template<>
void object::test<2>
()
{

    std::string wkt0("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    std::string wktres("POLYGON((0 0, 10.001 0, 10 10, 0 10, 0 0))");
    GeomPtr p_gres(wktreader.read(wktres));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(! validator.isValid(OverlayNG::UNION));
}

template<>
template<>
void object::test<3>
()
{

    std::string wkt0("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    std::string wktres("POLYGON((0 0, 9.999 0, 10 10, 0 10, 0 0))");
    GeomPtr p_gres(wktreader.read(wktres));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(! validator.isValid(OverlayNG::UNION));
}

// Result of union has an hole not in input
template<>
template<>
void object::test<4>
()
{

    std::string wkt0("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    std::string wktres("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(5 5, 5 6, 6 6, 5 5))");
    GeomPtr p_gres(wktreader.read(wktres));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(! validator.isValid(OverlayNG::UNION));
}


template<>
template<>
void object::test<5>
()
{

    std::string wkt0("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON((5 0, 1500 0, 1500 10, 5 10, 5 0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    // small shift
    std::string wktres("POLYGON((0 0, 750 0.0001, 150 0, 150 10, 0 10, 0 0))");
    GeomPtr p_gres(wktreader.read(wktres));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(! validator.isValid(OverlayNG::UNION));
}

template<>
template<>
void object::test<6>
()
{

    std::string
    wkt0("POLYGON ((20.0 40.0, 20.0 200.0, 180.0 200.0, 180.0 120.0, 140.0 120.0, 180.0 119.0, 180.0 40.0, 20.0 40.0), (140.0 160.0, 80.0 120.0, 140.0 80.0, 140.0 160.0))");
    GeomPtr p_g0(wktreader.read(wkt0));

    std::string wkt1("POLYGON ((200.0 160.0, 150.0 160.0, 150.0 80.0, 200.0 80.0, 200.0 160.0))");
    GeomPtr p_g1(wktreader.read(wkt1));

    std::string
    wktres("MULTIPOLYGON (((20.0 40.0, 20.0 200.0, 180.0 200.0, 180.0 160.0, 150.0 160.0, 150.0 120.0, 150.0 80.0, 180.0 80.0, 180.0 40.0, 20.0 40.0), (80.0 120.0, 140.0 80.0, 140.0 120.0, 140.0 160.0, 80.0 120.0)), ((150.0 120.0, 180.0 120.0, 180.0 160.0, 200.0 160.0, 200.0 80.0, 180.0 80.0, 180.0 119.0, 150.0 120.0)))");
    GeomPtr p_gres(wktreader.read(wktres));

    OverlayResultValidator validator(*p_g0, *p_g1, *p_gres);

    ensure(! validator.isValid(OverlayNG::SYMDIFFERENCE));
}


//
// TODO: add tests for other operations
//       and other geometry types (IFF we drop the limit of the
//       class to only work with areal geoms)


} // namespace tut

