/************************************************************************
 *
 *
 * C-Wrapper for GEOS library
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2010-2012 Sandro Santilli <strk@kbt.io>
 * Copyright (C) 2016-2021 Daniel Baston <dbaston@gmail.com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 * Author: Sandro Santilli <strk@kbt.io>
 * Thread Safety modifications: Chuck Thibert <charles.thibert@ingres.com>
 *
 ***********************************************************************/

#include <geos/algorithm/BoundaryNodeRule.h>
#include <geos/algorithm/MinimumBoundingCircle.h>
#include <geos/algorithm/MinimumDiameter.h>
#include <geos/algorithm/MinimumAreaRectangle.h>
#include <geos/algorithm/Orientation.h>
#include <geos/algorithm/construct/MaximumInscribedCircle.h>
#include <geos/algorithm/construct/LargestEmptyCircle.h>
#include <geos/algorithm/distance/DiscreteHausdorffDistance.h>
#include <geos/algorithm/distance/DiscreteFrechetDistance.h>
#include <geos/algorithm/hull/ConcaveHull.h>
#include <geos/algorithm/hull/ConcaveHullOfPolygons.h>
#include <geos/coverage/CoverageValidator.h>
#include <geos/coverage/CoverageSimplifier.h>
#include <geos/coverage/CoverageUnion.h>
#include <geos/geom/CircularString.h>
#include <geos/geom/CompoundCurve.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Curve.h>
#include <geos/geom/CurvePolygon.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/IntersectionMatrix.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineSegment.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiCurve.h>
#include <geos/geom/MultiLineString.h>
#include <geos/geom/MultiPoint.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/MultiSurface.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/SimpleCurve.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/geom/util/Densifier.h>
#include <geos/geom/util/GeometryFixer.h>
#include <geos/index/ItemVisitor.h>
#include <geos/index/strtree/TemplateSTRtree.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/io/GeoJSONReader.h>
#include <geos/io/GeoJSONWriter.h>
#include <geos/linearref/LengthIndexedLine.h>
#include <geos/noding/GeometryNoder.h>
#include <geos/noding/Noder.h>
#include <geos/operation/buffer/BufferBuilder.h>
#include <geos/operation/buffer/BufferOp.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/operation/buffer/OffsetCurve.h>
#include <geos/operation/cluster/DBSCANClusterFinder.h>
#include <geos/operation/cluster/EnvelopeDistanceClusterFinder.h>
#include <geos/operation/cluster/EnvelopeIntersectsClusterFinder.h>
#include <geos/operation/cluster/GeometryDistanceClusterFinder.h>
#include <geos/operation/cluster/GeometryIntersectsClusterFinder.h>
#include <geos/operation/distance/DistanceOp.h>
#include <geos/operation/distance/IndexedFacetDistance.h>
#include <geos/operation/linemerge/LineMerger.h>
#include <geos/operation/intersection/Rectangle.h>
#include <geos/operation/intersection/RectangleIntersection.h>
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/operation/overlayng/PrecisionReducer.h>
#include <geos/operation/overlayng/OverlayNG.h>
#include <geos/operation/overlayng/OverlayNGRobust.h>
#include <geos/operation/overlayng/UnaryUnionNG.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/polygonize/BuildArea.h>
#include <geos/operation/relate/RelateOp.h>
#include <geos/operation/sharedpaths/SharedPathsOp.h>
#include <geos/operation/union/CascadedPolygonUnion.h>
#include <geos/operation/union/DisjointSubsetUnion.h>
#include <geos/operation/valid/IsValidOp.h>
#include <geos/operation/valid/MakeValid.h>
#include <geos/operation/valid/RepeatedPointRemover.h>

#include <geos/operation/relateng/RelateNG.h>

#include <geos/precision/GeometryPrecisionReducer.h>
#include <geos/shape/fractal/HilbertEncoder.h>
#include <geos/simplify/DouglasPeuckerSimplifier.h>
#include <geos/simplify/PolygonHullSimplifier.h>
#include <geos/simplify/TopologyPreservingSimplifier.h>
#include <geos/triangulate/DelaunayTriangulationBuilder.h>
#include <geos/triangulate/VoronoiDiagramBuilder.h>
#include <geos/triangulate/polygon/ConstrainedDelaunayTriangulator.h>
#include <geos/util.h>
#include <geos/util/IllegalArgumentException.h>
#include <geos/util/Interrupt.h>
#include <geos/util/UniqueCoordinateArrayFilter.h>
#include <geos/util/Machine.h>
#include <geos/version.h>

// This should go away
#include <cmath> // finite
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

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
#define GEOSCoordSequence geos::geom::CoordinateSequence
#define GEOSClusterInfo geos::operation::cluster::Clusters
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

// Intentional, to allow non-standard C elements like C99 functions to be
// imported through C++ headers of C library, like <cmath>.
using namespace std;

/// Define this if you want operations triggering Exceptions to
/// be printed.
/// (will use the NOTIFY channel - only implemented for GEOSUnion so far)
///
#undef VERBOSE_EXCEPTIONS

#include <geos/export.h>
#include <geos/precision/MinimumClearance.h>


// import the most frequently used definitions globally
using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::CoordinateXYM;
using geos::geom::CoordinateXYZM;
using geos::geom::CoordinateSequence;
using geos::geom::Curve;
using geos::geom::Envelope;
using geos::geom::Geometry;
using geos::geom::GeometryCollection;
using geos::geom::GeometryFactory;
using geos::geom::LineString;
using geos::geom::LinearRing;
using geos::geom::MultiCurve;
using geos::geom::MultiLineString;
using geos::geom::MultiPolygon;
using geos::geom::Point;
using geos::geom::Polygon;
using geos::geom::PrecisionModel;
using geos::geom::SimpleCurve;
using geos::geom::Surface;

using geos::io::WKTReader;
using geos::io::WKTWriter;
using geos::io::WKBReader;
using geos::io::WKBWriter;
using geos::io::GeoJSONReader;
using geos::io::GeoJSONWriter;

using geos::algorithm::distance::DiscreteFrechetDistance;
using geos::algorithm::distance::DiscreteHausdorffDistance;
using geos::algorithm::hull::ConcaveHull;
using geos::algorithm::hull::ConcaveHullOfPolygons;

using geos::operation::buffer::BufferBuilder;
using geos::operation::buffer::BufferParameters;
using geos::operation::buffer::OffsetCurve;
using geos::operation::cluster::Clusters;
using geos::operation::distance::IndexedFacetDistance;
using geos::operation::geounion::CascadedPolygonUnion;
using geos::operation::overlayng::OverlayNG;
using geos::operation::overlayng::UnaryUnionNG;
using geos::operation::overlayng::OverlayNGRobust;
using geos::operation::relateng::RelateNG;
using geos::operation::valid::TopologyValidationError;

using geos::precision::GeometryPrecisionReducer;

using geos::simplify::PolygonHullSimplifier;

using geos::util::IllegalArgumentException;

typedef struct GEOSContextHandle_HS {
    const GeometryFactory* geomFactory;
    char msgBuffer[1024];
    GEOSMessageHandler noticeMessageOld;
    GEOSMessageHandler_r noticeMessageNew;
    void* noticeData;
    GEOSMessageHandler errorMessageOld;
    GEOSMessageHandler_r errorMessageNew;
    void* errorData;
    uint8_t WKBOutputDims;
    int WKBByteOrder;
    int initialized;
    std::unique_ptr<Point> point2d;

    GEOSContextHandle_HS()
        :
        geomFactory(nullptr),
        noticeMessageOld(nullptr),
        noticeMessageNew(nullptr),
        noticeData(nullptr),
        errorMessageOld(nullptr),
        errorMessageNew(nullptr),
        errorData(nullptr),
        point2d(nullptr)
    {
        memset(msgBuffer, 0, sizeof(msgBuffer));
        geomFactory = GeometryFactory::getDefaultInstance();
        point2d = geomFactory->createPoint(CoordinateXY{0, 0});
        WKBOutputDims = 2;
        WKBByteOrder = getMachineByteOrder();
        setNoticeHandler(nullptr);
        setErrorHandler(nullptr);
        initialized = 1;
    }

    GEOSMessageHandler
    setNoticeHandler(GEOSMessageHandler nf)
    {
        GEOSMessageHandler f = noticeMessageOld;
        noticeMessageOld = nf;
        noticeMessageNew = nullptr;
        noticeData = nullptr;

        return f;
    }

    GEOSMessageHandler
    setErrorHandler(GEOSMessageHandler nf)
    {
        GEOSMessageHandler f = errorMessageOld;
        errorMessageOld = nf;
        errorMessageNew = nullptr;
        errorData = nullptr;

        return f;
    }

    GEOSMessageHandler_r
    setNoticeHandler(GEOSMessageHandler_r nf, void* userData)
    {
        GEOSMessageHandler_r f = noticeMessageNew;
        noticeMessageOld = nullptr;
        noticeMessageNew = nf;
        noticeData = userData;

        return f;
    }

    GEOSMessageHandler_r
    setErrorHandler(GEOSMessageHandler_r ef, void* userData)
    {
        GEOSMessageHandler_r f = errorMessageNew;
        errorMessageOld = nullptr;
        errorMessageNew = ef;
        errorData = userData;

        return f;
    }

    void
    NOTICE_MESSAGE(GEOS_PRINTF_FORMAT const char *fmt, ...) GEOS_PRINTF_FORMAT_ATTR(2, 3)
    {
        if(nullptr == noticeMessageOld && nullptr == noticeMessageNew) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        #ifdef __MINGW32__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
        #endif
        int result = vsnprintf(msgBuffer, sizeof(msgBuffer) - 1, fmt, args);
        #ifdef __MINGW32__
        #pragma GCC diagnostic pop
        #endif
        va_end(args);

        if(result > 0) {
            if(noticeMessageOld) {
                noticeMessageOld("%s", msgBuffer);
            }
            else {
                noticeMessageNew(msgBuffer, noticeData);
            }
        }
    }

    void
    ERROR_MESSAGE(GEOS_PRINTF_FORMAT const char *fmt, ...) GEOS_PRINTF_FORMAT_ATTR(2, 3)
    {
        if(nullptr == errorMessageOld && nullptr == errorMessageNew) {
            return;
        }

        va_list args;
        va_start(args, fmt);
        #ifdef __MINGW32__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
        #endif
        int result = vsnprintf(msgBuffer, sizeof(msgBuffer) - 1, fmt, args);
        #ifdef __MINGW32__
        #pragma GCC diagnostic pop
        #endif
        va_end(args);

        if(result > 0) {
            if(errorMessageOld) {
                errorMessageOld("%s", msgBuffer);
            }
            else {
                errorMessageNew(msgBuffer, errorData);
            }
        }
    }
} GEOSContextHandleInternal_t;

// CAPI_ItemVisitor is used internally by the CAPI STRtree
// wrappers. It's defined here just to keep it out of the
// extern "C" block.
class CAPI_ItemVisitor : public geos::index::ItemVisitor {
    GEOSQueryCallback callback;
    void* userdata;
public:
    CAPI_ItemVisitor(GEOSQueryCallback cb, void* ud)
        : ItemVisitor(), callback(cb), userdata(ud) {}

    void operator()(void* item) {
        callback(item, userdata);
    }

    void
    visitItem(void* item) override
    {
        callback(item, userdata);
    }
};


//## PROTOTYPES #############################################

extern "C" const char GEOS_DLL* GEOSjtsport();
extern "C" char GEOS_DLL* GEOSasText(Geometry* g1);


namespace { // anonymous

char*
gstrdup_s(const char* str, const std::size_t size)
{
    char* out = static_cast<char*>(malloc(size + 1));
    if(nullptr != out) {
        // as no strlen call necessary, memcpy may be faster than strcpy
        std::memcpy(out, str, size + 1);
    }

    assert(nullptr != out);

    // we haven't been checking allocation before ticket #371
    if(nullptr == out) {
        throw(std::runtime_error("Failed to allocate memory for duplicate string"));
    }

    return out;
}

char*
gstrdup(std::string const& str)
{
    return gstrdup_s(str.c_str(), str.size());
}

} // namespace anonymous

// Execute a lambda, using the given context handle to process errors.
// Return errval on error.
// Errval should be of the type returned by f, unless f returns a bool in which case we promote to char.
template<typename F>
inline auto execute(
        GEOSContextHandle_t extHandle,
        typename std::conditional<std::is_same<decltype(std::declval<F>()()),bool>::value,
                                  char,
                                  decltype(std::declval<F>()())>::type errval,
        F&& f) -> decltype(errval) {
    if (extHandle == nullptr) {
        throw std::runtime_error("GEOS context handle is uninitialized, call initGEOS");
    }

    GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
    if (!handle->initialized) {
        return errval;
    }

    try {
        return f();
    } catch (const std::exception& e) {
        handle->ERROR_MESSAGE("%s", e.what());
    } catch (...) {
        handle->ERROR_MESSAGE("Unknown exception thrown");
    }

    return errval;
}

// Execute a lambda, using the given context handle to process errors.
// Return nullptr on error.
template<typename F, typename std::enable_if<!std::is_void<decltype(std::declval<F>()())>::value, std::nullptr_t>::type = nullptr>
inline auto execute(GEOSContextHandle_t extHandle, F&& f) -> decltype(f()) {
    if (extHandle == nullptr) {
        throw std::runtime_error("context handle is uninitialized, call initGEOS");
    }

    GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
    if (!handle->initialized) {
        return nullptr;
    }

    try {
        return f();
    } catch (const std::exception& e) {
        handle->ERROR_MESSAGE("%s", e.what());
    } catch (...) {
        handle->ERROR_MESSAGE("Unknown exception thrown");
    }

    return nullptr;
}

// Execute a lambda, using the given context handle to process errors.
// No return value.
template<typename F, typename std::enable_if<std::is_void<decltype(std::declval<F>()())>::value, std::nullptr_t>::type = nullptr>
inline void execute(GEOSContextHandle_t extHandle, F&& f) {
    GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
    try {
        f();
    } catch (const std::exception& e) {
        handle->ERROR_MESSAGE("%s", e.what());
    } catch (...) {
        handle->ERROR_MESSAGE("Unknown exception thrown");
    }
}

extern "C" {

    GEOSContextHandle_t
    initGEOS_r(GEOSMessageHandler nf, GEOSMessageHandler ef)
    {
        GEOSContextHandle_t handle = GEOS_init_r();

        if(nullptr != handle) {
            GEOSContext_setNoticeHandler_r(handle, nf);
            GEOSContext_setErrorHandler_r(handle, ef);
        }

        return handle;
    }

    GEOSContextHandle_t
    GEOS_init_r()
    {
        GEOSContextHandleInternal_t* handle = new GEOSContextHandleInternal_t();

        geos::util::Interrupt::cancel();

        return static_cast<GEOSContextHandle_t>(handle);
    }

    GEOSMessageHandler
    GEOSContext_setNoticeHandler_r(GEOSContextHandle_t extHandle, GEOSMessageHandler nf)
    {
        GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
        if(0 == handle->initialized) {
            return nullptr;
        }

        return handle->setNoticeHandler(nf);
    }

    GEOSMessageHandler
    GEOSContext_setErrorHandler_r(GEOSContextHandle_t extHandle, GEOSMessageHandler nf)
    {
        GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
        if(0 == handle->initialized) {
            return nullptr;
        }

        return handle->setErrorHandler(nf);
    }

    GEOSMessageHandler_r
    GEOSContext_setNoticeMessageHandler_r(GEOSContextHandle_t extHandle, GEOSMessageHandler_r nf, void* userData)
    {
        GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
        if(0 == handle->initialized) {
            return nullptr;
        }

        return handle->setNoticeHandler(nf, userData);
    }

    GEOSMessageHandler_r
    GEOSContext_setErrorMessageHandler_r(GEOSContextHandle_t extHandle, GEOSMessageHandler_r ef, void* userData)
    {
        GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
        if(0 == handle->initialized) {
            return nullptr;
        }

        return handle->setErrorHandler(ef, userData);
    }

    void
    finishGEOS_r(GEOSContextHandle_t extHandle)
    {
        // Fix up freeing handle w.r.t. malloc above
        delete extHandle;
        extHandle = nullptr;
    }

    void
    GEOS_finish_r(GEOSContextHandle_t extHandle)
    {
        finishGEOS_r(extHandle);
    }

    void
    GEOSFree_r(GEOSContextHandle_t extHandle, void* buffer)
    {
        assert(nullptr != extHandle);
        geos::ignore_unused_variable_warning(extHandle);

        free(buffer);
    }

//-----------------------------------------------------------
// relate()-related functions
//  return 0 = false, 1 = true, 2 = error occurred
//-----------------------------------------------------------

    char
    GEOSDisjoint_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->disjoint(g2);
        });
    }

    char
    GEOSTouches_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->touches(g2);
        });
    }

    char
    GEOSIntersects_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->intersects(g2);
        });
    }

    char
    GEOSCrosses_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->crosses(g2);
        });
    }

    char
    GEOSWithin_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->within(g2);
        });
    }

    char
    GEOSContains_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->contains(g2);
        });
    }

    char
    GEOSOverlaps_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->overlaps(g2);
        });
    }

    char
    GEOSCovers_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->covers(g2);
        });
    }

    char
    GEOSCoveredBy_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->coveredBy(g2);
        });
    }

    char
    GEOSEquals_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->equals(g2);
        });
    }


//-------------------------------------------------------------------
// low-level relate functions
//------------------------------------------------------------------

    char
    GEOSRelatePattern_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, const char* imPattern)
    {
        return execute(extHandle, 2, [&]() {
            std::string s(imPattern);
            return g1->relate(g2, s);
        });
    }

    char
    GEOSRelatePatternMatch_r(GEOSContextHandle_t extHandle, const char* intMatrix,
                             const char* imPattern)
    {
        return execute(extHandle, 2, [&]() {
            using geos::geom::IntersectionMatrix;

            std::string m(intMatrix);
            std::string p(imPattern);
            IntersectionMatrix im(m);

            return im.matches(p);
        });
    }

    char*
    GEOSRelate_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() {
            using geos::geom::IntersectionMatrix;

            auto im = g1->relate(g2);
            if(im == nullptr) {
                return (char*) nullptr;
            }

            return gstrdup(im->toString());
        });
    }

    char*
    GEOSRelateBoundaryNodeRule_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, int bnr)
    {
        using geos::operation::relate::RelateOp;
        using geos::geom::IntersectionMatrix;
        using geos::algorithm::BoundaryNodeRule;

        return execute(extHandle, [&]() -> char* {
            std::unique_ptr<IntersectionMatrix> im;

            switch (bnr) {
                case GEOSRELATE_BNR_MOD2: /* same as OGC */
                    im = RelateNG::relate(g1, g2,
                                          BoundaryNodeRule::getBoundaryRuleMod2());
                    break;
                case GEOSRELATE_BNR_ENDPOINT:
                    im = RelateNG::relate(g1, g2,
                                          BoundaryNodeRule::getBoundaryEndPoint());
                    break;
                case GEOSRELATE_BNR_MULTIVALENT_ENDPOINT:
                    im = RelateNG::relate(g1, g2,
                                          BoundaryNodeRule::getBoundaryMultivalentEndPoint());
                    break;
                case GEOSRELATE_BNR_MONOVALENT_ENDPOINT:
                    im = RelateNG::relate(g1, g2,
                                          BoundaryNodeRule::getBoundaryMonovalentEndPoint());
                    break;
                default:
                    std::ostringstream ss;
                    ss << "Invalid boundary node rule " << bnr;
                    throw std::runtime_error(ss.str());
            }

            if(!im) {
                return nullptr;
            }

            char* result = gstrdup(im->toString());

            return result;
        });
    }



//-----------------------------------------------------------------
// isValid
//-----------------------------------------------------------------


    char
    GEOSisValid_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, 2, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            using geos::operation::valid::IsValidOp;

            IsValidOp ivo(g1);
            const TopologyValidationError* err = ivo.getValidationError();

            if(err) {
                handle->NOTICE_MESSAGE("%s", err->toString().c_str());
                return false;
            }
            else {
                return true;
            }
        });
    }

    char*
    GEOSisValidReason_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            using geos::operation::valid::IsValidOp;

            char* result = nullptr;
            char const* const validstr = "Valid Geometry";

            IsValidOp ivo(g1);
            const TopologyValidationError* err = ivo.getValidationError();

            if(err) {
                std::ostringstream ss;
                ss.precision(15);
                ss << err->getCoordinate();
                const std::string errloc = ss.str();
                std::string errmsg(err->getMessage());
                errmsg += "[" + errloc + "]";
                result = gstrdup(errmsg);
            }
            else {
                result = gstrdup(std::string(validstr));
            }

            return result;
        });
    }

    char
    GEOSisValidDetail_r(GEOSContextHandle_t extHandle, const Geometry* g,
                        int flags, char** reason, Geometry** location)
    {
        using geos::operation::valid::IsValidOp;

        return execute(extHandle, 2, [&]() {
            IsValidOp ivo(g);
            if(flags & GEOSVALID_ALLOW_SELFTOUCHING_RING_FORMING_HOLE) {
                ivo.setSelfTouchingRingFormingHoleValid(true);
            }
            const TopologyValidationError* err = ivo.getValidationError();
            if(err != nullptr) {
                if(location) {
                    *location = g->getFactory()->createPoint(err->getCoordinate()).release();
                }
                if(reason) {
                    std::string errmsg(err->getMessage());
                    *reason = gstrdup(errmsg);
                }
                return false;
            }

            if(location) {
                *location = nullptr;
            }
            if(reason) {
                *reason = nullptr;
            }
            return true; /* valid */

        });
    }

//-----------------------------------------------------------------
// general purpose
//-----------------------------------------------------------------

    char
    GEOSEqualsExact_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double tolerance)
    {
        return execute(extHandle, 2, [&]() {
            return g1->equalsExact(g2, tolerance);
        });
    }

    char
    GEOSEqualsIdentical_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, 2, [&]() {
            return g1->equalsIdentical(g2);
        });
    }

    int
    GEOSDistance_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = g1->distance(g2);
            return 1;
        });
    }

    char
    GEOSDistanceWithin_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double dist)
    {
        return execute(extHandle, 2, [&]() {
            return g1->isWithinDistance(g2, dist);
        });
    }

    int
    GEOSDistanceIndexed_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = IndexedFacetDistance::distance(g1, g2);
            return 1;
        });
    }

    int
    GEOSHausdorffDistance_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = DiscreteHausdorffDistance::distance(*g1, *g2);
            return 1;
        });
    }

    int
    GEOSHausdorffDistanceDensify_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2,
                                   double densifyFrac, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = DiscreteHausdorffDistance::distance(*g1, *g2, densifyFrac);
            return 1;
        });
    }

    int
    GEOSFrechetDistance_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = DiscreteFrechetDistance::distance(*g1, *g2);
            return 1;
        });
    }

    int
    GEOSFrechetDistanceDensify_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double densifyFrac,
                                 double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = DiscreteFrechetDistance::distance(*g1, *g2, densifyFrac);
            return 1;
        });
    }

    int
    GEOSArea_r(GEOSContextHandle_t extHandle, const Geometry* g, double* area)
    {
        return execute(extHandle, 0, [&]() {
            *area = g->getArea();
            return 1;
        });
    }

    int
    GEOSLength_r(GEOSContextHandle_t extHandle, const Geometry* g, double* length)
    {
        return execute(extHandle, 0, [&]() {
            *length = g->getLength();
            return 1;
        });
    }

    CoordinateSequence*
    GEOSNearestPoints_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() -> CoordinateSequence* {
            if(g1->isEmpty() || g2->isEmpty()) {
                return nullptr;
            }
            return geos::operation::distance::DistanceOp::nearestPoints(g1, g2).release();
        });
    }

    static Clusters* capi_clusters(const Geometry* g,
                                   geos::operation::cluster::AbstractClusterFinder& finder)
    {
        std::vector<const Geometry*> input{g->getNumGeometries()};
        for (std::size_t i = 0; i < input.size(); i++) {
            input[i] = g->getGeometryN(i);
        }

        return new Clusters(finder.cluster(input));
    }

    Clusters*
    GEOSClusterDBSCAN_r(GEOSContextHandle_t extHandle, const Geometry* g, double eps, unsigned minPoints)
    {
        return execute(extHandle, [&]() {
            geos::operation::cluster::DBSCANClusterFinder finder(eps, minPoints);

            return capi_clusters(g, finder);
        });
    }

    Clusters*
    GEOSClusterGeometryIntersects_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            geos::operation::cluster::GeometryIntersectsClusterFinder finder;
            return capi_clusters(g, finder);
        });
    }

    Clusters*
    GEOSClusterEnvelopeIntersects_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            geos::operation::cluster::EnvelopeIntersectsClusterFinder finder;
            return capi_clusters(g, finder);
        });
    }

    Clusters*
    GEOSClusterEnvelopeDistance_r(GEOSContextHandle_t extHandle, const Geometry* g, double d)
    {
        return execute(extHandle, [&]() {
            geos::operation::cluster::EnvelopeDistanceClusterFinder finder(d);
            return capi_clusters(g, finder);
        });
    }

    Clusters*
    GEOSClusterGeometryDistance_r(GEOSContextHandle_t extHandle, const Geometry* g, double d)
    {
        return execute(extHandle, [&]() {
            geos::operation::cluster::GeometryDistanceClusterFinder finder(d);
            return capi_clusters(g, finder);
        });
    }

    std::size_t GEOSClusterInfo_getNumClusters_r(GEOSContextHandle_t extHandle, const Clusters* clusters)
    {
        return execute(extHandle, 0, [&]() -> std::size_t {
            return static_cast<unsigned>(clusters->getNumClusters());
        });
    }

    std::size_t GEOSClusterInfo_getClusterSize_r(GEOSContextHandle_t extHandle, const Clusters* clusters, std::size_t i)
    {
        return execute(extHandle, 0, [&]() {
            return static_cast<unsigned>(clusters->getSize(i));
        });
    }

    const std::size_t* GEOSClusterInfo_getInputsForClusterN_r(GEOSContextHandle_t extHandle, const Clusters* clusters, std::size_t i)
    {
        return execute(extHandle, [&]() {
            return &*clusters->begin(i);
        });
    }

    std::size_t* GEOSClusterInfo_getClustersForInputs_r(GEOSContextHandle_t extHandle, const Clusters* clusters)
    {
        return execute(extHandle, [&]() {
            auto ids = clusters->getClusterIds(GEOS_CLUSTER_NONE);
            std::size_t* ids_buf = (size_t*) malloc(ids.size() * sizeof(std::size_t));
            std::copy(ids.begin(), ids.end(), ids_buf);
            return ids_buf;
        });
    }

    void GEOSClusterInfo_destroy_r(GEOSContextHandle_t, Clusters* clusters)
    {
        delete clusters;
    }

    Geometry*
    GEOSGeomFromWKT_r(GEOSContextHandle_t extHandle, const char* wkt)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            const std::string wktstring(wkt);
            WKTReader r(static_cast<GeometryFactory const*>(handle->geomFactory));

            auto g = r.read(wktstring);
            return g.release();
        });
    }

    char*
    GEOSGeomToWKT_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            // Deprecated, show untrimmed 2D output
            geos::io::WKTWriter writer;
            writer.setTrim(false);
            writer.setOutputDimension(2);
            char* result = gstrdup(writer.write(g1));
            return result;
        });
    }

    // Remember to free the result!
    unsigned char*
    GEOSGeomToWKB_buf_r(GEOSContextHandle_t extHandle, const Geometry* g, std::size_t* size)
    {
        using geos::io::WKBWriter;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            int byteOrder = handle->WKBByteOrder;
            WKBWriter w(handle->WKBOutputDims, byteOrder);
            std::ostringstream os(std::ios_base::binary);
            w.write(*g, os);
            std::string wkbstring(os.str());
            const std::size_t len = wkbstring.length();

            unsigned char* result = static_cast<unsigned char*>(malloc(len));
            if(result) {
                std::memcpy(result, wkbstring.c_str(), len);
                *size = len;
            }
            return result;
        });
    }

    Geometry*
    GEOSGeomFromWKB_buf_r(GEOSContextHandle_t extHandle, const unsigned char* wkb, std::size_t size)
    {
        using geos::io::WKBReader;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            WKBReader r(*(static_cast<GeometryFactory const*>(handle->geomFactory)));
            return r.read(wkb, size).release();
        });
    }

    /* Read/write wkb hex values.  Returned geometries are
       owned by the caller.*/
    unsigned char*
    GEOSGeomToHEX_buf_r(GEOSContextHandle_t extHandle, const Geometry* g, std::size_t* size)
    {
        using geos::io::WKBWriter;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            int byteOrder = handle->WKBByteOrder;
            WKBWriter w(handle->WKBOutputDims, byteOrder);
            std::ostringstream os(std::ios_base::binary);
            w.writeHEX(*g, os);
            std::string hexstring(os.str());

            char* result = gstrdup(hexstring);
            if(result) {
                *size = hexstring.length();
            }

            return reinterpret_cast<unsigned char*>(result);
        });
    }

    Geometry*
    GEOSGeomFromHEX_buf_r(GEOSContextHandle_t extHandle, const unsigned char* hex, std::size_t size)
    {
        using geos::io::WKBReader;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            std::string hexstring(reinterpret_cast<const char*>(hex), size);
            WKBReader r(*(static_cast<GeometryFactory const*>(handle->geomFactory)));
            std::istringstream is(std::ios_base::binary);
            is.str(hexstring);
            is.seekg(0, std::ios::beg); // rewind reader pointer

            auto g = r.readHEX(is);
            return g.release();
        });
    }

    char
    GEOSisEmpty_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, 2, [&]() {
            return g1->isEmpty();
        });
    }

    char
    GEOSisSimple_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, 2, [&]() {
            return g1->isSimple();
        });
    }

    char
    GEOSisRing_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            const Curve* ls = dynamic_cast<const Curve*>(g);
            if(ls) {
                return ls->isRing();
            }
            else {
                return false;
            }
        });
    }

    //free the result of this
    char*
    GEOSGeomType_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            std::string s = g1->getGeometryType();

            char* result = gstrdup(s);
            return result;
        });
    }

    // Return postgis geometry type index
    int
    GEOSGeomTypeId_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, -1, [&]() {
            return static_cast<int>(g1->getGeometryTypeId());
        });
    }

//-------------------------------------------------------------------
// GEOS functions that return geometries
//-------------------------------------------------------------------

    Geometry*
    GEOSEnvelope_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            Geometry* g3 = g1->getEnvelope().release();
            g3->setSRID(g1->getSRID());
            return g3;
        });
    }

    Geometry*
    GEOSIntersection_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->intersection(g2);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSIntersectionPrec_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return execute(extHandle, [&]() {
            std::unique_ptr<PrecisionModel> pm;
            if(gridSize != 0) {
                pm.reset(new PrecisionModel(1.0 / gridSize));
            }
            else {
                pm.reset(new PrecisionModel());
            }
            auto g3 = gridSize != 0 ?
              OverlayNG::overlay(g1, g2, OverlayNG::INTERSECTION, pm.get())
              :
              OverlayNGRobust::Overlay(g1, g2, OverlayNG::INTERSECTION);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSBuffer_r(GEOSContextHandle_t extHandle, const Geometry* g1, double width, int quadrantsegments)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->buffer(width, quadrantsegments);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSBufferWithStyle_r(GEOSContextHandle_t extHandle, const Geometry* g1, double width, int quadsegs, int endCapStyle,
                          int joinStyle, double mitreLimit)
    {
        using geos::operation::buffer::BufferParameters;
        using geos::operation::buffer::BufferOp;
        using geos::util::IllegalArgumentException;

        return execute(extHandle, [&]() {
            BufferParameters bp;
            bp.setQuadrantSegments(quadsegs);

            if(endCapStyle > BufferParameters::CAP_SQUARE) {
                throw IllegalArgumentException("Invalid buffer endCap style");
            }
            bp.setEndCapStyle(
                static_cast<BufferParameters::EndCapStyle>(endCapStyle)
            );

            if(joinStyle > BufferParameters::JOIN_BEVEL) {
                throw IllegalArgumentException("Invalid buffer join style");
            }
            bp.setJoinStyle(
                static_cast<BufferParameters::JoinStyle>(joinStyle)
            );
            bp.setMitreLimit(mitreLimit);
            BufferOp op(g1, bp);
            std::unique_ptr<Geometry> g3 = op.getResultGeometry(width);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSDensify_r(GEOSContextHandle_t extHandle, const Geometry* g, double tolerance)
    {
        using geos::geom::util::Densifier;

        return execute(extHandle, [&]() {
            Densifier densifier(g);
            densifier.setDistanceTolerance(tolerance);
            auto g3 = densifier.getResultGeometry();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSOffsetCurve_r(GEOSContextHandle_t extHandle, const Geometry* g1, double width, int quadsegs, int joinStyle,
                      double mitreLimit)
    {
        return execute(extHandle, [&]() {
            BufferParameters bp;
            //-- use default cap style ROUND
            bp.setQuadrantSegments(quadsegs);

            if(joinStyle > BufferParameters::JOIN_BEVEL) {
                throw IllegalArgumentException("Invalid buffer join style");
            }
            bp.setJoinStyle(
                static_cast<BufferParameters::JoinStyle>(joinStyle)
            );
            bp.setMitreLimit(mitreLimit);

            OffsetCurve oc(*g1, width, bp);
            std::unique_ptr<Geometry> g3 = oc.getCurve();
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    /* @deprecated in 3.3.0 */
    Geometry*
    GEOSSingleSidedBuffer_r(GEOSContextHandle_t extHandle, const Geometry* g1, double width, int quadsegs, int joinStyle,
                            double mitreLimit, int leftSide)
    {
        return execute(extHandle, [&]() {
            BufferParameters bp;
            bp.setEndCapStyle(BufferParameters::CAP_FLAT);
            bp.setQuadrantSegments(quadsegs);

            if(joinStyle > BufferParameters::JOIN_BEVEL) {
                throw IllegalArgumentException("Invalid buffer join style");
            }
            bp.setJoinStyle(
                static_cast<BufferParameters::JoinStyle>(joinStyle)
            );
            bp.setMitreLimit(mitreLimit);

            bool isLeftSide = leftSide == 0 ? false : true;
            BufferBuilder bufBuilder(bp);
            std::unique_ptr<Geometry> g3 = bufBuilder.bufferLineSingleSided(g1, width, isLeftSide);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSConvexHull_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->convexHull();
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSConcaveHull_r(GEOSContextHandle_t extHandle,
        const Geometry* g1,
        double ratio,
        unsigned int allowHoles)
    {
        return execute(extHandle, [&]() {
            ConcaveHull hull(g1);
            hull.setMaximumEdgeLengthRatio(ratio);
            hull.setHolesAllowed(allowHoles);
            std::unique_ptr<Geometry> g3 = hull.getHull();
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSConcaveHullByLength_r(GEOSContextHandle_t extHandle,
        const Geometry* g1,
        double length,
        unsigned int allowHoles)
    {
        return execute(extHandle, [&]() {
            ConcaveHull hull(g1);
            hull.setMaximumEdgeLength(length);
            hull.setHolesAllowed(allowHoles);
            std::unique_ptr<Geometry> g3 = hull.getHull();
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSPolygonHullSimplify_r(GEOSContextHandle_t extHandle,
        const Geometry* g1,
        unsigned int isOuter,
        double vertexNumFraction)
    {
        return execute(extHandle, [&]() {
            std::unique_ptr<Geometry> g3 = PolygonHullSimplifier::hull(g1, isOuter, vertexNumFraction);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSPolygonHullSimplifyMode_r(GEOSContextHandle_t extHandle,
        const Geometry* g1,
        unsigned int isOuter,
        unsigned int parameterMode,
        double parameter)
    {
        return execute(extHandle, [&]() {
            if (parameterMode == GEOSHULL_PARAM_AREA_RATIO) {
                std::unique_ptr<Geometry> g3 = PolygonHullSimplifier::hullByAreaDelta(g1, isOuter, parameter);
                g3->setSRID(g1->getSRID());
                return g3.release();
            }
            else if (parameterMode == GEOSHULL_PARAM_VERTEX_RATIO) {
                std::unique_ptr<Geometry> g3 = PolygonHullSimplifier::hull(g1, isOuter, parameter);
                g3->setSRID(g1->getSRID());
                return g3.release();
            }
            else {
                throw IllegalArgumentException("GEOSPolygonHullSimplifyMode_r: Unknown parameterMode");
            }
        });
    }

    Geometry*
    GEOSConcaveHullOfPolygons_r(GEOSContextHandle_t extHandle,
        const Geometry* g1,
        double lengthRatio,
        unsigned int isTight,
        unsigned int isHolesAllowed)
    {
        return execute(extHandle, [&]() {
            std::unique_ptr<Geometry> g3 =
                ConcaveHullOfPolygons::concaveHullByLengthRatio(
                    g1, lengthRatio,
                    isTight > 0,
                    isHolesAllowed > 0);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSMinimumRotatedRectangle_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        using geos::algorithm::MinimumAreaRectangle;

        return execute(extHandle, [&]() {
            auto g3 = MinimumAreaRectangle::getMinimumRectangle(g);
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSMaximumInscribedCircle_r(GEOSContextHandle_t extHandle, const Geometry* g, double tolerance)
    {
        return execute(extHandle, [&]() {
            geos::algorithm::construct::MaximumInscribedCircle mic(g, tolerance);
            auto g3 = mic.getRadiusLine();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSLargestEmptyCircle_r(GEOSContextHandle_t extHandle, const Geometry* g, const GEOSGeometry* boundary, double tolerance)
    {
        return execute(extHandle, [&]() {
            geos::algorithm::construct::LargestEmptyCircle lec(g, boundary, tolerance);
            auto g3 = lec.getRadiusLine();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSMinimumWidth_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            geos::algorithm::MinimumDiameter m(g);
            auto g3 = m.getDiameter();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSMinimumClearanceLine_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            geos::precision::MinimumClearance mc(g);
            auto g3 = mc.getLine();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    int
    GEOSMinimumClearance_r(GEOSContextHandle_t extHandle, const Geometry* g, double* d)
    {
        return execute(extHandle, 2, [&]() {
            geos::precision::MinimumClearance mc(g);
            double res = mc.getDistance();
            *d = res;
            return 0;
        });
    }


    Geometry*
    GEOSDifference_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->difference(g2);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSDifferencePrec_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return execute(extHandle, [&]() {

            std::unique_ptr<PrecisionModel> pm;
            if(gridSize != 0) {
                pm.reset(new PrecisionModel(1.0 / gridSize));
            }
            else {
                pm.reset(new PrecisionModel());
            }
            auto g3 = gridSize != 0 ?
                OverlayNG::overlay(g1, g2, OverlayNG::DIFFERENCE, pm.get())
                :
                OverlayNGRobust::Overlay(g1, g2, OverlayNG::DIFFERENCE);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSBoundary_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->getBoundary();
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSSymDifference_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->symDifference(g2);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSSymDifferencePrec_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return execute(extHandle, [&]() {

            std::unique_ptr<PrecisionModel> pm;
            if(gridSize != 0) {
                pm.reset(new PrecisionModel(1.0 / gridSize));
            }
            else {
                pm.reset(new PrecisionModel());
            }
            auto g3 = gridSize != 0 ?
              OverlayNG::overlay(g1, g2, OverlayNG::SYMDIFFERENCE, pm.get())
              :
              OverlayNGRobust::Overlay(g1, g2, OverlayNG::SYMDIFFERENCE);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSUnion_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2)
    {
        return execute(extHandle, [&]() {
            auto g3 = g1->Union(g2);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSUnionPrec_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* g2, double gridSize)
    {
        return execute(extHandle, [&]() {

            std::unique_ptr<PrecisionModel> pm;
            if(gridSize != 0) {
                pm.reset(new PrecisionModel(1.0 / gridSize));
            }
            else {
                pm.reset(new PrecisionModel());
            }
            auto g3 = gridSize != 0 ?
              OverlayNG::overlay(g1, g2, OverlayNG::UNION, pm.get())
              :
              OverlayNGRobust::Overlay(g1, g2, OverlayNG::UNION);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSCoverageUnion_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            auto g3 = geos::coverage::CoverageUnion::Union(g);
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSDisjointSubsetUnion_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            auto g3 = geos::operation::geounion::DisjointSubsetUnion::Union(g);
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSUnaryUnion_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            std::unique_ptr<Geometry> g3(g->Union());
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSUnaryUnionPrec_r(GEOSContextHandle_t extHandle, const Geometry* g1, double gridSize)
    {
        return execute(extHandle, [&]() {

            std::unique_ptr<PrecisionModel> pm;
            if(gridSize != 0) {
                pm.reset(new PrecisionModel(1.0 / gridSize));
            }
            else {
                pm.reset(new PrecisionModel());
            }
            auto g3 = gridSize != 0 ?
              UnaryUnionNG::Union(g1, *pm)
              :
              OverlayNGRobust::Union(g1);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSNode_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            auto g3 = geos::noding::GeometryNoder::node(*g);
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSUnionCascaded_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            // CascadedUnion is the same as UnaryUnion, except that
            // CascadedUnion only works on MultiPolygon, so we just delegate
            // now and retain a check on MultiPolygon type.
            const MultiPolygon *p = dynamic_cast<const MultiPolygon *>(g1);
            if (!p) {
                throw IllegalArgumentException("Invalid argument (must be a MultiPolygon)");
            }
            return GEOSUnaryUnion_r(extHandle, g1);
        });
    }

    Geometry*
    GEOSPointOnSurface_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            auto ret = g1->getInteriorPoint();
            ret->setSRID(g1->getSRID());
            return ret.release();
        });
    }

    Geometry*
    GEOSClipByRect_r(GEOSContextHandle_t extHandle, const Geometry* g, double xmin, double ymin, double xmax, double ymax)
    {
        return execute(extHandle, [&]() {
            using geos::operation::intersection::Rectangle;
            using geos::operation::intersection::RectangleIntersection;
            Rectangle rect(xmin, ymin, xmax, ymax);
            std::unique_ptr<Geometry> g3 = RectangleIntersection::clip(*g, rect);
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSGeom_transformXY_r(GEOSContextHandle_t handle, const GEOSGeometry* g, GEOSTransformXYCallback callback, void* userdata) {

        struct TransformFilter final: public geos::geom::CoordinateFilter {
            TransformFilter(GEOSTransformXYCallback p_callback,
                            void* p_userdata) :
                            m_callback(p_callback),
                            m_userdata(p_userdata) {}

            void filter_rw(CoordinateXY* c) const override {
                if (!m_callback(&(c->x), &(c->y), m_userdata)) {
                    throw std::runtime_error(std::string("Failed to transform coordinates."));
                }
            }

            GEOSTransformXYCallback m_callback;
            void* m_userdata;
        };

        return execute(handle, [&]() {
            TransformFilter filter(callback, userdata);
            auto ret = g->clone();
            ret->apply_rw(&filter);
            ret->geometryChanged();
            return ret.release();
        });
    }


    Geometry*
    GEOSGeom_transformXYZ_r(GEOSContextHandle_t handle, const GEOSGeometry* g, GEOSTransformXYZCallback callback, void* userdata) {

        struct TransformFilter final: public geos::geom::CoordinateFilter {
            TransformFilter(GEOSTransformXYZCallback p_callback,
                            void* p_userdata) :
                            m_callback(p_callback),
                            m_userdata(p_userdata) {}

            void filter_rw(Coordinate* c) const override {
                if (!m_callback(&(c->x), &(c->y), &(c->z), m_userdata)) {
                    throw std::runtime_error(std::string("Failed to transform coordinates."));
                }
            }

            GEOSTransformXYZCallback m_callback;
            void* m_userdata;
        };

        return execute(handle, [&]() {
            TransformFilter filter(callback, userdata);
            auto ret = g->clone();
            ret->apply_rw(&filter);
            ret->geometryChanged();
            return ret.release();
        });
    }


//-------------------------------------------------------------------
// memory management functions
//------------------------------------------------------------------

    void
    GEOSGeom_destroy_r(GEOSContextHandle_t extHandle, Geometry* a)
    {
        execute(extHandle, [&]() {
            // FIXME: mloskot: Does this try-catch around delete means that
            // destructors in GEOS may throw? If it does, this is a serious
            // violation of "never throw an exception from a destructor" principle
            delete a;
        });
    }

    void
    GEOSGeom_setUserData_r(GEOSContextHandle_t extHandle, Geometry* g, void* userData)
    {
        execute(extHandle, [&]() {
            g->setUserData(userData);
        });
    }

    void
    GEOSSetSRID_r(GEOSContextHandle_t extHandle, Geometry* g, int srid)
    {
        execute(extHandle, [&]() {
            g->setSRID(srid);
        });
    }


    int
    GEOSGetNumCoordinates_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, -1, [&]() {
            return static_cast<int>(g->getNumPoints());
        });
    }

    /*
     * Return -1 on exception, 0 otherwise.
     * Converts Geometry to normal form (or canonical form).
     */
    int
    GEOSNormalize_r(GEOSContextHandle_t extHandle, Geometry* g)
    {
        return execute(extHandle, -1, [&]() {
            g->normalize();
            return 0; // SUCCESS
        });
    }

    int
    GEOSOrientPolygons_r(GEOSContextHandle_t extHandle, Geometry* g, int exteriorCW)
    {
        return execute(extHandle, -1, [&]() {
            class OrientPolygons : public geos::geom::GeometryComponentFilter {
            public:
                OrientPolygons(bool isExteriorCW) : exteriorCW(isExteriorCW) {}

                void filter_rw(Geometry* g) override {
                    if (g->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_POLYGON) {
                        auto p = geos::detail::down_cast<Polygon*>(g);
                        p->orientRings(exteriorCW);
                    } else if (g->getGeometryTypeId() == geos::geom::GeometryTypeId::GEOS_CURVEPOLYGON) {
                        throw geos::util::UnsupportedOperationException("Curved geometries not supported.");
                    }
                }

            private:
                bool exteriorCW;
            };

            OrientPolygons op(exteriorCW);
            g->apply_rw(&op);

            return 0;
        });
    }

    int
    GEOSGetNumInteriorRings_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, -1, [&]() {
            const Surface* p = dynamic_cast<const Surface*>(g1);
            if(!p) {
                throw IllegalArgumentException("Argument is not a Surface");
            }
            return static_cast<int>(p->getNumInteriorRing());
        });
    }


    // returns -1 on error and 1 for non-multi geometries
    int
    GEOSGetNumGeometries_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, -1, [&]() {
            return static_cast<int>(g1->getNumGeometries());
        });
    }


    /*
     * Call only on GEOMETRYCOLLECTION or MULTI*.
     * Return a pointer to the internal Geometry.
     */
    const Geometry*
    GEOSGetGeometryN_r(GEOSContextHandle_t extHandle, const Geometry* g1, int n)
    {
        return execute(extHandle, [&]() {
            if(n < 0) {
                throw IllegalArgumentException("Index must be non-negative.");
            }
            return g1->getGeometryN(static_cast<size_t>(n));
        });
    }

    /*
     * Call only on LINESTRING
     * Returns NULL on exception
     */
    Geometry*
    GEOSGeomGetPointN_r(GEOSContextHandle_t extHandle, const Geometry* g1, int n)
    {
        return execute(extHandle, [&]() {
            const LineString* ls = dynamic_cast<const LineString*>(g1);
            if(!ls) {
                throw IllegalArgumentException("Argument is not a LineString");
            }
            if(n < 0) {
                throw IllegalArgumentException("Index must be non-negative.");
            }
            return ls->getPointN(static_cast<size_t>(n)).release();
        });
    }

    /*
     * Call only on LINESTRING
     */
    Geometry*
    GEOSGeomGetStartPoint_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            const LineString* ls = dynamic_cast<const LineString*>(g1);
            if(!ls) {
                throw IllegalArgumentException("Argument is not a LineString");
            }

            return ls->getStartPoint().release();
        });
    }

    /*
     * Call only on LINESTRING
     */
    Geometry*
    GEOSGeomGetEndPoint_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            const LineString* ls = dynamic_cast<const LineString*>(g1);
            if(!ls) {
                throw IllegalArgumentException("Argument is not a LineString");
            }
            return ls->getEndPoint().release();
        });
    }

    /*
     * Call only on LINESTRING or MULTILINESTRING
     * return 2 on exception, 1 on true, 0 on false
     */
    char
    GEOSisClosed_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, 2, [&]() {
            const Curve* ls = dynamic_cast<const Curve*>(g1);
            if(ls) {
                return ls->isClosed();
            }

            const MultiLineString* mls = dynamic_cast<const MultiLineString*>(g1);
            if(mls) {
                return mls->isClosed();
            }

            const MultiCurve* mc = dynamic_cast<const MultiCurve*>(g1);
            if(mc) {
                return mc->isClosed();
            }

            throw IllegalArgumentException("Argument is not a Curve, MultiLineString, or MultiCurve");
        });
    }

    /*
     * Call only on LINESTRING
     * return 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetLength_r(GEOSContextHandle_t extHandle, const Geometry* g1, double* length)
    {
        return execute(extHandle, 0, [&]() {
            const LineString* ls = dynamic_cast<const LineString*>(g1);
            if(!ls) {
                throw IllegalArgumentException("Argument is not a LineString");
            }
            *length = ls->getLength();
            return 1;
        });
    }

    /*
     * Call only on LINESTRING
     */
    int
    GEOSGeomGetNumPoints_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, -1, [&]() {
            const SimpleCurve* ls = dynamic_cast<const SimpleCurve*>(g1);
            if(!ls) {
                throw IllegalArgumentException("Argument is not a SimpleCurve");
            }
            return static_cast<int>(ls->getNumPoints());
        });
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetX_r(GEOSContextHandle_t extHandle, const Geometry* g1, double* x)
    {
        return execute(extHandle, 0, [&]() {
            const Point* po = dynamic_cast<const Point*>(g1);
            if(!po) {
                throw IllegalArgumentException("Argument is not a Point");
            }
            *x = po->getX();
            return 1;
        });
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetY_r(GEOSContextHandle_t extHandle, const Geometry* g1, double* y)
    {
        return execute(extHandle, 0, [&]() {
            const Point* po = dynamic_cast<const Point*>(g1);
            if(!po) {
                throw IllegalArgumentException("Argument is not a Point");
            }
            *y = po->getY();
            return 1;
        });
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetZ_r(GEOSContextHandle_t extHandle, const Geometry* g1, double* z)
    {
        return execute(extHandle, 0, [&]() {
            const Point* po = dynamic_cast<const Point*>(g1);
            if(!po) {
                throw IllegalArgumentException("Argument is not a Point");
            }
            *z = po->getZ();
            return 1;
        });
    }

    /*
     * For POINT
     * returns 0 on exception, otherwise 1
     */
    int
    GEOSGeomGetM_r(GEOSContextHandle_t extHandle, const Geometry* g1, double* m)
    {
        using geos::geom::Point;

        return execute(extHandle, 0, [&]() {
            const Point* po = dynamic_cast<const Point*>(g1);
            if(!po) {
                throw IllegalArgumentException("Argument is not a Point");
            }
            *m = po->getM();
            return 1;
        });
    }

    /*
     * Call only on polygon
     * Return a pointer to the internal Geometry.
     */
    const Geometry*
    GEOSGetExteriorRing_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        return execute(extHandle, [&]() {
            const Surface* p = dynamic_cast<const Surface*>(g1);
            if(!p) {
                throw IllegalArgumentException("Invalid argument (must be a Surface)");
            }
            return p->getExteriorRing();
        });
    }

    /*
     * Call only on polygon
     * Return a pointer to internal storage, do not destroy it.
     */
    const Geometry*
    GEOSGetInteriorRingN_r(GEOSContextHandle_t extHandle, const Geometry* g1, int n)
    {
        return execute(extHandle, [&]() {
            const Surface* p = dynamic_cast<const Surface*>(g1);
            if(!p) {
                throw IllegalArgumentException("Invalid argument (must be a Surface)");
            }
            if(n < 0) {
                throw IllegalArgumentException("Index must be non-negative.");
            }
            return p->getInteriorRingN(static_cast<size_t>(n));
        });
    }

    Geometry*
    GEOSGetCentroid_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() -> Geometry* {
            auto ret = g->getCentroid();
            ret->setSRID(g->getSRID());
            return ret.release();
        });
    }

    int
    GEOSHilbertCode_r(GEOSContextHandle_t extHandle, const GEOSGeometry *geom,
                const GEOSGeometry* extent, unsigned int level,
                unsigned int *code)
    {
        using geos::shape::fractal::HilbertEncoder;

        return execute(extHandle, 0, [&]() {
            Envelope e = *extent->getEnvelopeInternal();
            HilbertEncoder encoder(level, e);
            *code = encoder.encode(geom->getEnvelopeInternal());
            return 1;
        });
    }

    Geometry*
    GEOSMinimumBoundingCircle_r(GEOSContextHandle_t extHandle, const Geometry* g,
        double* radius, Geometry** center)
    {
        return execute(extHandle, [&]() -> Geometry* {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            geos::algorithm::MinimumBoundingCircle mc(g);
            std::unique_ptr<Geometry> ret = mc.getCircle();
            const GeometryFactory* gf = handle->geomFactory;
            if (center) *center = gf->createPoint(mc.getCentre()).release();
            if (radius) *radius = mc.getRadius();
            ret->setSRID(g->getSRID());
            return ret.release();
        });
    }

    Geometry*
    GEOSGeom_createEmptyCollection_r(GEOSContextHandle_t extHandle, int type)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            std::unique_ptr<Geometry> g = 0;
            switch(type) {
            case GEOS_GEOMETRYCOLLECTION:
                g = gf->createGeometryCollection();
                break;
            case GEOS_MULTIPOINT:
                g = gf->createMultiPoint();
                break;
            case GEOS_MULTILINESTRING:
                g = gf->createMultiLineString();
                break;
            case GEOS_MULTIPOLYGON:
                g = gf->createMultiPolygon();
                break;
            default:
                throw IllegalArgumentException("Unsupported type request for GEOSGeom_createEmptyCollection_r");
            }

            return g.release();
        });
    }

    Geometry*
    GEOSGeom_createCollection_r(GEOSContextHandle_t extHandle, int type, Geometry** geoms, unsigned int ngeoms)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            const GeometryFactory* gf = handle->geomFactory;

            std::vector<std::unique_ptr<Geometry>> vgeoms(ngeoms);
            for (std::size_t i = 0; i < ngeoms; i++) {
                vgeoms[i].reset(geoms[i]);
            }

            std::unique_ptr<Geometry> g;
            switch(type) {
            case GEOS_GEOMETRYCOLLECTION:
                g = gf->createGeometryCollection(std::move(vgeoms));
                break;
            case GEOS_MULTIPOINT:
                g = gf->createMultiPoint(std::move(vgeoms));
                break;
            case GEOS_MULTILINESTRING:
                g = gf->createMultiLineString(std::move(vgeoms));
                break;
            case GEOS_MULTIPOLYGON:
                g = gf->createMultiPolygon(std::move(vgeoms));
                break;
            case GEOS_MULTICURVE:
                g = gf->createMultiCurve(std::move(vgeoms));
                break;
            case GEOS_MULTISURFACE:
                g = gf->createMultiSurface(std::move(vgeoms));
                break;
            default:
                handle->ERROR_MESSAGE("Unsupported type request for GEOSGeom_createCollection_r");
            }

            return g.release();
        });
    }

    Geometry**
    GEOSGeom_releaseCollection_r(GEOSContextHandle_t extHandle, Geometry* collection, unsigned int * ngeoms)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            if (ngeoms == nullptr) {
                handle->ERROR_MESSAGE("Parameter ngeoms of GEOSGeom_releaseCollection_r must not be null");
            }

            GeometryCollection *col = dynamic_cast<GeometryCollection*>(collection);
            if (!col) {
                handle->ERROR_MESSAGE("Parameter collection of GEOSGeom_releaseCollection_r must not be a collection");
            } else {
                *ngeoms = static_cast<unsigned int>(col->getNumGeometries());
            }

            // Early exit on empty/null input
            if (!col || *ngeoms == 0) {
                return static_cast<Geometry**>(nullptr);
            }

            std::vector<std::unique_ptr<Geometry>> subgeoms = col->releaseGeometries();

            Geometry** subgeomArray = static_cast<Geometry**>(malloc(sizeof(Geometry*) * subgeoms.size()));
            for (std::size_t i = 0; i < subgeoms.size(); i++) {
                subgeomArray[i] = subgeoms[i].release();
            }

            return subgeomArray;
        });
    }

    Geometry*
    GEOSPolygonize_r(GEOSContextHandle_t extHandle, const Geometry* const* g, unsigned int ngeoms)
    {
        using geos::operation::polygonize::Polygonizer;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            // Polygonize
            Polygonizer plgnzr;
            for(std::size_t i = 0; i < ngeoms; ++i) {
                plgnzr.add(g[i]);
            }

            auto polys = plgnzr.getPolygons();
            const GeometryFactory* gf = handle->geomFactory;
            return gf->createGeometryCollection(std::move(polys)).release();
        });
    }

    Geometry*
    GEOSPolygonize_valid_r(GEOSContextHandle_t extHandle, const Geometry* const* g, unsigned int ngeoms)
    {
        using geos::operation::polygonize::Polygonizer;

        return execute(extHandle, [&]() -> Geometry* {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            Geometry* out;

            // Polygonize
            Polygonizer plgnzr(true);
            int srid = 0;
            for(std::size_t i = 0; i < ngeoms; ++i) {
                plgnzr.add(g[i]);
                srid = g[i]->getSRID();
            }

            auto polys = plgnzr.getPolygons();
            if (polys.empty()) {
                out = handle->geomFactory->createGeometryCollection().release();
            } else if (polys.size() == 1) {
                return polys[0].release();
            } else {
                return handle->geomFactory->createMultiPolygon(std::move(polys)).release();
            }

            out->setSRID(srid);
            return out;
        });
    }

    Geometry*
    GEOSBuildArea_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        using geos::operation::polygonize::BuildArea;

        return execute(extHandle, [&]() {
            BuildArea builder;
            auto out = builder.build(g);
            out->setSRID(g->getSRID());
            return out.release();
        });
    }

    Geometry*
    GEOSMakeValid_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        GEOSMakeValidParams params;
        params.method = GEOS_MAKE_VALID_LINEWORK;
        params.keepCollapsed = 1;
        return GEOSMakeValidWithParams_r(extHandle, g, &params);
    }

    GEOSMakeValidParams*
    GEOSMakeValidParams_create_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSMakeValidParams* p = new GEOSMakeValidParams();
            p->method = GEOS_MAKE_VALID_LINEWORK;
            p->keepCollapsed = 0;
            return p;
        });
    }

    void
    GEOSMakeValidParams_destroy_r(GEOSContextHandle_t extHandle, GEOSMakeValidParams* parms)
    {
        (void)extHandle;
        delete parms;
    }

    int
    GEOSMakeValidParams_setKeepCollapsed_r(GEOSContextHandle_t extHandle,
        GEOSMakeValidParams* p, int keepCollapsed)
    {
        return execute(extHandle, 0, [&]() {
            p->keepCollapsed = keepCollapsed;
            return 1;
        });
    }

    int
    GEOSMakeValidParams_setMethod_r(GEOSContextHandle_t extHandle,
        GEOSMakeValidParams* p, GEOSMakeValidMethods method)
    {
        return execute(extHandle, 0, [&]() {
            p->method = method;
            return 1;
        });
    }

    Geometry*
    GEOSMakeValidWithParams_r(
        GEOSContextHandle_t extHandle,
        const Geometry* g,
        const GEOSMakeValidParams* params)
    {
        using geos::geom::util::GeometryFixer;
        using geos::operation::valid::MakeValid;

        if (params && params->method == GEOS_MAKE_VALID_LINEWORK) {
            return execute(extHandle, [&]() {
                MakeValid makeValid;
                auto out = makeValid.build(g);
                out->setSRID(g->getSRID());
                return out.release();
            });
        }
        else if (params && params->method == GEOS_MAKE_VALID_STRUCTURE) {
            return execute(extHandle, [&]() {
                GeometryFixer fixer(g);
                fixer.setKeepCollapsed(params->keepCollapsed == 0 ? false : true);
                auto out = fixer.getResult();
                out->setSRID(g->getSRID());
                return out.release();
            });
        }
        else {
            extHandle->ERROR_MESSAGE("Unknown method in GEOSMakeValidParams");
            return nullptr;
        }
    }

    Geometry*
    GEOSRemoveRepeatedPoints_r(
        GEOSContextHandle_t extHandle,
        const Geometry* g,
        double tolerance)
    {
        using geos::operation::valid::RepeatedPointRemover;

        return execute(extHandle, [&]() {
            auto out = RepeatedPointRemover::removeRepeatedPoints(g, tolerance);
            out->setSRID(g->getSRID());
            return out.release();
        });
    }

    Geometry*
    GEOSPolygonizer_getCutEdges_r(GEOSContextHandle_t extHandle, const Geometry* const* g, unsigned int ngeoms)
    {
        using geos::operation::polygonize::Polygonizer;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            // Polygonize
            Polygonizer plgnzr;
            int srid = 0;
            for(std::size_t i = 0; i < ngeoms; ++i) {
                plgnzr.add(g[i]);
                srid = g[i]->getSRID();
            }

            const std::vector<const LineString*>& lines = plgnzr.getCutEdges();

            // We need a vector of Geometry pointers, not Polygon pointers.
            // STL vector doesn't allow transparent upcast of this
            // nature, so we explicitly convert.
            // (it's just a waste of processor and memory, btw)
            // XXX mloskot: See comment for GEOSPolygonize_r

            std::vector<std::unique_ptr<Geometry>> linevec(lines.size());

            for(std::size_t i = 0, n = lines.size(); i < n; ++i) {
                linevec[i] = lines[i]->clone();
            }

            auto out = gf->createGeometryCollection(std::move(linevec));
            out->setSRID(srid);

            return out.release();
        });
    }

    Geometry*
    GEOSPolygonize_full_r(GEOSContextHandle_t extHandle, const Geometry* g,
                          Geometry** cuts, Geometry** dangles, Geometry** invalid)
    {
        using geos::operation::polygonize::Polygonizer;

        return execute(extHandle, [&]() {
            // Polygonize
            Polygonizer plgnzr;
            for(std::size_t i = 0; i < g->getNumGeometries(); ++i) {
                plgnzr.add(g->getGeometryN(i));
            }

            const GeometryFactory* gf = g->getFactory();

            if(cuts) {
                const std::vector<const LineString*>& lines = plgnzr.getCutEdges();
                std::vector<std::unique_ptr<Geometry>> linevec(lines.size());
                for(std::size_t i = 0, n = lines.size(); i < n; ++i) {
                    linevec[i] = lines[i]->clone();
                }

                *cuts = gf->createGeometryCollection(std::move(linevec)).release();
            }

            if(dangles) {
                const std::vector<const LineString*>& lines = plgnzr.getDangles();
                std::vector<std::unique_ptr<Geometry>> linevec(lines.size());
                for(std::size_t i = 0, n = lines.size(); i < n; ++i) {
                    linevec[i] = lines[i]->clone();
                }

                *dangles = gf->createGeometryCollection(std::move(linevec)).release();
            }

            if(invalid) {
                const std::vector<std::unique_ptr<LineString>>& lines = plgnzr.getInvalidRingLines();
                std::vector<std::unique_ptr<Geometry>> linevec(lines.size());
                for(std::size_t i = 0, n = lines.size(); i < n; ++i) {
                    linevec[i] = lines[i]->clone();
                }

                *invalid = gf->createGeometryCollection(std::move(linevec)).release();
            }

            auto polys = plgnzr.getPolygons();
            Geometry* out = gf->createGeometryCollection(std::move(polys)).release();
            out->setSRID(g->getSRID());
            return out;
        });
    }

    Geometry*
    GEOSLineMerge_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        using geos::operation::linemerge::LineMerger;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            LineMerger lmrgr;
            lmrgr.add(g);

            auto lines = lmrgr.getMergedLineStrings();

            auto out = gf->buildGeometry(std::move(lines));
            out->setSRID(g->getSRID());

            return out.release();
        });
    }

    Geometry*
    GEOSLineMergeDirected_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        using geos::operation::linemerge::LineMerger;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            LineMerger lmrgr(true);
            lmrgr.add(g);

            auto lines = lmrgr.getMergedLineStrings();

            auto out = gf->buildGeometry(std::move(lines));
            out->setSRID(g->getSRID());

            return out.release();
        });
    }

    Geometry*
    GEOSLineSubstring_r(GEOSContextHandle_t extHandle, const Geometry* g, double start_fraction, double end_fraction)
    {
        using geos::linearref::LengthIndexedLine;

        return execute(extHandle, [&]() {
            if (start_fraction < 0 || end_fraction < 0) {
                throw IllegalArgumentException("start fraction must be >= 0");
            }
            if (start_fraction > 1 || end_fraction > 1) {
                throw IllegalArgumentException("end fraction must be <= 1");
            }

            LengthIndexedLine lil(g);

            auto length = g->getLength();

            auto out = lil.extractLine(start_fraction * length, end_fraction * length);
            out->setSRID(g->getSRID());

            return out.release();
        });
    }

    Geometry*
    GEOSReverse_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            auto g3 = g->reverse();
            g3->setSRID(g->getSRID());
            return g3.release();
        });
    }

    void*
    GEOSGeom_getUserData_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            return g->getUserData();
        });
    }

    int
    GEOSGetSRID_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 0, [&]() {
            return g->getSRID();
        });
    }

    const char* GEOSversion()
    {
        static char version[256];
        snprintf(version, 256, "%s", GEOS_CAPI_VERSION);
        return version;
    }

    const char* GEOSjtsport()
    {
        return GEOS_JTS_PORT;
    }

    char
    GEOSHasZ_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return g->hasZ();
        });
    }

    char
    GEOSHasM_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return g->hasM();
        });
    }

    int
    GEOS_getWKBOutputDims_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, -1, [&]() -> int {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            return handle->WKBOutputDims;
        });
    }

    int
    GEOS_setWKBOutputDims_r(GEOSContextHandle_t extHandle, int newdims)
    {
        return execute(extHandle, -1, [&]() {
            GEOSContextHandleInternal_t *handle = reinterpret_cast<GEOSContextHandleInternal_t *>(extHandle);

            if (newdims < 2 || newdims > 4) {
                handle->ERROR_MESSAGE("WKB output dimensions out of range 2..4");
            }

            const int olddims = handle->WKBOutputDims;
            handle->WKBOutputDims = static_cast<uint8_t>(newdims);

            return olddims;
        });
    }

    int
    GEOS_getWKBByteOrder_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, -1, [&]() {
            GEOSContextHandleInternal_t *handle = reinterpret_cast<GEOSContextHandleInternal_t *>(extHandle);
            return handle->WKBByteOrder;
        });
    }

    int
    GEOS_setWKBByteOrder_r(GEOSContextHandle_t extHandle, int byteOrder)
    {
        return execute(extHandle, -1, [&]() {
            GEOSContextHandleInternal_t *handle = reinterpret_cast<GEOSContextHandleInternal_t *>(extHandle);
            const int oldByteOrder = handle->WKBByteOrder;
            handle->WKBByteOrder = byteOrder;

            return oldByteOrder;
        });
    }


    CoordinateSequence*
    GEOSCoordSeq_create_r(GEOSContextHandle_t extHandle, unsigned int size, unsigned int dims)
    {
        return execute(extHandle, [&]() {
            return new CoordinateSequence(size, dims);
        });
    }

    CoordinateSequence*
    GEOSCoordSeq_copyFromBuffer_r(GEOSContextHandle_t extHandle, const double* buf, unsigned int size, int hasZ, int hasM)
    {
        return execute(extHandle, [&]() {
            std::ptrdiff_t stride = 2 + hasZ + hasM;
            auto coords = geos::detail::make_unique<CoordinateSequence>(size, hasZ, hasM, false);
            if (hasZ) {
                if (hasM) {
                    // XYZM
                    assert(coords->getCoordinateType() == geos::geom::CoordinateType::XYZM);
                    std::memcpy(coords->data(), buf, size * sizeof(CoordinateXYZM));
                } else {
                    // XYZ
                    assert(coords->getCoordinateType() == geos::geom::CoordinateType::XYZ);
                    std::memcpy(coords->data(), buf, size * sizeof(Coordinate));
                }
            } else {
                if (hasM) {
                    // XYM
                    for (std::size_t i = 0; i < size; i++) {
                        coords->setAt(CoordinateXYM{ *buf, *(buf + 1), *(buf + 2)}, i);
                        buf += stride;
                    }
                } else {
                    // XY
                    for (std::size_t i = 0; i < size; i++) {
                        coords->setAt(Coordinate{ *buf, *(buf + 1) }, i);
                        buf += stride;
                    }
                }
            }

            return coords.release();
        });
    }

    CoordinateSequence*
    GEOSCoordSeq_copyFromArrays_r(GEOSContextHandle_t extHandle, const double* x, const double* y, const double* z, const double* m, unsigned int size)
    {
        return execute(extHandle, [&]() {
            bool hasZ = z != nullptr;
            bool hasM = m != nullptr;

            auto coords = geos::detail::make_unique<geos::geom::CoordinateSequence>(size, hasZ, hasM, false);

            CoordinateXYZM c;
            for (std::size_t i = 0; i < size; i++) {
                c.x = x[i];
                c.y = y[i];
                if (z) {
                    c.z = z[i];
                }
                if (m) {
                    c.m = m[i];
                }

                coords->setAt(c, i);
            }

            return coords.release();
        });
    }

    int
    GEOSCoordSeq_copyToArrays_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs,
                                double* x, double* y, double* z, double* m)
    {
        return execute(extHandle, 0, [&]() {
            CoordinateXYZM c;
            for (std::size_t i = 0; i < cs->size(); i++) {
                cs->getAt(i, c);
                x[i] = c.x;
                y[i] = c.y;
                if (z) {
                    z[i] = c.z;
                }
                if (m) {
                    m[i] = c.m;
                }
            }

            return 1;
        });
    }

    int
    GEOSCoordSeq_copyToBuffer_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs,
                                double* buf, int hasZ, int hasM)
    {
        using geos::geom::CoordinateType;

        return execute(extHandle, 0, [&]() {
            CoordinateType srcType = cs->getCoordinateType();
            CoordinateType dstType;
            std::size_t stride;
            if (hasZ) {
                if (hasM) {
                    dstType = CoordinateType::XYZM;
                    stride = 4;
                } else {
                    dstType = CoordinateType::XYZ;
                    stride = 3;
                }
            } else {
                if (hasM) {
                    dstType = CoordinateType::XYM;
                    stride = 3;
                } else {
                    dstType = CoordinateType::XY;
                    stride = 2;
                }
            }

            if (srcType == dstType) {
                std::memcpy(buf, cs->data(), cs->size() * stride * sizeof(double));
            } else {
                switch(dstType) {
                    case CoordinateType::XY: {
                        for (std::size_t i = 0; i < cs->size(); i++) {
                            CoordinateXY* c = reinterpret_cast<CoordinateXY*>(buf + i*stride);
                            cs->getAt(i, *c);
                        }
                        break;
                    }
                    case CoordinateType::XYZ: {
                        for (std::size_t i = 0; i < cs->size(); i++) {
                            Coordinate* c = reinterpret_cast<Coordinate*>(buf + i*stride);
                            cs->getAt(i, *c);
                        }
                        break;
                    }
                    case CoordinateType::XYM: {
                        for (std::size_t i = 0; i < cs->size(); i++) {
                            CoordinateXYM* c = reinterpret_cast<CoordinateXYM*>(buf + i*stride);
                            cs->getAt(i, *c);
                        }
                        break;
                    }
                    case CoordinateType::XYZM: {
                        for (std::size_t i = 0; i < cs->size(); i++) {
                            CoordinateXYZM* c = reinterpret_cast<CoordinateXYZM*>(buf + i*stride);
                            cs->getAt(i, *c);
                        }
                        break;
                    }
                }
            }

            return 1;
        });
    }

    int
    GEOSCoordSeq_setOrdinate_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs,
                               unsigned int idx, unsigned int dim, double val)
    {
        return execute(extHandle, 0, [&]() {
            cs->setOrdinate(idx, dim, val);
            return 1;
        });
    }

    int
    GEOSCoordSeq_setX_r(GEOSContextHandle_t extHandle, CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate_r(extHandle, s, idx, 0, val);
    }

    int
    GEOSCoordSeq_setY_r(GEOSContextHandle_t extHandle, CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate_r(extHandle, s, idx, 1, val);
    }

    int
    GEOSCoordSeq_setZ_r(GEOSContextHandle_t extHandle, CoordinateSequence* s, unsigned int idx, double val)
    {
        return GEOSCoordSeq_setOrdinate_r(extHandle, s, idx, 2, val);
    }

    int
    GEOSCoordSeq_setXY_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs, unsigned int idx, double x, double y)
    {
        return execute(extHandle, 0, [&]() {
            cs->setAt(CoordinateXY{x, y}, idx);
            return 1;
        });
    }

    int
    GEOSCoordSeq_setXYZ_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs, unsigned int idx, double x, double y, double z)
    {
        return execute(extHandle, 0, [&]() {
            cs->setAt(Coordinate{x, y, z}, idx);
            return 1;
        });
    }

    CoordinateSequence*
    GEOSCoordSeq_clone_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs)
    {
        return execute(extHandle, [&]() {
            return cs->clone().release();
        });
    }

    int
    GEOSCoordSeq_getOrdinate_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs,
                               unsigned int idx, unsigned int dim, double* val)
    {
        return execute(extHandle, 0, [&]() {
            *val = cs->getOrdinate(idx, dim);
            return 1;
        });
    }

    int
    GEOSCoordSeq_getX_r(GEOSContextHandle_t extHandle, const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate_r(extHandle, s, idx, 0, val);
    }

    int
    GEOSCoordSeq_getY_r(GEOSContextHandle_t extHandle, const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate_r(extHandle, s, idx, 1, val);
    }

    int
    GEOSCoordSeq_getZ_r(GEOSContextHandle_t extHandle, const CoordinateSequence* s, unsigned int idx, double* val)
    {
        return GEOSCoordSeq_getOrdinate_r(extHandle, s, idx, 2, val);
    }

    int
    GEOSCoordSeq_getXY_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs, unsigned int idx, double* x, double* y)
    {
        return execute(extHandle, 0, [&]() {
            auto& c = cs->getAt<CoordinateXY>(idx);
            *x = c.x;
            *y = c.y;
            return 1;
        });
    }

    int
    GEOSCoordSeq_getXYZ_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs, unsigned int idx, double* x, double* y, double* z)
    {
        return execute(extHandle, 0, [&]() {
            auto& c = cs->getAt(idx);
            *x = c.x;
            *y = c.y;
            *z = c.z;
            return 1;
        });
    }

    int
    GEOSCoordSeq_getSize_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs, unsigned int* size)
    {
        return execute(extHandle, 0, [&]() {
            const std::size_t sz = cs->getSize();
            *size = static_cast<unsigned int>(sz);
            return 1;
        });
    }

    int
    GEOSCoordSeq_getDimensions_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs, unsigned int* dims)
    {
        return execute(extHandle, 0, [&]() {
            const std::size_t dim = cs->getDimension();
            *dims = static_cast<unsigned int>(dim);

            return 1;
        });
    }

    int
    GEOSCoordSeq_isCCW_r(GEOSContextHandle_t extHandle, const CoordinateSequence* cs, char* val)
    {
        return execute(extHandle, 0, [&]() {
            *val = geos::algorithm::Orientation::isCCW(cs);
            return 1;
        });
    }

    void
    GEOSCoordSeq_destroy_r(GEOSContextHandle_t extHandle, CoordinateSequence* s)
    {
        return execute(extHandle, [&]() {
            delete s;
        });
    }

    const CoordinateSequence*
    GEOSGeom_getCoordSeq_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            const SimpleCurve* ls = dynamic_cast<const SimpleCurve*>(g);
            if(ls) {
                return ls->getCoordinatesRO();
            }

            const Point* p = dynamic_cast<const Point*>(g);
            if(p) {
                return p->getCoordinatesRO();
            }

            throw IllegalArgumentException("Geometry must be a Point or LineString");
        });
    }

    Geometry*
    GEOSGeom_createEmptyPoint_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            return gf->createPoint().release();
        });
    }

    Geometry*
    GEOSGeom_createPoint_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createPoint(std::unique_ptr<CoordinateSequence>(cs)).release();
        });
    }

    Geometry*
    GEOSGeom_createPointFromXY_r(GEOSContextHandle_t extHandle, double x, double y)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            CoordinateXY c(x, y);
            return gf->createPoint(c).release();
        });
    }

    Geometry*
    GEOSGeom_createLinearRing_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createLinearRing(std::unique_ptr<CoordinateSequence>(cs)).release();
        });
    }

    Geometry*
    GEOSGeom_createEmptyLineString_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createLineString().release();
        });
    }

    Geometry*
    GEOSGeom_createLineString_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createLineString(std::unique_ptr<CoordinateSequence>(cs)).release();
        });
    }

    Geometry*
    GEOSGeom_createEmptyPolygon_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            return gf->createPolygon().release();
        });
    }

    Geometry*
    GEOSGeom_createPolygon_r(GEOSContextHandle_t extHandle, Geometry* shell, Geometry** holes, unsigned int nholes)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            bool good_holes = true, good_shell = true;

            // Validate input before taking ownership
            for (std::size_t i = 0; i < nholes; i++) {
                if ((!holes) || (!dynamic_cast<LinearRing*>(holes[i]))) {
                    good_holes = false;
                    break;
                }
            }
            if (!dynamic_cast<LinearRing*>(shell)) {
                good_shell = false;
            }

            // Contract for GEOSGeom_createPolygon is to take ownership of arguments
            // which implies freeing them on exception,
            // see https://trac.osgeo.org/geos/ticket/1111
            if (!(good_holes && good_shell)) {
                if (shell) delete shell;
                for (std::size_t i = 0; i < nholes; i++) {
                    if (holes && holes[i])
                        delete holes[i];
                }
                if (!good_shell)
                    throw IllegalArgumentException("Shell is not a LinearRing");
                else
                    throw IllegalArgumentException("Hole is not a LinearRing");
            }

            std::unique_ptr<LinearRing> tmpshell(static_cast<LinearRing*>(shell));
            if (nholes) {
                std::vector<std::unique_ptr<LinearRing>> tmpholes(nholes);
                for (size_t i = 0; i < nholes; i++) {
                    tmpholes[i].reset(static_cast<LinearRing*>(holes[i]));
                }

                return gf->createPolygon(std::move(tmpshell), std::move(tmpholes)).release();
            }

            return gf->createPolygon(std::move(tmpshell)).release();
        });
    }

    Geometry*
    GEOSGeom_createRectangle_r(GEOSContextHandle_t extHandle,
                            double xmin, double ymin,
                            double xmax, double ymax)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            Envelope env(xmin, xmax, ymin, ymax);
            return (gf->toGeometry(&env)).release();
        });
    }

    Geometry*
    GEOSGeom_createCircularString_r(GEOSContextHandle_t extHandle, CoordinateSequence* cs)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createCircularString(std::unique_ptr<CoordinateSequence>(cs)).release();
        });
    }

    Geometry*
    GEOSGeom_createEmptyCircularString_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createCircularString(false, false).release();
        });
    }

    Geometry*
    GEOSGeom_createCompoundCurve_r(GEOSContextHandle_t extHandle, Geometry** geoms, unsigned int ngeoms)
    {
        return execute(extHandle, [&]() -> Geometry* {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            bool invalid_input = false;
            std::vector<std::unique_ptr<SimpleCurve>> geom_vec(ngeoms);
            for (std::size_t i = 0; i < ngeoms; i++) {
                if (SimpleCurve* c = dynamic_cast<SimpleCurve*>(geoms[i])) {
                    geom_vec[i].reset(c);
                } else {
                    delete geoms[i];
                    invalid_input = true;
                }
            }

            if (invalid_input) {
                throw IllegalArgumentException("Input is not a SimpleCurve");
            }

            return gf->createCompoundCurve(std::move(geom_vec)).release();
        });
    }

    Geometry*
    GEOSGeom_createEmptyCompoundCurve_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;

            return gf->createCompoundCurve().release();
        });
    }

    Geometry*
    GEOSGeom_createCurvePolygon_r(GEOSContextHandle_t extHandle, Geometry* p_shell, Geometry** p_holes, unsigned int nholes)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            bool good_holes = true, good_shell = true;

            std::unique_ptr<Curve> shell;
            std::vector<std::unique_ptr<Curve>> holes(nholes);

            if (Curve* c = dynamic_cast<Curve*>(p_shell)) {
                shell.reset(c);
            } else {
                good_shell = false;
                delete p_shell;
            }

            for (std::size_t i = 0; i < nholes; i++) {
                if (Curve* c = dynamic_cast<Curve*>(p_holes[i])) {
                    holes[i].reset(c);
                } else {
                    good_shell = false;
                    delete p_holes[i];
                }
            }

            if (good_shell && good_holes) {
                return gf->createCurvePolygon(std::move(shell), std::move(holes)).release();
            } else if (!good_shell) {
                throw IllegalArgumentException("Shell is not a Curve");
            } else {
                throw IllegalArgumentException("Hole is not a Curve");
            }
        });
    }


    Geometry*
    GEOSGeom_createEmptyCurvePolygon_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            const GeometryFactory* gf = handle->geomFactory;
            return gf->createCurvePolygon(false, false).release();
        });
    }

    Geometry*
    GEOSGeom_clone_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            return g->clone().release();
        });
    }

    Geometry*
    GEOSGeom_setPrecision_r(GEOSContextHandle_t extHandle, const GEOSGeometry* g,
                            double gridSize, int flags)
    {
        using namespace geos::geom;

        return execute(extHandle, [&]() {
            PrecisionModel newpm;
            if(gridSize != 0) {
                // Convert gridSize to scale factor
                double scale = 1.0 / std::abs(gridSize);
                newpm = PrecisionModel(scale);
            }

            const PrecisionModel* pm = g->getPrecisionModel();
            double cursize = pm->isFloating() ? 0 : 1.0 / pm->getScale();
            std::unique_ptr<Geometry> ret;
            GeometryFactory::Ptr gf =
                GeometryFactory::create(&newpm, g->getSRID());
            if(gridSize != 0 && cursize != gridSize) {
                GeometryPrecisionReducer reducer(*gf);
                reducer.setChangePrecisionModel(true);
                reducer.setUseAreaReducer(!(flags & GEOS_PREC_NO_TOPO));
                reducer.setPointwise(flags & GEOS_PREC_NO_TOPO);
                reducer.setRemoveCollapsedComponents(!(flags & GEOS_PREC_KEEP_COLLAPSED));
                ret = reducer.reduce(*g);
            }
            else {
                // No need or willing to snap, just change the factory
                ret = gf->createGeometry(g);
            }
            return ret.release();
        });
    }

    double
    GEOSGeom_getPrecision_r(GEOSContextHandle_t extHandle, const GEOSGeometry* g)
    {
        using namespace geos::geom;

        return execute(extHandle, -1.0, [&]() {
            const PrecisionModel* pm = g->getPrecisionModel();
            double cursize = pm->isFloating() ? 0 : 1.0 / pm->getScale();
            return cursize;
        });
    }

    int
    GEOSGeom_getDimensions_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 0, [&]() {
            return (int) g->getDimension();
        });
    }

    int
    GEOSGeom_getCoordinateDimension_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, 0, [&]() {
            return (int)(g->getCoordinateDimension());
        });
    }

    int
    GEOSGeom_getXMin_r(GEOSContextHandle_t extHandle, const Geometry* g, double* value)
    {
        return execute(extHandle, 0, [&]() {
            if(g->isEmpty()) {
                return 0;
            }

            *value = g->getEnvelopeInternal()->getMinX();
            return 1;
        });
    }

    int
    GEOSGeom_getXMax_r(GEOSContextHandle_t extHandle, const Geometry* g, double* value)
    {
        return execute(extHandle, 0, [&]() {
            if(g->isEmpty()) {
                return 0;
            }

            *value = g->getEnvelopeInternal()->getMaxX();
            return 1;
        });
    }

    int
    GEOSGeom_getYMin_r(GEOSContextHandle_t extHandle, const Geometry* g, double* value)
    {
        return execute(extHandle, 0, [&]() {
            if(g->isEmpty()) {
                return 0;
            }

            *value = g->getEnvelopeInternal()->getMinY();
            return 1;
        });
    }

    int
    GEOSGeom_getYMax_r(GEOSContextHandle_t extHandle, const Geometry* g, double* value)
    {
        return execute(extHandle, 0, [&]() {
            if(g->isEmpty()) {
                return 0;
            }

            *value = g->getEnvelopeInternal()->getMaxY();
            return 1;
        });
    }

    int
    GEOSGeom_getExtent_r(GEOSContextHandle_t extHandle, const Geometry* g, double* xmin, double* ymin, double* xmax, double* ymax)
    {
        return execute(extHandle, 0, [&]() {
            if(g->isEmpty()) {
                return 0;
            }
            const Envelope* extent = g->getEnvelopeInternal();
            *xmin = extent->getMinX();
            *ymin = extent->getMinY();
            *xmax = extent->getMaxX();
            *ymax = extent->getMaxY();
            return 1;
        });
    }

    Geometry*
    GEOSSimplify_r(GEOSContextHandle_t extHandle, const Geometry* g1, double tolerance)
    {
        using namespace geos::simplify;

        return execute(extHandle, [&]() {
            Geometry::Ptr g3(DouglasPeuckerSimplifier::simplify(g1, tolerance));
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSTopologyPreserveSimplify_r(GEOSContextHandle_t extHandle, const Geometry* g1, double tolerance)
    {
        using namespace geos::simplify;

        return execute(extHandle, [&]() {
            Geometry::Ptr g3(TopologyPreservingSimplifier::simplify(g1, tolerance));
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }


    /* WKT Reader */
    WKTReader*
    GEOSWKTReader_create_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t *handle = reinterpret_cast<GEOSContextHandleInternal_t *>(extHandle);
            return new WKTReader((GeometryFactory *) handle->geomFactory);
        });
    }

    void
    GEOSWKTReader_destroy_r(GEOSContextHandle_t extHandle, WKTReader* reader)
    {
        return execute(extHandle, [&]() {
            delete reader;
        });
    }

    void
    GEOSWKTReader_setFixStructure_r(GEOSContextHandle_t extHandle, WKTReader* reader, char doFix)
    {
        return execute(extHandle, [&]() {
            return reader->setFixStructure(doFix);
        });
    }

    Geometry*
    GEOSWKTReader_read_r(GEOSContextHandle_t extHandle, WKTReader* reader, const char* wkt)
    {
        return execute(extHandle, [&]() {
            const std::string wktstring(wkt);
            return reader->read(wktstring).release();
        });
    }

    /* WKT Writer */
    WKTWriter*
    GEOSWKTWriter_create_r(GEOSContextHandle_t extHandle)
    {
        using geos::io::WKTWriter;

        return execute(extHandle, [&]() {
            return new WKTWriter();
        });
    }

    void
    GEOSWKTWriter_destroy_r(GEOSContextHandle_t extHandle, WKTWriter* Writer)
    {
        execute(extHandle, [&]() {
            delete Writer;
        });
    }


    char*
    GEOSWKTWriter_write_r(GEOSContextHandle_t extHandle, WKTWriter* writer, const Geometry* geom)
    {
        return execute(extHandle, [&]() {
            std::string sgeom(writer->write(geom));
            char* result = gstrdup(sgeom);
            return result;
        });
    }

    void
    GEOSWKTWriter_setTrim_r(GEOSContextHandle_t extHandle, WKTWriter* writer, char trim)
    {
        execute(extHandle, [&]() {
            writer->setTrim(0 != trim);
        });
    }

    void
    GEOSWKTWriter_setRoundingPrecision_r(GEOSContextHandle_t extHandle, WKTWriter* writer, int precision)
    {
        execute(extHandle, [&]() {
            writer->setRoundingPrecision(precision);
        });
    }

    void
    GEOSWKTWriter_setOutputDimension_r(GEOSContextHandle_t extHandle, WKTWriter* writer, int dim)
    {
        execute(extHandle, [&]() {
            writer->setOutputDimension(static_cast<uint8_t>(dim));
        });
    }

    int
    GEOSWKTWriter_getOutputDimension_r(GEOSContextHandle_t extHandle, WKTWriter* writer)
    {
        return execute(extHandle, -1, [&]() {
            return writer->getOutputDimension();
        });
    }

    void
    GEOSWKTWriter_setOld3D_r(GEOSContextHandle_t extHandle, WKTWriter* writer, int useOld3D)
    {
        execute(extHandle, [&]() {
            writer->setOld3D(0 != useOld3D);
        });
    }

    /* WKB Reader */
    WKBReader*
    GEOSWKBReader_create_r(GEOSContextHandle_t extHandle)
    {
        using geos::io::WKBReader;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
            return new WKBReader(*(GeometryFactory*)handle->geomFactory);
        });
    }

    void
    GEOSWKBReader_destroy_r(GEOSContextHandle_t extHandle, WKBReader* reader)
    {
        execute(extHandle, [&]() {
            delete reader;
        });
    }

    void
    GEOSWKBReader_setFixStructure_r(GEOSContextHandle_t extHandle, WKBReader* reader, char doFix)
    {
        return execute(extHandle, [&]() {
            return reader->setFixStructure(doFix);
        });
    }

    struct membuf : public std::streambuf {
        membuf(char* s, std::size_t n)
        {
            setg(s, s, s + n);
        }
    };

    Geometry*
    GEOSWKBReader_read_r(GEOSContextHandle_t extHandle, WKBReader* reader, const unsigned char* wkb, std::size_t size)
    {
        return execute(extHandle, [&]() {
            return reader->read(wkb, size).release();
        });
    }

    Geometry*
    GEOSWKBReader_readHEX_r(GEOSContextHandle_t extHandle, WKBReader* reader, const unsigned char* hex, std::size_t size)
    {
        return execute(extHandle, [&]() {
            std::string hexstring(reinterpret_cast<const char*>(hex), size);
            std::istringstream is(std::ios_base::binary);
            is.str(hexstring);
            is.seekg(0, std::ios::beg); // rewind reader pointer

            return reader->readHEX(is).release();
        });
    }

    /* WKB Writer */
    WKBWriter*
    GEOSWKBWriter_create_r(GEOSContextHandle_t extHandle)
    {
        using geos::io::WKBWriter;

        return execute(extHandle, [&]() {
            return new WKBWriter();
        });
    }

    void
    GEOSWKBWriter_destroy_r(GEOSContextHandle_t extHandle, WKBWriter* Writer)
    {
        execute(extHandle, [&]() {
            delete Writer;
        });
    }


    /* The caller owns the result */
    unsigned char*
    GEOSWKBWriter_write_r(GEOSContextHandle_t extHandle, WKBWriter* writer, const Geometry* geom, std::size_t* size)
    {
        return execute(extHandle, [&]() {
            std::ostringstream os(std::ios_base::binary);
            writer->write(*geom, os);

            const std::string& wkbstring = os.str();
            const std::size_t len = wkbstring.length();

            unsigned char* result = (unsigned char*) malloc(len);
            std::memcpy(result, wkbstring.c_str(), len);
            *size = len;
            return result;
        });
    }

    /* The caller owns the result */
    unsigned char*
    GEOSWKBWriter_writeHEX_r(GEOSContextHandle_t extHandle, WKBWriter* writer, const Geometry* geom, std::size_t* size)
    {
        return execute(extHandle, [&]() {
            std::ostringstream os(std::ios_base::binary);
            writer->writeHEX(*geom, os);
            std::string wkbstring(os.str());
            const std::size_t len = wkbstring.length();

            unsigned char* result = (unsigned char*) malloc(len);
            std::memcpy(result, wkbstring.c_str(), len);
            *size = len;
            return result;
        });
    }

    int
    GEOSWKBWriter_getOutputDimension_r(GEOSContextHandle_t extHandle, const GEOSWKBWriter* writer)
    {
        return execute(extHandle, 0, [&]() {
            return writer->getOutputDimension();
        });
    }

    void
    GEOSWKBWriter_setOutputDimension_r(GEOSContextHandle_t extHandle, GEOSWKBWriter* writer, int newDimension)
    {
        execute(extHandle, [&]() {
            writer->setOutputDimension(static_cast<uint8_t>(newDimension));
        });
    }

    int
    GEOSWKBWriter_getByteOrder_r(GEOSContextHandle_t extHandle, const GEOSWKBWriter* writer)
    {
        return execute(extHandle, 0, [&]() {
            return writer->getByteOrder();
        });
    }

    void
    GEOSWKBWriter_setByteOrder_r(GEOSContextHandle_t extHandle, GEOSWKBWriter* writer, int newByteOrder)
    {
        execute(extHandle, [&]() {
            writer->setByteOrder(newByteOrder);
        });
    }

    char
    GEOSWKBWriter_getIncludeSRID_r(GEOSContextHandle_t extHandle, const GEOSWKBWriter* writer)
    {
        return execute(extHandle, 2, [&]{
            return writer->getIncludeSRID();
        });
    }

    void
    GEOSWKBWriter_setIncludeSRID_r(GEOSContextHandle_t extHandle, GEOSWKBWriter* writer, const char newIncludeSRID)
    {
        execute(extHandle, [&]{
            writer->setIncludeSRID(newIncludeSRID);
        });
    }

    int
    GEOSWKBWriter_getFlavor_r(GEOSContextHandle_t extHandle, const GEOSWKBWriter* writer)
    {
        return execute(extHandle, -1, [&]{
            return writer->getFlavor();
        });
    }

    void
    GEOSWKBWriter_setFlavor_r(GEOSContextHandle_t extHandle, GEOSWKBWriter* writer, int flavor)
    {
        execute(extHandle, [&]{
            writer->setFlavor(flavor);
        });
    }

    /* GeoJSON Reader */
    GeoJSONReader*
    GEOSGeoJSONReader_create_r(GEOSContextHandle_t extHandle)
    {
        using geos::io::GeoJSONReader;

        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t *handle = reinterpret_cast<GEOSContextHandleInternal_t *>(extHandle);
            return new GeoJSONReader(*(GeometryFactory*)handle->geomFactory);
        });
    }

    void
    GEOSGeoJSONReader_destroy_r(GEOSContextHandle_t extHandle, GEOSGeoJSONReader* reader)
    {
        return execute(extHandle, [&]() {
            delete reader;
        });
    }

    Geometry*
    GEOSGeoJSONReader_readGeometry_r(GEOSContextHandle_t extHandle, GEOSGeoJSONReader* reader, const char* geojson)
    {
        return execute(extHandle, [&]() {
            const std::string geojsonstring(geojson);
            return reader->read(geojsonstring).release();
        });
    }

    /* GeoJSON Writer */
    GeoJSONWriter*
    GEOSGeoJSONWriter_create_r(GEOSContextHandle_t extHandle)
    {
        using geos::io::GeoJSONWriter;

        return execute(extHandle, [&]() {
            return new GeoJSONWriter();
        });
    }

    void
    GEOSGeoJSONWriter_destroy_r(GEOSContextHandle_t extHandle, GEOSGeoJSONWriter* writer)
    {
        return execute(extHandle, [&]() {
            delete writer;
        });
    }

    char*
    GEOSGeoJSONWriter_writeGeometry_r(GEOSContextHandle_t extHandle, GEOSGeoJSONWriter* writer, const GEOSGeometry* g, int indent)
    {
        return execute(extHandle, [&]() {
            std::string geojson;
            if (indent >= 0) {
                geojson = writer->writeFormatted(g, geos::io::GeoJSONType::GEOMETRY, indent);
            } else {
                geojson = writer->write(g, geos::io::GeoJSONType::GEOMETRY);
            }
            char* result = gstrdup(geojson);
            return result;
        });
    }


//-----------------------------------------------------------------
// Prepared Geometry
//-----------------------------------------------------------------

    const geos::geom::prep::PreparedGeometry*
    GEOSPrepare_r(GEOSContextHandle_t extHandle, const Geometry* g)
    {
        return execute(extHandle, [&]() {
            return geos::geom::prep::PreparedGeometryFactory::prepare(g).release();
        });
    }

    void
    GEOSPreparedGeom_destroy_r(GEOSContextHandle_t extHandle, const geos::geom::prep::PreparedGeometry* a)
    {
        execute(extHandle, [&]() {
            delete a;
        });
    }

    char
    GEOSPreparedContains_r(GEOSContextHandle_t extHandle,
                           const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->contains(g);
        });
    }

    char
    GEOSPreparedContainsXY_r(GEOSContextHandle_t extHandle,
                           const geos::geom::prep::PreparedGeometry* pg, double x, double y)
    {
        extHandle->point2d->setXY(x, y);

        return GEOSPreparedContains_r(extHandle, pg, extHandle->point2d.get());
    }

    char
    GEOSPreparedContainsProperly_r(GEOSContextHandle_t extHandle,
                                   const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->containsProperly(g);
        });
    }

    char
    GEOSPreparedCoveredBy_r(GEOSContextHandle_t extHandle,
                            const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->coveredBy(g);
        });
    }

    char
    GEOSPreparedCovers_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->covers(g);
        });
    }

    char
    GEOSPreparedCrosses_r(GEOSContextHandle_t extHandle,
                          const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->crosses(g);
        });
    }

    char
    GEOSPreparedDisjoint_r(GEOSContextHandle_t extHandle,
                           const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->disjoint(g);
        });
    }

    char
    GEOSPreparedIntersects_r(GEOSContextHandle_t extHandle,
                             const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->intersects(g);
        });
    }

    char
    GEOSPreparedIntersectsXY_r(GEOSContextHandle_t extHandle,
                             const geos::geom::prep::PreparedGeometry* pg, double x, double y)
    {
        extHandle->point2d->setXY(x, y);

        return GEOSPreparedIntersects_r(extHandle, pg, extHandle->point2d.get());
    }

    char
    GEOSPreparedOverlaps_r(GEOSContextHandle_t extHandle,
                           const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->overlaps(g);
        });
    }

    char
    GEOSPreparedTouches_r(GEOSContextHandle_t extHandle,
                          const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->touches(g);
        });
    }

    char
    GEOSPreparedWithin_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, 2, [&]() {
            return pg->within(g);
        });
    }

    char *
    GEOSPreparedRelate_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        return execute(extHandle, [&]() -> char * {
            return gstrdup(pg->relate(g)->toString());
        });
    }

    char
    GEOSPreparedRelatePattern_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg, const Geometry* g, const char* imPattern)
    {
        return execute(extHandle, 2, [&]() {
            return pg->relate(g, std::string(imPattern));
        });
    }

    CoordinateSequence*
    GEOSPreparedNearestPoints_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg, const Geometry* g)
    {
        using namespace geos::geom;

        return execute(extHandle, [&]() -> CoordinateSequence* {
            return pg->nearestPoints(g).release();
        });
    }

    int
    GEOSPreparedDistance_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg,
                         const Geometry* g, double* dist)
    {
        return execute(extHandle, 0, [&]() {
            *dist = pg->distance(g);
            return 1;
        });
    }

    char
    GEOSPreparedDistanceWithin_r(GEOSContextHandle_t extHandle,
                         const geos::geom::prep::PreparedGeometry* pg,
                         const Geometry* g, double dist)
    {
        return execute(extHandle, 2, [&]() {
            return pg->isWithinDistance(g, dist);
        });
    }

//-----------------------------------------------------------------
// STRtree
//-----------------------------------------------------------------

    GEOSSTRtree*
    GEOSSTRtree_create_r(GEOSContextHandle_t extHandle,
                         std::size_t nodeCapacity)
    {
        return execute(extHandle, [&]() {
            return new GEOSSTRtree(nodeCapacity);
        });
    }

    int
    GEOSSTRtree_build_r(GEOSContextHandle_t extHandle,
                        GEOSSTRtree* tree)
    {
        return execute(extHandle, 0, [&]() {
            tree->build();
            return 1;
        });
    }

    void
    GEOSSTRtree_insert_r(GEOSContextHandle_t extHandle,
                         GEOSSTRtree* tree,
                         const geos::geom::Geometry* g,
                         void* item)
    {
        execute(extHandle, [&]() {
            tree->insert(g->getEnvelopeInternal(), item);
        });
    }

    void
    GEOSSTRtree_query_r(GEOSContextHandle_t extHandle,
                        GEOSSTRtree* tree,
                        const geos::geom::Geometry* g,
                        GEOSQueryCallback callback,
                        void* userdata)
    {
        execute(extHandle, [&]() {
            CAPI_ItemVisitor visitor(callback, userdata);
            tree->query(g->getEnvelopeInternal(), visitor);
        });
    }

    const GEOSGeometry*
    GEOSSTRtree_nearest_r(GEOSContextHandle_t extHandle,
                          GEOSSTRtree* tree,
                          const geos::geom::Geometry* geom)
    {
        return (const GEOSGeometry*) GEOSSTRtree_nearest_generic_r(extHandle, tree, geom, geom, nullptr, nullptr);
    }

    const void*
    GEOSSTRtree_nearest_generic_r(GEOSContextHandle_t extHandle,
                                  GEOSSTRtree* tree,
                                  const void* item,
                                  const geos::geom::Geometry* itemEnvelope,
                                  GEOSDistanceCallback distancefn,
                                  void* userdata)
    {
        using namespace geos::index::strtree;

        struct CustomItemDistance {
            CustomItemDistance(GEOSDistanceCallback p_distancefn, void* p_userdata)
                : m_distancefn(p_distancefn), m_userdata(p_userdata) {}

            GEOSDistanceCallback m_distancefn;
            void* m_userdata;

            double operator()(const void* a, const void* b) const
            {
                double d;

                if(!m_distancefn(a, b, &d, m_userdata)) {
                    throw std::runtime_error(std::string("Failed to compute distance."));
                }

                return d;
            }
        };

        struct GeometryDistance {
            double operator()(void* a, void* b) const {
                return static_cast<const Geometry*>(a)->distance(static_cast<const Geometry*>(b));
            }
        };

        return execute(extHandle, [&]() {
            if(distancefn) {
                CustomItemDistance itemDistance(distancefn, userdata);
                return tree->nearestNeighbour(*itemEnvelope->getEnvelopeInternal(), (void*) item, itemDistance);
            }
            else {
                return tree->nearestNeighbour<GeometryDistance>(*itemEnvelope->getEnvelopeInternal(), (void*) item);
            }
        });
    }

    void
    GEOSSTRtree_iterate_r(GEOSContextHandle_t extHandle,
                          GEOSSTRtree* tree,
                          GEOSQueryCallback callback,
                          void* userdata)
    {
        return execute(extHandle, [&]() {
            CAPI_ItemVisitor visitor(callback, userdata);
            tree->iterate(visitor);
        });
    }

    char
    GEOSSTRtree_remove_r(GEOSContextHandle_t extHandle,
                         GEOSSTRtree* tree,
                         const geos::geom::Geometry* g,
                         void* item) {
        return execute(extHandle, 2, [&]() {
            return tree->remove(g->getEnvelopeInternal(), item);
        });
    }

    void
    GEOSSTRtree_destroy_r(GEOSContextHandle_t extHandle,
                          GEOSSTRtree* tree)
    {
        return execute(extHandle, [&]() {
            delete tree;
        });
    }

    double
    GEOSProject_r(GEOSContextHandle_t extHandle,
                  const Geometry* g,
                  const Geometry* p)
    {
        return execute(extHandle, -1.0, [&]() {
            const geos::geom::Point* point = dynamic_cast<const geos::geom::Point*>(p);
            if(!point) {
                throw std::runtime_error("third argument of GEOSProject_r must be Point");
            }
            const geos::geom::Coordinate inputPt(*p->getCoordinate());
            return geos::linearref::LengthIndexedLine(g).project(inputPt);
        });
    }


    Geometry*
    GEOSInterpolate_r(GEOSContextHandle_t extHandle, const Geometry* g, double d)
    {
        return execute(extHandle, [&]() {
            GEOSContextHandleInternal_t* handle = reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);

            geos::linearref::LengthIndexedLine lil(g);
            geos::geom::Coordinate coord = lil.extractPoint(d);
            const GeometryFactory* gf = handle->geomFactory;
            auto point = coord.isNull() ? gf->createPoint(g->getCoordinateDimension()) : gf->createPoint(coord);
            point->setSRID(g->getSRID());
            return point.release();
        });
    }


    double
    GEOSProjectNormalized_r(GEOSContextHandle_t extHandle, const Geometry* g,
                            const Geometry* p)
    {

        double length;
        double distance;
        if(GEOSLength_r(extHandle, g, &length) != 1) {
            return -1.0;
        };

        distance = GEOSProject_r(extHandle, g, p);

        if (distance == 0.0 && length == 0.0)
            return 0.0;

        /* Meaningless projection? error */
        if (distance < 0.0 || ! std::isfinite(distance) || length == 0.0) {
            return -1.0;
        } else {
            return distance / length;
        }
    }


    Geometry*
    GEOSInterpolateNormalized_r(GEOSContextHandle_t extHandle, const Geometry* g,
                                double d)
    {
        double length;
        if (GEOSLength_r(extHandle, g, &length) != 1) {
            return 0;
        }
        return GEOSInterpolate_r(extHandle, g, d * length);
    }

    GEOSGeometry*
    GEOSGeom_extractUniquePoints_r(GEOSContextHandle_t extHandle,
                                   const GEOSGeometry* g)
    {
        using namespace geos::geom;
        using namespace geos::util;

        return execute(extHandle, [&]() {
            /* 1: extract points */
            std::vector<const Coordinate*> coords;
            UniqueCoordinateArrayFilter filter(coords);
            g->apply_ro(&filter);

            /* 2: for each point, create a geometry and put into a vector */
            std::vector<std::unique_ptr<Geometry>> points;
            points.reserve(coords.size());
            const GeometryFactory* factory = g->getFactory();
            for(std::vector<const Coordinate*>::iterator it = coords.begin(),
                    itE = coords.end();
                    it != itE; ++it) {
                auto point = factory->createPoint(*(*it));
                points.push_back(std::move(point));
            }

            /* 3: create a multipoint */
            auto out = factory->createMultiPoint(std::move(points));
            out->setSRID(g->getSRID());
            return out.release();

        });
    }

    int GEOSOrientationIndex_r(GEOSContextHandle_t extHandle,
                               double Ax, double Ay, double Bx, double By, double Px, double Py)
    {
        using geos::algorithm::Orientation;

        return execute(extHandle, 2, [&]() {
            Coordinate A(Ax, Ay);
            Coordinate B(Bx, By);
            Coordinate P(Px, Py);
            return Orientation::index(A, B, P);
        });
    }

    GEOSGeometry*
    GEOSSharedPaths_r(GEOSContextHandle_t extHandle, const GEOSGeometry* g1, const GEOSGeometry* g2)
    {
        using namespace geos::operation::sharedpaths;

        if(nullptr == extHandle) {
            return nullptr;
        }
        GEOSContextHandleInternal_t* handle =
            reinterpret_cast<GEOSContextHandleInternal_t*>(extHandle);
        if(handle->initialized == 0) {
            return nullptr;
        }

        SharedPathsOp::PathList forw, back;
        try {
            SharedPathsOp::sharedPathsOp(*g1, *g2, forw, back);
        }
        catch(const std::exception& e) {
            SharedPathsOp::clearEdges(forw);
            SharedPathsOp::clearEdges(back);
            handle->ERROR_MESSAGE("%s", e.what());
            return nullptr;
        }
        catch(...) {
            SharedPathsOp::clearEdges(forw);
            SharedPathsOp::clearEdges(back);
            handle->ERROR_MESSAGE("Unknown exception thrown");
            return nullptr;
        }

        // Now forw and back have the geoms we want to use to construct
        // our output GeometryCollections...

        const GeometryFactory* factory = g1->getFactory();
        std::size_t count;

        std::vector<std::unique_ptr<Geometry>> out1;
        count = forw.size();
        out1.reserve(count);
        for(std::size_t i = 0; i < count; ++i) {
            out1.emplace_back(forw[i]);
        }
        std::unique_ptr<Geometry> out1g(
            factory->createMultiLineString(std::move(out1))
        );

        std::vector<std::unique_ptr<Geometry>> out2;
        count = back.size();
        out2.reserve(count);
        for(std::size_t i = 0; i < count; ++i) {
            out2.emplace_back(back[i]);
        }
        std::unique_ptr<Geometry> out2g(
            factory->createMultiLineString(std::move(out2))
        );

        std::vector<std::unique_ptr<Geometry>> out;
        out.reserve(2);
        out.push_back(std::move(out1g));
        out.push_back(std::move(out2g));

        std::unique_ptr<Geometry> outg(
            factory->createGeometryCollection(std::move(out))
        );

        outg->setSRID(g1->getSRID());
        return outg.release();
    }

    GEOSGeometry*
    GEOSSnap_r(GEOSContextHandle_t extHandle, const GEOSGeometry* g1,
               const GEOSGeometry* g2, double tolerance)
    {
        using namespace geos::operation::overlay::snap;

        return execute(extHandle, [&]() {
            GeometrySnapper snapper(*g1);
            std::unique_ptr<Geometry> ret = snapper.snapTo(*g2, tolerance);
            ret->setSRID(g1->getSRID());
            return ret.release();
        });
    }

    BufferParameters*
    GEOSBufferParams_create_r(GEOSContextHandle_t extHandle)
    {
        return execute(extHandle, [&]() {
            return new BufferParameters();
        });
    }

    void
    GEOSBufferParams_destroy_r(GEOSContextHandle_t extHandle, BufferParameters* p)
    {
        (void)extHandle;
        delete p;
    }

    int
    GEOSBufferParams_setEndCapStyle_r(GEOSContextHandle_t extHandle,
                                      GEOSBufferParams* p, int style)
    {
        return execute(extHandle, 0, [&]() {
            if(style > BufferParameters::CAP_SQUARE) {
                throw IllegalArgumentException("Invalid buffer endCap style");
            }
            p->setEndCapStyle(static_cast<BufferParameters::EndCapStyle>(style));
            return 1;
        });
    }

    int
    GEOSBufferParams_setJoinStyle_r(GEOSContextHandle_t extHandle,
                                    GEOSBufferParams* p, int style)
    {
        return execute(extHandle, 0, [&]() {
            if(style > BufferParameters::JOIN_BEVEL) {
                throw IllegalArgumentException("Invalid buffer join style");
            }
            p->setJoinStyle(static_cast<BufferParameters::JoinStyle>(style));

            return 1;
        });
    }

    int
    GEOSBufferParams_setMitreLimit_r(GEOSContextHandle_t extHandle,
                                     GEOSBufferParams* p, double limit)
    {
        return execute(extHandle, 0, [&]() {
            p->setMitreLimit(limit);
            return 1;
        });
    }

    int
    GEOSBufferParams_setQuadrantSegments_r(GEOSContextHandle_t extHandle,
                                           GEOSBufferParams* p, int segs)
    {
        return execute(extHandle, 0, [&]() {
            p->setQuadrantSegments(segs);
            return 1;
        });
    }

    int
    GEOSBufferParams_setSingleSided_r(GEOSContextHandle_t extHandle,
                                      GEOSBufferParams* p, int ss)
    {
        return execute(extHandle, 0, [&]() {
            p->setSingleSided((ss != 0));
            return 1;
        });
    }

    Geometry*
    GEOSBufferWithParams_r(GEOSContextHandle_t extHandle, const Geometry* g1, const BufferParameters* bp, double width)
    {
        using geos::operation::buffer::BufferOp;

        return execute(extHandle, [&]() {
            BufferOp op(g1, *bp);
            std::unique_ptr<Geometry> g3 = op.getResultGeometry(width);
            g3->setSRID(g1->getSRID());
            return g3.release();
        });
    }

    Geometry*
    GEOSDelaunayTriangulation_r(GEOSContextHandle_t extHandle, const Geometry* g1, double tolerance, int onlyEdges)
    {
        using geos::triangulate::DelaunayTriangulationBuilder;

        return execute(extHandle, [&]() -> Geometry* {
            DelaunayTriangulationBuilder builder;
            builder.setTolerance(tolerance);
            builder.setSites(*g1);

            if(onlyEdges) {
                Geometry* out = builder.getEdges(*g1->getFactory()).release();
                out->setSRID(g1->getSRID());
                return out;
            }
            else {
                Geometry* out = builder.getTriangles(*g1->getFactory()).release();
                out->setSRID(g1->getSRID());
                return out;
            }
        });
    }

    Geometry*
    GEOSConstrainedDelaunayTriangulation_r(GEOSContextHandle_t extHandle, const Geometry* g1)
    {
        using geos::triangulate::polygon::ConstrainedDelaunayTriangulator;

        return execute(extHandle, [&]() -> Geometry* {
            return ConstrainedDelaunayTriangulator::triangulate(g1).release();
        });
    }

    Geometry*
    GEOSVoronoiDiagram_r(GEOSContextHandle_t extHandle, const Geometry* g1, const Geometry* env, double tolerance,
                         int flags)
    {
        using geos::triangulate::VoronoiDiagramBuilder;

        return execute(extHandle, [&]() -> Geometry* {
            VoronoiDiagramBuilder builder;
            builder.setSites(*g1);
            builder.setTolerance(tolerance);
            builder.setOrdered(flags & GEOS_VORONOI_PRESERVE_ORDER);
            std::unique_ptr<Geometry> out;
            if(env) {
                builder.setClipEnvelope(env->getEnvelopeInternal());
            }
            if(flags & GEOS_VORONOI_ONLY_EDGES) {
                out = builder.getDiagramEdges(*g1->getFactory());
            }
            else {
                out = builder.getDiagram(*g1->getFactory());
            }

            out->setSRID(g1->getSRID());
            return out.release();
        });
    }

    int
    GEOSSegmentIntersection_r(GEOSContextHandle_t extHandle,
                              double ax0, double ay0, double ax1, double ay1,
                              double bx0, double by0, double bx1, double by1,
                              double* cx, double* cy)
    {
        return execute(extHandle, 0, [&]() {
            geos::geom::LineSegment a(ax0, ay0, ax1, ay1);
            geos::geom::LineSegment b(bx0, by0, bx1, by1);
            geos::geom::Coordinate isect = a.intersection(b);

            if(isect.isNull()) {
                return -1;
            }

            *cx = isect.x;
            *cy = isect.y;

            return 1;
        });
    }

    int
    GEOSCoverageIsValid_r(GEOSContextHandle_t extHandle,
        const Geometry* input,
        double gapWidth,
        Geometry** invalidEdges)
    {
        using geos::coverage::CoverageValidator;

        return execute(extHandle, 2, [&]() {
            const GeometryCollection* col = dynamic_cast<const GeometryCollection*>(input);
            if (!col)
                throw geos::util::IllegalArgumentException("input is not a collection");

            // Initialize to nullptr
            if (invalidEdges) *invalidEdges = nullptr;

            std::vector<const Geometry*> coverage;
            for (const auto& g : *col) {
                coverage.push_back(g.get());
            }

            CoverageValidator cov(coverage);
            cov.setGapWidth(gapWidth);
            std::vector<std::unique_ptr<Geometry>> invalid = cov.validate();
            bool hasInvalid = CoverageValidator::hasInvalidResult(invalid);

            if (invalidEdges) {
                const GeometryFactory* gf = input->getFactory();
                for (auto& g : invalid) {
                    // Replace nullptr with 'MULTILINESTRING EMPTY'
                    if (g == nullptr) {
                        auto empty = gf->createEmpty(1);
                        g.reset(empty.release());
                    }
                }
                auto r = gf->createGeometryCollection(std::move(invalid));
                *invalidEdges = r.release();
            }

            return hasInvalid ? 0 : 1;
        });
    }

    Geometry*
    GEOSCoverageSimplifyVW_r(GEOSContextHandle_t extHandle,
        const Geometry* input,
        double tolerance,
        int preserveBoundary)
    {
        using geos::coverage::CoverageSimplifier;

        return execute(extHandle, [&]() -> Geometry* {
            const GeometryCollection* col = dynamic_cast<const GeometryCollection*>(input);
            if (!col)
                return nullptr;

            std::vector<const Geometry*> coverage;
            for (const auto& g : *col) {
                coverage.push_back(g.get());
            }
            CoverageSimplifier cov(coverage);
            std::vector<std::unique_ptr<Geometry>> simple;
            if (preserveBoundary == 1) {
                simple = cov.simplifyInner(tolerance);
            }
            else if (preserveBoundary == 0) {
                simple = cov.simplify(tolerance);
            }
            else return nullptr;

            const GeometryFactory* gf = input->getFactory();
            std::unique_ptr<Geometry> r = gf->createGeometryCollection(std::move(simple));
            return r.release();
        });
    }




} /* extern "C" */
