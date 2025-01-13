/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2009-2011  Sandro Santilli <strk@kbt.io>
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

#pragma once

#include <geos/export.h>
#include <geos/algorithm/Orientation.h> // for constants

// Forward declarations
namespace geos {
namespace geom {
class Coordinate;
}
}

namespace geos {
namespace algorithm { // geos::algorithm

/// Utility functions for working with angles.
//
/// Unless otherwise noted, methods in this class express angles in radians.
///
class GEOS_DLL Angle {
public:

    static constexpr double PI_TIMES_2 = 2.0 * MATH_PI;
    static constexpr double PI_OVER_2 = MATH_PI / 2.0;
    static constexpr double PI_OVER_4 = MATH_PI / 4.0;

    /// Constant representing counterclockwise orientation
    static const int COUNTERCLOCKWISE = Orientation::COUNTERCLOCKWISE;

    /// Constant representing clockwise orientation
    static const int CLOCKWISE = Orientation::CLOCKWISE;

    /// Constant representing no orientation
    static const int NONE = Orientation::COLLINEAR;

    /// Converts from radians to degrees.
    ///
    /// @param radians an angle in radians
    /// @return the angle in degrees
    ///
    static double toDegrees(double radians);

    /// Converts from degrees to radians.
    ///
    /// @param angleDegrees an angle in degrees
    /// @return the angle in radians
    ///
    static double toRadians(double angleDegrees);

    /// \brief
    /// Returns the angle of the vector from p0 to p1,
    /// relative to the positive X-axis.
    ///
    /// The angle is normalized to be in the range [ -Pi, Pi ].
    ///
    /// @return the normalized angle (in radians) that p0-p1 makes
    ///         with the positive x-axis.
    ///
    static double angle(const geom::CoordinateXY& p0,
                        const geom::CoordinateXY& p1);

    /// \brief
    /// Returns the angle that the vector from (0,0) to p,
    /// relative to the positive X-axis.
    //
    /// The angle is normalized to be in the range ( -Pi, Pi ].
    ///
    /// @return the normalized angle (in radians) that p makes
    ///          with the positive x-axis.
    ///
    static double angle(const geom::CoordinateXY& p);

    /// Tests whether the angle between p0-p1-p2 is acute.
    ///
    /// An angle is acute if it is less than 90 degrees.
    ///
    /// Note: this implementation is not precise (deterministic) for
    ///       angles very close to 90 degrees.
    ///
    /// @param p0 an endpoint of the angle
    /// @param p1 the base of the angle
    /// @param p2 the other endpoint of the angle
    ///
    static bool isAcute(const geom::CoordinateXY& p0,
                        const geom::CoordinateXY& p1,
                        const geom::CoordinateXY& p2);

    /// Tests whether the angle between p0-p1-p2 is obtuse.
    ///
    /// An angle is obtuse if it is greater than 90 degrees.
    ///
    /// Note: this implementation is not precise (deterministic) for
    ///       angles very close to 90 degrees.
    ///
    /// @param p0 an endpoint of the angle
    /// @param p1 the base of the angle
    /// @param p2 the other endpoint of the angle
    ///
    static bool isObtuse(const geom::CoordinateXY& p0,
                         const geom::CoordinateXY& p1,
                         const geom::CoordinateXY& p2);

    /// Returns the unoriented smallest angle between two vectors.
    ///
    /// The computed angle will be in the range [0, Pi).
    ///
    /// @param tip1 the tip of one vector
    /// @param tail the tail of each vector
    /// @param tip2 the tip of the other vector
    /// @return the angle between tail-tip1 and tail-tip2
    ///
    static double angleBetween(const geom::CoordinateXY& tip1,
                               const geom::CoordinateXY& tail,
                               const geom::CoordinateXY& tip2);

    /// Returns the oriented smallest angle between two vectors.
    ///
    /// The computed angle will be in the range (-Pi, Pi].
    /// A positive result corresponds to a counterclockwise rotation
    /// from v1 to v2;
    /// a negative result corresponds to a clockwise rotation.
    ///
    /// @param tip1 the tip of v1
    /// @param tail the tail of each vector
    /// @param tip2 the tip of v2
    /// @return the angle between v1 and v2, relative to v1
    ///
    static double angleBetweenOriented(const geom::CoordinateXY& tip1,
                                       const geom::CoordinateXY& tail,
                                       const geom::CoordinateXY& tip2);

    /// Computes the interior angle between two segments of a ring.
    ///
    /// The ring is assumed to be oriented in a clockwise direction.
    /// The computed angle will be in the range [0, 2Pi]
    ///
    /// @param p0
    ///          a point of the ring
    /// @param p1
    ///          the next point of the ring
    /// @param p2
    ///          the next point of the ring
    /// @return the interior angle based at <code>p1</code>
    ///
    static double interiorAngle(const geom::CoordinateXY& p0,
                                const geom::CoordinateXY& p1,
                                const geom::CoordinateXY& p2);

    /// \brief
    /// Returns whether an angle must turn clockwise or counterclockwise
    /// to overlap another angle.
    ///
    /// @param ang1 an angle (in radians)
    /// @param ang2 an angle (in radians)
    /// @return whether a1 must turn CLOCKWISE, COUNTERCLOCKWISE or
    ///         NONE to overlap a2.
    ///
    static int getTurn(double ang1, double ang2);

    /// \brief
    /// Computes the normalized value of an angle, which is the
    /// equivalent angle in the range ( -Pi, Pi ].
    ///
    /// @param angle the angle to normalize
    /// @return an equivalent angle in the range (-Pi, Pi]
    ///
    static double normalize(double angle);

    /// \brief
    /// Computes the normalized positive value of an angle,
    /// which is the equivalent angle in the range [ 0, 2*Pi ).
    ///
    /// E.g.:
    /// - normalizePositive(0.0) = 0.0
    /// - normalizePositive(-PI) = PI
    /// - normalizePositive(-2PI) = 0.0
    /// - normalizePositive(-3PI) = PI
    /// - normalizePositive(-4PI) = 0
    /// - normalizePositive(PI) = PI
    /// - normalizePositive(2PI) = 0.0
    /// - normalizePositive(3PI) = PI
    /// - normalizePositive(4PI) = 0.0
    ///
    /// @param angle the angle to normalize, in radians
    /// @return an equivalent positive angle
    ///
    static double normalizePositive(double angle);

    /// Returns true if angle x is within the counterclockwise
    /// arc from angle a to angle b
    ///
    /// @param angle angle to test
    /// @param from starting angle of arc
    /// @param to ending angle of arc
    ///
    /// @return true if `angle` is within [from, to]
    static bool isWithinCCW(double angle, double from, double to);

    /// Computes the unoriented smallest difference between two angles.
    ///
    /// The angles are assumed to be normalized to the range [-Pi, Pi].
    /// The result will be in the range [0, Pi].
    ///
    /// @param ang1 the angle of one vector (in [-Pi, Pi] )
    /// @param ang2 the angle of the other vector (in range [-Pi, Pi] )
    /// @return the angle (in radians) between the two vectors
    ///         (in range [0, Pi] )
    ///
    static double diff(double ang1, double ang2);

    /// \brief
    /// Computes both sin and cos of an angle, snapping near-zero values
    /// to zero.
    ///
    /// The angle does not need to be normalized. Unlike std::sin
    /// and std::cos, this method will snap near-zero values to zero
    /// for (e.g.) sin(pi) and cos(pi/2).
    ///
    /// @param ang the input angle (in radians)
    /// @param rSin the result of sin(ang)
    /// @param rCos the result of cos(ang)
    ///
    static inline void sinCosSnap(const double ang, double& rSin, double& rCos) {
        // calculate both; may be optimized with FSINCOS instruction
        rSin = std::sin(ang);
        rCos = std::cos(ang);
        // snap near-zero values
        if (std::fabs(rSin) < 5e-16) rSin = 0.0;
        if (std::fabs(rCos) < 5e-16) rCos = 0.0;
    }

};


} // namespace geos::algorithm
} // namespace geos


