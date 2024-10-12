//
// Test Suite for geos::operation::sharedpaths::SharedPathsOp class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/sharedpaths/SharedPathsOp.h>
#include <geos/constants.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/util/IllegalArgumentException.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_shpathop_data {
    typedef geos::operation::sharedpaths::SharedPathsOp SharedPathsOp;

    geos::io::WKTReader wktreader;
    geos::io::WKTWriter wktwriter;

    typedef geos::geom::Geometry::Ptr GeomPtr;

    SharedPathsOp::PathList forwDir;
    SharedPathsOp::PathList backDir;

    test_shpathop_data()
        : wktreader(), wktwriter()
    {
    }

private:
    // noncopyable
    test_shpathop_data(test_shpathop_data const& other) = delete;
    test_shpathop_data& operator=(test_shpathop_data const& rhs) = delete;
};

typedef test_group<test_shpathop_data> group;
typedef group::object object;

group test_shpathop_group("geos::operation::sharedpaths::SharedPathsOp");

//
// Test Cases
//

// Point (illegal arg)
template<> template<>
void object::test<1>
()
{
    GeomPtr g0(wktreader.read("POINT(0 0)"));
    GeomPtr g1(wktreader.read("POINT(1 1)"));
    bool threw = false;
    try {
        SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    }
    catch(const geos::util::IllegalArgumentException& /*ex*/) {
        threw = true;
    }
    ensure(threw);
}

// Poly (illegal arg)
template<> template<>
void object::test<2>
()
{
    GeomPtr g0(wktreader.read("POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))"));
    GeomPtr g1(wktreader.read("LINESTRING(0 0, 10 0)"));
    bool threw = false;
    try {
        SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    }
    catch(const geos::util::IllegalArgumentException& /*ex*/) {
        threw = true;
    }
    ensure(threw);
}

// Line-Line (disjoint)
template<> template<>
void object::test<3>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(0 0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(20 0, 20 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(forwDir.empty());
    ensure(backDir.empty());
}

// Line-Line (crossing)
template<> template<>
void object::test<4>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(0 0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(-10 5, 10 5)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(forwDir.empty());
    ensure(backDir.empty());
}

// Line-Line (overlapping, forward)
template<> template<>
void object::test<5>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(0 0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(5 0, 15 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(backDir.empty());
    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (5 0, 10 0)");
    SharedPathsOp::clearEdges(forwDir);
}

// Line-Line (overlapping, backward)
template<> template<>
void object::test<6>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(0 0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(15 0, 5 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(forwDir.empty());
    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (5 0, 10 0)");
    SharedPathsOp::clearEdges(backDir);
}

// Line-Line (overlapping, backward, swapped)
template<> template<>
void object::test<7>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(15 0, 5 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(0 0, 10 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(forwDir.empty());
    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (10 0, 5 0)");
    SharedPathsOp::clearEdges(backDir);
}

// Line-Line (contained, forward)
template<> template<>
void object::test<8>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(-15 0, 15 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(-10 0, 10 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(backDir.empty());
    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-10 0, 10 0)");
    SharedPathsOp::clearEdges(forwDir);
}

// Line-Line (contained, backward)
template<> template<>
void object::test<9>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(-15 0, 15 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(10 0, -10 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure(forwDir.empty());
    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (-10 0, 10 0)");
    SharedPathsOp::clearEdges(backDir);
}

// Line-Line (back and forw)
template<> template<>
void object::test<10>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(-15 0, 15 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(-10 0, -5 0, 0 5, 10 0, 5 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-10 0, -5 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (5 0, 10 0)");
    SharedPathsOp::clearEdges(backDir);
}

// Line-Line (back and forw, swapped)
template<> template<>
void object::test<11>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(-10 0, -5 0, 0 5, 10 0, 5 0)"));
    GeomPtr g1(wktreader.read("LINESTRING(-15 0, 15 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-10 0, -5 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (10 0, 5 0)");
    SharedPathsOp::clearEdges(backDir);
}

// MultiLine-Line (back and forth)
template<> template<>
void object::test<12>
()
{
    GeomPtr g0(wktreader.read("MULTILINESTRING((-10 0, -5 0),(10 0, 5 0))"));
    GeomPtr g1(wktreader.read("LINESTRING(-15 0, 15 0)"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-10 0, -5 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (10 0, 5 0)");
    SharedPathsOp::clearEdges(backDir);
}

// MultiLine-MultiLine (back and forth)
template<> template<>
void object::test<13>
()
{
    GeomPtr g0(wktreader.read("MULTILINESTRING((-10 0, -5 0),(10 0, 5 0))"));
    GeomPtr g1(wktreader.read("MULTILINESTRING((-8 0, -2 0),(6 0, 12 0))"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-8 0, -5 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (10 0, 6 0)");
    SharedPathsOp::clearEdges(backDir);
}

// MultiLine-MultiLine (forth and forth)
template<> template<>
void object::test<14>
()
{
    GeomPtr g0(wktreader.read("MULTILINESTRING((-10 0, -5 0),(10 0, 5 0))"));
    GeomPtr g1(wktreader.read("MULTILINESTRING((-8 0, -2 0),(12 0, 6 0))"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(forwDir.size(), 2u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (-8 0, -5 0)");
    ensure_equals(wktwriter.write(forwDir[1]), "LINESTRING (10 0, 6 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure(backDir.empty());
}

// MultiLine-MultiLine (back and back)
template<> template<>
void object::test<15>
()
{
    GeomPtr g0(wktreader.read("MULTILINESTRING((-10 0, -5 0),(10 0, 5 0))"));
    GeomPtr g1(wktreader.read("MULTILINESTRING((6 0, 12 0),(-2 0, -8 0))"));
    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);

    ensure_equals(backDir.size(), 2u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (-8 0, -5 0)");
    ensure_equals(wktwriter.write(backDir[1]), "LINESTRING (10 0, 6 0)");
    SharedPathsOp::clearEdges(backDir);

    ensure(forwDir.empty());
}

// line-line (equals, forward)
template<> template<>
void object::test<16>
()
{
    GeomPtr g0(wktreader.read("LINESTRING(0 0, 5 10, 10 10)"));
    GeomPtr g1(wktreader.read("LINESTRING(0 0, 5 10, 10 10)"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(forwDir.size(), 2u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (0 0, 5 10)");
    ensure_equals(wktwriter.write(forwDir[1]), "LINESTRING (5 10, 10 10)");
    SharedPathsOp::clearEdges(forwDir);

    ensure(backDir.empty());
}

// line-line (equals, backward)
template<> template<>
void object::test<17>
()
{
    GeomPtr g0(wktreader.read("LINESTRING( 0  0, 5 10, 10 10)"));
    GeomPtr g1(wktreader.read("LINESTRING(10 10, 5 10,  0  0)"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(backDir.size(), 2u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (0 0, 5 10)");
    ensure_equals(wktwriter.write(backDir[1]), "LINESTRING (5 10, 10 10)");
    SharedPathsOp::clearEdges(backDir);

    ensure(forwDir.empty());
}

// line - line_closed_no_rhr
template<> template<>
void object::test<18>
()
{
    GeomPtr g0(wktreader.read("LINESTRING( 0  0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING( 0  0, 10 0, 10 10, 0 10, 0 0 )"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (0 0, 10 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure(backDir.empty());
}

// line_closed_no_rhr - line
template<> template<>
void object::test<19>
()
{
    GeomPtr g0(wktreader.read("LINESTRING( 0  0, 10 0, 10 10, 0 10, 0 0 )"));
    GeomPtr g1(wktreader.read("LINESTRING( 0  0, 10 0)"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(forwDir.size(), 1u);
    ensure_equals(wktwriter.write(forwDir[0]), "LINESTRING (0 0, 10 0)");
    SharedPathsOp::clearEdges(forwDir);

    ensure(backDir.empty());
}

// line - line_closed_rhr (see ticket #391)
template<> template<>
void object::test<20>
()
{
    GeomPtr g0(wktreader.read("LINESTRING( 0  0, 10 0)"));
    GeomPtr g1(wktreader.read("LINESTRING( 0  0, 0 10, 10 10, 10 0, 0 0 )"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (0 0, 10 0)");
    SharedPathsOp::clearEdges(backDir);

    ensure(forwDir.empty());
}

// line_closed_rhr - line
template<> template<>
void object::test<21>
()
{
    GeomPtr g0(wktreader.read("LINESTRING( 0  0, 0 10, 10 10, 10 0, 0 0 )"));
    GeomPtr g1(wktreader.read("LINESTRING( 0  0, 10 0)"));

    forwDir.clear();
    backDir.clear();
    SharedPathsOp::sharedPathsOp(*g0, *g1, forwDir, backDir);
    ensure_equals(backDir.size(), 1u);
    ensure_equals(wktwriter.write(backDir[0]), "LINESTRING (10 0, 0 0)");
    SharedPathsOp::clearEdges(backDir);

    ensure(forwDir.empty());
}

} // namespace tut

