/************************************************************************
 *
 *
 * C-Wrapper for GEOS library
 *
 * Copyright (C) 2010 2011 Sandro Santilli <strk@kbt.io>
 * Copyright (C) 2005-2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 * Author: Sandro Santilli <strk@kbt.io>
 *
 ***********************************************************************/

#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/index/strtree/TemplateSTRtree.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/GeoJSONReader.h>
#include <geos/io/GeoJSONWriter.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/operation/cluster/Clusters.h>
#include <geos/util/Interrupt.h>

#include <stdexcept>
#include <new>

#ifdef _MSC_VER
#pragma warning(disable : 4099)
#endif

// Some extra magic to make type declarations in geos_c.h work -
// for cross-checking of types in header.
// NOTE: the below defines or struct definition must be kept in exact
// sync between geos_c.cpp and geos_ts_c.cpp to avoid C++ One Definition Rule
// violations.
#define GEOSGeometry geos::geom::Geometry
#define GEOSPreparedGeometry geos::geom::prep::PreparedGeometry
#define GEOSClusterInfo geos::operation::cluster::Clusters
#define GEOSCoordSequence geos::geom::CoordinateSequence
#define GEOSBufferParams geos::operation::buffer::BufferParameters
#define GEOSSTRtree geos::index::strtree::TemplateSTRtree<void*>
#define GEOSWKTReader geos::io::WKTReader
#define GEOSWKTWriter geos::io::WKTWriter
#define GEOSWKBReader geos::io::WKBReader
#define GEOSWKBWriter geos::io::WKBWriter
#define GEOSGeoJSONReader geos::io::GeoJSONReader
#define GEOSGeoJSONWriter geos::io::GeoJSONWriter

// Implementation struct for the GEOSMakeValidParams object
typedef struct {
    int method;
    int keepCollapsed;
} GEOSMakeValidParams;

#include "geos_c.h"

/// Define this if you want operations triggering Exceptions to
/// be printed (will use the NOTIFY channel - only implemented for GEOSUnion so far)
///
#undef VERBOSE_EXCEPTIONS

#include <geos/export.h>

/*
#if defined(_MSC_VER)
#  define GEOS_DLL     __declspec(dllexport)
#else
#  define GEOS_DLL
#endif
*/

// import the most frequently used definitions globally
using geos::geom::Geometry;
using geos::geom::LineString;
using geos::geom::Polygon;
using geos::geom::CoordinateSequence;
using geos::geom::GeometryFactory;

using geos::io::WKTReader;
using geos::io::WKTWriter;
using geos::io::WKBReader;
using geos::io::WKBWriter;
using geos::io::GeoJSONReader;
using geos::io::GeoJSONWriter;


typedef std::unique_ptr<Geometry> GeomPtr;

//## GLOBALS ################################################

// NOTE: SRID will have to be changed after geometry creation
GEOSContextHandle_t handle = NULL;

extern "C" {

    void
    initGEOS(GEOSMessageHandler nf, GEOSMessageHandler ef)
    {
        if(! handle) {
            handle = initGEOS_r(nf, ef);
        }
        else {
            GEOSContext_setNoticeHandler_r(handle, nf);
            GEOSContext_setErrorHandler_r(handle, ef);
        }

        geos::util::Interrupt::cancel();
    }

    void
    finishGEOS()
    {
        if(handle != NULL) {
            finishGEOS_r(handle);
            handle = NULL;
        }
    }

    GEOSInterruptCallback*
    GEOS_interruptRegisterCallback(GEOSInterruptCallback* cb)
    {
        return geos::util::Interrupt::registerCallback(cb);
    }

    void
    GEOS_interruptRequest()
    {
        geos::util::Interrupt::request();
    }

    void
    GEOS_interruptCancel()
    {
        geos::util::Interrupt::cancel();
    }

    void
    GEOSFree(void* buffer)
    {
        GEOSFree_r(handle, buffer);
    }

    /****************************************************************
    ** relate()-related functions
    ** return 0 = false, 1 = true, 2 = error occurred
    **
    */
    char
    GEOSDisjoint(const Geometry* g1, const Geometry* g2)
    {
        return GEOSDisjoint_r(handle, g1, g2);
    }

    char
    GEOSTouches(const Geometry* g1, const Geometry* g2)
    {
        return GEOSTouches_r(handle, g1, g2);
    }

    char
    GEOSIntersects(const Geometry* g1, const Geometry* g2)
    {
        return GEOSIntersects_r(handle, g1, g2);
    }

    char
    GEOSCrosses(const Geometry* g1, const Geometry* g2)
    {
        return GEOSCrosses_r(handle, g1, g2);
    }

    char
    GEOSWithin(const Geometry* g1, const Geometry* g2)
    {
        return GEOSWithin_r(handle, g1, g2);
    }

// call g1->contains(g2)
// returns 0 = false
//         1 = true
//         2 = error was trapped
    char
    GEOSContains(const Geometry* g1, const Geometry* g2)
    {
        return GEOSContains_r(handle, g1, g2);
    }

    char
    GEOSOverlaps(const Geometry* g1, const Geometry* g2)
    {
        return GEOSOverlaps_r(handle, g1, g2);
    }

    char
    GEOSCovers(const Geometry* g1, const Geometry* g2)
    {
        return GEOSCovers_r(handle, g1, g2);
    }

    char
    GEOSCoveredBy(const Geometry* g1, const Geometry* g2)
    {
        return GEOSCoveredBy_r(handle, g1, g2);
    }


//-------------------------------------------------------------------
// low-level relate functions
//------------------------------------------------------------------

    char
    GEOSRelatePattern(const Geometry* g1, const Geometry* g2, const char* imPattern)
    {
        return GEOSRelatePattern_r(handle, g1, g2, imPattern);
    }

    char
    GEOSRelatePatternMatch(const char* intMatrix, const char* imPattern)
    {
        return GEOSRelatePatternMatch_r(handle, intMatrix, imPattern);
    }

    char*
    GEOSRelate(const Geometry* g1, const Geometry* g2)
    {
        return GEOSRelate_r(handle, g1, g2);
    }

    char*
    GEOSRelateBoundaryNodeRule(const Geometry* g1, const Geometry* g2, int bnr)
    {
        return GEOSRelateBoundaryNodeRule_r(handle, g1, g2, bnr);
    }


//-----------------------------------------------------------------
// isValid
//-----------------------------------------------------------------


    char
    GEOSisValid(const Geometry* g)
    {
        return GEOSisValid_r(handle, g);
    }

    char*
    GEOSisValidReason(const Geometry* g)
    {
        return GEOSisValidReason_r(handle, g);
    }

    char
    GEOSisValidDetail(const Geometry* g, int flags,
                      char** reason, Geometry** location)
    {
        return GEOSisValidDetail_r(handle, g, flags, reason, location);
    }

//-----------------------------------------------------------------
// general purpose
//-----------------------------------------------------------------

    char
    GEOSEquals(const Geometry* g1, const Geometry* g2)
    {
        return GEOSEquals_r(handle, g1, g2);
    }

    char
    GEOSEqualsExact(const Geometry* g1, const Geometry* g2, double tolerance)
    {
        return GEOSEqualsExact_r(handle, g1, g2, tolerance);
    }

    char
    GEOSEqualsIdentical(const Geometry* g1, const Geometry* g2)
    {
        return GEOSEqualsIdentical_r(handle, g1, g2);
    }

    int
    GEOSDistance(const Geometry* g1, const Geometry* g2, double* dist)
    {
        return GEOSDistance_r(handle, g1, g2, dist);
    }

    char
    GEOSDistanceWithin(const Geometry* g1, const Geometry* g2, double dist)
    {
        return GEOSDistanceWithin_r(handle, g1, g2, dist);
    }

    int
    GEOSDistanceIndexed(const Geometry* g1, const Geometry* g2, double* dist)
    {
        return GEOSDistanceIndexed_r(handle, g1, g2, dist);
    }

    int
    GEOSHausdorffDistance(const Geometry* g1, const Geometry* g2, double* dist)
    {
        return GEOSHausdorffDistance_r(handle, g1, g2, dist);
    }

    int
    GEOSHausdorffDistanceDensify(const Geometry* g1, const Geometry* g2, double densifyFrac, double* dist)
    {
        return GEOSHausdorffDistanceDensify_r(handle, g1, g2, densifyFrac, dist);
    }

    int
    GEOSFrechetDistance(const Geometry* g1, const Geometry* g2, double* dist)
    {
        return GEOSFrechetDistance_r(handle, g1, g2, dist);
    }

    int
    GEOSFrechetDistanceDensify(const Geometry* g1, const Geometry* g2, double densifyFrac, double* dist)
    {
        return GEOSFrechetDistanceDensify_r(handle, g1, g2, densifyFrac, dist);
    }

    int
    GEOSArea(const Geometry* g, double* area)
    {
        return GEOSArea_r(handle, g, area);
    }

    int
    GEOSLength(const Geometry* g, double* length)
    {
        return GEOSLength_r(handle, g, length);
    }

    CoordinateSequence*
    GEOSNearestPoints(const Geometry* g1, const Geometry* g2)
    {
        return GEOSNearestPoints_r(handle, g1, g2);
    }

    GEOSClusterInfo*
    GEOSClusterDBSCAN(const GEOSGeometry* g, double eps, unsigned minPoints)
    {
        return GEOSClusterDBSCAN_r(handle, g, eps, minPoints);
    }

    GEOSClusterInfo*
    GEOSClusterGeometryDistance(const GEOSGeometry* g, double d)
    {
        return GEOSClusterGeometryDistance_r(handle, g, d);
    }

    GEOSClusterInfo*
    GEOSClusterGeometryIntersects(const GEOSGeometry* g)
    {
        return GEOSClusterGeometryIntersects_r(handle, g);
    }

    GEOSClusterInfo*
    GEOSClusterEnvelopeDistance(const GEOSGeometry* g, double d)
    {
        return GEOSClusterEnvelopeDistance_r(handle, g, d);
    }

    GEOSClusterInfo*
    GEOSClusterEnvelopeIntersects(const GEOSGeometry* g)
    {
        return GEOSClusterEnvelopeIntersects_r(handle, g);
    }

    std::size_t GEOSClusterInfo_getNumClusters(const GEOSClusterInfo* clusters)
    {
        return GEOSClusterInfo_getNumClusters_r(handle, clusters);
    }

    std::size_t GEOSClusterInfo_getClusterSize(const GEOSClusterInfo* clusters, size_t i)
    {
        return GEOSClusterInfo_getClusterSize_r(handle, clusters, i);
    }

    const std::size_t* GEOSClusterInfo_getInputsForClusterN(const GEOSClusterInfo* clusters, size_t i)
    {
        return GEOSClusterInfo_getInputsForClusterN_r(handle, clusters, i);
    }

    std::size_t* GEOSClusterInfo_getClustersForInputs(const GEOSClusterInfo* clusters)
    {
        return GEOSClusterInfo_getClustersForInputs_r(handle, clusters);
    }

    void GEOSClusterInfo_destroy(GEOSClusterInfo* info)
    {
        GEOSClusterInfo_destroy_r(handle, info);
    }

    Geometry*
    GEOSGeomFromWKT(const char* wkt)
    {
        return GEOSGeomFromWKT_r(handle, wkt);
    }

    char*
    GEOSGeomToWKT(const Geometry* g)
    {
        return GEOSGeomToWKT_r(handle, g);
    }

// Remember to free the result!
    unsigned char*
    GEOSGeomToWKB_buf(const Geometry* g, std::size_t* size)
    {
        return GEOSGeomToWKB_buf_r(handle, g, size);
    }

    Geometry*
    GEOSGeomFromWKB_buf(const unsigned char* wkb, std::size_t size)
    {
        return GEOSGeomFromWKB_buf_r(handle, wkb, size);
    }

    /* Read/write wkb hex values.  Returned geometries are
       owned by the caller.*/
    unsigned char*
    GEOSGeomToHEX_buf(const Geometry* g, std::size_t* size)
    {
        return GEOSGeomToHEX_buf_r(handle, g, size);
    }

    Geometry*
    GEOSGeomFromHEX_buf(const unsigned char* hex, std::size_t size)
    {
        return GEOSGeomFromHEX_buf_r(handle, hex, size);
    }

    char
    GEOSisEmpty(const Geometry* g)
    {
        return GEOSisEmpty_r(handle, g);
    }

    char
    GEOSisSimple(const Geometry* g)
    {
        return GEOSisSimple_r(handle, g);
    }

    char
    GEOSisRing(const Geometry* g)
    {
        return GEOSisRing_r(handle, g);
    }



//free the result of this
    char*
    GEOSGeomType(const Geometry* g)
    {
        return GEOSGeomType_r(handle, g);
    }

// Return postgis geometry type index
    int
    GEOSGeomTypeId(const Geometry* g)
    {
        return GEOSGeomTypeId_r(handle, g);
    }




//-------------------------------------------------------------------
// GEOS functions that return geometries
//-------------------------------------------------------------------

    Geometry*
    GEOSEnvelope(const Geometry* g)
    {
        return GEOSEnvelope_r(handle, g);
    }

    Geometry*
    GEOSIntersection(const Geometry* g1, const Geometry* g2)
    {
        return GEOSIntersection_r(handle, g1, g2);
    }

    Geometry*
    GEOSIntersectionPrec(const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return GEOSIntersectionPrec_r(handle, g1, g2, gridSize);
    }

    Geometry*
    GEOSBuffer(const Geometry* g, double width, int quadrantsegments)
    {
        return GEOSBuffer_r(handle, g, width, quadrantsegments);
    }

    Geometry*
    GEOSBufferWithStyle(const Geometry* g, double width, int quadsegs,
                        int endCapStyle, int joinStyle, double mitreLimit)
    {
        return GEOSBufferWithStyle_r(handle, g, width, quadsegs, endCapStyle,
                                     joinStyle, mitreLimit);
    }

    Geometry*
    GEOSDensify(const Geometry* g, double tolerance)
    {
        return GEOSDensify_r(handle, g, tolerance);
    }


    Geometry*
    GEOSSingleSidedBuffer(const Geometry* g, double width, int quadsegs,
                          int joinStyle, double mitreLimit, int leftSide)
    {
        return GEOSSingleSidedBuffer_r(handle, g, width, quadsegs,
                                       joinStyle, mitreLimit, leftSide);
    }

    Geometry*
    GEOSOffsetCurve(const Geometry* g, double width, int quadsegs,
                    int joinStyle, double mitreLimit)
    {
        return GEOSOffsetCurve_r(handle, g, width, quadsegs,
                                 joinStyle, mitreLimit);
    }

    Geometry*
    GEOSConvexHull(const Geometry* g)
    {
        return GEOSConvexHull_r(handle, g);
    }

    Geometry*
    GEOSConcaveHull(const Geometry* g,
                    double ratio,
                    unsigned int allowHoles)

    {
        return GEOSConcaveHull_r(handle, g, ratio, allowHoles);
    }

    Geometry*
    GEOSConcaveHullByLength(const Geometry* g,
                    double length,
                    unsigned int allowHoles)

    {
        return GEOSConcaveHullByLength_r(handle, g, length, allowHoles);
    }

    Geometry*
    GEOSPolygonHullSimplify(const Geometry* g,
                            unsigned int isOuter,
                            double vertexNumFraction)
    {
        return GEOSPolygonHullSimplify_r(handle, g, isOuter, vertexNumFraction);
    }

    Geometry*
    GEOSPolygonHullSimplifyMode(const Geometry* g,
                            unsigned int isOuter,
                            unsigned int parameterMode,
                            double parameter)
    {
        return GEOSPolygonHullSimplifyMode_r(handle, g, isOuter, parameterMode, parameter);
    }

    Geometry*
    GEOSConcaveHullOfPolygons(const Geometry* g,
        double lengthRatio,
        unsigned int isTight,
        unsigned int isHolesAllowed)
    {
        return GEOSConcaveHullOfPolygons_r(handle,
            g, lengthRatio, isTight, isHolesAllowed);
    }

    Geometry*
    GEOSMinimumRotatedRectangle(const Geometry* g)
    {
        return GEOSMinimumRotatedRectangle_r(handle, g);
    }

    Geometry*
    GEOSMaximumInscribedCircle(const Geometry* g, double tolerance)
    {
        return GEOSMaximumInscribedCircle_r(handle, g, tolerance);
    }

    Geometry*
    GEOSLargestEmptyCircle(const Geometry* g, const Geometry* boundary, double tolerance)
    {
        return GEOSLargestEmptyCircle_r(handle, g, boundary, tolerance);
    }

    Geometry*
    GEOSMinimumWidth(const Geometry* g)
    {
        return GEOSMinimumWidth_r(handle, g);
    }

    Geometry*
    GEOSMinimumClearanceLine(const Geometry* g)
    {
        return GEOSMinimumClearanceLine_r(handle, g);
    }

    int
    GEOSMinimumClearance(const Geometry* g, double* d)
    {
        return GEOSMinimumClearance_r(handle, g, d);
    }

    Geometry*
    GEOSDifference(const Geometry* g1, const Geometry* g2)
    {
        return GEOSDifference_r(handle, g1, g2);
    }

    Geometry*
    GEOSDifferencePrec(const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return GEOSDifferencePrec_r(handle, g1, g2, gridSize);
    }

    Geometry*
    GEOSBoundary(const Geometry* g)
    {
        return GEOSBoundary_r(handle, g);
    }

    Geometry*
    GEOSSymDifference(const Geometry* g1, const Geometry* g2)
    {
        return GEOSSymDifference_r(handle, g1, g2);
    }

    Geometry*
    GEOSSymDifferencePrec(const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return GEOSSymDifferencePrec_r(handle, g1, g2, gridSize);
    }

    Geometry*
    GEOSUnion(const Geometry* g1, const Geometry* g2)
    {
        return GEOSUnion_r(handle, g1, g2);
    }

    Geometry*
    GEOSUnionPrec(const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return GEOSUnionPrec_r(handle, g1, g2, gridSize);
    }

    Geometry*
    GEOSUnaryUnion(const Geometry* g)
    {
        return GEOSUnaryUnion_r(handle, g);
    }

    Geometry*
    GEOSUnaryUnionPrec(const Geometry* g, double gridSize)
    {
        return GEOSUnaryUnionPrec_r(handle, g, gridSize);
    }

    Geometry*
    GEOSCoverageUnion(const Geometry* g)
    {
        return GEOSCoverageUnion_r(handle, g);
    }

    Geometry*
    GEOSDisjointSubsetUnion(const Geometry* g)
    {
        return GEOSDisjointSubsetUnion_r(handle, g);
    }

    Geometry*
    GEOSNode(const Geometry* g)
    {
        return GEOSNode_r(handle, g);
    }

    Geometry*
    GEOSUnionCascaded(const Geometry* g)
    {
        return GEOSUnionCascaded_r(handle, g);
    }

    Geometry*
    GEOSPointOnSurface(const Geometry* g)
    {
        return GEOSPointOnSurface_r(handle, g);
    }


    Geometry*
    GEOSClipByRect(const Geometry* g, double xmin, double ymin, double xmax, double ymax)
    {
        return GEOSClipByRect_r(handle, g, xmin, ymin, xmax, ymax);
    }


    Geometry*
    GEOSGeom_transformXY(const GEOSGeometry* g, GEOSTransformXYCallback callback, void* userdata) {
        return GEOSGeom_transformXY_r(handle, g, callback, userdata);
    }


    Geometry*
    GEOSGeom_transformXYZ(const GEOSGeometry* g, GEOSTransformXYZCallback callback, void* userdata) {
        return GEOSGeom_transformXYZ_r(handle, g, callback, userdata);
    }


//-------------------------------------------------------------------
// memory management functions
//------------------------------------------------------------------


    void
    GEOSGeom_destroy(Geometry* a)
    {
        return GEOSGeom_destroy_r(handle, a);
    }


    int
    GEOSGetNumCoordinates(const Geometry* g)
    {
        return GEOSGetNumCoordinates_r(handle, g);
    }

    /*
     * Return -1 on exception, 0 otherwise.
     * Converts Geometry to normal form (or canonical form).
     */
    int
    GEOSNormalize(Geometry* g)
    {
        return GEOSNormalize_r(handle, g);
    }

    int
    GEOSOrientPolygons(Geometry* g, int exteriorCW)
    {
        return GEOSOrientPolygons_r(handle, g, exteriorCW);
    }

    int
    GEOSGetNumInteriorRings(const Geometry* g)
    {
        return GEOSGetNumInteriorRings_r(handle, g);
    }


// returns -1 on error and 1 for non-multi geometries
    int
    GEOSGetNumGeometries(const Geometry* g)
    {
        return GEOSGetNumGeometries_r(handle, g);
    }


    /*
     * Call only on GEOMETRYCOLLECTION or MULTI*.
     * Return a pointer to the internal Geometry.
     */
    const Geometry*
    GEOSGetGeometryN(const Geometry* g, int n)
    {
        return GEOSGetGeometryN_r(handle, g, n);
    }

    /*
     * Call only on LINESTRING
     * Returns NULL on exception
     */
    Geometry*
    GEOSGeomGetPointN(const Geometry* g, int n)
    {
        return GEOSGeomGetPointN_r(handle, g, n);
    }

    /*
     * Call only on LINESTRING
     */
    Geometry*
    GEOSGeomGetStartPoint(const Geometry* g)
    {
        return GEOSGeomGetStartPoint_r(handle, g);
    }

    /*
     * Call only on LINESTRING
     */
    Geometry*
    GEOSGeomGetEndPoint(const Geometry* g)
    {
        return GEOSGeomGetEndPoint_r(handle, g);
    }

    /*
     * Call only on LINESTRING
     * return 2 on exception, 1 on true, 0 on false
     */
    char
    GEOSisClosed(const Geometry* g)
    {
        return GEOSisClosed_r(handle, g);
    }

    /*
     * Call only on LINESTRING
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetLength(const Geometry* g, double* length)
    {
        return GEOSGeomGetLength_r(handle, g, length);
    }

    /*
     * Call only on LINESTRING
     * returns -1 on exception
     */
    int
    GEOSGeomGetNumPoints(const Geometry* g)
    {
        return GEOSGeomGetNumPoints_r(handle, g);
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetX(const Geometry* g, double* x)
    {
        return GEOSGeomGetX_r(handle, g, x);
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetY(const Geometry* g, double* y)
    {
        return GEOSGeomGetY_r(handle, g, y);
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetZ(const Geometry* g1, double* z)
    {
        return GEOSGeomGetZ_r(handle, g1, z);
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetM(const Geometry* g1, double* m)
    {
        return GEOSGeomGetM_r(handle, g1, m);
    }

    /*
     * Call only on polygon
     * Return a copy of the internal Geometry.
     */
    const Geometry*
    GEOSGetExteriorRing(const Geometry* g)
    {
        return GEOSGetExteriorRing_r(handle, g);
    }

    /*
     * Call only on polygon
     * Return a pointer to internal storage, do not destroy it.
     */
    const Geometry*
    GEOSGetInteriorRingN(const Geometry* g, int n)
    {
        return GEOSGetInteriorRingN_r(handle, g, n);
    }

    Geometry*
    GEOSGetCentroid(const Geometry* g)
    {
        return GEOSGetCentroid_r(handle, g);
    }

    int
    GEOSHilbertCode(const GEOSGeometry *geom, const GEOSGeometry* extent,
                unsigned int level, unsigned int *code)
    {
        return GEOSHilbertCode_r(handle, geom, extent, level, code);
    }

    Geometry*
    GEOSMinimumBoundingCircle(const Geometry* g, double* radius, Geometry** center)
    {
        return GEOSMinimumBoundingCircle_r(handle, g, radius, center);
    }

    Geometry*
    GEOSGeom_createCollection(int type, Geometry** geoms, unsigned int ngeoms)
    {
        return GEOSGeom_createCollection_r(handle, type, geoms, ngeoms);
    }

    Geometry**
    GEOSGeom_releaseCollection(Geometry* collection, unsigned int * ngeoms)
    {
        return GEOSGeom_releaseCollection_r(handle, collection, ngeoms);
    }

    Geometry*
    GEOSPolygonize(const Geometry* const* g, unsigned int ngeoms)
    {
        return GEOSPolygonize_r(handle, g, ngeoms);
    }

    Geometry*
    GEOSPolygonize_valid(const Geometry* const* g, unsigned int ngeoms)
    {
        return GEOSPolygonize_valid_r(handle, g, ngeoms);
    }

    Geometry*
    GEOSPolygonizer_getCutEdges(const Geometry* const* g, unsigned int ngeoms)
    {
        return GEOSPolygonizer_getCutEdges_r(handle, g, ngeoms);
    }

    GEOSGeometry*
    GEOSPolygonize_full(const GEOSGeometry* input,
                        GEOSGeometry** cuts, GEOSGeometry** dangles, GEOSGeometry** invalid)
    {
        return GEOSPolygonize_full_r(handle, input, cuts, dangles, invalid);
    }

    Geometry*
    GEOSBuildArea(const Geometry* g)
    {
        return GEOSBuildArea_r(handle, g);
    }

    Geometry*
    GEOSMakeValid(const Geometry* g)
    {
        return GEOSMakeValid_r(handle, g);
    }

    GEOSMakeValidParams*
    GEOSMakeValidParams_create()
    {
        return GEOSMakeValidParams_create_r(handle);
    }

    void
    GEOSMakeValidParams_destroy(GEOSMakeValidParams* parms)
    {
        return GEOSMakeValidParams_destroy_r(handle, parms);
    }

    int
    GEOSMakeValidParams_setMethod(
        GEOSMakeValidParams* p,
        GEOSMakeValidMethods method)
    {
        return GEOSMakeValidParams_setMethod_r(handle, p, method);
    }

    int
    GEOSMakeValidParams_setKeepCollapsed(
        GEOSMakeValidParams* p,
        int keepCollapsed)
    {
        return GEOSMakeValidParams_setKeepCollapsed_r(handle, p, keepCollapsed);
    }

    Geometry*
    GEOSMakeValidWithParams(
        const Geometry* g,
        const GEOSMakeValidParams* params)
    {
        return GEOSMakeValidWithParams_r(handle, g, params);
    }

    Geometry*
    GEOSRemoveRepeatedPoints(
        const Geometry* g,
        double tolerance)
    {
        return GEOSRemoveRepeatedPoints_r(handle, g, tolerance);
    }

    Geometry*
    GEOSLineMerge(const Geometry* g)
    {
        return GEOSLineMerge_r(handle, g);
    }

    Geometry*
    GEOSLineMergeDirected(const Geometry* g)
    {
        return GEOSLineMergeDirected_r(handle, g);
    }

    Geometry*
    GEOSLineSubstring(const Geometry* g, double start_fraction, double end_fraction)
    {
        return GEOSLineSubstring_r(handle, g, start_fraction, end_fraction);
    }

    Geometry*
    GEOSReverse(const Geometry* g)
    {
        return GEOSReverse_r(handle, g);
    }

    int
    GEOSGetSRID(const Geometry* g)
    {
        return GEOSGetSRID_r(handle, g);
    }

    void
    GEOSSetSRID(Geometry* g, int srid)
    {
        return GEOSSetSRID_r(handle, g, srid);
    }

    void*
    GEOSGeom_getUserData(const Geometry* g)
    {
        return GEOSGeom_getUserData_r(handle, g);
    }

    void
    GEOSGeom_setUserData(Geometry* g, void* userData)
    {
        return GEOSGeom_setUserData_r(handle, g, userData);
    }

    char
    GEOSHasZ(const Geometry* g)
    {
        return GEOSHasZ_r(handle, g);
    }

    char
    GEOSHasM(const Geometry* g)
    {
        return GEOSHasM_r(handle, g);
    }

    int
    GEOS_getWKBOutputDims()
    {
        return GEOS_getWKBOutputDims_r(handle);
    }

    int
    GEOS_setWKBOutputDims(int newdims)
    {
        return GEOS_setWKBOutputDims_r(handle, newdims);
    }

    int
    GEOS_getWKBByteOrder()
    {
        return GEOS_getWKBByteOrder_r(handle);
    }

    int
    GEOS_setWKBByteOrder(int byteOrder)
    {
        return GEOS_setWKBByteOrder_r(handle, byteOrder);
    }

    CoordinateSequence*
    GEOSCoordSeq_create(unsigned int size, unsigned int dims)
    {
        return GEOSCoordSeq_create_r(handle, size, dims);
    }

    CoordinateSequence*
    GEOSCoordSeq_copyFromBuffer(const double* buf, unsigned int size, int hasZ, int hasM)
    {
        return GEOSCoordSeq_copyFromBuffer_r(handle, buf, size, hasZ, hasM);
    }

    int
    GEOSCoordSeq_copyToBuffer(const CoordinateSequence* s, double* buf, int hasZ, int hasM)
    {
        return GEOSCoordSeq_copyToBuffer_r(handle, s, buf, hasZ, hasM);
    }

    CoordinateSequence*
    GEOSCoordSeq_copyFromArrays(const double* x, const double* y, const double* z, const double* m, unsigned int size)
    {
        return GEOSCoordSeq_copyFromArrays_r(handle, x, y, z, m, size);
    }

    int
    GEOSCoordSeq_copyToArrays(const CoordinateSequence* s, double* x, double* y, double* z, double* m)
    {
        return GEOSCoordSeq_copyToArrays_r(handle, s, x, y, z, m);
    }

    int
    GEOSCoordSeq_setOrdinate(CoordinateSequence* s, unsigned int idx, unsigned int dim, double val)
    {
        return GEOSCoordSeq_setOrdinate_r(handle, s, idx, dim, val);
    }

    int
    GEOSCoordSeq_setX(CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate(s, idx, 0, val);
    }

    int
    GEOSCoordSeq_setY(CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate(s, idx, 1, val);
    }

    int
    GEOSCoordSeq_setZ(CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate(s, idx, 2, val);
    }

    int
    GEOSCoordSeq_setXY(CoordinateSequence* s, unsigned int idx, double x, double y)
    {
        return GEOSCoordSeq_setXY_r(handle, s, idx, x, y);
    }

    int
    GEOSCoordSeq_setXYZ(CoordinateSequence* s, unsigned int idx, double x, double y, double z)
    {
        return GEOSCoordSeq_setXYZ_r(handle, s, idx, x, y, z);
    }

    CoordinateSequence*
    GEOSCoordSeq_clone(const CoordinateSequence* s)
    {
        return GEOSCoordSeq_clone_r(handle, s);
    }

    int
    GEOSCoordSeq_getOrdinate(const CoordinateSequence* s, unsigned int idx, unsigned int dim, double* val)
    {
        return GEOSCoordSeq_getOrdinate_r(handle, s, idx, dim, val);
    }

    int
    GEOSCoordSeq_getX(const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate(s, idx, 0, val);
    }

    int
    GEOSCoordSeq_getY(const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate(s, idx, 1, val);
    }

    int
    GEOSCoordSeq_getZ(const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate(s, idx, 2, val);
    }

    int
    GEOSCoordSeq_getXY(const CoordinateSequence* s, unsigned int idx, double* x, double* y)
    {
        return GEOSCoordSeq_getXY_r(handle, s, idx, x, y);
    }

    int
    GEOSCoordSeq_getXYZ(const CoordinateSequence* s, unsigned int idx, double* x, double* y, double* z)
    {
        return GEOSCoordSeq_getXYZ_r(handle, s, idx, x, y, z);
    }

    int
    GEOSCoordSeq_getSize(const CoordinateSequence* s, unsigned int* size)
    {
        return GEOSCoordSeq_getSize_r(handle, s, size);
    }

    int
    GEOSCoordSeq_getDimensions(const CoordinateSequence* s, unsigned int* dims)
    {
        return GEOSCoordSeq_getDimensions_r(handle, s, dims);
    }

    int
    GEOSCoordSeq_isCCW(const CoordinateSequence* s, char* is_ccw)
    {
        return GEOSCoordSeq_isCCW_r(handle, s, is_ccw);
    }

    void
    GEOSCoordSeq_destroy(CoordinateSequence* s)
    {
        return GEOSCoordSeq_destroy_r(handle, s);
    }

    const CoordinateSequence*
    GEOSGeom_getCoordSeq(const Geometry* g)
    {
        return GEOSGeom_getCoordSeq_r(handle, g);
    }

    Geometry*
    GEOSGeom_createPoint(CoordinateSequence* cs)
    {
        return GEOSGeom_createPoint_r(handle, cs);
    }

    Geometry*
    GEOSGeom_createPointFromXY(double x, double y)
    {
        return GEOSGeom_createPointFromXY_r(handle, x, y);
    }

    Geometry*
    GEOSGeom_createLinearRing(CoordinateSequence* cs)
    {
        return GEOSGeom_createLinearRing_r(handle, cs);
    }

    Geometry*
    GEOSGeom_createLineString(CoordinateSequence* cs)
    {
        return GEOSGeom_createLineString_r(handle, cs);
    }

    Geometry*
    GEOSGeom_createPolygon(Geometry* shell, Geometry** holes, unsigned int nholes)
    {
        return GEOSGeom_createPolygon_r(handle, shell, holes, nholes);
    }

    Geometry*
    GEOSGeom_createCircularString(CoordinateSequence* cs)
    {
        return GEOSGeom_createCircularString_r(handle, cs);
    }

    Geometry*
    GEOSGeom_createCompoundCurve(Geometry** curves, unsigned int ngeoms)
    {
        return GEOSGeom_createCompoundCurve_r(handle, curves, ngeoms);
    }

    Geometry*
    GEOSGeom_createCurvePolygon(Geometry* shell, Geometry** holes, unsigned int nholes)
    {
        return GEOSGeom_createCurvePolygon_r(handle, shell, holes, nholes);
    }

    Geometry*
    GEOSGeom_clone(const Geometry* g)
    {
        return GEOSGeom_clone_r(handle, g);
    }

    GEOSGeometry*
    GEOSGeom_setPrecision(const GEOSGeometry* g, double gridSize, int flags)
    {
        return GEOSGeom_setPrecision_r(handle, g, gridSize, flags);
    }

    double
    GEOSGeom_getPrecision(const GEOSGeometry* g)
    {
        return GEOSGeom_getPrecision_r(handle, g);
    }

    int
    GEOSGeom_getDimensions(const Geometry* g)
    {
        return GEOSGeom_getDimensions_r(handle, g);
    }

    int
    GEOSGeom_getCoordinateDimension(const Geometry* g)
    {
        return GEOSGeom_getCoordinateDimension_r(handle, g);
    }

    int GEOS_DLL GEOSGeom_getXMin(const GEOSGeometry* g, double* value)
    {
        return GEOSGeom_getXMin_r(handle, g, value);
    }

    int GEOS_DLL GEOSGeom_getYMin(const GEOSGeometry* g, double* value)
    {
        return GEOSGeom_getYMin_r(handle, g, value);
    }

    int GEOS_DLL GEOSGeom_getXMax(const GEOSGeometry* g, double* value)
    {
        return GEOSGeom_getXMax_r(handle, g, value);
    }

    int GEOS_DLL GEOSGeom_getYMax(const GEOSGeometry* g, double* value)
    {
        return GEOSGeom_getYMax_r(handle, g, value);
    }

    int GEOS_DLL GEOSGeom_getExtent(const GEOSGeometry* g, double* xmin, double* ymin, double* xmax, double* ymax)
    {
        return GEOSGeom_getExtent_r(handle, g, xmin, ymin, xmax, ymax);
    }

    Geometry*
    GEOSSimplify(const Geometry* g, double tolerance)
    {
        return GEOSSimplify_r(handle, g, tolerance);
    }

    Geometry*
    GEOSTopologyPreserveSimplify(const Geometry* g, double tolerance)
    {
        return GEOSTopologyPreserveSimplify_r(handle, g, tolerance);
    }


    /* WKT Reader */
    WKTReader*
    GEOSWKTReader_create()
    {
        return GEOSWKTReader_create_r(handle);
    }

    void
    GEOSWKTReader_destroy(WKTReader* reader)
    {
        GEOSWKTReader_destroy_r(handle, reader);
    }

    void
    GEOSWKTReader_setFixStructure(WKTReader* reader, char doFix)
    {
        GEOSWKTReader_setFixStructure_r(handle, reader, doFix);
    }

    Geometry*
    GEOSWKTReader_read(WKTReader* reader, const char* wkt)
    {
        return GEOSWKTReader_read_r(handle, reader, wkt);
    }

    /* WKT Writer */
    WKTWriter*
    GEOSWKTWriter_create()
    {
        return GEOSWKTWriter_create_r(handle);
    }

    void
    GEOSWKTWriter_destroy(WKTWriter* Writer)
    {
        GEOSWKTWriter_destroy_r(handle, Writer);
    }

    char*
    GEOSWKTWriter_write(WKTWriter* writer, const Geometry* geom)
    {
        return GEOSWKTWriter_write_r(handle, writer, geom);
    }

    void
    GEOSWKTWriter_setTrim(WKTWriter* writer, char trim)
    {
        GEOSWKTWriter_setTrim_r(handle, writer, trim);
    }

    void
    GEOSWKTWriter_setRoundingPrecision(WKTWriter* writer, int precision)
    {
        return GEOSWKTWriter_setRoundingPrecision_r(handle, writer, precision);
    }

    void
    GEOSWKTWriter_setOutputDimension(WKTWriter* writer, int dim)
    {
        GEOSWKTWriter_setOutputDimension_r(handle, writer, dim);
    }

    int
    GEOSWKTWriter_getOutputDimension(WKTWriter* writer)
    {
        return GEOSWKTWriter_getOutputDimension_r(handle, writer);
    }

    void
    GEOSWKTWriter_setOld3D(WKTWriter* writer, int useOld3D)
    {
        GEOSWKTWriter_setOld3D_r(handle, writer, useOld3D);
    }

    /* WKB Reader */
    WKBReader*
    GEOSWKBReader_create()
    {
        return GEOSWKBReader_create_r(handle);
    }

    void
    GEOSWKBReader_destroy(WKBReader* reader)
    {
        GEOSWKBReader_destroy_r(handle, reader);
    }

    void
    GEOSWKBReader_setFixStructure(WKBReader* reader, char doFix)
    {
        GEOSWKBReader_setFixStructure_r(handle, reader, doFix);
    }

    Geometry*
    GEOSWKBReader_read(WKBReader* reader, const unsigned char* wkb, std::size_t size)
    {
        return GEOSWKBReader_read_r(handle, reader, wkb, size);
    }

    Geometry*
    GEOSWKBReader_readHEX(WKBReader* reader, const unsigned char* hex, std::size_t size)
    {
        return GEOSWKBReader_readHEX_r(handle, reader, hex, size);
    }

    /* WKB Writer */
    WKBWriter*
    GEOSWKBWriter_create()
    {
        return GEOSWKBWriter_create_r(handle);
    }

    void
    GEOSWKBWriter_destroy(WKBWriter* Writer)
    {
        GEOSWKBWriter_destroy_r(handle, Writer);
    }


    /* The caller owns the result */
    unsigned char*
    GEOSWKBWriter_write(WKBWriter* writer, const Geometry* geom, std::size_t* size)
    {
        return GEOSWKBWriter_write_r(handle, writer, geom, size);
    }

    /* The caller owns the result */
    unsigned char*
    GEOSWKBWriter_writeHEX(WKBWriter* writer, const Geometry* geom, std::size_t* size)
    {
        return GEOSWKBWriter_writeHEX_r(handle, writer, geom, size);
    }

    int
    GEOSWKBWriter_getOutputDimension(const GEOSWKBWriter* writer)
    {
        return GEOSWKBWriter_getOutputDimension_r(handle, writer);
    }

    void
    GEOSWKBWriter_setOutputDimension(GEOSWKBWriter* writer, int newDimension)
    {
        GEOSWKBWriter_setOutputDimension_r(handle, writer, newDimension);
    }

    int
    GEOSWKBWriter_getByteOrder(const GEOSWKBWriter* writer)
    {
        return GEOSWKBWriter_getByteOrder_r(handle, writer);
    }

    void
    GEOSWKBWriter_setByteOrder(GEOSWKBWriter* writer, int newByteOrder)
    {
        GEOSWKBWriter_setByteOrder_r(handle, writer, newByteOrder);
    }

    int
    GEOSWKBWriter_getFlavor(const GEOSWKBWriter* writer)
    {
        return GEOSWKBWriter_getFlavor_r(handle, writer);
    }

    void
    GEOSWKBWriter_setFlavor(GEOSWKBWriter* writer, int newFlavor)
    {
        GEOSWKBWriter_setFlavor_r(handle, writer, newFlavor);
    }

    char
    GEOSWKBWriter_getIncludeSRID(const GEOSWKBWriter* writer)
    {
        return GEOSWKBWriter_getIncludeSRID_r(handle, writer);
    }

    void
    GEOSWKBWriter_setIncludeSRID(GEOSWKBWriter* writer, const char newIncludeSRID)
    {
        GEOSWKBWriter_setIncludeSRID_r(handle, writer, newIncludeSRID);
    }

    int
    GEOS_printDouble(double d, unsigned int precision, char *result) {
        return WKTWriter::writeTrimmedNumber(d, precision, result);
    }

    /* GeoJSON Reader */
    GeoJSONReader*
    GEOSGeoJSONReader_create()
    {
        return GEOSGeoJSONReader_create_r(handle);
    }

    void
    GEOSGeoJSONReader_destroy(GeoJSONReader* reader)
    {
        GEOSGeoJSONReader_destroy_r(handle, reader);
    }

    Geometry*
    GEOSGeoJSONReader_readGeometry(GeoJSONReader* reader, const char* geojson)
    {
        return GEOSGeoJSONReader_readGeometry_r(handle, reader, geojson);
    }

    /* GeoJSON Writer */
    GeoJSONWriter*
    GEOSGeoJSONWriter_create()
    {
        return GEOSGeoJSONWriter_create_r(handle);
    }

    void
    GEOSGeoJSONWriter_destroy(GEOSGeoJSONWriter* writer)
    {
        GEOSGeoJSONWriter_destroy_r(handle, writer);
    }

    char*
    GEOSGeoJSONWriter_writeGeometry(GEOSGeoJSONWriter* writer, const GEOSGeometry* g, int indent)
    {
        return GEOSGeoJSONWriter_writeGeometry_r(handle, writer, g, indent);
    }


//-----------------------------------------------------------------
// Prepared Geometry
//-----------------------------------------------------------------

    const geos::geom::prep::PreparedGeometry*
    GEOSPrepare(const Geometry* g)
    {
        return GEOSPrepare_r(handle, g);
    }

    void
    GEOSPreparedGeom_destroy(const geos::geom::prep::PreparedGeometry* a)
    {
        GEOSPreparedGeom_destroy_r(handle, a);
    }

    char
    GEOSPreparedContains(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedContains_r(handle, pg1, g2);
    }

    char
    GEOSPreparedContainsXY(const geos::geom::prep::PreparedGeometry* pg1, double x, double y)
    {
        return GEOSPreparedContainsXY_r(handle, pg1, x, y);
    }

    char
    GEOSPreparedContainsProperly(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedContainsProperly_r(handle, pg1, g2);
    }

    char
    GEOSPreparedCoveredBy(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedCoveredBy_r(handle, pg1, g2);
    }

    char
    GEOSPreparedCovers(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedCovers_r(handle, pg1, g2);
    }

    char
    GEOSPreparedCrosses(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedCrosses_r(handle, pg1, g2);
    }

    char
    GEOSPreparedDisjoint(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedDisjoint_r(handle, pg1, g2);
    }

    char
    GEOSPreparedIntersects(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedIntersects_r(handle, pg1, g2);
    }

    char
    GEOSPreparedIntersectsXY(const geos::geom::prep::PreparedGeometry* pg1, double x, double y)
    {
        return GEOSPreparedIntersectsXY_r(handle, pg1, x, y);
    }

    char
    GEOSPreparedOverlaps(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedOverlaps_r(handle, pg1, g2);
    }

    char
    GEOSPreparedTouches(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedTouches_r(handle, pg1, g2);
    }

    char
    GEOSPreparedWithin(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedWithin_r(handle, pg1, g2);
    }

    char *
    GEOSPreparedRelate(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2)
    {
        return GEOSPreparedRelate_r(handle, pg1, g2);
    }

    char
    GEOSPreparedRelatePattern(const geos::geom::prep::PreparedGeometry* pg1, const Geometry* g2, const char* imPattern)
    {
        return GEOSPreparedRelatePattern_r(handle, pg1, g2, imPattern);
    }

    CoordinateSequence*
    GEOSPreparedNearestPoints(const geos::geom::prep::PreparedGeometry* g1, const Geometry* g2)
    {
        return GEOSPreparedNearestPoints_r(handle, g1, g2);
    }

    int
    GEOSPreparedDistance(const geos::geom::prep::PreparedGeometry* g1, const Geometry* g2, double *dist)
    {
        return GEOSPreparedDistance_r(handle, g1, g2, dist);
    }

    char
    GEOSPreparedDistanceWithin(const geos::geom::prep::PreparedGeometry* g1, const Geometry* g2, double dist)
    {
        return GEOSPreparedDistanceWithin_r(handle, g1, g2, dist);
    }

    GEOSSTRtree*
    GEOSSTRtree_create(std::size_t nodeCapacity)
    {
        return GEOSSTRtree_create_r(handle, nodeCapacity);
    }

    int
    GEOSSTRtree_build(GEOSSTRtree* tree)
    {
        return GEOSSTRtree_build_r(handle, tree);
    }

    void
    GEOSSTRtree_insert(GEOSSTRtree* tree,
                       const geos::geom::Geometry* g,
                       void* item)
    {
        GEOSSTRtree_insert_r(handle, tree, g, item);
    }

    void
    GEOSSTRtree_query(GEOSSTRtree* tree,
                      const geos::geom::Geometry* g,
                      GEOSQueryCallback cb,
                      void* userdata)
    {
        GEOSSTRtree_query_r(handle, tree, g, cb, userdata);
    }

    const GEOSGeometry*
    GEOSSTRtree_nearest(GEOSSTRtree* tree,
                        const geos::geom::Geometry* g)
    {
        return GEOSSTRtree_nearest_r(handle, tree, g);
    }

    const void* GEOSSTRtree_nearest_generic(GEOSSTRtree* tree,
                                            const void* item,
                                            const GEOSGeometry* itemEnvelope,
                                            GEOSDistanceCallback distancefn,
                                            void* userdata)
    {
        return GEOSSTRtree_nearest_generic_r(handle, tree, item, itemEnvelope, distancefn, userdata);
    }

    void
    GEOSSTRtree_iterate(GEOSSTRtree* tree,
                        GEOSQueryCallback callback,
                        void* userdata)
    {
        GEOSSTRtree_iterate_r(handle, tree, callback, userdata);
    }

    char
    GEOSSTRtree_remove(GEOSSTRtree* tree,
                       const geos::geom::Geometry* g,
                       void* item)
    {
        return GEOSSTRtree_remove_r(handle, tree, g, item);
    }

    void
    GEOSSTRtree_destroy(GEOSSTRtree* tree)
    {
        GEOSSTRtree_destroy_r(handle, tree);
    }

    double
    GEOSProject(const geos::geom::Geometry* g,
                const geos::geom::Geometry* p)
    {
        return GEOSProject_r(handle, g, p);
    }

    geos::geom::Geometry*
    GEOSInterpolate(const geos::geom::Geometry* g,
                    double d)
    {
        return GEOSInterpolate_r(handle, g, d);
    }

    double
    GEOSProjectNormalized(const geos::geom::Geometry* g,
                          const geos::geom::Geometry* p)
    {
        return GEOSProjectNormalized_r(handle, g, p);
    }

    geos::geom::Geometry*
    GEOSInterpolateNormalized(const geos::geom::Geometry* g,
                              double d)
    {
        return GEOSInterpolateNormalized_r(handle, g, d);
    }

    geos::geom::Geometry*
    GEOSGeom_extractUniquePoints(const geos::geom::Geometry* g)
    {
        return GEOSGeom_extractUniquePoints_r(handle, g);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyCollection(int type)
    {
        return GEOSGeom_createEmptyCollection_r(handle, type);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyPoint()
    {
        return GEOSGeom_createEmptyPoint_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyLineString()
    {
        return GEOSGeom_createEmptyLineString_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyPolygon()
    {
        return GEOSGeom_createEmptyPolygon_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyCircularString()
    {
        return GEOSGeom_createEmptyCircularString_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyCompoundCurve()
    {
        return GEOSGeom_createEmptyCompoundCurve_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createEmptyCurvePolygon()
    {
        return GEOSGeom_createEmptyCurvePolygon_r(handle);
    }

    geos::geom::Geometry*
    GEOSGeom_createRectangle(double xmin, double ymin, double xmax,
                            double ymax)
    {
        return GEOSGeom_createRectangle_r(handle, xmin, ymin, xmax, ymax);
    }

    int
    GEOSOrientationIndex(double Ax, double Ay, double Bx, double By,
                         double Px, double Py)
    {
        return GEOSOrientationIndex_r(handle, Ax, Ay, Bx, By, Px, Py);
    }

    GEOSGeometry*
    GEOSSharedPaths(const GEOSGeometry* g1, const GEOSGeometry* g2)
    {
        return GEOSSharedPaths_r(handle, g1, g2);
    }

    GEOSGeometry*
    GEOSSnap(const GEOSGeometry* g1, const GEOSGeometry* g2, double tolerance)
    {
        return GEOSSnap_r(handle, g1, g2, tolerance);
    }

    GEOSBufferParams*
    GEOSBufferParams_create()
    {
        return GEOSBufferParams_create_r(handle);
    }

    void
    GEOSBufferParams_destroy(GEOSBufferParams* p)
    {
        return GEOSBufferParams_destroy_r(handle, p);
    }

    int
    GEOSBufferParams_setEndCapStyle(GEOSBufferParams* p, int style)
    {
        return GEOSBufferParams_setEndCapStyle_r(handle, p, style);
    }

    int
    GEOSBufferParams_setJoinStyle(GEOSBufferParams* p, int joinStyle)
    {
        return GEOSBufferParams_setJoinStyle_r(handle, p, joinStyle);
    }

    int
    GEOSBufferParams_setMitreLimit(GEOSBufferParams* p, double l)
    {
        return GEOSBufferParams_setMitreLimit_r(handle, p, l);
    }

    int
    GEOSBufferParams_setQuadrantSegments(GEOSBufferParams* p, int joinStyle)
    {
        return GEOSBufferParams_setQuadrantSegments_r(handle, p, joinStyle);
    }

    int
    GEOSBufferParams_setSingleSided(GEOSBufferParams* p, int singleSided)
    {
        return GEOSBufferParams_setSingleSided_r(handle, p, singleSided);
    }

    Geometry*
    GEOSBufferWithParams(const Geometry* g, const GEOSBufferParams* p, double w)
    {
        return GEOSBufferWithParams_r(handle, g, p, w);
    }

    Geometry*
    GEOSDelaunayTriangulation(const Geometry* g, double tolerance, int onlyEdges)
    {
        return GEOSDelaunayTriangulation_r(handle, g, tolerance, onlyEdges);
    }

    Geometry*
    GEOSConstrainedDelaunayTriangulation(const Geometry* g)
    {
        return GEOSConstrainedDelaunayTriangulation_r(handle, g);
    }

    Geometry*
    GEOSVoronoiDiagram(const Geometry* g, const Geometry* env, double tolerance, int flags)
    {
        return GEOSVoronoiDiagram_r(handle, g, env, tolerance, flags);
    }

    int
    GEOSSegmentIntersection(double ax0, double ay0, double ax1, double ay1,
                            double bx0, double by0, double bx1, double by1,
                            double* cx, double* cy)
    {
        return GEOSSegmentIntersection_r(handle,
                                         ax0, ay0, ax1, ay1,
                                         bx0, by0, bx1, by1,
                                         cx, cy);
    }

    int
    GEOSCoverageIsValid(
        const Geometry* input,
        double gapWidth,
        Geometry** invalidEdges)
    {
        return GEOSCoverageIsValid_r(handle, input, gapWidth, invalidEdges);
    }

    Geometry*
    GEOSCoverageSimplifyVW(const Geometry* input, double tolerance, int preserveBoundary)
    {
        return GEOSCoverageSimplifyVW_r(handle, input, tolerance, preserveBoundary);
    }


} /* extern "C" */
