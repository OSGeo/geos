/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2019 Martin Davis <mtnclimb@gmail.com>
 * Copyright (C) 2011      Sandro Santilli <strk@kbt.io>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

//
// Test Suite for geos::algorithm::InteriorPointArea

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/InteriorPointArea.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/Geometry.h>
// std
#include <string>
#include <memory>

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_interiorpointarea_data {
    typedef geos::geom::Geometry Geometry;
    typedef geos::geom::Coordinate Coordinate;
    typedef geos::algorithm::InteriorPointArea InteriorPointArea;

    geos::io::WKTReader reader;
    std::unique_ptr<Geometry> geom;

    test_interiorpointarea_data()
    {}

};

typedef test_group<test_interiorpointarea_data> group;
typedef group::object object;

group test_interiorpointarea_group("geos::algorithm::InteriorPointArea");

//
// Test Cases
//

// http://trac.osgeo.org/geos/ticket/475
// This test no longer throws, since invalid inputs are now handled
template<>
template<>
void object::test<1>
()
{
    Coordinate result;

    // invalid polygon - classic hourglass-shape with a self intersection
    // without a node
    geom = reader.read("POLYGON((6 54, 15 54, 6 47, 15 47, 6 54))");

    InteriorPointArea interiorPointArea(geom.get());
    interiorPointArea.getInteriorPoint(result);

    geos::geom::Coordinate expected(6, 54);

    ensure_equals(result, expected);
}

} // namespace tut

