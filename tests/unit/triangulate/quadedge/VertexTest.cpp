
//
// Test Suite for geos::triangulate::quadedge::Vertex
//
// tut
#include <tut/tut.hpp>
// geos
#include <geos/triangulate/quadedge/Vertex.h>
//#include <geos/io/WKTWriter.h>
#include <geos/geom/Envelope.h>

using namespace geos::triangulate::quadedge;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_vertex_data {
    test_vertex_data()
    {
    }
};

typedef test_group<test_vertex_data> group;
typedef group::object object;

group test_vertex_group("geos::triangulate::quadedge::Vertex");


//
// Test Cases
//

// 1 - Test for operator<
template<>
template<>
void object::test<1>
()
{
    Vertex v1(10, 20);
    Vertex v2(20, 30);
    Vertex v3(0, 100);
    Vertex v4(10, 30);
    Vertex v5(10, 10);

    ensure(v1 < v2);
    ensure(!(v1 < v3));
    ensure(v1 < v4);
    ensure(!(v1 < v5));
    ensure(!(v4 < v5));
}

} // namespace tut


