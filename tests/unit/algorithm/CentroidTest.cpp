//
// Test Suite for geos::algorithm::Centroid

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/Centroid.h>
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
struct test_centroid_data {

    std::unique_ptr<Geometry> geom_;
    PrecisionModel pm_;
    GeometryFactory::Ptr factory_;
    io::WKTReader reader_;

    test_centroid_data():
        geom_(nullptr),
        pm_(1),
        factory_(GeometryFactory::create(&pm_, 0)),
        reader_(factory_.get())
    {
        assert(nullptr == geom_);
    }

    ~test_centroid_data()
    {
    }

    void
    checkCentroid(std::string inputWkt, bool expectResult, double expectedX, double expectedY)
    {
        std::unique_ptr<Geometry> inputGeom(reader_.read(inputWkt));
        Coordinate resultCoord;
        bool gotResult = algorithm::Centroid::getCentroid(*inputGeom, resultCoord);
        ensure_equals(gotResult, expectResult);
        if (expectResult) {
            ensure_equals(expectedX, resultCoord.x);
            ensure_equals(expectedY, resultCoord.y);
        }
    }

    void
    checkCentroid(std::string inputWkt, double expectedX, double expectedY)
    {
        checkCentroid(inputWkt, true, expectedX, expectedY);
    }

    void
    checkCentroidFails(std::string inputWkt)
    {
        checkCentroid(inputWkt, false, 0, 0);
    }

};



typedef test_group<test_centroid_data> group;
typedef group::object object;

group test_centroid_group("geos::algorithm::Centroid");


//
// Test Cases
//
template<>
template<>
void object::test<1>() {
    checkCentroid(
        "LINESTRING (0 0, 200 200)",
        100.0, 100.0);
}

template<>
template<>
void object::test<2>() {
    checkCentroid(
        "POLYGON ((0 0, 100 0, 100 100, 0 100, 0 0))",
        50.0, 50.0);
}

template<>
template<>
void object::test<3>() {
    checkCentroid(
        "GEOMETRYCOLLECTION(POLYGON ((0 0, 100 0, 100 100, 0 100, 0 0)))",
        50.0, 50.0);
}

template<>
template<>
void object::test<4>() {
    checkCentroid(
        "GEOMETRYCOLLECTION(POLYGON ((0 0, 100 0, 100 100, 0 100, 0 0)), POINT EMPTY)",
        50.0, 50.0);
}

template<>
template<>
void object::test<5>() {
    checkCentroidFails(
        "POLYGON EMPTY");
}

template<>
template<>
void object::test<6>() {
    checkCentroid("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0), EMPTY)", 0.5, 0.5);
}

} // namespace tut

