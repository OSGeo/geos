//
// Test Suite for geos::algorithm::Angle
// Ported from JTS junit/algorithm/AngleTest.java r378

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/Angle.h>
// std
#include <string>

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_angle_data {
    typedef geos::geom::Coordinate Coordinate;
    typedef geos::algorithm::Angle Angle;

    double TOL;
    double PI;

    test_angle_data()
        :
        TOL(1e-5),
        PI(3.14159265358979323846)
    {}

};

typedef test_group<test_angle_data> group;
typedef group::object object;

group test_angle_group("geos::algorithm::Angle");

//
// Test Cases
//

// testAngle()
template<>
template<>
void object::test<1>
()
{
    ensure_equals("1", Angle::angle(Coordinate(10, 0)), 0.0, TOL);
    ensure_equals("2", Angle::angle(Coordinate(10, 10)), PI / 4, TOL);
    ensure_equals("3", Angle::angle(Coordinate(0, 10)), PI / 2, TOL);
    ensure_equals("4", Angle::angle(Coordinate(-10, 10)), 0.75 * PI, TOL);
    ensure_equals("5", Angle::angle(Coordinate(-10, 0)), PI, TOL);
    ensure_equals("6", Angle::angle(Coordinate(-10, -0.1)), -3.131592987, TOL);
    ensure_equals("7", Angle::angle(Coordinate(-10, -10)), -0.75 * PI, TOL);
}

// testIsAcute()
template<>
template<>
void object::test<2>
()
{
    ensure(Angle::isAcute(
               Coordinate(10, 0), Coordinate(0, 0), Coordinate(5, 10)));
    ensure(Angle::isAcute(
               Coordinate(10, 0), Coordinate(0, 0), Coordinate(5, -10)));
    // angle of 0
    ensure(Angle::isAcute(
               Coordinate(10, 0), Coordinate(0, 0), Coordinate(10, 0)));
    ensure_not(Angle::isAcute(
                   Coordinate(10, 0), Coordinate(0, 0), Coordinate(-5, 10)));
    ensure_not(Angle::isAcute(
                   Coordinate(10, 0), Coordinate(0, 0), Coordinate(-5, -10)));
}

// testNormalizePositive()
template<>
template<>
void object::test<3>
()
{
    ensure_equals("", Angle::normalizePositive(0.0), 0.0, TOL);

    ensure_equals("", Angle::normalizePositive(-0.5 * PI), 1.5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(-PI), PI, TOL);
    ensure_equals("", Angle::normalizePositive(-1.5 * PI), .5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(-2 * PI), 0.0, TOL);
    ensure_equals("", Angle::normalizePositive(-2.5 * PI), 1.5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(-3 * PI), PI, TOL);
    ensure_equals("", Angle::normalizePositive(-4 * PI), 0.0, TOL);

    ensure_equals("", Angle::normalizePositive(0.5 * PI), 0.5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(PI), PI, TOL);
    ensure_equals("", Angle::normalizePositive(1.5 * PI), 1.5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(2 * PI), 0.0, TOL);
    ensure_equals("", Angle::normalizePositive(2.5 * PI), 0.5 * PI, TOL);
    ensure_equals("", Angle::normalizePositive(3 * PI), PI, TOL);
    ensure_equals("", Angle::normalizePositive(4 * PI), 0.0, TOL);
}

// testNormalize()
template<>
template<>
void object::test<4>
()
{
    ensure_equals("1", Angle::normalize(0.0), 0.0, TOL);

    ensure_equals("2", Angle::normalize(-0.5 * PI), -0.5 * PI, TOL);
    ensure_equals("3", Angle::normalize(-PI), PI, TOL);
    ensure_equals("4", Angle::normalize(-1.5 * PI), .5 * PI, TOL);
    ensure_equals("5", Angle::normalize(-2 * PI), 0.0, TOL);
    ensure_equals("6", Angle::normalize(-2.5 * PI), -0.5 * PI, TOL);
    ensure_equals("7", Angle::normalize(-3 * PI), PI, TOL);
    ensure_equals("8", Angle::normalize(-4 * PI), 0.0, TOL);

    ensure_equals("9", Angle::normalize(0.5 * PI), 0.5 * PI, TOL);
    ensure_equals("10", Angle::normalize(PI), PI, TOL);
    ensure_equals("11", Angle::normalize(1.5 * PI), -0.5 * PI, TOL);
    ensure_equals("12", Angle::normalize(2 * PI), 0.0, TOL);
    ensure_equals("13", Angle::normalize(2.5 * PI), 0.5 * PI, TOL);
    ensure_equals("14", Angle::normalize(3 * PI), PI, TOL);
    ensure_equals("15", Angle::normalize(4 * PI), 0.0, TOL);
}

// testInteriorAngle()
template<>
template<>
void object::test<5>
()
{
    Coordinate p1(1, 2);
    Coordinate p2(3, 2);
    Coordinate p3(2, 1);
    // Tests all interior angles of a triangle "POLYGON ((1 2, 3 2, 2 1, 1 2))"
    ensure_equals("45",
        45.0,
        Angle::toDegrees(Angle::interiorAngle(p1, p2, p3)),
        TOL);
    ensure_equals("90",
        90.0,
        Angle::toDegrees(Angle::interiorAngle(p2, p3, p1)),
        TOL);
    ensure_equals("45",
        45.0,
        Angle::toDegrees(Angle::interiorAngle(p3, p1, p2)),
        TOL);
    // Tests interior angles greater than 180 degrees
    ensure_equals("315",
        315.0,
        Angle::toDegrees(Angle::interiorAngle(p3, p2, p1)),
        TOL);
    ensure_equals("270",
        270.0,
        Angle::toDegrees(Angle::interiorAngle(p1, p3, p2)),
        TOL);
    ensure_equals("315",
        315.0,
        Angle::toDegrees(Angle::interiorAngle(p2, p1, p3)),
        TOL);
}

// testSinCosSnap()
template<>
template<>
void object::test<6>
()
{
    double rSin, rCos;

    // -720 to 720 degrees with 1 degree increments
    for (int angdeg = -720; angdeg <= 720; angdeg++) {
        const double ang = Angle::toRadians(angdeg);

        Angle::sinCosSnap(ang, rSin, rCos);

        double cSin = std::sin(ang);
        double cCos = std::cos(ang);
        if ( (angdeg % 90) == 0 ) {
            // not always the same for multiples of 90 degrees
            ensure(std::to_string(angdeg), std::fabs(rSin - cSin) < 1e-15);
            ensure(std::to_string(angdeg), std::fabs(rCos - cCos) < 1e-15);
        } else {
            ensure_equals(std::to_string(angdeg), rSin, cSin);
            ensure_equals(std::to_string(angdeg), rCos, cCos);
        }

    }

    // use radian increments that don't snap to exact degrees or zero
    for (double angrad = -6.3; angrad < 6.3; angrad += 0.013) {

        Angle::sinCosSnap(angrad, rSin, rCos);

        ensure_equals(std::to_string(angrad), rSin, std::sin(angrad));
        ensure_equals(std::to_string(angrad), rCos, std::cos(angrad));

    }

}


} // namespace tut

