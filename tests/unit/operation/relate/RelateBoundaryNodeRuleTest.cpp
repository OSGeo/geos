#include <tut/tut.hpp>
// geos
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/algorithm/BoundaryNodeRule.h>
#include <geos/operation/relate/RelateOp.h>
// std
#include <string>
#include <memory>

using namespace geos::algorithm;
using namespace geos::geom;
using namespace geos::operation::relate;

namespace tut {
//----------------------------------------------
// Test Group
//----------------------------------------------

struct test_relateboundarynoderule_data {
    geos::io::WKTReader wktreader;

    void runRelateTest(const std::string& wkt1, const std::string& wkt2, const BoundaryNodeRule& bnRule, const std::string& imExpected)
    {
        auto g1 = wktreader.read(wkt1);
        auto g2 = wktreader.read(wkt2);

        auto im = RelateOp::relate(g1.get(), g2.get(), bnRule);
        auto imActual = im->toString();

        ensure_equals(imExpected, imActual);
    }
};

typedef test_group<test_relateboundarynoderule_data> group;
typedef group::object object;

group test_relateboundarynoderule_group("geos::operation::relate::RelateBoundaryNodeRule");

// testMultiLineStringSelfIntTouchAtEndpoint
template<>
template<>
void object::test<1>()
{
    std::string a = "MULTILINESTRING ((20 20, 100 100, 100 20, 20 100), (60 60, 60 140))";
    std::string b = "LINESTRING (60 60, 20 60)";
    // under EndPoint, A has a boundary node - A.bdy / B.bdy = 0
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "FF1F00102");
}

// testLinestd::stringSelfIntTouchAtEndpoint
template<>
template<>
void object::test<2>()
{
    std::string a = "LINESTRING (20 20, 100 100, 100 20, 20 100)";
    std::string b = "LINESTRING (60 60, 20 60)";
    // results for both rules are the same
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryOGCSFS(),   "F01FF0102");
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "F01FF0102");
}

// testMultiLinestd::stringTouchAtEndpoint
template<>
template<>
void object::test<3>()
{
    std::string a = "MULTILINESTRING ((0 0, 10 10), (10 10, 20 20))";
    std::string b = "LINESTRING (10 10, 20 0)";

    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "FF1F00102");
}

// testLineRingTouchAtEndpoints
template<>
template<>
void object::test<4>()
{
    std::string a = "LINESTRING (20 100, 20 220, 120 100, 20 100)";
    std::string b = "LINESTRING (20 20, 20 100)";

    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryMultivalentEndPoint(),  "0F1FFF1F2");
}

// testLineRingTouchAtEndpointAndInterior
template<>
template<>
void object::test<5>()
{
    std::string a = "LINESTRING (20 100, 20 220, 120 100, 20 100)";
    std::string b = "LINESTRING (20 20, 40 100)";

    // this is the same result as for the above test
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryOGCSFS(),   "F01FFF102");
    // this result is different - the A node is now on the boundary, so A.bdy/B.ext = 0
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "F01FF0102");
}

// testPolygonEmptyRing
template<>
template<>
void object::test<6>()
{
    std::string a = "POLYGON EMPTY";
    std::string b = "LINESTRING (20 100, 20 220, 120 100, 20 100)";

    // closed line has no boundary under SFS rule
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryOGCSFS(),   "FFFFFF1F2");

    // closed line has boundary under ENDPOINT rule
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "FFFFFF102");
}

// testPolygonEmptyMultiLinestd::stringClosed
template<>
template<>
void object::test<7>()
{
    std::string a = "POLYGON EMPTY";
    std::string b = "MULTILINESTRING ((0 0, 0 1), (0 1, 1 1, 1 0, 0 0))";

    // closed line has no boundary under SFS rule
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryOGCSFS(),   "FFFFFF1F2");

    // closed line has boundary under ENDPOINT rule
    runRelateTest(a, b,  BoundaryNodeRule::getBoundaryEndPoint(),  "FFFFFF102");
}

}
