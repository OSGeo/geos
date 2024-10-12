//
// Test Suite for C-API GEOSGeoJSONReader_read

#include <tut/tut.hpp>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeomwritegeojson_data : public capitest::utility {
    GEOSGeoJSONWriter* writer_;

    test_capigeosgeomwritegeojson_data() : writer_(nullptr)
    {
        writer_ = GEOSGeoJSONWriter_create();
    }

    ~test_capigeosgeomwritegeojson_data()
    {
        GEOSGeoJSONWriter_destroy(writer_);
        writer_ = nullptr;
    }

    void
    test_geojson(std::string const& wkt, std::string const& expected, int indent)
    {
        geom1_ = fromWKT(&wkt[0]);
        char* geojson_c = GEOSGeoJSONWriter_writeGeometry(writer_, geom1_, indent);
        ensure("GEOSGeoJSONWriter_writeGeometry failed to create GeoJSON", nullptr != geojson_c);

        std::string actual(geojson_c);
        free(geojson_c);
        ensure_equals(actual, expected);
    }
};

typedef test_group<test_capigeosgeomwritegeojson_data> group;
typedef group::object object;

group test_capigeosgeomwritegeojson_group("capi::GEOSGeomGeoJSONWrite");

//
// Test Cases
//

// Write a Point to GeoJSON
template<>
template<>
void object::test<1>
()
{
    std::string wkt("POINT(-117.0 33.0)");
    std::string expected("{\"type\":\"Point\",\"coordinates\":[-117.0,33.0]}");
    test_geojson(wkt, expected, -1);
}

// Write a GeometryCollection to GeoJSON
template<>
template<>
void object::test<2>
()
{
    std::string wkt("GEOMETRYCOLLECTION(POINT(1 1),POINT(2 2))");
    std::string expected("{\"type\":\"GeometryCollection\",\"geometries\":[{\"type\":\"Point\",\"coordinates\":[1.0,1.0]},{\"type\":\"Point\",\"coordinates\":[2.0,2.0]}]}");
    test_geojson(wkt, expected, -1);
}


// Write a LineString to formatted GeoJSON
template<>
template<>
void object::test<3>
()
{
    std::string wkt("LINESTRING(102.0 0.0, 103.0 1.0, 104.0 0.0, 105.0 1.0)");
    std::string expected(std::string{"{\n"
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
    test_geojson(wkt, expected, 4);
}

template<>
template<>
void object::test<4>
()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure(input_);

    char* geojson = GEOSGeoJSONWriter_writeGeometry(writer_, input_, 0);
    ensure("curved geometry not supported", geojson == nullptr);
}

}
