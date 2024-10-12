//
// Test Suite for geos::io::GeoJSONWriter

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/WKTReader.h>
#include <geos/io/GeoJSONWriter.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
// std
#include <sstream>
#include <string>
#include <memory>

namespace tut {

//
// Test Group
//

// dummy data, not used
struct test_geojsonwriter_data {
    geos::geom::PrecisionModel pm;
    geos::geom::GeometryFactory::Ptr gf;
    geos::io::WKTReader wktreader;
    geos::io::GeoJSONWriter geojsonwriter;
    typedef std::unique_ptr<geos::geom::Geometry> GeomPtr;

    test_geojsonwriter_data()
        :
        pm(1000.0),
        gf(geos::geom::GeometryFactory::create(&pm)),
        wktreader(gf.get())
    {}

};

typedef test_group<test_geojsonwriter_data> group;
typedef group::object object;

group test_geojsonwriter_group("geos::io::GeoJSONWriter");

// Write a Point to GeoJSON 
template<>
template<>
void object::test<1>
()
{
    GeomPtr geom(wktreader.read("POINT(-117 33)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}");
}

// Write a LineString to GeoJSON
template<>
template<>
void object::test<2>
()
{
    GeomPtr geom(wktreader.read("LINESTRING(102.0 0.0, 103.0 1.0, 104.0 0.0, 105.0 1.0)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"LineString\",\"coordinates\":[[102.0,0.0],[103.0,1.0],[104.0,0.0],[105.0,1.0]]}");
}

// Write a Polygon with just an outer ring to GeoJSON
template<>
template<>
void object::test<3>
()
{
    GeomPtr geom(wktreader.read("POLYGON((30 10, 40 40, 20 40, 10 20, 30 10))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Polygon\",\"coordinates\":[[[30.0,10.0],[40.0,40.0],[20.0,40.0],[10.0,20.0],[30.0,10.0]]]}");
}

// Write a Polygon with outer ring and one inner ring to GeoJSON
template<>
template<>
void object::test<4>
()
{
    GeomPtr geom(wktreader.read("POLYGON((35 10, 45 45, 15 40, 10 20, 35 10), (20 30, 35 35, 30 20, 20 30))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Polygon\",\"coordinates\":[[[35.0,10.0],[45.0,45.0],[15.0,40.0],[10.0,20.0],[35.0,10.0]],[[20.0,30.0],[35.0,35.0],[30.0,20.0],[20.0,30.0]]]}");
}

// Write a MultiPoint to GeoJSON
template<>
template<>
void object::test<5>
()
{
    GeomPtr geom(wktreader.read("MULTIPOINT ((10 40), (40 30), (20 20), (30 10))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"MultiPoint\",\"coordinates\":[[10.0,40.0],[40.0,30.0],[20.0,20.0],[30.0,10.0]]}");
}

// Write a MultiLineString to GeoJSON
template<>
template<>
void object::test<6>
()
{
    GeomPtr geom(wktreader.read("MULTILINESTRING ((10 10, 20 20, 10 40),(40 40, 30 30, 40 20, 30 10))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"MultiLineString\",\"coordinates\":[[[10.0,10.0],[20.0,20.0],[10.0,40.0]],[[40.0,40.0],[30.0,30.0],[40.0,20.0],[30.0,10.0]]]}");
}

// Write a MultiPolygon with two simple Polygons to GeoJSON
template<>
template<>
void object::test<7>
()
{
    GeomPtr geom(wktreader.read("MULTIPOLYGON (((30 20, 45 40, 10 40, 30 20)),((15 5, 40 10, 10 20, 5 10, 15 5)))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"MultiPolygon\",\"coordinates\":[[[[30.0,20.0],[45.0,40.0],[10.0,40.0],[30.0,20.0]]],[[[15.0,5.0],[40.0,10.0],[10.0,20.0],[5.0,10.0],[15.0,5.0]]]]}");
}

// Write a GeometryCollection to GeoJSON
template<>
template<>
void object::test<8>
()
{
    GeomPtr geom(wktreader.read("GEOMETRYCOLLECTION(POINT(1 1),POINT(2 2))"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"GeometryCollection\",\"geometries\":[{\"type\":\"Point\",\"coordinates\":[1.0,1.0]},{\"type\":\"Point\",\"coordinates\":[2.0,2.0]}]}");
}

// Write a Point to GeoJSON Feature
template<>
template<>
void object::test<9>
()
{
    GeomPtr geom(wktreader.read("POINT(-117 33)"));
    std::string result = geojsonwriter.write(geom.get(),geos::io::GeoJSONType::FEATURE);
    ensure_equals(result, "{\"type\":\"Feature\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}}");
}

// Write a Point to GeoJSON FeatureCollection
template<>
template<>
void object::test<10>
()
{
    GeomPtr geom(wktreader.read("POINT(-117 33)"));
    std::string result = geojsonwriter.write(geom.get(), geos::io::GeoJSONType::FEATURE_COLLECTION);
    ensure_equals(result, "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}}]}");
}

// Write a LineString to formatted GeoJSON
template<>
template<>
void object::test<11>
()
{
    GeomPtr geom(wktreader.read("LINESTRING(102.0 0.0, 103.0 1.0, 104.0 0.0, 105.0 1.0)"));
    std::string result = geojsonwriter.writeFormatted(geom.get());
    ensure_equals(result, std::string{"{\n"
        "    \"type\": \"LineString\",\n"
        "    \"coordinates\": [\n"
        "        [\n"
        "            102.0,\n"
        "            0.0\n"
        "        ],\n"
        "        [\n"
        "            103.0,\n"
        "            1.0\n"
        "        ],\n"
        "        [\n"
        "            104.0,\n"
        "            0.0\n"
        "        ],\n"
        "        [\n"
        "            105.0,\n"
        "            1.0\n"
        "        ]\n"
        "    ]\n"
        "}"});
}

// Write a LineString to formatted GeoJSON with custom indentation
template<>
template<>
void object::test<12>
()
{
    GeomPtr geom(wktreader.read("LINESTRING(102.0 0.0, 103.0 1.0, 104.0 0.0, 105.0 1.0)"));
    std::string result = geojsonwriter.writeFormatted(geom.get(), geos::io::GeoJSONType::GEOMETRY, 2);
    ensure_equals(result, std::string{"{\n"
        "  \"type\": \"LineString\",\n"
        "  \"coordinates\": [\n"
        "    [\n"
        "      102.0,\n"
        "      0.0\n"
        "    ],\n"
        "    [\n"
        "      103.0,\n"
        "      1.0\n"
        "    ],\n"
        "    [\n"
        "      104.0,\n"
        "      0.0\n"
        "    ],\n"
        "    [\n"
        "      105.0,\n"
        "      1.0\n"
        "    ]\n"
        "  ]\n"
        "}"});
}

// Write a Feature
template<>
template<>
void object::test<13>
()
{
    geos::io::GeoJSONFeature feature { wktreader.read("POINT(-117 33)"), std::map<std::string, geos::io::GeoJSONValue> {
        {"id",   geos::io::GeoJSONValue(1.0)     },
        {"name", geos::io::GeoJSONValue(std::string{"One"}) },
    }};
    std::string result = geojsonwriter.write(feature);
    ensure_equals(result, "{\"type\":\"Feature\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]},\"properties\":{\"id\":1.0,\"name\":\"One\"}}");
}

// Write a FeatureCollection
template<>
template<>
void object::test<14>
()
{
    geos::io::GeoJSONFeatureCollection features {{
        geos::io::GeoJSONFeature { wktreader.read("POINT(-117 33)"), std::map<std::string, geos::io::GeoJSONValue> {
            {"id",   geos::io::GeoJSONValue(1.0)     },
            {"name", geos::io::GeoJSONValue(std::string{"One"})}},
             "id123"},
        geos::io::GeoJSONFeature { wktreader.read("POINT(-127 53)"), std::map<std::string, geos::io::GeoJSONValue> {
            {"id",   geos::io::GeoJSONValue(2.0)     },
            {"name", geos::io::GeoJSONValue(std::string{"Two"}) },
        }}
    }};
    std::string result = geojsonwriter.write(features);
    ensure_equals(result, "{\"type\":\"FeatureCollection\",\"features\":[{\"type\":\"Feature\",\"id\":\"id123\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]},\"properties\":{\"id\":1.0,\"name\":\"One\"}},{\"type\":\"Feature\",\"geometry\":{\"type\":\"Point\",\"coordinates\":[-127.0,53.0]},\"properties\":{\"id\":2.0,\"name\":\"Two\"}}]}");
}

// Write an empty point
template<>
template<>
void object::test<15>
()
{
    GeomPtr geom(wktreader.read("POINT EMPTY"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[]}");
}

// Write an empty linestring
template<>
template<>
void object::test<16>
()
{
    GeomPtr geom(wktreader.read("LINESTRING EMPTY"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"LineString\",\"coordinates\":[]}");
}

// Write an empty polygon
template<>
template<>
void object::test<17>
()
{
    GeomPtr geom(wktreader.read("POLYGON EMPTY"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Polygon\",\"coordinates\":[[]]}");
}

// Write an empty polygon
template<>
template<>
void object::test<18>
()
{
    GeomPtr geom(wktreader.read("GEOMETRYCOLLECTION EMPTY"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"GeometryCollection\",\"geometries\":[]}");
}

// Write a linear ring (as a linestring)
template<>
template<>
void object::test<19>
()
{
    GeomPtr geom(wktreader.read("LINEARRING (0 0, 1 1, 1 0, 0 0)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"LineString\",\"coordinates\":[[0.0,0.0],[1.0,1.0],[1.0,0.0],[0.0,0.0]]}");
}

// Write a point with all-nan coordinates
// https://github.com/libgeos/geos/issues/885
template<>
template<>
void object::test<20>
()
{
    GeomPtr geom(wktreader.read("POINT (NaN NaN)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[null,null]}");
}

// Write a Point Z to GeoJSON 
template<>
template<>
void object::test<21>
()
{
    GeomPtr geom(wktreader.read("POINT Z (-117 33 10)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0,10.0]}");
}

// Write a Point Z with NaN to GeoJSON 
template<>
template<>
void object::test<22>
()
{
    GeomPtr geom(wktreader.read("POINT Z (-117 33 NaN)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}");
}

// Write a Point M to GeoJSON ignores M
template<>
template<>
void object::test<23>
()
{
    GeomPtr geom(wktreader.read("POINT M (-117 33 10)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}");
}

// Write a Point ZM to GeoJSON ignores M
template<>
template<>
void object::test<24>
()
{
    GeomPtr geom(wktreader.read("POINT ZM (-117 33 10 2)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0,10.0]}");
}

// Write a LineString Z to GeoJSON
template<>
template<>
void object::test<25>
()
{
    GeomPtr geom(wktreader.read("LINESTRING Z (102 0 2, 103 1 4, 104 0 8, 105 1 16)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"LineString\",\"coordinates\":[[102.0,0.0,2.0],[103.0,1.0,4.0],[104.0,0.0,8.0],[105.0,1.0,16.0]]}");
}

// Write a LineString Z with some NaN Z to GeoJSON
template<>
template<>
void object::test<26>
()
{
    GeomPtr geom(wktreader.read("LINESTRING Z (102 0 2, 103 1 NaN, 104 0 8, 105 1 NaN)"));
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"LineString\",\"coordinates\":[[102.0,0.0,2.0],[103.0,1.0],[104.0,0.0,8.0],[105.0,1.0]]}");
}


// Setting outputs dimensions to an invalid value should raise
template<>
template<>
void object::test<27>
()
{
    std::string errorMessage;    
    bool error;
    for (auto dims: { uint8_t{1}, uint8_t{4} }) {
        errorMessage = "";
        error = false;
        try {
            geojsonwriter.setOutputDimension(dims);
        } catch (geos::util::IllegalArgumentException& e) {
            error = true;
            errorMessage = e.what();
        }
        ensure(error == true);
        ensure_equals(errorMessage, "IllegalArgumentException: GeoJSON output dimension must be 2 or 3");
    }
}


// GeoJSONWriter without output dimensions set to 2 ignores Z and M values
template<>
template<>
void object::test<28>
()
{
    GeomPtr geom(wktreader.read("POINT ZM (-117 33 10 2)"));
    geojsonwriter.setOutputDimension(2);
    std::string result = geojsonwriter.write(geom.get());
    ensure_equals(result, "{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}");
}

}
