//
// GEOS Unit Test utilities, extension of TUT Framework namespace
//
#pragma once


// tut
#include <tut/tut.hpp>
// geos
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Dimension.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiPoint.h>
#include <geos/geom/MultiLineString.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
// std
#include <memory>
#include <cstdlib>
#include <cassert>
#include <string>
#include <vector>

using geos::geom::Coordinate;
using geos::geom::CoordinateSequence;
using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::geom::LineString;
using geos::geom::LinearRing;
using geos::geom::Polygon;
using geos::io::WKTReader;
using geos::io::WKTWriter;

namespace tut {

//
// Helper typedefs
//
typedef geos::geom::Coordinate* CoordinatePtr;
typedef geos::geom::Coordinate const* CoordinateCPtr;

typedef geos::geom::CoordinateSequence* CoordSeqPtr;
typedef geos::geom::CoordinateSequence const* CoordSeqCPtr;

typedef geos::geom::Geometry* GeometryPtr;
typedef geos::geom::Geometry const* GeometryCPtr;

typedef geos::geom::Point* PointPtr;
typedef geos::geom::Point const* PointCPtr;
typedef geos::geom::LinearRing* LinearRingPtr;
typedef geos::geom::LinearRing const* LinearRingCPtr;
typedef geos::geom::LineString* LineStringPtr;
typedef geos::geom::LineString const* LineStringCPtr;
typedef geos::geom::Polygon* PolygonPtr;
typedef geos::geom::Polygon const* PolygonCPtr;

typedef geos::geom::GeometryCollection* GeometryColPtr;
typedef geos::geom::GeometryCollection const* GeometryColCPtr;

typedef geos::geom::MultiPoint* MultiPointPtr;
typedef geos::geom::MultiPoint const* MultiPointCPtr;
typedef geos::geom::MultiLineString* MultiLineStringPtr;
typedef geos::geom::MultiLineString const* MultiLineStringCPtr;
typedef geos::geom::MultiPolygon* MultiPolygonPtr;
typedef geos::geom::MultiPolygon const* MultiPolygonCPtr;

// prepared geometries always returned as const
typedef geos::geom::prep::PreparedGeometry const* PreparedGeometryPtr;

//
// Type cast helper utilities
//

template<typename Type, typename InstanceType>
inline bool
isInstanceOf(InstanceType const* instance)
{
    assert(nullptr != instance);
    return (nullptr != dynamic_cast<Type const*>(instance));
}

template<typename Type, typename InstanceType>
inline Type const*
instanceOf(InstanceType const* instance)
{
    assert(nullptr != instance);
    return dynamic_cast<Type const*>(instance);
}

inline void
ensure_equals_xy(geos::geom::CoordinateXY const& actual,
                  geos::geom::CoordinateXY const& expected)
{
    ensure_equals("Coordinate X", actual.x, expected.x );
    ensure_equals("Coordinate Y", actual.y, expected.y );
}

inline void
ensure_equals_xy(geos::geom::CoordinateXY const& actual,
                  geos::geom::CoordinateXY const& expected,
                  double tol)
{
    ensure_equals("Coordinate X", actual.x, expected.x, tol );
    ensure_equals("Coordinate Y", actual.y, expected.y, tol );
}

inline void
ensure_equals_xyz(geos::geom::Coordinate const& actual,
                  geos::geom::Coordinate const& expected)
{
    ensure_equals("Coordinate X", actual.x, expected.x );
    ensure_equals("Coordinate Y", actual.y, expected.y );
    if ( std::isnan(expected.z) ) {
        ensure("Coordinate Z should be NaN", std::isnan(actual.z) );
    } else {
        ensure_equals("Coordinate Z", actual.z, expected.z );
    }
}

inline void
ensure_equals_xym(geos::geom::CoordinateXYM const& actual,
                  geos::geom::CoordinateXYM const& expected)
{
    ensure_equals("Coordinate X", actual.x, expected.x );
    ensure_equals("Coordinate Y", actual.y, expected.y );
    if ( std::isnan(expected.m) ) {
        ensure("Coordinate M should be NaN", std::isnan(actual.m) );
    } else {
        ensure_equals("Coordinate M", actual.m, expected.m );
    }
}

inline void
ensure_equals_xyzm(geos::geom::CoordinateXYZM const& actual,
                   geos::geom::CoordinateXYZM const& expected)
{
    ensure_equals("Coordinate X", actual.x, expected.x );
    ensure_equals("Coordinate Y", actual.y, expected.y );
    if ( std::isnan(expected.z) ) {
        ensure("Coordinate Z should be NaN", std::isnan(actual.z) );
    } else {
        ensure_equals("Coordinate Z", actual.z, expected.z );
    }
    if ( std::isnan(expected.m) ) {
        ensure("Coordinate M should be NaN", std::isnan(actual.m) );
    } else {
        ensure_equals("Coordinate M", actual.m, expected.m );
    }
}

inline void ensure_same(const char* msg, double a, double b)
{
    if (std::isnan(a) && std::isnan(b)) {
        return;
    }

    ensure_equals(msg, a, b);
}

inline void ensure_same(double a, double b)
{
    ensure_same("values are not equal", a, b);
}

//
// Geometries structure comparators
//





template <typename T1, typename T2>
inline void
ensure_equals_geometry(T1 const* lhs, T2 const* rhs)
{
    assert(0 != lhs);
    assert(0 != rhs);
    assert(!"DIFFERENT TYPES ARE NOT OF THE SAME STRUCTURE");
    ensure(lhs != 0 && rhs != 0 && lhs != rhs);
}


template <typename T>
inline void
ensure_equals_geometry(T const* lhs_in, T const* rhs_in, double tolerance = 0.0)
{
    assert(nullptr != lhs_in);
    assert(nullptr != rhs_in);

    using geos::geom::Polygon;
    using geos::geom::GeometryCollection;
    using geos::io::WKTWriter;

    // Take clones so we can normalize them
    std::unique_ptr<geos::geom::Geometry> lhs = lhs_in->clone();
    std::unique_ptr<geos::geom::Geometry> rhs = rhs_in->clone();
    lhs->normalize();
    rhs->normalize();

    ensure_equals("is-valid do not match",
                  lhs->isValid(), rhs->isValid());

    ensure_equals("is-empty do not match",
                  lhs->isEmpty(), rhs->isEmpty());

    if(!isInstanceOf<GeometryCollection>(lhs.get()) &&
       !isInstanceOf<GeometryCollection>(rhs.get())) {
        ensure_equals("is-simple do not match",
                      lhs->isSimple(), rhs->isSimple());
    }

    ensure_equals("type do not match",
                  lhs->getGeometryType(), rhs->getGeometryType());

    ensure_equals("type id do not match",
                  lhs->getGeometryTypeId(), rhs->getGeometryTypeId());

    ensure_equals("dimension do not match",
                  lhs->getDimension(), rhs->getDimension());

    ensure_equals("boundary dimension do not match",
                  lhs->getBoundaryDimension(), rhs->getBoundaryDimension());

    bool areaNumPointsEqual = lhs->getNumPoints() == rhs->getNumPoints();
    bool areCoordsEqual = lhs->equalsExact(rhs.get(), tolerance);

    if(! (areCoordsEqual && areaNumPointsEqual)) {
        WKTWriter writer;
        std::cout << std::endl
            << writer.write(*rhs) << std::endl
            << writer.write(*lhs) << std::endl;
    }

    ensure("number of points do not match", areaNumPointsEqual);

    ensure("coordinates do not match", areCoordsEqual);

    // Dispatch to run more specific testes
    // if(isInstanceOf<Polygon>(lhs)
    //         && isInstanceOf<Polygon>(rhs)) {
    //     ensure_equals_geometry(instanceOf<Polygon>(lhs),
    //                            instanceOf<Polygon>(rhs));
    // }
    // else if(isInstanceOf<GeometryCollection>(lhs)
    //         && isInstanceOf<GeometryCollection>(rhs)) {
    //     ensure_equals_geometry(instanceOf<GeometryCollection>(lhs),
    //                            instanceOf<GeometryCollection>(rhs));
    // }
}

template<typename T>
inline void
ensure_equals_geometry(const T& lhs, const char* rhs, double tolerance = 0.0) {
    geos::io::WKTReader reader(lhs->getFactory());
    auto rhs_geom = reader.read(rhs);
    ensure_equals_geometry(lhs, rhs_geom.get(), tolerance);
}

template<typename T>
inline void
ensure_equals_geometry(const T& lhs, const std::string& rhs, double tolerance = 0.0) {
    geos::io::WKTReader reader(lhs->getFactory());
    auto rhs_geom = reader.read(rhs);
    ensure_equals_geometry(lhs, rhs_geom.get(), tolerance);
}

template <>
inline void
ensure_equals_geometry(geos::geom::Polygon const* lhs,
                       geos::geom::Polygon const* rhs)
{
    assert(nullptr != lhs);
    assert(nullptr != rhs);

    ensure_equals("number of interior ring do not match",
                  lhs->getNumInteriorRing(), rhs->getNumInteriorRing());
}

template <>
inline void
ensure_equals_geometry(geos::geom::GeometryCollection const* lhs,
                       geos::geom::GeometryCollection const* rhs)
{
    assert(nullptr != lhs);
    assert(nullptr != rhs);

    using geos::geom::Geometry;

    ensure_equals("number of geometries do not match",
                  lhs->getNumGeometries(), rhs->getNumGeometries());

    for(std::size_t i = 0, n = lhs->getNumGeometries(); i < n; ++i) {
        Geometry const* g1 = lhs->getGeometryN(i);
        Geometry const* g2 = rhs->getGeometryN(i);
        ensure_equals_geometry(g1, g2); // breaks on failure
    }
}

template <>
inline void
ensure_equals_geometry(geos::geom::Geometry const* lhs,
                       geos::geom::prep::PreparedGeometry const* rhs)
{
    assert(nullptr != lhs);
    assert(nullptr != rhs);

    geos::geom::Geometry const& pg = rhs->getGeometry();
    ensure_equals_geometry(lhs, &pg);
}

template <typename T>
inline void
ensure_equals_dims(T const *, T const *,
                   unsigned int dims,
                   double tolerance = 0.0);

template <>
inline void
ensure_equals_dims(const geos::geom::CoordinateSequence* seq1,
                   const geos::geom::CoordinateSequence* seq2,
                   unsigned int dims, double tolerance)
{
    assert(nullptr != seq1);
    assert(nullptr != seq2);

    ensure_equals (seq1->size(), seq2->size());

    ensure( seq1->getDimension() >= dims );
    ensure( seq2->getDimension() >= dims );

    for (unsigned int i = 0; i < seq1->size(); i++) {
      for (unsigned int j = 0; j < dims; j++) {
        double val1 = seq1->getOrdinate(i, j);
        double val2 = seq2->getOrdinate(i, j);
        if ( std::isnan(val1) )
        {
            ensure( std::isnan(val2) );
        }
        else
        {
            if ( tolerance > 0.0 )
                ensure_distance( val1, val2, tolerance );
            else
                ensure_equals( val1, val2 );
        }
      }
    }
}

template <typename T> inline void ensure_equals_exact_geometry_xyz(const T *lhs_in, const T *rhs_in, double tolerance = 0.0);

template <>
inline void
ensure_equals_exact_geometry_xyz(const geos::geom::Geometry *lhs_in,
                                 const geos::geom::Geometry *rhs_in,
                                 double tolerance)
{
    assert(nullptr != lhs_in);
    assert(nullptr != rhs_in);

    using geos::geom::Point;
    using geos::geom::LineString;
    using geos::geom::Polygon;
    using geos::geom::CoordinateSequence;
    using geos::geom::GeometryCollection;

    ensure_equals("type id do not match",
                  lhs_in->getGeometryTypeId(), rhs_in->getGeometryTypeId());


    if (const Point* gpt1 = dynamic_cast<const Point *>(lhs_in)) {
      const Point *gpt2 = static_cast<const Point *>(rhs_in);
      return ensure_equals_dims( gpt1->getCoordinatesRO(), gpt2->getCoordinatesRO(), 3, tolerance);
    }
    else if (const LineString* gln1 = dynamic_cast<const LineString *>(lhs_in)) {
      const LineString *gln2 = static_cast<const LineString *>(rhs_in);
      return ensure_equals_dims( gln1->getCoordinatesRO(), gln2->getCoordinatesRO(), 3, tolerance);
    }
    else if (dynamic_cast<const Polygon *>(lhs_in)) {
      ensure("Not implemented yet", 0);
    }
    else if (const GeometryCollection* gc1 = dynamic_cast<const GeometryCollection *>(lhs_in)) {
      const GeometryCollection *gc2 = static_cast<const GeometryCollection *>(rhs_in);
      for (unsigned int i = 0; i < gc1->getNumGeometries(); i++) {
        ensure_equals_exact_geometry_xyz(gc1->getGeometryN(i), gc2->getGeometryN(i), tolerance);
      }
    }
}

inline void
ensure_equals_exact_xyzm(const geos::geom::CoordinateSequence* seq1,
             const geos::geom::CoordinateSequence* seq2,
             double tol)
{
    ensure_equals("hasZ not equal", seq1->hasZ(), seq2->hasZ());
    ensure_equals("hasM not equal", seq1->hasM(), seq2->hasM());
    ensure_equals("size not equal", seq1->getSize(), seq2->getSize());

    geos::geom::CoordinateXYZM c1, c2;
    for (std::size_t i = 0; i < seq1->getSize(); i++) {
        seq1->getAt(i, c1);
        seq2->getAt(i, c2);

        ensure("xy not in tolerance", c1.distance(c2) <= tol);
        ensure_same("z not same", c1.z, c2.z);
        ensure_same("z not same", c1.m, c2.m);
    }
}

inline void
ensure_equals_exact_geometry_xyzm(const geos::geom::Geometry *lhs_in,
                                  const geos::geom::Geometry *rhs_in,
                                  double tolerance)
{
    assert(nullptr != lhs_in);
    assert(nullptr != rhs_in);

    using geos::geom::Point;
    using geos::geom::LineString;
    using geos::geom::Polygon;
    using geos::geom::CoordinateSequence;
    using geos::geom::GeometryCollection;

    ensure_equals("type id do not match",
                  lhs_in->getGeometryTypeId(), rhs_in->getGeometryTypeId());

    if (const Point* gpt1 = dynamic_cast<const Point *>(lhs_in)) {
      const Point *gpt2 = static_cast<const Point *>(rhs_in);
      return ensure_equals_exact_xyzm(gpt1->getCoordinatesRO(), gpt2->getCoordinatesRO(), tolerance);
    }
    else if (const LineString* gln1 = dynamic_cast<const LineString *>(lhs_in)) {
      const LineString *gln2 = static_cast<const LineString *>(rhs_in);
      return ensure_equals_exact_xyzm(gln1->getCoordinatesRO(), gln2->getCoordinatesRO(), tolerance);
    }
    else if (const Polygon* gply1 = dynamic_cast<const Polygon*>(lhs_in)) {
      const Polygon* gply2 = static_cast<const Polygon*>(rhs_in);
      const LinearRing* extRing1 = gply1->getExteriorRing();
      const LinearRing* extRing2 = gply2->getExteriorRing();

      ensure_equals_exact_geometry_xyzm(extRing1, extRing2, tolerance);

      ensure_equals("number of holes does not match",
                    gply1->getNumInteriorRing(),
                    gply2->getNumInteriorRing());

      for (std::size_t i = 0; i < gply1->getNumInteriorRing(); i++) {
        ensure_equals_exact_geometry_xyzm(gply1->getInteriorRingN(i),
                                    gply2->getInteriorRingN(i),
                                    tolerance);
      }
    }
    else if (const GeometryCollection* gc1 = dynamic_cast<const GeometryCollection *>(lhs_in)) {
      const GeometryCollection *gc2 = static_cast<const GeometryCollection *>(rhs_in);
      for (unsigned int i = 0; i < gc1->getNumGeometries(); i++) {
        ensure_equals_exact_geometry_xyzm(gc1->getGeometryN(i), gc2->getGeometryN(i), tolerance);
      }
    }
}

template <typename T>
inline void
ensure_equals_geometry_xyz(const T *lhs_in,
                           const T *rhs_in,
                           double tolerance=0.0)
{
    std::unique_ptr<geos::geom::Geometry> g1 = lhs_in->clone();
    g1->normalize();
    std::unique_ptr<geos::geom::Geometry> g2 = rhs_in->clone();
    g2->normalize();
    ensure_equals_exact_geometry_xyz(g1.get(), g2.get(), tolerance);
}

template <typename T>
inline void
ensure_equals_geometry_xyzm(const T *lhs_in,
                           const T *rhs_in,
                           double tolerance=0.0)
{
    ensure_equals("hasZ is not consistent", lhs_in->hasZ(), rhs_in->hasZ());
    ensure_equals("hasM is not consistent", lhs_in->hasM(), rhs_in->hasM());

    std::unique_ptr<geos::geom::Geometry> g1 = lhs_in->clone();
    g1->normalize();
    std::unique_ptr<geos::geom::Geometry> g2 = rhs_in->clone();
    g2->normalize();

    ensure_equals_exact_geometry_xyzm(g1.get(), g2.get(), tolerance);
}

/*
 * Checks for geometries exactly equal in XY only
 */

template <typename T> inline void ensure_equals_exact_geometry(const T *lhs_in, const T *rhs_in, double tolerance = 0.0);

template <>
inline void
ensure_equals_exact_geometry(const geos::geom::Geometry *lhs_in,
                                 const geos::geom::Geometry *rhs_in,
                                 double tolerance)
{
    assert(nullptr != lhs_in);
    assert(nullptr != rhs_in);

    using geos::geom::Point;
    using geos::geom::LineString;
    using geos::geom::Polygon;
    using geos::geom::CoordinateSequence;
    using geos::geom::GeometryCollection;

    ensure_equals("type id do not match",
                  lhs_in->getGeometryTypeId(), rhs_in->getGeometryTypeId());

    if (const Point* gpt1 = dynamic_cast<const Point *>(lhs_in)) {
      const Point *gpt2 = static_cast<const Point *>(rhs_in);
      return ensure_equals_dims( gpt1->getCoordinatesRO(), gpt2->getCoordinatesRO(), 2, tolerance);
    }
    else if (const LineString* gln1 = dynamic_cast<const LineString *>(lhs_in)) {
      const LineString *gln2 = static_cast<const LineString *>(rhs_in);
      return ensure_equals_dims( gln1->getCoordinatesRO(), gln2->getCoordinatesRO(), 2, tolerance);
    }
    else if (dynamic_cast<const Polygon *>(lhs_in)) {
      ensure("Not implemented yet", 0);
    }
    else if (const GeometryCollection* gc1 = dynamic_cast<const GeometryCollection *>(lhs_in)) {
      const GeometryCollection *gc2 = static_cast<const GeometryCollection *>(rhs_in);
      for (unsigned int i = 0; i < gc1->getNumGeometries(); i++) {
        ensure_equals_exact_geometry(gc1->getGeometryN(i), gc2->getGeometryN(i), tolerance);
      }
    }
}

//
// Utility functions
//

// Decodes hex-encoded WKB/EWKB to raw binary.
struct wkb_hex_decoder {
    typedef std::vector<unsigned char> binary_type;

    // bytes [out] - buffer for binary output
    static void
    decode(std::string const& hexstr, binary_type& bytes)
    {
        bytes.clear();
        for(std::string::size_type i = 0; i < hexstr.size() / 2; ++i) {
            std::istringstream iss(hexstr.substr(i * 2, 2));
            unsigned int n;
            iss >> std::hex >> n;
            bytes.push_back(static_cast<unsigned char>(n));
        }
    }
};


} // namespace tut

