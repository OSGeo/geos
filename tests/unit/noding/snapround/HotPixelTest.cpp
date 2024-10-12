//
// Test Suite for geos::noding::snapround::HotPixel class.

#include <tut/tut.hpp>
// geos
#include <geos/algorithm/LineIntersector.h>
#include <geos/noding/snapround/HotPixel.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/PrecisionModel.h>

using namespace geos::geom;
using namespace geos::noding::snapround;

namespace tut {
//
// Test Group
//

// Common data used by all tests
struct test_hotpixel_data {

    void checkIntersects(std::string testname, bool expected,
        double x, double y, double scale,
        double x1, double y1, double x2, double y2)
    {
        Coordinate p(x, y);
        HotPixel hp(p, scale);
        Coordinate p1(x1,y1);
        Coordinate p2(x2,y2);
        bool actual = hp.intersects(p1, p2);
        ensure(testname, expected == actual);
    }

    // test_hotpixel_data() {}
};

typedef test_group<test_hotpixel_data> group;
typedef group::object object;

group test_hotpixel_group("geos::noding::snapround::HotPixel");

//
// Test Cases
//

// testBelow
template<>
template<>
void object::test<1>
()
{
    checkIntersects("testBelow", false, 1, 1, 100,
        1, 0.98, 3, 0.5);
}
// testAbove
template<>
template<>
void object::test<2>
()
{
    checkIntersects("testAbove", false, 1, 1, 100,
        1, 1.011, 3, 1.5);
}
// testRightSideVerticalTouchAbove
template<>
template<>
void object::test<3>
()
{
    checkIntersects("testRightSideVerticalTouchAbove", false, 1.2, 1.2, 10,
        1.25, 1.25, 1.25, 2);
}
// testRightSideVerticalTouchBelow
template<>
template<>
void object::test<4>
()
{
    checkIntersects("testRightSideVerticalTouchBelow", false, 1.2, 1.2, 10,
        1.25, 0, 1.25, 1.15);
}
// testRightSideVerticalOverlap
template<>
template<>
void object::test<5>
()
{
    checkIntersects("testRightSideVerticalOverlap", false, 1.2, 1.2, 10,
        1.25, 0, 1.25, 1.5);
}

//-----------------------------

// testTopSideHorizontalTouchRight
template<>
template<>
void object::test<6>
()
{
    checkIntersects("testTopSideHorizontalTouchRight", false, 1.2, 1.2, 10,
        1.25, 1.25, 2, 1.25);
}
// testTopSideHorizontalTouchLeft
template<>
template<>
void object::test<7>
()
{
    checkIntersects("testTopSideHorizontalTouchLeft", false, 1.2, 1.2, 10,
        0, 1.25, 1.15, 1.25);
}
// testTopSideHorizontalOverlap
template<>
template<>
void object::test<8>
()
{
    checkIntersects("testTopSideHorizontalOverlap", false, 1.2, 1.2, 10,
        0, 1.25, 1.9, 1.25);
}

//-----------------------------

// testLeftSideVerticalTouchAbove
template<>
template<>
void object::test<9>
()
{
    checkIntersects("testLeftSideVerticalTouchAbove", false, 1.2, 1.2, 10,
        1.15, 1.25, 1.15, 2);
}
// testLeftSideVerticalOverlap
template<>
template<>
void object::test<10>
()
{
    checkIntersects("testLeftSideVerticalOverlap", true, 1.2, 1.2, 10,
        1.15, 0, 1.15, 1.8);
}
// testLeftSideVerticalTouchBelow
template<>
template<>
void object::test<11>
()
{
    checkIntersects("testLeftSideVerticalTouchBelow", true, 1.2, 1.2, 10,
        1.15, 0, 1.15, 1.15);
}
// testLeftSideCrossRight
template<>
template<>
void object::test<12>
()
{
    checkIntersects("testLeftSideCrossRight", true, 1.2, 1.2, 10,
        0, 1.19, 2, 1.21);
}
// testLeftSideCrossTop
template<>
template<>
void object::test<13>
()
{
    checkIntersects("testLeftSideCrossTop", true, 1.2, 1.2, 10,
        0.8, 0.8, 1.3, 1.39);
}
// testLeftSideCrossBottom
template<>
template<>
void object::test<14>
()
{
    checkIntersects("testLeftSideCrossBottom", true, 1.2, 1.2, 10,
        1, 1.5, 1.3, 0.9 );
}

//-----------------------------

// testBottomSideHorizontalTouchRight
template<>
template<>
void object::test<15>
()
{
    checkIntersects("testBottomSideHorizontalTouchRight", false, 1.2, 1.2, 10,
        1.25, 1.15, 2, 1.15);
}
// testBottomSideHorizontalTouchLeft
template<>
template<>
void object::test<16>
()
{
    checkIntersects("testBottomSideHorizontalTouchLeft", true, 1.2, 1.2, 10,
        0, 1.15, 1.15, 1.15);
}
// testBottomSideHorizontalOverlapLeft
template<>
template<>
void object::test<17>
()
{
    checkIntersects("testBottomSideHorizontalOverlapLeft", true, 1.2, 1.2, 10,
        0, 1.15, 1.2, 1.15);
}
// testBottomSideHorizontalOverlap
template<>
template<>
void object::test<18>
()
{
    checkIntersects("testBottomSideHorizontalOverlap", true, 1.2, 1.2, 10,
        0, 1.15, 1.9, 1.15);
}
// testBottomSideHorizontalOverlapRight
template<>
template<>
void object::test<19>
()
{
    checkIntersects("testBottomSideHorizontalOverlapRight", true, 1.2, 1.2, 10,
        1.2, 1.15, 1.4, 1.15);
}
// testBottomSideCrossRight
template<>
template<>
void object::test<20>
()
{
    checkIntersects("testBottomSideCrossRight", true, 1.2, 1.2, 10,
        1.1, 1, 1.4, 1.4);
}
// testBottomSideCrossTop
template<>
template<>
void object::test<21>
()
{
    checkIntersects("testBottomSideCrossTop", true, 1.2, 1.2, 10,
        1.1, 0.9, 1.3, 1.6);
}


//-----------------------------

// testDiagonalDown
template<>
template<>
void object::test<22>
()
{
    checkIntersects("testDiagonalDown", true, 1.2, 1.2, 10,
        0.9, 1.5, 1.4, 1 );
}
// testDiagonalUp
template<>
template<>
void object::test<23>
()
{
    checkIntersects("testDiagonalUp", true, 1.2, 1.2, 10,
        0.9, 0.9, 1.5, 1.5 );
}


//-----------------------------
// Test segments entering through a corner and terminating inside pixel

// testCornerULEndInside
template<>
template<>
void object::test<24>
()
{
    checkIntersects("testCornerULEndInside", true, 1, 1, 10,
        0.7, 1.3, 0.98, 1.02 );
}
// testCornerLLEndInside
template<>
template<>
void object::test<25>
()
{
    checkIntersects("testCornerLLEndInside", true, 1, 1, 10,
        0.8, 0.8, 0.98, 0.98 );
}
// testCornerURStartInside
template<>
template<>
void object::test<26>
()
{
    checkIntersects("testCornerURStartInside", true, 1, 1, 10,
        1.02, 1.02, 1.3, 1.3 );
}
// testCornerLRStartInside
template<>
template<>
void object::test<27>
()
{
    checkIntersects("testCornerLRStartInside", true, 1, 1, 10,
        1.02, 0.98, 1.3, 0.7 );
}

//-----------------------------
// Test segments tangent to a corner

// testCornerLLTangent
template<>
template<>
void object::test<28>()
{
    checkIntersects("testCornerLLTangent", true, 1, 1, 10,
        0.9, 1, 1, 0.9 );
}

// testCornerLLTangentNoTouch
template<>
template<>
void object::test<29>()
{
    checkIntersects("testCornerLLTangentNoTouch", false, 1, 1, 10,
        0.9, 0.9, 1, 0.9 );
}

// testCornerULTangent
template<>
template<>
void object::test<30>()
{
    // does not intersect due to open top
    checkIntersects("testCornerULTangent", false, 1, 1, 10,
        0.9, 1, 1, 1.1 );
}

// testCornerURTangent
template<>
template<>
void object::test<31>()
{
    // does not intersect due to open top
    checkIntersects("testCornerURTangent", false, 1, 1, 10,
        1, 1.1, 1.1, 1 );
}

// testCornerLRTangent
template<>
template<>
void object::test<32>()
{
    // does not intersect due to open right side
    checkIntersects("testCornerLRTangent", false, 1, 1, 10,
        1, 0.9, 1.1, 1 );
}

// testCornerULTouchEnd
template<>
template<>
void object::test<33>()
{
    // does not intersect due to bounding box check for open top
    checkIntersects("testCornerULTouchEnd", false, 1, 1, 10,
        0.9, 1.1, 0.95, 1.05 );
}



} // namespace tut
