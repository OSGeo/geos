//
// Test Suite for geos::io::GeoJSONVaue

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/GeoJSON.h>
// std
#include <string>

namespace tut {

//
// Test Group
//
struct test_geojson_data {
};

typedef test_group<test_geojson_data> group;
typedef group::object object;

group test_geojson_group("geos::io::GeoJSONValue");

// Number
template<>
template<>
void object::test<1>
()
{
    geos::io::GeoJSONValue value {1.2};
    ensure(value.isNumber());
    ensure_equals(1.2, value.getNumber());
}

// Boolean
template<>
template<>
void object::test<2>
()
{
    geos::io::GeoJSONValue value { true };
    ensure(value.isBoolean());
    ensure_equals(true, value.getBoolean());
}

// Null
template<>
template<>
void object::test<3>
()
{
    geos::io::GeoJSONValue value;
    ensure(value.isNull());
    ensure(nullptr == value.getNull());
}

// String
template<>
template<>
void object::test<4>
()
{
    geos::io::GeoJSONValue value { std::string {"hello"} };
    ensure(value.isString());
    ensure_equals(std::string {"hello"}, value.getString());
}

// Object
template<>
template<>
void object::test<5>
()
{
    std::map<std::string, geos::io::GeoJSONValue> values {
        {"id", geos::io::GeoJSONValue{1.0}}
    };
    geos::io::GeoJSONValue value {values};
    ensure(value.isObject());
    ensure_equals(1, value.getObject().at("id").getNumber());
}

// Array
template<>
template<>
void object::test<6>
()
{
    std::vector<geos::io::GeoJSONValue> values {
        geos::io::GeoJSONValue{1.0},
        geos::io::GeoJSONValue{2.0},
        geos::io::GeoJSONValue{3.0}
    };
    geos::io::GeoJSONValue value {values};
    ensure(value.isArray());
    ensure_equals(1, value.getArray()[0].getNumber());
    ensure_equals(2, value.getArray()[1].getNumber());
    ensure_equals(3, value.getArray()[2].getNumber());
}

}
