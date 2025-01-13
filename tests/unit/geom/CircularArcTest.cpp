#include <tut/tut.hpp>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CircularArc.h>
#include <geos/constants.h>

using geos::geom::CoordinateXY;
using geos::geom::CircularArc;
using geos::MATH_PI;

namespace tut {

struct test_circulararc_data {

    const double eps = 1e-8;

    void checkAngle(const CoordinateXY& p0, const CoordinateXY& p1, const CoordinateXY& p2, double expected)
    {
        CircularArc arc(p0, p1, p2);
        ensure_equals(p0.toString() + " / " + p1.toString() + " / " + p2.toString(), arc.getAngle(), expected, eps);

        CircularArc rev(p2, p1, p0);
        ensure_equals(p2.toString() + " / " + p1.toString() + " / " + p0.toString(), rev.getAngle(), expected, eps);
    }

    void checkLength(const CoordinateXY& p0, const CoordinateXY& p1, const CoordinateXY& p2, double expected)
    {
        CircularArc arc(p0, p1, p2);
        ensure_equals(p0.toString() + " / " + p1.toString() + " / " + p2.toString(), arc.getLength(), expected, eps);

        CircularArc rev(p2, p1, p0);
        ensure_equals(p2.toString() + " / " + p1.toString() + " / " + p0.toString(), rev.getLength(), expected, eps);
    }
};

using group = test_group<test_circulararc_data>;
using object = group::object;

group test_circulararc_group("geos::geom::CircularArc");

template<>
template<>
void object::test<1>()
{
    set_test_name("CircularArc::getAngle() on a unit circle");

    auto x = std::sqrt(2.0)/2;

    // full circle
    checkAngle({-1, 0}, {1, 0}, {-1, 0}, 2*MATH_PI);

    // check half-circles
    checkAngle({-1, 0}, {0, 1}, {1, 0}, MATH_PI); // top
    checkAngle({-1, 0}, {0, -1}, {1, 0}, MATH_PI); // bottom
    checkAngle({0, -1}, {-1, 0}, {0, 1}, MATH_PI); // left
    checkAngle({0, -1}, {1, 0}, {0, 1}, MATH_PI); // right

    // check quadrants
    checkAngle({-1, 0}, {-x, x}, {0, 1}, MATH_PI/2); // upper left
    checkAngle({0, 1}, {x, x}, {1, 0}, MATH_PI/2); // upper right
    checkAngle({0, -1}, {x, -x}, {1, 0}, MATH_PI/2); // lower right
    checkAngle({0, -1}, {-x, -x}, {-1, 0}, MATH_PI/2); // lower left

    // 3/4
    checkAngle({-x, x}, {0, -1}, {x, x}, 1.5*MATH_PI); // mouth up
    checkAngle({-x, -x}, {0, 1}, {x, -x}, 1.5*MATH_PI); // mouth down
    checkAngle({-x, x}, {1, 0}, {-x, -x}, 1.5*MATH_PI); // mouth left
    checkAngle({x, -x}, {-1, 0}, {x, x}, 1.5*MATH_PI); // mouth right
}

template<>
template<>
void object::test<2>()
{
    set_test_name("CircularArc::getLength()");

    checkLength({1.6, 0.4}, {1.6, 0.5}, {1.7, 1}, 0.6122445326877711);
}

template<>
template<>
void object::test<3>()
{
    set_test_name("CircularArc::getArea()");

    ensure_equals("half circle, R=2", CircularArc({-2, 0}, {0, 2}, {2, 0}).getArea(), MATH_PI*2);

    ensure_equals("full circle, R=3", CircularArc({-3, 0}, {3, 0}, {-3, 0}).getArea(), MATH_PI*3*3);

    ensure_equals("3/4, mouth up, R=2", CircularArc({-std::sqrt(2), std::sqrt(2)}, {0, -2}, {std::sqrt(2), std::sqrt(2)}).getArea(),
                  MATH_PI*4 - 2*(MATH_PI/2-1), 1e-8);

    ensure_equals("1/4, pointing up, R=2", CircularArc({-std::sqrt(2), std::sqrt(2)}, {0, 2}, {std::sqrt(2), std::sqrt(2)}).getArea(),
                  2*(MATH_PI/2-1), 1e-8);
}

template<>
template<>
void object::test<4>()
{
    set_test_name("CircularArc::isLinear()");

    ensure_equals("not linear", CircularArc({-1, 0}, {0, 1}, {1, 0}).isLinear(), false);
    ensure_equals("linear", CircularArc({0, 0}, {1, 1}, {2, 2}).isLinear(), true);
}

template<>
template<>
void object::test<5>()
{
    set_test_name("CircularArc::containsPointOnCircle");

    // complete circle
    CircularArc({5, 0}, {-5, 0}, {5, 0}).containsPointOnCircle({5, 0});
    CircularArc({5, 0}, {-5, 0}, {5, 0}).containsPointOnCircle({4, 3});

    // lower semi-circle
    CircularArc({-5, 0}, {0, -5}, {5, 0}).containsPointOnCircle({5, 0});

    // upper semi-circle
    CircularArc({-5, 0}, {0, 5}, {5, 0}).containsPointOnCircle({5, 0});
}

}
