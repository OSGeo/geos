/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2024 ISciences, LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/algorithm/Angle.h>
#include <geos/algorithm/CircularArcIntersector.h>
#include <geos/algorithm/LineIntersector.h>

namespace geos::algorithm {

static double
nextAngleCCW(double from, double a, double b)
{
    if (Angle::normalizePositive(a - from) < Angle::normalizePositive(b - from)) {
        return a;
    }
    else {
        return b;
    }
}

int
CircularArcIntersector::circleIntersects(const CoordinateXY& center, double r, const CoordinateXY& p0, const CoordinateXY& p1, CoordinateXY& ret0, CoordinateXY& ret1)
{
    const double& x0 = center.x;
    const double& y0 = center.y;

    Envelope segEnv(p0, p1);

    CoordinateXY isect0, isect1;
    int n = 0;

    if (p1.x == p0.x) {
        // vertical line
        double x = p1.x;

        double A = 1;
        double B = 2*y0;
        double C = x*x - 2*x*x0 + x0*x0 + y0*y0 - r*r;

        double d = std::sqrt(B*B - 4*A*C);
        double Y1 = (-B + d)/(2*A);
        double Y2 = (-B - d)/(2*A);

        isect0 = {x, Y1};
        isect1 = {x, Y2};
    }
    else {
        double m = (p1.y - p0.y) / (p1.x - p0.x);
        double b = p1.y - p1.x*m;

        // Ax^2 + Bx + C = 0
        double A = 1 + m*m;
        double B = -2*x0 + 2*m*b - 2*m*y0;
        double C = x0*x0 + b*b - 2*b*y0 + y0*y0 - r*r;

        // TODO robust quadratic equation
        double d = std::sqrt(B*B - 4*A*C);
        double X1 = (-B + d)/(2*A);
        double X2 = (-B - d)/(2*A);

        isect0 = {X1, m* X1 + b};
        isect1 = {X2, m* X2 + b};
    }

    if (segEnv.intersects(isect0)) {
        ret0 = isect0;
        if (segEnv.intersects(isect1) && !isect1.equals2D(isect0)) {
            ret1 = isect1;
            n = 2;
        } else {
            n = 1;
        }
    } else if (segEnv.intersects(isect1)) {
        ret0 = isect1;
        n = 1;
    }

    return n;
}

void
CircularArcIntersector::intersects(const CircularArc& arc, const CoordinateXY& p0, const CoordinateXY& p1)
{
    if (arc.isLinear()) {
        intersects(arc.p0, arc.p2, p0, p1);
        return;
    }

    // TODO: envelope check?
    const CoordinateXY& c = arc.getCenter();
    const double r = arc.getRadius();

    CoordinateXY isect0, isect1;
    auto n = circleIntersects(c, r, p0, p1, isect0, isect1);

    if (n > 0 && arc.containsPointOnCircle(isect0)) {
        intPt[nPt++] = isect0;
    }

    if (n > 1  && arc.containsPointOnCircle(isect1)) {
        intPt[nPt++] = isect1;
    }

    switch (nPt) {
    case 2:
        result = TWO_POINT_INTERSECTION;
        break;
    case 1:
        result = ONE_POINT_INTERSECTION;
        break;
    default:
        result = NO_INTERSECTION;
    }
}

void
CircularArcIntersector::intersects(const CoordinateXY& p0, const CoordinateXY& p1,
                                   const CoordinateXY& q0, const CoordinateXY& q1)
{
    algorithm::LineIntersector li;
    li.computeIntersection(p0, p1, q0, q1);
    if (li.getIntersectionNum() == 2) {
        // FIXME this means a collinear intersection, so we should report as cocircular?
        intPt[0] = li.getIntersection(0);
        intPt[1] = li.getIntersection(1);
        result = TWO_POINT_INTERSECTION;
    } else if (li.getIntersectionNum() == 1) {
        intPt[0] = li.getIntersection(0);
        nPt = 1;
        result = ONE_POINT_INTERSECTION;
    } else {
        result = NO_INTERSECTION;
    }
}

void
CircularArcIntersector::intersects(const CircularArc& arc1, const CircularArc& arc2)
{
    // Handle cases where one or both arcs are degenerate
    if (arc1.isLinear()) {
        if (arc2.isLinear()) {
            intersects(arc1.p0, arc1.p2, arc2.p0, arc2.p2);
            return;
        } else {
            intersects(arc2, arc1.p0, arc1.p2);
            return;
        }
    } else if (arc2.isLinear()) {
        intersects(arc1, arc2.p0, arc2.p2);
        return;
    }

    const auto& c1 = arc1.getCenter();
    const auto& c2 = arc2.getCenter();

    const auto r1 = arc1.getRadius();
    const auto r2 = arc2.getRadius();

    auto d = c1.distance(c2);

    if (d > r1 + r2) {
        // Circles are disjoint
        result = NO_INTERSECTION;
        return;
    }

    if (d < std::abs(r1-r2)) {
        // One circle contained within the other; arcs cannot intersect
        result = NO_INTERSECTION;
        return;
    }

    // a: the distance from c1 to the "radical line", which connects the two intersection points
    double a = (d*d + r1*r1 - r2*r2) / (2*d);

    // FIXME shouldn't be possible for a to be more than d, and yet it can happen for
    // arcs that are very nearly cocircular
    //if (a > d) {
    //    std::cerr << "a > d" << std::endl;
    //}

    // FIXME because the circle center calculation is inexact we need some kind of tolerance here.
    // Take a PrecisionModel like LineIntersector?
    if (a == 0 || (d == 0 && r1 == r2)) {
        // COCIRCULAR

        double ap0 = arc1.theta0();
        double ap1 = arc1.theta2();
        double bp0 = arc2.theta0();
        double bp1 = arc2.theta2();

        bool resultArcIsCCW = true;

        if (arc1.orientation() != Orientation::COUNTERCLOCKWISE) {
            std::swap(ap0, ap1);
            resultArcIsCCW = false;
        }
        if (arc2.orientation() != Orientation::COUNTERCLOCKWISE) {
            std::swap(bp0, bp1);
        }
        ap0 = Angle::normalizePositive(ap0);
        ap1 = Angle::normalizePositive(ap1);
        bp0 = Angle::normalizePositive(bp0);
        bp1 = Angle::normalizePositive(bp1);

        bool checkBp1inA = true;

        // check start of B within A?
        if (Angle::isWithinCCW(bp0, ap0, ap1)) {
            double start = bp0;
            double end = nextAngleCCW(start, bp1, ap1);

            if (end == bp1) {
                checkBp1inA = false;
            }

            if (start == end) {
                intPt[nPt++] = CircularArcs::createPoint(c1, r1, start);
            }
            else {
                if (resultArcIsCCW) {
                    intArc[nArc++] = CircularArcs::createArc(c1, r1, start, end, true);
                }
                else {
                    intArc[nArc++] = CircularArcs::createArc(c1, r1, end, start, false);
                }
            }
        }

        if (checkBp1inA && Angle::isWithinCCW(bp1, ap0, ap1)) {
            // end of B within A?
            double start = ap0;
            double end = bp1;
            if (start == end) {
                intPt[nPt++] = CircularArcs::createPoint(c1, r1, start);
            }
            else {
                if (resultArcIsCCW) {
                    intArc[nArc++] = CircularArcs::createArc(c1, r1, start, end, true);
                }
                else {
                    intArc[nArc++] = CircularArcs::createArc(c1, r1, end, start, false);
                }
            }
        }
    } else {
        // NOT COCIRCULAR

        double dx = c2.x-c1.x;
        double dy = c2.y-c1.y;

        // point where a line between the two circle center points intersects
        // the radical line
        CoordinateXY p{c1.x + a* dx/d, c1.y+a* dy/d};

        // distance from p to the intersection points
        double h = std::sqrt(r1*r1 - a*a);

        CoordinateXY isect0{p.x + h* dy/d, p.y - h* dx/d };
        CoordinateXY isect1{p.x - h* dy/d, p.y + h* dx/d };

        if (arc1.containsPointOnCircle(isect0) && arc2.containsPointOnCircle(isect0)) {
            intPt[nPt++] = isect0;
        }
        if (!isect1.equals2D(isect0) && arc1.containsPointOnCircle(isect1) && arc2.containsPointOnCircle(isect1)) {
            intPt[nPt++] = isect1;
        }
    }

    if (nArc) {
        result = COCIRCULAR_INTERSECTION;
    }
    else {
        switch (nPt) {
        case 2:
            result = TWO_POINT_INTERSECTION;
            break;
        case 1:
            result = ONE_POINT_INTERSECTION;
            break;
        case 0:
            result = NO_INTERSECTION;
            break;
        }
    }
}

}
