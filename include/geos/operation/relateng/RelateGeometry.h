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

//#include <geos/operation/relateng/NodeSection.h>
#include <geos/export.h>


// Forward declarations
namespace geos {
namespace operation {
namespace relateng {
// class RelateNode;
}
}
}


// using geos::geom::CoordinateXY;
// using geos::geom::Geometry;


namespace geos {      // geos.
namespace operation { // geos.operation
namespace relateng { // geos.operation.relateng


class GEOS_DLL RelateGeometry {

private:

    // Members

    // Methods


public:

    static constexpr bool GEOM_A = true;
    static constexpr bool GEOM_B = false;

};

} // namespace geos.operation.relateng
} // namespace geos.operation
} // namespace geos

