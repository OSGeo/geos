#include <tut/tut.hpp>
// geos
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/algorithm/BoundaryNodeRule.h>
#include <geos/operation/BoundaryOp.h>
// std
#include <string>
#include <memory>

using namespace geos::algorithm;
using namespace geos::geom;
using geos::operation::BoundaryOp;

namespace tut {
//----------------------------------------------
// Test Group
//----------------------------------------------

struct test_boundaryop_data {
    geos::io::WKTReader wktreader;

    void runBoundaryTest(const std::string& wkt, const BoundaryNodeRule& bnRule, const std::string& wktExpected)
    {
        auto g = wktreader.read(wkt);
        auto expected = wktreader.read(wktExpected);

        BoundaryOp op(*g, bnRule);
        auto boundary = op.getBoundary();

        ensure(boundary->equals(expected.get()));
    }

    void checkHasBoundary(const std::string& wkt)
    {
        checkHasBoundary(wkt, BoundaryNodeRule::getBoundaryRuleMod2(), true);
    }

    void checkHasBoundary(const std::string& wkt, bool expected)
    {
        checkHasBoundary(wkt, BoundaryNodeRule::getBoundaryRuleMod2(), expected);
    }

    void checkHasBoundary(const std::string& wkt, const BoundaryNodeRule& bnRule, bool expected)
    {
        auto g = wktreader.read(wkt);
        ensure_equals(expected, BoundaryOp::hasBoundary(*g, bnRule));
    }

};

typedef test_group<test_boundaryop_data> group;
typedef group::object object;

group test_boundaryop_group("geos::operation::BoundaryOp");

// test1
template<>
template<>
void object::test<1>()
{
    std::string a = "MULTILINESTRING ((0 0, 10 10), (10 10, 20 20))";
    // under MultiValent, the common point is the only point on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryMultivalentEndPoint(),
            "POINT (10 10)"  );
}

// test2LinesTouchAtEndpoint2
template<>
template<>
void object::test<2>()
{
    std::string a = "MULTILINESTRING ((0 0, 10 10), (10 10, 20 20))";
    // under Mod-2, the common point is not on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryRuleMod2(),
            "MULTIPOINT ((0 0), (20 20))" );
    // under Endpoint, the common point is on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryEndPoint(),
            "MULTIPOINT ((0 0), (10 10), (20 20))"  );
    // under MonoValent, the common point is not on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryMonovalentEndPoint(),
            "MULTIPOINT ((0 0), (20 20))"  );
    // under MultiValent, the common point is the only point on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryMultivalentEndPoint(),
            "POINT (10 10)"  );
}

// test3LinesTouchAtEndpoint2
template<>
template<>
void object::test<3>()
{
    std::string a = "MULTILINESTRING ((0 0, 10 10), (10 10, 20 20), (10 10, 10 20))";
    // under Mod-2, the common point is on the boundary (3 mod 2 = 1)
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryRuleMod2(),
            "MULTIPOINT ((0 0), (10 10), (10 20), (20 20))" );
    // under Endpoint, the common point is on the boundary (it is an endpoint)
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryEndPoint(),
            "MULTIPOINT ((0 0), (10 10), (10 20), (20 20))"  );
    // under MonoValent, the common point is not on the boundary (it has valence > 1)
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryMonovalentEndPoint(),
            "MULTIPOINT ((0 0), (10 20), (20 20))"  );
    // under MultiValent, the common point is the only point on the boundary
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryMultivalentEndPoint(),
            "POINT (10 10)"  );
}

// testMultiLinestd::stringWithRingTouchAtEndpoint
template<>
template<>
void object::test<4>()
{
    std::string a = "MULTILINESTRING ((100 100, 20 20, 200 20, 100 100), (100 200, 100 100))";
    // under Mod-2, the ring has no boundary, so the line intersects the interior ==> not simple
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryRuleMod2(),
            "MULTIPOINT ((100 100), (100 200))" );
    // under Endpoint, the ring has a boundary point, so the line does NOT intersect the interior ==> simple
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryEndPoint(),
            "MULTIPOINT ((100 100), (100 200))"  );
}

// testRing
template<>
template<>
void object::test<5>()
{
    std::string a = "LINESTRING (100 100, 20 20, 200 20, 100 100)";
    // rings are simple under all rules
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryRuleMod2(),
            "MULTIPOINT EMPTY");
    runBoundaryTest(a, BoundaryNodeRule::getBoundaryEndPoint(),
            "POINT (100 100)"  );
}

// testHasBoundaryPoint
template<>
template<>
void object::test<6>()
{
    checkHasBoundary( "POINT (0 0)", false);
}

// testHasBoundaryPointEmpty
template<>
template<>
void object::test<7>()
{
    checkHasBoundary( "POINT EMPTY", false);
}

// testHasBoundaryRingClosed
template<>
template<>
void object::test<8>()
{
    checkHasBoundary( "LINESTRING (100 100, 20 20, 200 20, 100 100)", false);
}

// testHasBoundaryMultiLinestd::stringClosed
template<>
template<>
void object::test<9>()
{
    checkHasBoundary( "MULTILINESTRING ((0 0, 0 1), (0 1, 1 1, 1 0, 0 0))", false);
}

// testHasBoundaryMultiLinestd::stringOpen
template<>
template<>
void object::test<10>()
{
    checkHasBoundary( "MULTILINESTRING ((0 0, 0 2), (0 1, 1 1, 1 0, 0 0))");
}

// testHasBoundaryPolygon
template<>
template<>
void object::test<11>()
{
    checkHasBoundary( "POLYGON ((1 9, 9 9, 9 1, 1 1, 1 9))");
}

// testHasBoundaryPolygonEmpty
template<>
template<>
void object::test<12>()
{
    checkHasBoundary( "POLYGON EMPTY", false);
}

}
