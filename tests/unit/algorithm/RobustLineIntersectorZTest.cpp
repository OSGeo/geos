//
// Ported from JTS
// junit/algorithm/RobustLineIntersectorZTest.java
// 1ecc94caa9e188beb63bfb95089e7dd6869bab20

#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/algorithm/LineIntersector.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/LineSegment.h>
// std
#include <utility>


using namespace geos::geom;

typedef geos::algorithm::LineIntersector RobustLineIntersector;

namespace tut {
//
// Test Group
//

struct test_robustlineintersectorz_data {
    template<typename C>
    using Segment = std::pair<C, C>;

    using XY = CoordinateXY;
    using XYZ = Coordinate;
    using XYM = CoordinateXYM;
    using XYZM = CoordinateXYZM;

    template<typename C1, typename C2>
    void checkIntersection(const Segment<C1>& line1,
                           const Segment<C2>& line2,
                           const CoordinateXYZM& p1,
                           const CoordinateXYZM& p2 = CoordinateXYZM::getNull())
    {
        checkIntersectionDir(line1, line2, p1, p2);
        checkIntersectionDir(line2, line1, p1, p2);
        Segment<C1> line1Rev(line1.second, line1.first);
        Segment<C2> line2Rev(line2.second, line2.first);
        checkIntersectionDir(line1Rev, line2Rev, p1, p2);
        checkIntersectionDir(line2Rev, line1Rev, p1, p2);
    }

    template<typename C1, typename C2>
    void checkIntersectionDir(
                const Segment<C1>& line1,
                const Segment<C2>& line2,
                const CoordinateXYZM& p1,
                const CoordinateXYZM& p2)
    {
        RobustLineIntersector li;
        li.computeIntersection(
            line1.first, line1.second,
            line2.first, line2.second);

        auto actual1 = li.getIntersection(0);
        auto actual2 = li.getIntersection(1);

        if (p2.isNull()) {
            ensure_equals(li.getIntersectionNum(), 1u);
        } else {
            ensure_equals(li.getIntersectionNum(), 2u);

            // normalize actual results
            if (actual1.compareTo(actual2) > 0) {
                actual1 = li.getIntersection(1);
                actual2 = li.getIntersection(0);
            }
        }

        ensure_equals_xyzm( actual1, p1 );
        if (!p2.isNull())
            ensure_equals_xyzm( actual2, p2 );
  }

    static CoordinateXYZM xyz(double x, double y, double z) {
        return CoordinateXYZM(Coordinate(x, y, z));
    }

    static CoordinateXYZM xy(double x, double y) {
        return CoordinateXYZM(CoordinateXY(x, y));
    }

    static CoordinateXYZM xym(double x, double y, double m) {
        return CoordinateXYZM(CoordinateXYM(x, y, m));
    }

    static CoordinateXYZM xyzm(double x, double y, double z, double m) {
        return CoordinateXYZM(x, y, z, m);
    }

    template<typename C=Coordinate>
    static Segment<C> line(const C& p0, const C& p1) {
        return Segment<C>(p0, p1);
    }

};

typedef test_group<test_robustlineintersectorz_data> group;
typedef group::object object;

group test_robustlineintersectorz_group(
    "geos::algorithm::RobustLineIntersectorZ");




//
// Test Cases
//


// 1 - testInterior
template<>
template<>
void object::test<1>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({1, 3, 10}, {3, 1, 30}),
                xyz(2, 2, 11));
}


// 2 - testInterior2D
template<>
template<>
void object::test<2>
()
{
    checkIntersection(
                line<XY>({1, 1}, {3, 3}),
                line<XY>({1, 3}, {3, 1}),
                xy(2, 2));
}

// testInterior3D2D
template<>
template<>
void object::test<3>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XY>({1, 3}, {3, 1}),
                xyz(2, 2, 2));
}

// testInterior2D3D
template<>
template<>
void object::test<4>
()
{
    checkIntersection(
                line<XY>({1, 1}, {3, 3}),
                line<XYZ>({1, 3, 10}, {3, 1, 30}),
                xyz(2, 2, 20));
}

// testInterior2D3DPart
// result is average of line1 interpolated and line2 p0 Z
template<>
template<>
void object::test<5>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({1, 3, 10}, {3, 1, geos::DoubleNotANumber}),
                xyz(2, 2, 6));
}

// testEndpoint
template<>
template<>
void object::test<6>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({3, 3, 3}, {3, 1, 30}),
                xyz(3, 3, 3));
}

// testEndpoint2D
template<>
template<>
void object::test<7>
()
{
    checkIntersection(
                line<XY>({1, 1}, {3, 3}),
                line<XY>({3, 3}, {3, 1}),
                xyz(3, 3, geos::DoubleNotANumber));
}

// testEndpoint2D3D
template<>
template<>
void object::test<8>
()
{
    // result Z is from 3D point
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XY>({3, 3}, {3, 1}),
                xyz(3, 3, 3));
}

// testInteriorEndpoint
template<>
template<>
void object::test<9>
()
{
    // result Z is from 3D point
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({2, 2, 10}, {3, 1, 30}),
                xyz(2, 2, 10));
}

// testInteriorEndpoint3D2D
template<>
template<>
void object::test<10>
()
{
    // result Z is interpolated
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XY>({2, 2}, {3, 1}),
                xyz(2, 2, 2));
}

// testInteriorEndpoint2D3D
// result Z is from 3D point
template<>
template<>
void object::test<11>
()
{
    checkIntersection(
                line<XY>({1, 1}, {3, 3}),
                line<XYZ>({2, 2, 10}, {3, 1, 20}),
                xyz(2, 2, 10));
}

// testCollinearEqual
template<>
template<>
void object::test<12>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                xyz(1, 1, 1), xyz( 3, 3, 3));
}

// testCollinearEqual3D2D
template<>
template<>
void object::test<13>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XY>({1, 1}, {3, 3}),
                xyz(1, 1, 1), xyz( 3, 3, 3));
}

// testCollinearEndpoint
template<>
template<>
void object::test<14>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XYZ>({3, 3, 3}, {5, 5, 5}),
                xyz(3, 3, 3));
}

// testCollinearEndpoint3D2D
// result Z is from 3D point
template<>
template<>
void object::test<15>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {3, 3, 3}),
                line<XY>({3, 3}, {5, 5}),
                xyz(3, 3, 3));
}

// testCollinearContained
template<>
template<>
void object::test<16>
()
{
    checkIntersection(
                line<XYZ>({1, 1, 1}, {5, 5, 5}),
                line<XYZ>({3, 3, 3}, {4, 4, 4}),
                xyz(3, 3, 3),
                xyz(4, 4, 4));
}

// testCollinearContained3D2D
template<>
template<>
void object::test<17>
()
{
    // result Z is interpolated
    checkIntersection(
                line<XYZ>({1, 1, 1}, {5, 5, 5}),
                line<XY>({3, 3}, {4, 4}),
                xyz(3, 3, 3),
                xyz(4, 4, 4));
}

// Interior XYM-XYM
template<>
template<>
void object::test<18>
()
{
    checkIntersection(
                line<XYM>({1, 1, 1}, {3, 3, 3}),
                line<XYM>({1, 3, 10}, {3, 1, 30}),
                xym(2, 2, 11));
}

// Interior XYZM-XYZM
template<>
template<>
void object::test<19>
()
{
    checkIntersection(
                line<XYZM>({1, 1, 1, -1}, {3, 3, 3, -3}),
                line<XYZM>({1, 3, 10, -10}, {3, 1, 30, -30}),
                xyzm(2, 2, 11, -11));
}

} // namespace tut

