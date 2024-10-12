/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2011      Sandro Santilli <strk@kbt.io>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

//
// Test Suite for geos::algorithm::MinimumBoundingCircle

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/MinimumBoundingCircle.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/io/WKTReader.h>

// std
#include <string>
#include <memory>

namespace tut {
//
// Test Group
//

using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::geom::PrecisionModel;
using geos::algorithm::MinimumBoundingCircle;


// dummy data, not used
struct test_minimumboundingcircle_data {
    geos::io::WKTReader reader;
    std::unique_ptr<Geometry> geom;
    std::unique_ptr<Geometry> geomOut;
    GeometryFactory::Ptr geomFact = GeometryFactory::create();

    test_minimumboundingcircle_data()
    {}

    void
    doMinimumBoundingCircleTest(std::string wktIn, std::string wktOut,
                                geos::geom::Coordinate& centreOut, double radiusOut)
    {

        geom = reader.read(wktIn);
        MinimumBoundingCircle mbc(geom.get());
        std::vector<CoordinateXY> exPts = mbc.getExtremalPoints();
        const auto& actual = geomFact->createMultiPoint(std::move(exPts));
        double actualRadius = mbc.getRadius();
        geos::geom::CoordinateXY actualCentre = mbc.getCentre();

        geomOut = reader.read(wktOut);
        bool isEqual = actual->equals(geomOut.get());

        // need this hack because apparently equals does not work for MULTIPOINT EMPTY
        if(geomOut->isEmpty() && geom->isEmpty()) {
            isEqual = true;
        }
        if(!isEqual) {
            std::cout << "Centre = " << actualCentre << " Radius = " << actualRadius << " isEqual = " << isEqual << std::endl;
            std::cout << "Actual:"   << std::endl << actual->toString() << std::endl;
            std::cout << "Expected:" << std::endl << geomOut->toString() << std::endl;
        }
        ensure(isEqual);

        if(!centreOut.isNull()) {
            if (centreOut.distance(actualCentre) > 0.001) {
                std::cout << "centreOut " << centreOut << std::endl;
                std::cout << "actualCentre " << actualCentre << std::endl;
            }
            ensure_equals("centerOut", centreOut.distance(actualCentre), 0.0, 0.001);
        }
        if(radiusOut >= 0) {
            ensure_equals("radius", actualRadius, radiusOut, 0.0001);
        }
    }

    void
    doMinimumBoundingCircleTest(std::string wktIn, std::string wktOut)
    {
        geos::geom::Coordinate c;
        c.setNull();
        doMinimumBoundingCircleTest(wktIn, wktOut, c, -1.0);
    }

};

typedef test_group<test_minimumboundingcircle_data> group;
typedef group::object object;

group test_minimumboundingcircle_group("geos::algorithm::MinimumBoundingCircle");


//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    Coordinate c(10, 10);
    doMinimumBoundingCircleTest(
        "POINT (10 10)",
        "POINT (10 10)",
        c,
        0);

}

template<>
template<>
void object::test<2>
()
{
    Coordinate c(15, 15);
    doMinimumBoundingCircleTest(
        "MULTIPOINT ((10 10), (20 20))",
        "MULTIPOINT ((10 10), (20 20))",
        c,
        7.0710678118654755);
}

template<>
template<>
void object::test<3>
()
{
    Coordinate c(20, 20);
    doMinimumBoundingCircleTest(
        "MULTIPOINT ((10 10), (20 20), (30 30))",
        "MULTIPOINT ((10 10), (30 30))",
        c,
        14.142135623730951);
}

template<>
template<>
void object::test<4>
()
{
    Coordinate c(15, 15);
    doMinimumBoundingCircleTest(
        "MULTIPOINT ((10 10), (20 20), (10 20))",
        "MULTIPOINT ((10 10), (20 20), (10 20))",
        c,
        7.0710678118654755);
}

template<>
template<>
void object::test<5>
()
{
    Coordinate c(150, 100);
    doMinimumBoundingCircleTest(
        "POLYGON ((100 100, 200 100, 150 90, 100 100))",
        "MULTIPOINT ((100 100), (200 100))",
        c,
        50);
}

template<>
template<>
void object::test<6>
()
{
    Coordinate c(15, 15);
    doMinimumBoundingCircleTest(
        "MULTIPOINT ((10 10), (20 20), (10 20), (15 19))",
        "MULTIPOINT ((10 10), (20 20), (10 20))",
        c,
        7.0710678118654755);
}

template<>
template<>
void object::test<7>
()
{
    Coordinate c(26284.84180271327, 65267.114509082545);
    doMinimumBoundingCircleTest(
        "POLYGON ((26426 65078, 26531 65242, 26096 65427, 26075 65136, 26426 65078))",
        "MULTIPOINT ((26531 65242), (26075 65136), (26096 65427))",
        c,
        247.4360455914027);
}

template<>
template<>
void object::test<8>
()
{
    Coordinate c(196.026, 159.103);
    doMinimumBoundingCircleTest(
        "POLYGON ((100 200, 300 150, 110 100, 100 200))",
        "MULTIPOINT ((110 100), (300 150), (100 200))",
        c,
        104.372);
}

template<>
template<>
void object::test<9>
()
{
    Coordinate c(196.026, 140.897);
    doMinimumBoundingCircleTest(
        "POLYGON ((110 200, 300 150, 100 100, 110 200))",
        "MULTIPOINT ((100 100), (300 150), (110 200))",
        c,
        104.37204);
}

template<>
template<>
void object::test<10>
()
{
    Coordinate c(3, 2);
    doMinimumBoundingCircleTest(
        "POLYGON ((0 0, 6 0, 5 5, 0 0))",
        "MULTIPOINT ((0 0), (6 0), (5 5))",
        c,
        3.60555);
}

} // namespace tut

