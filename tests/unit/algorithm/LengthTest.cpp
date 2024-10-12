//
// Test Suite for geos::algorithm::Length
// Ported from JTS junit/algorithm/LengthTest.java

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/Length.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>
// std
#include <string>
#include <memory>

using namespace geos;
using namespace geos::geom;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_length_data {
    geos::geom::Geometry* geom_;
    geos::geom::PrecisionModel pm_;
    geos::geom::GeometryFactory::Ptr factory_;
    geos::io::WKTReader reader_;
    test_length_data():
        geom_(nullptr),
        pm_(1),
        factory_(GeometryFactory::create(&pm_, 0)), reader_(factory_.get())
    {
        assert(nullptr == geom_);
    }

    ~test_length_data()
    {
        factory_->destroyGeometry(geom_);
        geom_ = nullptr;
    }

    void
    checkLengthOfLine(std::string wkt, double expectedLength)
    {
        std::unique_ptr<Geometry> lineGeom(reader_.read(wkt));
        std::unique_ptr<LineString> line(dynamic_cast<LineString*>(lineGeom.release()));
        ensure(nullptr != line.get());
        const CoordinateSequence* lineSeq = line->getCoordinatesRO();
        double actual = algorithm::Length::ofLine(lineSeq);
        ensure_equals(actual, expectedLength);
    }

};

// void checkLengthOfLine(String wkt, double expectedLen) {
//    LineString ring = (LineString) read(wkt);

//    CoordinateSequence pts = ring.getCoordinateSequence();
//    double actual = Length.ofLine(pts);
//    assertEquals(actual, expectedLen);
//  }




typedef test_group<test_length_data> group;
typedef group::object object;

group test_length_group("geos::algorithm::Length");


//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    checkLengthOfLine("LINESTRING (100 200, 200 200, 200 100, 100 100, 100 200)", 400.0);
}



} // namespace tut

