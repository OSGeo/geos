/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2009-2011 Sandro Santilli <strk@kbt.io>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: algorithm/Angle.java r378 (JTS-1.12)
 *
 **********************************************************************/

#include <cmath>

#include <geos/algorithm/Angle.h>
#include <geos/geom/Coordinate.h>

namespace geos {
namespace algorithm { // geos.algorithm


/* public static */
double
Angle::toDegrees(double radians)
{
    return (radians * 180) / (MATH_PI);
}

/* public static */
double
Angle::toRadians(double angleDegrees)
{
    return (angleDegrees * MATH_PI) / 180.0;
}

/* public static */
double
Angle::angle(const geom::CoordinateXY& p0,
             const geom::CoordinateXY& p1)
{
    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;
    return atan2(dy, dx);
}

/* public static */
double
Angle::angle(const geom::CoordinateXY& p)
{
    return atan2(p.y, p.x);
}

/* public static */
bool
Angle::isAcute(const geom::CoordinateXY& p0,
               const geom::CoordinateXY& p1,
               const geom::CoordinateXY& p2)
{
    // relies on fact that A dot B is positive iff A ang B is acute
    double dx0 = p0.x - p1.x;
    double dy0 = p0.y - p1.y;
    double dx1 = p2.x - p1.x;
    double dy1 = p2.y - p1.y;
    double dotprod = dx0 * dx1 + dy0 * dy1;
    return dotprod > 0;
}

/* public static */
bool
Angle::isObtuse(const geom::CoordinateXY& p0,
                const geom::CoordinateXY& p1,
                const geom::CoordinateXY& p2)
{
    // relies on fact that A dot B is negative iff A ang B is obtuse
    double dx0 = p0.x - p1.x;
    double dy0 = p0.y - p1.y;
    double dx1 = p2.x - p1.x;
    double dy1 = p2.y - p1.y;
    double dotprod = dx0 * dx1 + dy0 * dy1;
    return dotprod < 0;
}

/* public static */
double
Angle::angleBetween(const geom::CoordinateXY& tip1,
                    const geom::CoordinateXY& tail,
                    const geom::CoordinateXY& tip2)
{
    double a1 = angle(tail, tip1);
    double a2 = angle(tail, tip2);

    return diff(a1, a2);
}

/* public static */
double
Angle::angleBetweenOriented(const geom::CoordinateXY& tip1,
                            const geom::CoordinateXY& tail,
                            const geom::CoordinateXY& tip2)
{
    double a1 = angle(tail, tip1);
    double a2 = angle(tail, tip2);
    double angDel = a2 - a1;

    // normalize, maintaining orientation
    if(angDel <= -MATH_PI) {
        return angDel + PI_TIMES_2;
    }
    if(angDel > MATH_PI) {
        return angDel - PI_TIMES_2;
    }
    return angDel;
}

/* public static */
double
Angle::interiorAngle(const geom::CoordinateXY& p0, const geom::CoordinateXY& p1,
                     const geom::CoordinateXY& p2)
{
    double anglePrev = angle(p1, p0);
    double angleNext = angle(p1, p2);
    return normalizePositive(angleNext - anglePrev);
}

/* public static */
int
Angle::getTurn(double ang1, double ang2)
{
    double crossproduct = sin(ang2 - ang1);

    if(crossproduct > 0) {
        return COUNTERCLOCKWISE;
    }
    if(crossproduct < 0) {
        return CLOCKWISE;
    }
    return NONE;
}

/* public static */
double
Angle::normalize(double angle)
{
    while(angle > MATH_PI) {
        angle -= PI_TIMES_2;
    }
    while(angle <= -MATH_PI) {
        angle += PI_TIMES_2;
    }
    return angle;
}

/* public static */
double
Angle::normalizePositive(double angle)
{
    if(angle < 0.0) {
        while(angle < 0.0) {
            angle += PI_TIMES_2;
        }
        // in case round-off error bumps the value over
        if(angle >= PI_TIMES_2) {
            angle = 0.0;
        }
    }
    else {
        while(angle >= PI_TIMES_2) {
            angle -= PI_TIMES_2;
        }
        // in case round-off error bumps the value under
        if(angle < 0.0) {
            angle = 0.0;
        }
    }
    return angle;
}

bool
Angle::isWithinCCW(double x, double a, double b) {
    if (b > a) {
        return x >= a && x <= b;
    } else {
        return x >= a || x <= b;
    }
}

/* public static */
double
Angle::diff(double ang1, double ang2)
{
    double delAngle;

    if(ang1 < ang2) {
        delAngle = ang2 - ang1;
    }
    else {
        delAngle = ang1 - ang2;
    }

    if(delAngle > MATH_PI) {
        delAngle = PI_TIMES_2 - delAngle;
    }

    return delAngle;
}

} // namespace geos.algorithm
} //namespace geos

