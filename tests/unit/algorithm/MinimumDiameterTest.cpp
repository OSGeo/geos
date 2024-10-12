/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2015      Nyall Dawson <nyall dot dawson at gmail dot com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

//
// Test Suite for geos::algorithm::MinimumDiameter

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/MinimumDiameter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
// std
#include <string>
#include <memory>

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_minimumdiameter_data {
    typedef geos::geom::Geometry Geometry;
    typedef std::unique_ptr<geos::geom::Geometry> GeomPtr;

    typedef geos::geom::Coordinate Coordinate;
    typedef geos::algorithm::MinimumDiameter MinimumDiameter;

    geos::io::WKTReader reader;
    std::unique_ptr<Geometry> geom;

    test_minimumdiameter_data()
    {}

};

typedef test_group<test_minimumdiameter_data> group;
typedef group::object object;

group test_minimumdiameter_data_group("geos::algorithm::MinimumDiameter");

//
// Test Cases
//

// Test of getMinimumRectangle
template<>
template<>
void object::test<1>
()
{
    GeomPtr p_geom(reader.read("POLYGON ((0 0, 0 20, 20 20, 20 0, 0 0))"));
    ensure(nullptr != p_geom.get());

    geos::algorithm::MinimumDiameter m(p_geom.get());
    GeomPtr minRect(m.getMinimumRectangle());
    ensure(nullptr != minRect.get());

    GeomPtr expectedGeom(reader.read("POLYGON ((0 0, 20 0, 20 20, 0 20, 0 0))"));
    ensure(nullptr != expectedGeom.get());

    ensure(minRect.get()->equalsExact(expectedGeom.get()));
}

// Test with expected rotated rectangle
template<>
template<>
void object::test<2>
()
{
    GeomPtr p_geom(reader.read("POLYGON ((0 5, 5 10, 10 5, 5 0, 0 5))"));
    ensure(nullptr != p_geom.get());

    geos::algorithm::MinimumDiameter m(p_geom.get());
    GeomPtr minRect(m.getMinimumRectangle());
    ensure(nullptr != minRect.get());

    GeomPtr expectedGeom(reader.read("POLYGON ((5 0, 10 5, 5 10, 0 5, 5 0))"));
    ensure(nullptr != expectedGeom.get());

    ensure(minRect.get()->equalsExact(expectedGeom.get()));
}

// Test with EMPTY input
template<>
template<>
void object::test<3>
()
{
    GeomPtr p_geom(reader.read("POLYGON EMPTY"));
    ensure(nullptr != p_geom.get());

    geos::algorithm::MinimumDiameter m(p_geom.get());
    GeomPtr minRect(m.getMinimumRectangle());
    ensure(nullptr != minRect.get());

    GeomPtr expectedGeom(reader.read("POLYGON EMPTY"));
    ensure(nullptr != expectedGeom.get());

    ensure(minRect.get()->equalsExact(expectedGeom.get()));
}

// Test with Point input
template<>
template<>
void object::test<4>
()
{
    GeomPtr p_geom(reader.read("Point(1 2)"));
    ensure(nullptr != p_geom.get());

    geos::algorithm::MinimumDiameter m(p_geom.get());
    GeomPtr minRect(m.getMinimumRectangle());
    ensure(nullptr != minRect.get());

    GeomPtr expectedGeom(reader.read("Point(1 2)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minRect.get()->equalsExact(expectedGeom.get()));
}

// Test with LineString input
template<>
template<>
void object::test<5>
()
{
    GeomPtr p_geom(reader.read("LineString(1 2, 2 4)"));
    ensure(nullptr != p_geom.get());

    geos::algorithm::MinimumDiameter m(p_geom.get());
    GeomPtr minRect(m.getMinimumRectangle());
    ensure(nullptr != minRect.get());

    GeomPtr expectedGeom(reader.read("LineString(1 2, 2 4)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minRect.get()->equalsExact(expectedGeom.get()));
}

// Test minimumDiameter with Point input
template<>
template<>
void object::test<6>
()
{
    GeomPtr p_geom(reader.read("POINT (0 240)"));
    ensure(nullptr != p_geom.get());

    GeomPtr minDiameter(geos::algorithm::MinimumDiameter::getMinimumDiameter(p_geom.get()));
    ensure(nullptr != minDiameter.get());

    GeomPtr expectedGeom(reader.read("LineString (0 240, 0 240)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minDiameter.get()->equalsExact(expectedGeom.get()));
}

// Test minimumDiameter with LineString input
template<>
template<>
void object::test<7>
()
{
    GeomPtr p_geom(reader.read("LINESTRING (0 240, 220 240)"));
    ensure(nullptr != p_geom.get());

    GeomPtr minDiameter(geos::algorithm::MinimumDiameter::getMinimumDiameter(p_geom.get()));
    ensure(nullptr != minDiameter.get());

    GeomPtr expectedGeom(reader.read("LINESTRING (0 240, 0 240)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minDiameter.get()->equalsExact(expectedGeom.get()));
}

// Test minimumDiameter with Polygon input
template<>
template<>
void object::test<8>
()
{
    GeomPtr p_geom(reader.read("POLYGON ((0 240, 220 240, 220 0, 0 0, 0 240))"));
    ensure(nullptr != p_geom.get());

    GeomPtr minDiameter(geos::algorithm::MinimumDiameter::getMinimumDiameter(p_geom.get()));
    ensure(nullptr != minDiameter.get());

    GeomPtr expectedGeom(reader.read("LINESTRING (0 0, 220 0)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minDiameter.get()->equalsExact(expectedGeom.get()));
}

// Test minimumDiameter with Polygon input
template<>
template<>
void object::test<9>
()
{
    GeomPtr p_geom(reader.read("POLYGON ((0 240, 160 140, 220 0, 0 0, 0 240))"));
    ensure(nullptr != p_geom.get());

    GeomPtr minDiameter(geos::algorithm::MinimumDiameter::getMinimumDiameter(p_geom.get()));
    ensure(nullptr != minDiameter.get());

    GeomPtr expectedGeom(reader.read("LINESTRING (185.86206896551724 79.65517241379311, 0 0)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minDiameter.get()->equalsExact(expectedGeom.get(), 0.00000000001));
}

// Test minimumDiameter with complex LineString
template<>
template<>
void object::test<10>
()
{
    GeomPtr p_geom(
        reader.read("LINESTRING ( 39 119, 162 197, 135 70, 95 35, 33 66, 111 82, 97 131, 48 160, -4 182, 57 195, 94 202, 90 174, 75 134, 47 114, 0 100, 59 81, 123 60, 136 43, 163 75, 145 114, 93 136, 92 159, 105 175 )"));
    ensure(nullptr != p_geom.get());

    GeomPtr minDiameter(geos::algorithm::MinimumDiameter::getMinimumDiameter(p_geom.get()));
    ensure(nullptr != minDiameter.get());

    GeomPtr expectedGeom(reader.read("LINESTRING (64.46262341325811 196.41184767277855, 95 35)"));
    ensure(nullptr != expectedGeom.get());

    ensure(minDiameter.get()->equalsExact(expectedGeom.get(), 0.00000000001));
}

} // namespace tut
