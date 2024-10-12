//
// Test Suite for geos::operation::distance::DistanceOp class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/distance/DistanceOp.h>
#include <geos/constants.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/LineSegment.h>
#include <geos/geom/LineString.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/algorithm/PointLocator.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/util.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_distanceop_data {
    geos::io::WKTReader wktreader;

    typedef geos::geom::Geometry::Ptr GeomPtr;
    typedef std::unique_ptr<geos::geom::CoordinateSequence> CSPtr;

    test_distanceop_data()
        : wktreader()
    {}
};

typedef test_group<test_distanceop_data> group;
typedef group::object object;

group test_distanceop_group("geos::operation::distance::DistanceOp");

//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(0 0)");
    std::string wkt1("POINT(10 0)");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(0, 0));
    ensure_equals(cs->getAt(1), Coordinate(10, 0));
}

template<>
template<>
void object::test<2>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(0 0)");
    std::string wkt1("MULTIPOINT((10 0), (50 30))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(0, 0));
    ensure_equals(cs->getAt(1), Coordinate(10, 0));

}

template<>
template<>
void object::test<3>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(3 0)");
    std::string wkt1("LINESTRING(0 10, 50 10, 100 50)");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(3, 0));
    ensure_equals(cs->getAt(1), Coordinate(3, 10));

}

template<>
template<>
void object::test<4>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(3 0)");
    std::string wkt1("MULTILINESTRING((34 54, 60 34),(0 10, 50 10, 100 50))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(3, 0));
    ensure_equals(cs->getAt(1), Coordinate(3, 10));

}

template<>
template<>
void object::test<5>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(35 60)");
    std::string wkt1("POLYGON((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));

}

template<>
template<>
void object::test<6>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(35 60)");
    std::string
    wkt1("MULTIPOLYGON(((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)),( (100 100, 150 100, 150 150, 100 150, 100 100),(120 120, 120 130, 130 130, 130 120, 120 120)))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));

}

template<>
template<>
void object::test<7>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(35 60)");

    // This is an invalid geom... anyway
    std::string
    wkt1("GEOMETRYCOLLECTION(MULTIPOLYGON(((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)),( (100 100, 150 100, 150 150, 100 150, 100 100),(120 120, 120 130, 130 130, 130 120, 120 120)) ), POLYGON((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)), MULTILINESTRING((34 54, 60 34),(0 10, 50 10, 100 50)), LINESTRING(0 10, 50 10, 100 50), MULTIPOINT((10 0), (50 30)), POINT(10 0))");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));
}

template<>
template<>
void object::test<8>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POINT(35 60)");

    // This is an invalid geom... anyway
    std::string wkt1("GEOMETRYCOLLECTION EMPTY");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 0);

    ensure(dist.nearestPoints() == nullptr);
}

template<>
template<>
void object::test<9>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((10 0), (50 30))");
    std::string wkt1("POINT(10 0)");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 0);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(10, 0));
    ensure_equals(cs->getAt(1), Coordinate(10, 0));

}

template<>
template<>
void object::test<10>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((10 0), (50 30))");
    std::string wkt1("MULTIPOINT((0 0), (150 30))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(10, 0));
    ensure_equals(cs->getAt(1), Coordinate(0, 0));

}

template<>
template<>
void object::test<11>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((3 0), (200 30))");
    std::string wkt1("LINESTRING(0 10, 50 10, 100 50)");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(3, 0));
    ensure_equals(cs->getAt(1), Coordinate(3, 10));

}

template<>
template<>
void object::test<12>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((3 0), (-50 30))");
    std::string wkt1("MULTILINESTRING((34 54, 60 34),(0 10, 50 10, 100 50))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 10);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(3, 0));
    ensure_equals(cs->getAt(1), Coordinate(3, 10));

}

template<>
template<>
void object::test<13>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((-100 0), (35 60))");
    std::string wkt1("POLYGON((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));

}

template<>
template<>
void object::test<14>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((-100 0), (35 60))");
    std::string
    wkt1("MULTIPOLYGON(((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)),( (100 100, 150 100, 150 150, 100 150, 100 100),(120 120, 120 130, 130 130, 130 120, 120 120)))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));
}

template<>
template<>
void object::test<15>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((-100 0), (35 60))");

    // This is an invalid geom... anyway
    std::string
    wkt1("GEOMETRYCOLLECTION(MULTIPOLYGON(((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)),( (100 100, 150 100, 150 150, 100 150, 100 100),(120 120, 120 130, 130 130, 130 120, 120 120)) ), POLYGON((34 54, 60 34, 60 54, 34 54),(50 50, 52 50, 52 48, 50 48, 50 50)), MULTILINESTRING((34 54, 60 34),(0 10, 50 10, 100 50)), LINESTRING(0 10, 50 10, 100 50), MULTIPOINT((10 0), (50 30)), POINT(10 0))");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 6);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(35, 60));
    ensure_equals(cs->getAt(1), Coordinate(35, 54));

}

template<>
template<>
void object::test<16>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("MULTIPOINT((-100 0), (35 60))");

    // This is an invalid geom... anyway
    std::string wkt1("GEOMETRYCOLLECTION EMPTY");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());

    ensure_equals(dist.distance(), 0);

    ensure(dist.nearestPoints() == nullptr);
}

// Test for crash reported in Ticket #236:
// http://trac.osgeo.org/geos/ticket/236
template<>
template<>
void object::test<17>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    std::string wkt1("POLYGON((1.25 0.25, 1.25 0.75, 1.75 0.75, 1.75 0.25, 1.25 0.25))");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    DistanceOp dist(g0.get(), g1.get());
    ensure_equals(dist.distance(), 0.25);

    CSPtr cs(dist.nearestPoints());
    ensure_equals(cs->getAt(0), Coordinate(1, 0.25));
    ensure_equals(cs->getAt(1), Coordinate(1.25, 0.25));
}

// Test for isWithinDistance
template<>
template<>
void object::test<18>
()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;

    std::string wkt0("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))");
    std::string wkt1("POLYGON((1.25 0.25, 1.25 0.75, 1.75 0.75, 1.75 0.25, 1.25 0.25))");

    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr g1(wktreader.read(wkt1));

    ensure_equals(DistanceOp::distance(*g0, *g1), 0.25);
    ensure(DistanceOp::isWithinDistance(*g0, *g1, 0.26));
    ensure(!DistanceOp::isWithinDistance(*g0, *g1, 0.24));

    wkt0 = "LINESTRING(0 0, 0 1, 1 1, 1 0, 0 0)";
    wkt1 = "LINESTRING(2 0, 10 1, 10 10)";
    g0 = wktreader.read(wkt0);
    g1 = wktreader.read(wkt1);

    ensure_equals(DistanceOp::distance(*g0, *g1), 1);
    ensure(DistanceOp::isWithinDistance(*g0, *g1, 2));
    ensure(!DistanceOp::isWithinDistance(*g0, *g1, 0.8));

    // TODO: test closestPoints
}

// Test case submitted to Ticket #367
// http://trac.osgeo.org/geos/ticket/367/
template<>
template<>
void object::test<19>
()
{
    using geos::geom::GeometryFactory;
    const char* wkb_geom1 = "01060000000100000001030000000100000000000000";
    const char* wkb_geom2 = "010100000000000000000000000000000000000000";

    geos::geom::PrecisionModel precision(geos::geom::PrecisionModel::FLOATING);
    GeometryFactory::Ptr f(GeometryFactory::create(&precision));
    std::istringstream istr1(wkb_geom1);
    std::istringstream istr2(wkb_geom2);
    geos::io::WKBReader wkb(*f);
    GeomPtr g1(wkb.readHEX(istr1));
    GeomPtr g2(wkb.readHEX(istr2));
    ensure(g1->isValid());
    ensure(g2->isValid());

    ensure_equals(g1->distance(g2.get()), 0);
}

// Test case reported in Shapely
// https://github.com/Toblerity/Shapely/issues/560
template<>
template<>
void object::test<20>()
{
    using geos::operation::distance::DistanceOp;
    using geos::geom::Coordinate;
    using geos::geom::LineSegment;
    using geos::geom::Geometry;
    using geos::geom::LineString;
    using geos::geom::GeometryFactory;
    using geos::geom::CoordinateSequence;

    auto gfact = GeometryFactory::create();

    auto seq0 = geos::detail::make_unique<CoordinateSequence>(2u);
    auto seq1 = geos::detail::make_unique<CoordinateSequence>(2u);

    Coordinate a0{1, 5.0/3.0};
    Coordinate a1{2, 10.0/3.0};

    Coordinate b0{3, 5};
    Coordinate b1{0, 0};

    seq0->setAt(a0, 0);
    seq0->setAt(a1, 1);

    seq1->setAt(b0, 0);
    seq1->setAt(b1, 1);

    auto g0 = gfact->createLineString(std::move(seq0));
    auto g1 = gfact->createLineString(std::move(seq1));

    DistanceOp dist(g0.get(), g1.get());
    CSPtr seq(dist.nearestPoints());

    // input lines overlap, so generated point should intersect both geometries
    ensure(geos::geom::LineSegment(a0, a1).distance(seq->getAt(0)) < 1e-8);
    ensure(geos::geom::LineSegment(a0, a1).distance(seq->getAt(1)) < 1e-8);
    ensure(geos::geom::LineSegment(b0, b1).distance(seq->getAt(0)) < 1e-8);
    ensure(geos::geom::LineSegment(b0, b1).distance(seq->getAt(1)) < 1e-8);

    // reverse argument order and check again
    DistanceOp dist2(g1.get(), g0.get());
    seq = dist2.nearestPoints();

    ensure(geos::geom::LineSegment(a0, a1).distance(seq->getAt(0)) < 1e-8);
    ensure(geos::geom::LineSegment(a0, a1).distance(seq->getAt(1)) < 1e-8);
    ensure(geos::geom::LineSegment(b0, b1).distance(seq->getAt(0)) < 1e-8);
    ensure(geos::geom::LineSegment(b0, b1).distance(seq->getAt(1)) < 1e-8);
}

// JTS testDisjointCollinearSegments
template<>
template<>
void object::test<21>()
{
    auto g1 = wktreader.read("LINESTRING (0.0 0.0, 9.9 1.4)");
    auto g2 = wktreader.read("LINESTRING (11.88 1.68, 21.78 3.08)");

    ensure_equals(g1->distance(g2.get()), 1.9996999774966246);
}

//
// Variations on a theme: testing EMPTY and collections with EMPTY
//

// Ignoring empty component
template<>
template<>
void object::test<22>()
{
    auto g1 = wktreader.read("GEOMETRYCOLLECTION (POINT EMPTY, LINESTRING (0 0, 1 1))");
    auto g2 = wktreader.read("POINT (1 2)");

    ensure_equals(g1->distance(g2.get()), 1);
    ensure_equals(g2->distance(g1.get()), 1);
}

// Empty is same as empty so zero...?
template<>
template<>
void object::test<23>()
{
    auto g1 = wktreader.read("POINT EMPTY");
    auto g2 = wktreader.read("LINESTRING EMPTY");

    ensure(g1 != nullptr && g2 != nullptr);
    ensure_equals(g1->distance(g2.get()), 0);
    ensure_equals(g2->distance(g1.get()), 0);
}

template<>
template<>
void object::test<24>()
{
    auto g1 = wktreader.read("GEOMETRYCOLLECTION(POINT EMPTY, LINESTRING EMPTY)");
    auto g2 = wktreader.read("LINESTRING EMPTY");

    ensure(g1 != nullptr && g2 != nullptr);
    ensure_equals(g1->distance(g2.get()), 0);
    ensure_equals(g2->distance(g1.get()), 0);
}

// But ignore empty if there's a real distance?
template<>
template<>
void object::test<25>()
{
    auto g1 = wktreader.read("GEOMETRYCOLLECTION(LINESTRING EMPTY, POINT(2 1))");
    auto g2 = wktreader.read("POINT(1 1)");

    ensure(g1 != nullptr && g2 != nullptr);
    ensure_equals(g1->distance(g2.get()), 1);
    ensure_equals(g2->distance(g1.get()), 1);
}

template<>
template<>
void object::test<26>()
{
    auto g1 = wktreader.read("GEOMETRYCOLLECTION(POINT(-2 0), POINT EMPTY)");
    auto g2 = wktreader.read("GEOMETRYCOLLECTION(POINT(1 0),LINESTRING(0 0,1 0))");

    ensure(g1 != nullptr && g2 != nullptr);
    ensure_equals(g1->distance(g2.get()), 2);
    ensure_equals(g2->distance(g1.get()), 2);
}

template<>
template<>
void object::test<27>()
{
    auto g1 = wktreader.read("GEOMETRYCOLLECTION(POINT EMPTY)");
    auto g2 = wktreader.read("GEOMETRYCOLLECTION(POINT(1 0))");

    ensure(g1 != nullptr && g2 != nullptr);
    ensure_equals(g1->distance(g2.get()), 0);
    ensure_equals(g2->distance(g1.get()), 0);
}


// TODO: finish the tests by adding:
// 	LINESTRING - *all*
// 	MULTILINESTRING - *all*
// 	POLYGON - *all*
// 	MULTIPOLYGON - *all*
// 	COLLECTION - *all*

} // namespace tut

