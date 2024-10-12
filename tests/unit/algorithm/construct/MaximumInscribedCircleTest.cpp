//
// Test Suite for geos::algorithm::construct::MaximumInscribedCircle

#include <tut/tut.hpp>
// geos
#include <geos/operation/distance/IndexedFacetDistance.h>
#include <geos/algorithm/construct/MaximumInscribedCircle.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/util.h>
// std
#include <string>
#include <memory>



using namespace geos;
using namespace geos::geom;

using geos::algorithm::construct::MaximumInscribedCircle;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_mic_data {
    geos::geom::Geometry* geom_;
    geos::geom::PrecisionModel pm_;
    geos::geom::GeometryFactory::Ptr factory_;
    geos::io::WKTReader reader_;
    geos::io::WKTWriter writer_;

    test_mic_data():
        geom_(nullptr),
        pm_(geos::geom::PrecisionModel::FLOATING),
        factory_(GeometryFactory::create(&pm_, 0)),
        reader_(factory_.get())
    {}

    ~test_mic_data()
    {
        factory_->destroyGeometry(geom_);
        geom_ = nullptr;
    }

    void
    ensure_equals_coordinate(const geos::geom::Coordinate &lhs,
                             const geos::geom::Coordinate &rhs, double tolerance)
    {
        ensure_equals("x coordinate does not match", lhs.x, rhs.x, tolerance);
        ensure_equals("y coordinate does not match", lhs.y, rhs.y, tolerance);
    }

    /**
     * A coarse distance check, mainly testing
     * that there is not a huge number of iterations.
     * (This will be revealed by CI taking a very long time!)
     */
    void
    checkCircle(std::string wkt, double tolerance)
    {
        std::unique_ptr<Geometry> geom(reader_.read(wkt));
        MaximumInscribedCircle mic(geom.get(), tolerance);
        std::unique_ptr<Point> centerPoint = mic.getCenter();
        std::unique_ptr<Geometry> bdy = geom->getBoundary();
        double dist = bdy->distance(centerPoint.get());
        //std::cout << dist << std::endl;
        ensure(dist < 2 * tolerance);
    }

    void
    checkCircle(const Geometry *geom, double tolerance, double x, double y, double expectedRadius)
    {
        MaximumInscribedCircle mic(geom, tolerance);
        std::unique_ptr<Point> centerPoint = mic.getCenter();
        Coordinate centerPt(*centerPoint->getCoordinate());
        Coordinate expectedCenter(x, y);
        ensure_equals_coordinate(centerPt, expectedCenter, tolerance);
        std::unique_ptr<LineString> radiusLine = mic.getRadiusLine();
        double actualRadius = radiusLine->getLength();
        ensure_equals("radius", actualRadius, expectedRadius, tolerance);
        const Coordinate& linePt0 = radiusLine->getCoordinateN(0);
        const Coordinate& linePt1 = radiusLine->getCoordinateN(1);

        // std::cout << std::endl;
        // std::cout << writer_.write(geom) << std::endl;
        // std::cout << writer_.write(radiusLine.get()) << std::endl;

        ensure_equals_coordinate(centerPt, linePt0, tolerance);
        Coordinate radiusPt(*mic.getRadiusPoint()->getCoordinate());
        ensure_equals_coordinate(radiusPt, linePt1, tolerance);
    }

    void
    checkCircle(std::string wkt, double tolerance, double x, double y, double expectedRadius)
    {
        std::unique_ptr<Geometry> geom(reader_.read(wkt));
        checkCircle(geom.get(), tolerance, x, y, expectedRadius);
    }

};

typedef test_group<test_mic_data> group;
typedef group::object object;

group test_mic_group("geos::algorithm::construct::MaximumInscribedCircle");

//
// testSquare
//
template<>
template<>
void object::test<1>
()
{
    checkCircle("POLYGON ((100 200, 200 200, 200 100, 100 100, 100 200))",
                0.001, 150, 150, 50);
}

//
// testDiamond
//
template<>
template<>
void object::test<2>
()
{
    checkCircle("POLYGON ((150 250, 50 150, 150 50, 250 150, 150 250))",
                0.001, 150, 150, 70.71 );
}

//
// testCircle
//
template<>
template<>
void object::test<3>
()
{
    std::unique_ptr<Geometry> geom(reader_.read("POINT (100 100)"));
    std::unique_ptr<Geometry> circle(geom->buffer(100, 20));
    // MIC radius is less than 100 because buffer boundary segments lie inside circle
    checkCircle(circle.get(), 0.01, 100, 100, 99.9229);
}

//
// testDoubleKite
//
template<>
template<>
void object::test<4>
()
{
    checkCircle("MULTIPOLYGON (((150 200, 100 150, 150 100, 250 150, 150 200)), ((400 250, 300 150, 400 50, 560 150, 400 250)))",
       0.001, 411.38877, 149.9996185, 78.7634662 );
}

//
// Invalid polygon collapsed to a line
//
template<>
template<>
void object::test<5>
()
{
    checkCircle("POLYGON ((100 100, 200 200, 100 100, 100 100))",
       0.01, 150, 150, 0 );
}


// //
// // Invalid polygon collapsed to a point
// //
template<>
template<>
void object::test<6>
()
{
     checkCircle("POLYGON ((100 100, 100 100, 100 100, 100 100))",
       0.01, 100, 100, 0 );
}

// //
// // Invalid polygon collapsed to a line
// //
template<>
template<>
void object::test<7>
()
{
     checkCircle("POLYGON((1 2, 1 2, 1 2, 1 2, 3 2, 1 2))",
       0.01, 2, 2, 0 );
}

// Exception thrown to avoid infinite loop with infinite envelope
template<>
template<>
void object::test<8>
()
{
    auto g1 = reader_.read("POLYGON ((0 0, 1 0, 1 1, 0 Inf, 0 0))");
    try {
        MaximumInscribedCircle mic(g1.get(), 1);
        mic.getCenter();
    } catch (const util::GEOSException & e) {
        ::geos::ignore_unused_variable_warning(e);
    }

    auto g2 = reader_.read("POLYGON ((0 0, 1 0, 2 NaN,  0 0))");
    try {
        MaximumInscribedCircle mic(g1.get(), 1);
        mic.getCenter();
    } catch (const util::GEOSException & e) {
        ::geos::ignore_unused_variable_warning(e);
    }
}

  /**
   * Tests that a nearly flat geometry doesn't make the initial cell grid huge.
   *
   * See https://github.com/libgeos/geos/issues/875
   */
// testNearlyFlat
template<>
template<>
void object::test<9>
()
{
    checkCircle("POLYGON ((59.3 100.00000000000001, 99.7 100.00000000000001, 99.7 100, 59.3 100, 59.3 100.00000000000001))",
       0.01 );
}

// testVeryThin
template<>
template<>
void object::test<10>
()
{
    checkCircle("POLYGON ((100 100, 200 300, 300 100, 450 250, 300 99.999999, 200 299.99999, 100 100))",
       0.01 );
}


} // namespace tut
