//
// Test Suite for geos::operation::buffer::BufferBuilder class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/buffer/BufferBuilder.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/constants.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/algorithm/PointLocator.h>
#include <geos/algorithm/Orientation.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/CoordinateSequence.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_bufferbuilder_data {
    const geos::geom::GeometryFactory& gf;
    geos::io::WKTReader wktreader;
    int const default_quadrant_segments;

    typedef geos::geom::Geometry::Ptr GeomPtr;
    typedef std::unique_ptr<geos::geom::CoordinateSequence> CSPtr;

    test_bufferbuilder_data()
        : gf(*geos::geom::GeometryFactory::getDefaultInstance())
        , wktreader(&gf)
        , default_quadrant_segments(geos::operation::buffer::BufferParameters::DEFAULT_QUADRANT_SEGMENTS)
    {
        ensure_equals(default_quadrant_segments, int(8));
    }
private:
    // noncopyable
    test_bufferbuilder_data(test_bufferbuilder_data const& other) = delete;
    test_bufferbuilder_data& operator=(test_bufferbuilder_data const& rhs) = delete;
};

typedef test_group<test_bufferbuilder_data> group;
typedef group::object object;

group test_bufferbuilder_group("geos::operation::buffer::BufferBuilder");

//
// Test Cases
//

// Left-side and right-side offset curve
// See http://trac.osgeo.org/geos/ticket/633
template<>
template<>
void object::test<1>
()
{
    using geos::operation::buffer::BufferBuilder;
    using geos::operation::buffer::BufferParameters;
    using geos::algorithm::Orientation;
    using geos::geom::LineString;

    // Original input from test in ticket #633
    //std::string wkt0("LINESTRING ("
    //    "665.7317504882812500 133.0762634277343700,"
    //    "1774.4752197265625000 19.9391822814941410,"
    //    "756.2413940429687500 466.8306579589843700,"
    //    "626.1337890625000000 1898.0147705078125000,"
    //    "433.8007202148437500 404.6052856445312500)");
    //double const distance = 57.164000837203;

    // Simplified equivalent input
    std::string wkt0("LINESTRING(0 0, 50 -10, 10 10, 0 50, -10 10)");
    double const distance = 5;

    GeomPtr g0(wktreader.read(wkt0));
    ensure(nullptr != g0.get());
    ensure_equals(g0->getNumPoints(), std::size_t(5));

    BufferParameters params;
    params.setEndCapStyle(BufferParameters::CAP_FLAT);
    params.setQuadrantSegments(8);
    params.setJoinStyle(BufferParameters::JOIN_MITRE);
    params.setMitreLimit(5.57);
    //params.setSingleSided(true); // DO NOT switch for non-areal input, see ticket #633
    BufferBuilder builder(params);
    ensure(distance > 0);

    // left-side
    {
        GeomPtr gB(builder.bufferLineSingleSided(g0.get(), distance, true));
        ensure(nullptr != gB.get());
        ensure_equals(gB->getGeometryTypeId(), geos::geom::GEOS_LINESTRING);
        // Left-side offset curve expected with 5+ vertices
        ensure(gB->getNumPoints() >= g0->getNumPoints());

        // For left-side offset curve, the offset will be at the left side of the input line
        // and retain the same direction.
        ensure_equals(
            Orientation::isCCW(dynamic_cast<LineString*>(g0.get())->getCoordinatesRO()),
            Orientation::isCCW(dynamic_cast<LineString*>(gB.get())->getCoordinatesRO()));
    }

    // right-side
    {
        GeomPtr gB(builder.bufferLineSingleSided(g0.get(), distance, false));
        ensure(nullptr != gB.get());
        ensure_equals(gB->getGeometryTypeId(), geos::geom::GEOS_LINESTRING);
        // Right-side offset curve expected with 5+ vertices
        ensure(gB->getNumPoints() >= g0->getNumPoints());

        // For right-side offset curve, it'll be at the right side
        // and in the opposite direction.
        ensure_equals(
            Orientation::isCCW(dynamic_cast<LineString*>(g0.get())->getCoordinatesRO()),
            !Orientation::isCCW(dynamic_cast<LineString*>(gB.get())->getCoordinatesRO()));
    }
}

} // namespace tut

