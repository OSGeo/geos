/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (c) 2024 Martin Davis
 * Copyright (C) 2024 Paul Ramsey <pramsey@cleverelephant.ca>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/operation/relateng/NodeSection.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>
#include <geos/export.h>


// Forward declarations
namespace geos {
namespace operation {
namespace relateng {
class RelateNode;
}
}
}


using geos::geom::CoordinateXY;
using geos::geom::Geometry;


namespace geos {      // geos.
namespace operation { // geos.operation
namespace relateng { // geos.operation.relateng


class GEOS_DLL NodeSections {

private:

    // Members
    const CoordinateXY* nodePt;
    std::vector<std::unique_ptr<NodeSection>> sections;

    // Methods

    /**
    * Sorts the sections so that:
    *  * lines are before areas
    *  * edges from the same polygon are contiguous
    */
    void prepareSections();

    static bool hasMultiplePolygonSections(
        std::vector<std::unique_ptr<NodeSection>>& sections,
        std::size_t i);

    static std::vector<const NodeSection*> collectPolygonSections(
        std::vector<std::unique_ptr<NodeSection>>& sections,
        std::size_t i);


public:

    NodeSections(const CoordinateXY* pt)
        : nodePt(pt)
        {};

    const CoordinateXY* getCoordinate() const;

    void addNodeSection(NodeSection* e);

    bool hasInteractionAB() const;

    const Geometry* getPolygonal(bool isA) const;

    std::unique_ptr<RelateNode> createNode();

};

} // namespace geos.operation.relateng
} // namespace geos.operation
} // namespace geos

