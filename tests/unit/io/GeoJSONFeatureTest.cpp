//
// Test Suite for geos::io::GeoJSONFeture and geos::io::GeoJSONFetureCollection

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/GeoJSON.h>
#include <geos/io/WKTReader.h>
#include <geos/io/GeoJSONWriter.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
// std
#include <string>
#include <memory>

namespace tut {

//
// Test Group
//
struct test_geojson_feature_data {
    geos::geom::PrecisionModel pm;
    geos::geom::GeometryFactory::Ptr gf;
    geos::io::WKTReader wktreader;

    test_geojson_feature_data()
        :
        pm(1000.0),
        gf(geos::geom::GeometryFactory::create(&pm)),
        wktreader(gf.get())
    {}
};

typedef test_group<test_geojson_feature_data> group;
typedef group::object object;

group test_geojson_feature_data("geos::io::GeoJSONFeature");

// Create a GeoJSON Feature
template<>
template<>
void object::test<1>
()
{
    geos::io::GeoJSONFeature feature { wktreader.read("POINT(-117 33)"), std::map<std::string, geos::io::GeoJSONValue> {
        {"id",   geos::io::GeoJSONValue(1.0)     },
        {"name", geos::io::GeoJSONValue(std::string{"One"}) },
    }};
    ensure_equals(feature.getGeometry()->toText(), "POINT (-117 33)");
    ensure_equals(feature.getProperties().at("id").getNumber(), 1.0);
    ensure_equals(feature.getProperties().at("name").getString(), "One");

    const geos::io::GeoJSONFeature feature2 = feature;
    ensure_equals(feature2.getGeometry()->toText(), "POINT (-117 33)");
    ensure_equals(feature2.getProperties().at("id").getNumber(), 1.0);
    ensure_equals(feature2.getProperties().at("name").getString(), "One");
}

// Write a GeoJSON FeatureCollection
template<>
template<>
void object::test<2>
()
{
    geos::io::GeoJSONFeatureCollection features {{
        geos::io::GeoJSONFeature { wktreader.read("POINT(-117 33)"), std::map<std::string, geos::io::GeoJSONValue> {
            {"id",   geos::io::GeoJSONValue(1.0)     },
            {"name", geos::io::GeoJSONValue(std::string{"One"}) },
        }},
        geos::io::GeoJSONFeature { wktreader.read("POINT(-127 53)"), std::map<std::string, geos::io::GeoJSONValue> {
            {"id",   geos::io::GeoJSONValue(2.0)     },
            {"name", geos::io::GeoJSONValue(std::string{"Two"}) },
        }}
    }};
    ensure_equals(static_cast<size_t>(2), features.getFeatures().size());
    ensure_equals(features.getFeatures()[0].getGeometry()->toText(), "POINT (-117 33)");
    ensure_equals(features.getFeatures()[0].getProperties().at("id").getNumber(), 1.0);
    ensure_equals(features.getFeatures()[0].getProperties().at("name").getString(), "One");
    ensure_equals(features.getFeatures()[1].getGeometry()->toText(), "POINT (-127 53)");
    ensure_equals(features.getFeatures()[1].getProperties().at("id").getNumber(), 2.0);
    ensure_equals(features.getFeatures()[1].getProperties().at("name").getString(), "Two");
}

}
