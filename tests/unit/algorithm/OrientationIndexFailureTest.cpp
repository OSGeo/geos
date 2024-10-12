//
// Test Suite for geos::algorithm::Orientation
// Ported from JTS junit/algorithm/OrientationIndexFailureTest.java

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/Orientation.h>
// std

using namespace geos::geom;
using namespace geos::algorithm;

namespace tut {
//
// Test Group
//
struct test_orientation_data {
    Coordinate c1;
    Coordinate c2;
    Coordinate c3;

    test_orientation_data() {};

};

static int
checkOrientation(Coordinate& c1, Coordinate& c2, Coordinate& c3)
{
    int orient0 = Orientation::index(c1, c2, c3);
    int orient1 = Orientation::index(c2, c3, c1);
    int orient2 = Orientation::index(c3, c1, c2);
    return (orient0 == orient1) && (orient0 == orient2);
}


typedef test_group<test_orientation_data> group;
typedef group::object object;

group test_orientation_group("geos::algorithm::OrientationIndexFailure");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    c1 = Coordinate(1.4540766091864998, -7.989685402102996);
    c2 = Coordinate(23.131039116367354, -7.004368924503866);
    c3 = Coordinate(1.4540766091865, -7.989685402102996);
    ensure_equals("ring orientation test 1 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<2>
()
{
    c1 = Coordinate(219.3649559090992, 140.84159161824724);
    c2 = Coordinate(168.9018919682399, -5.713787599646864);
    c3 = Coordinate(186.80814046338352, 46.28973405831556);
    ensure_equals("ring orientation test 2 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<3>
()
{
    c1 = Coordinate(279.56857838488514, -186.3790522565901);
    c2 = Coordinate(-20.43142161511487, 13.620947743409914);
    c3 = Coordinate(0, 0);
    ensure_equals("ring orientation test 3 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<4>
()
{
    c1 = Coordinate(-26.2, 188.7);
    c2 = Coordinate(37.0, 290.7);
    c3 = Coordinate(21.2, 265.2);
    ensure_equals("ring orientation test 4 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<5>
()
{
    c1 = Coordinate(-5.9, 163.1);
    c2 = Coordinate(76.1, 250.7);
    c3 = Coordinate(14.6, 185);
    ensure_equals("ring orientation test 5 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<6>
()
{
    c1 = Coordinate(-0.9575, 0.4511);
    c2 = Coordinate(-0.9295, 0.3291);
    c3 = Coordinate(-0.8945, 0.1766);
    ensure_equals("ring orientation test 6 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<7>
()
{
    c1 = Coordinate(-9575, 4511);
    c2 = Coordinate(-9295, 3291);
    c3 = Coordinate(-8945, 1766);
    ensure_equals("ring orientation test 7 failed", checkOrientation(c1, c2, c3), 1);
}

template<>
template<>
void object::test<8>
()
{
    c1 = Coordinate(-140.8859438214298, 140.88594382142983);
    c2 = Coordinate(-57.309236848216706, 57.30923684821671);
    c3 = Coordinate(-190.9188309203678, 190.91883092036784);
    ensure_equals("ring orientation test 8 failed", checkOrientation(c1, c2, c3), 1);
}


} // namespace tut

