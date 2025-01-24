/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2012 Excensus LLC.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: triangulate/quadedge/TrianglePredicate.java r524
 *
 **********************************************************************/

#include <geos/triangulate/quadedge/TrianglePredicate.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/Location.h>

using geos::geom::Coordinate;

namespace geos {
namespace triangulate {
namespace quadedge {

geom::Location
TrianglePredicate::isInCircleNonRobust(
    const CoordinateXY& p, const CoordinateXY& q, const CoordinateXY& r,
    const CoordinateXY& t)
{
    auto qpx = q.x - p.x;
    auto qpy = q.y - p.y;
    auto rpx = r.x - p.x;
    auto rpy = r.y - p.y;
    auto tpx = t.x - p.x;
    auto tpy = t.y - p.y;
    auto tqx = t.x - q.x;
    auto tqy = t.y - q.y;
    auto rqx = r.x - q.x;
    auto rqy = r.y - q.y;
    auto qpxtpy = qpx * tpy;
    auto qpytpx = qpy * tpx;
    auto tpxtqx = tpx * tqx;
    auto tpytqy = tpy * tqy;
    auto qpxrpy = qpx * rpy;
    auto qpyrpx = qpy * rpx;
    auto rpxrqx = rpx * rqx;
    auto rpyrqy = rpy * rqy;
    auto det = (qpxtpy - qpytpx) * (rpxrqx + rpyrqy)
               - (qpxrpy - qpyrpx) * (tpxtqx + tpytqy);
    return static_cast<geom::Location>( (det > 0) - (det < 0) + 1 );
}

geom::Location
TrianglePredicate::isInCircleNormalized(
    const CoordinateXY& a, const CoordinateXY& b, const CoordinateXY& c,
    const CoordinateXY& p)
{
    // Unfortunately this implementation is not robust either. For robust one see:
    // https://www.cs.cmu.edu/~quake/robust.html
    // https://www.cs.cmu.edu/afs/cs/project/quake/public/code/predicates.c

    long double adx = static_cast<long double>(a.x) - static_cast<long double>(p.x);
    long double ady = static_cast<long double>(a.y) - static_cast<long double>(p.y);
    long double bdx = static_cast<long double>(b.x) - static_cast<long double>(p.x);
    long double bdy = static_cast<long double>(b.y) - static_cast<long double>(p.y);
    long double cdx = static_cast<long double>(c.x) - static_cast<long double>(p.x);
    long double cdy = static_cast<long double>(c.y) - static_cast<long double>(p.y);

    long double bdxcdy = bdx * cdy;
    long double cdxbdy = cdx * bdy;
    long double alift = adx * adx + ady * ady;

    long double cdxady = cdx * ady;
    long double adxcdy = adx * cdy;
    long double blift = bdx * bdx + bdy * bdy;

    long double adxbdy = adx * bdy;
    long double bdxady = bdx * ady;
    long double clift = cdx * cdx + cdy * cdy;

    long double A = (alift * bdxcdy + blift * cdxady + clift * adxbdy);
    long double B = (alift * cdxbdy + blift * adxcdy + clift * bdxady);

    if (A < B) {
        return geom::Location::EXTERIOR;
    } else if (A == B) {
        return geom::Location::BOUNDARY;
    } else {
        return geom::Location::INTERIOR;
    }
}

double
TrianglePredicate::triArea(const CoordinateXY& a,
                           const CoordinateXY& b, const CoordinateXY& c)
{
    return (b.x - a.x) * (c.y - a.y)
           - (b.y - a.y) * (c.x - a.x);
}

geom::Location
TrianglePredicate::isInCircleRobust(
    const CoordinateXY& q, const CoordinateXY& p, const CoordinateXY& r,
    const CoordinateXY& t)
{
    // This implementation is not robust and defaults to BOUNDARY in case of
    // uncertainty.
    auto qpx = q.x - p.x;
    auto qpy = q.y - p.y;
    auto rpx = r.x - p.x;
    auto rpy = r.y - p.y;
    auto tpx = t.x - p.x;
    auto tpy = t.y - p.y;
    auto tqx = t.x - q.x;
    auto tqy = t.y - q.y;
    auto rqx = r.x - q.x;
    auto rqy = r.y - q.y;
    auto qpxtpy = qpx * tpy;
    auto qpytpx = qpy * tpx;
    auto tpxtqx = tpx * tqx;
    auto tpytqy = tpy * tqy;
    auto qpxrpy = qpx * rpy;
    auto qpyrpx = qpy * rpx;
    auto rpxrqx = rpx * rqx;
    auto rpyrqy = rpy * rqy;
    auto det = (qpxtpy - qpytpx) * (rpxrqx + rpyrqy)
               - (qpxrpy - qpyrpx) * (tpxtqx + tpytqy);
    auto deterror = ((std::abs(qpxtpy) + std::abs(qpytpx))
                    * (std::abs(rpxrqx) + std::abs(rpyrqy))
                    + (std::abs(qpxrpy) + std::abs(qpyrpx))
                    * (std::abs(tpxtqx) + std::abs(tpytqy)))
                    * 9.99200719823023e-16;
    return static_cast<geom::Location>( (det > deterror) - (det < -deterror) + 1 );
}

} // namespace geos.triangulate.quadedge
} // namespace geos.triangulate
} // namespace geos
