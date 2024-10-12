//
// Test Suite for geos::triangulate::polygon::VertexSequencePackedRtree
//
// tut
#include <tut/tut.hpp>

// geos
#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/index/VertexSequencePackedRtree.h>

using geos::index::VertexSequencePackedRtree;
using geos::geom::Point;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_packedrtree_data {

    geos::io::WKTReader r;
    geos::geom::CoordinateSequence coords;
    std::vector<std::size_t> expected;

    test_packedrtree_data() {}

    std::unique_ptr<VertexSequencePackedRtree>
    createSPRtree(std::string& multipointwkt)
    {
        coords.clear();
        auto geom_a = r.read(multipointwkt);
        for (std::size_t i = 0; i < geom_a->getNumGeometries(); i++) {
            auto pt = dynamic_cast<const Point*>(geom_a->getGeometryN(i));
            coords.add(Coordinate(pt->getX(), pt->getY()));
        }
        std::unique_ptr<VertexSequencePackedRtree> vspr(new VertexSequencePackedRtree(coords));
        return vspr;
    }

    Envelope
    getEnvelope(double minx, double miny, double maxx, double maxy)
    {
        return Envelope(minx, maxx, miny, maxy);
    }

    bool
    isEqualResult(std::vector<std::size_t>& expectedIds,
        std::vector<std::size_t>& resultIds)
    {
        for (std::size_t i = 0; i < resultIds.size(); i++) {
            if (expectedIds[i] != resultIds[i])
                return false;
        }
        return true;
    }

    void
    checkQuery(std::string& wkt,
        const Envelope& queryEnv,
        std::vector<std::size_t> expectedIds)
    {
        auto tree = createSPRtree(wkt);
        std::vector<std::size_t> resultIds;
        tree->query(queryEnv, resultIds);
        ensure("result size differs from expected", expected.size() == resultIds.size());
        ensure("result values differ from expected", isEqualResult(expectedIds, resultIds));
    }

};


typedef test_group<test_packedrtree_data> group;
typedef group::object object;

group test_packedrtree_group("geos::index::VertexSequencePackedRtree");


// test1
template<>
template<>
void object::test<1>
()
{
    std::string wkt("MULTIPOINT((1 1))");
    expected = {0};
    checkQuery(wkt, getEnvelope(1,1, 4,4), expected);
}

// test2
template<>
template<>
void object::test<2>
()
{
    std::string wkt("MULTIPOINT((0 0), (1 1))");
    expected = {1};
    checkQuery(wkt, getEnvelope(1,1, 4,4), expected);
}


// test3
template<>
template<>
void object::test<3>
()
{
    std::string wkt("MULTIPOINT((0 0), (1 1), (2 2), (3 3), (4 4), (5 5))");

    expected = {2,3,4};
    checkQuery(wkt, getEnvelope(2,2, 4,4), expected);

    expected = {0};
    checkQuery(wkt, getEnvelope(0,0, 0,0), expected);
}

// test10
template<>
template<>
void object::test<4>
()
{
    std::string wkt("MULTIPOINT((0 0), (1 1), (2 2), (3 3), (4 4), (5 5), (6 6), (7 7), (8 8), (9 9), (10 10))");

    expected = {2,3,4};
    checkQuery(wkt, getEnvelope(2,2, 4,4), expected);

    expected = {7,8};
    checkQuery(wkt, getEnvelope(7,7, 8,8), expected);

    expected = {0};
    checkQuery(wkt, getEnvelope(0,0, 0,0), expected);
}

// test6WithDups
template<>
template<>
void object::test<5>
()
{
    std::string wkt("MULTIPOINT((0 0), (1 1), (2 2), (3 3), (4 4), (5 5), (4 4), (3 3), (2 2), (1 1), (0 0))");

    expected = {2,3,4,6,7,8};
    checkQuery(wkt, getEnvelope(2,2, 4,4), expected);

    expected = {0, 10};
    checkQuery(wkt, getEnvelope(0,0, 0,0), expected);
}


// test10
template<>
template<>
void object::test<6>
()
{
    std::string wkt("MULTIPOINT((0 0), (1 1), (2 2), (3 3), (4 4), (5 5), (6 6), (7 7), (8 8), (9 9), (10 10), (11 11), (12 12), (13 13), (14 14), (15 15), (16 16), (17 17), (18 18), (17 17), (16 16))");

    expected = {2,3,4};
    checkQuery(wkt, getEnvelope(2,2, 4,4), expected);
}



} // namespace tut


