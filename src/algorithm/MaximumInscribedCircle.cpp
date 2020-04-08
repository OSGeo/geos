/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2019 Jonathan Adams <jd.adams16@gmail.com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/algorithm/MaximumInscribedCircle.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/triangulate/VoronoiDiagramBuilder.h>
#include <geos/util/UniqueCoordinateArrayFilter.h>

#include <memory> // for unique_ptr
#include <typeinfo>
#include <vector>
#include <cstdlib>

using namespace std;
using namespace geos::geom;
using namespace geos::triangulate;
using namespace geos::util;

namespace geos {
namespace algorithm { // geos.algorithm

/*public*/
std::unique_ptr<Geometry>
MaximumInscribedCircle::getCircle()
{
    compute();
    // handle degenerate or trivial cases
    if(input->isEmpty()) {
        return std::unique_ptr<Geometry>(input->getFactory()->createPolygon());
    } else if(input->getNumPoints() == 1) {
        return std::unique_ptr<Geometry>(input->clone());
    }
}

/**
 * Temporary development helper. To be removed in final version.
 */
/*public*/
std::vector<geom::Coordinate>*
MaximumInscribedCircle::getSites()
{
    compute();
    return sites;
}

/**
 * Temporary development helper. To be removed in final version.
 */
std::vector<const geom::Point*>
MaximumInscribedCircle::getVoronoiVertices()
{
    compute();
    computeVoronoiVertices();
    return voronoiVertices;
}

/*private*/
void
MaximumInscribedCircle::compute()
{
    if(typeid(*input) == typeid(Point)) {
        cout << "Geometry is of type Point" << endl;
        center = *(input->getCoordinate());
    } else if(typeid(*input) == typeid(Polygon)) {
        cout << "Geometry is of type Polygon" << endl;
        const geom::Polygon* poly = dynamic_cast<const geom::Polygon*>(input);
        computeSites(poly);
    } else if(typeid(*input) == typeid(MultiPolygon)) {
        cout << "Geometry is of type MultiPolygon" << endl;
        const geom::MultiPolygon* multiPoly = dynamic_cast<const geom::MultiPolygon*>(input);
        const geom::Polygon* maxAreaPoly = polygonOfMaxArea(multiPoly);
        computeSites(maxAreaPoly);
    } else {
        // degenerate/trivial cases, LineString, MultiLineString, and MultiPoint
        return;
    }
    // computeVoronoiVertices();
    return;
}

/*private*/
void
MaximumInscribedCircle::computeSites(const geom::Polygon* poly)
{
    if(sites == nullptr) {
        sites = new std::vector<Coordinate>;
    } else {
        return;
    }

    const LineString* exterior = poly->getExteriorRing();
    const LineString* interior;

    // Initialize bounding box
    xmin = exterior->getStartPoint()->getX();
    xmax = xmin;
    ymin = exterior->getStartPoint()->getY();
    ymax = ymin;

    addRingSites(exterior);

    for(unsigned int i = 0; i < poly->getNumInteriorRing(); i++) {
        interior = poly->getInteriorRingN(i);
        addRingSites(interior);
    }
}

/**
* This function implements the majority of GeoJSONUtils.sites from the
* nodeJS module max-inscribed-circle (npmjs.com/package/max-inscribed-circle).
* It takes a ring, and n-sects each segment of the ring, where n is the
* number of segments MaximumInscribedCircle has been called with. It then
* adds all of these n-sect coordinates into an array of coordinates that
* will eventually be converted to a CoordinateSequence and used to call
* Voronoi.
*
* This function also updates the coordinates of a bounding box that will
* be passed on the same call to Voronoi.
*
* @param ring an internal or external ring
*/
/*private*/
void
MaximumInscribedCircle::addRingSites(const LineString* ring)
{
    double fromX, fromY, toX, toY, segmentX, segmentY;
    std::unique_ptr<Point> fromPoint;
    std::unique_ptr<Point> toPoint;

    for(unsigned int i = 0; i < ring->getNumPoints()-1; i++) {
        fromPoint = ring->getPointN(i);
        toPoint = ring->getPointN(i+1);
        fromX = fromPoint->getX();
        fromY = fromPoint->getY();
        toX = toPoint->getX();
        toY = toPoint->getY();

        // Push segments
        for(unsigned int k = 0; k < inputNumSegments; k++) {
            segmentX = fromX + (toX - fromX) * k / inputNumSegments;
            segmentY = fromY + (toY - fromY) * k / inputNumSegments;
            sites->push_back(Coordinate(segmentX, segmentY));
        }

        // Update bounding box
        if(fromX < xmin) {
            xmin = fromX;
        }
        if(fromX > xmax) {
            xmax = fromX;
        }
        if(fromY < ymin) {
            ymin = fromY;
        }
        if(fromY > ymax) {
            ymax = fromY;
        }
    }
}

// TODO: Figure out if I can use one of the extractor functions defined
//       in GEOSGeom or MakeValid.
static std::unique_ptr<geom::MultiPoint>
extractUniquePoints(const geom::Geometry* geom)
{

    // Code taken from GEOSGeom_extractUniquePoints_r()

    /* 1: extract points */
    std::vector<const geom::Coordinate*> coords;
    UniqueCoordinateArrayFilter filter(coords);
    geom->apply_ro(&filter);

    /* 2: for each point, create a geometry and put into a vector */
    std::vector<std::unique_ptr<Geometry>> points;
    points.reserve(coords.size());
    const GeometryFactory* factory = geom->getFactory();
    for(const Coordinate* c : coords) {
        points.emplace_back(factory->createPoint(*c));
    }

    /* 3: create a multipoint */
    return factory->createMultiPoint(std::move(points));
}

/*private*/
void
MaximumInscribedCircle::computeVoronoiVertices()
{
    VoronoiDiagramBuilder builder;
    const GeometryFactory& geomFact(*GeometryFactory::getDefaultInstance());
    const CoordinateSequenceFactory* coordSeqFactory = geomFact.getCoordinateSequenceFactory();
    builder.setSites(*(coordSeqFactory->create(sites)));

    std::unique_ptr<Geometry> diagramEdges = builder.getDiagramEdges(geomFact);
    std::shared_ptr<Geometry> edges = std::move(diagramEdges);
    std::unique_ptr<CoordinateSequence> coords = extractUniquePoints(edges.get())->getCoordinates();
    for(unsigned int i = 0; i < coords->getSize(); i++) {
        voronoiVertices.push_back(geomFact.createPoint(coords->getAt(i)));
    }

    // TODO: Figure out if I can get this to work without buffer overflows so I can just use points returned by extractUniquePoints.
    // std::unique_ptr<MultiPoint> uniquePoints = extractUniquePoints(edges.get());
    // for(unsigned int i = 0; i < uniquePoints->getNumGeometries(); i++) {
    //     voronoiVertices.push_back(uniquePoints->getGeometryN(i));
    // }
}

/*private*/
void
MaximumInscribedCircle::computeCenterAndRadius(geom::Polygon& poly)
{
    
}

/**
* This function takes a multipolygon and returns the polygon of largest area
* in the collection.
*
* @param multiPoly
* @return the Polygon of largest area
*/
/*private*/
const geom::Polygon*
MaximumInscribedCircle::polygonOfMaxArea(const geom::MultiPolygon* multiPoly)
{
    double currentArea = 0, maxArea = 0;
    unsigned int maxAreaPolyIndex = 0;

    for(unsigned int i = 0; i < multiPoly->getNumGeometries(); i++) {
        currentArea = multiPoly->getGeometryN(i)->getArea();
        if(currentArea > maxArea) {
            maxArea = currentArea;
            maxAreaPolyIndex = i;
        }
    }

    return multiPoly->getGeometryN(maxAreaPolyIndex);
}

} // namespace geos.algorithm
} // namespace geos
