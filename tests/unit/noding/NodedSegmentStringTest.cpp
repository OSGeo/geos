//
// Test Suite for geos::noding::NodedSegmentString class.

#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/io/WKTReader.h>
#include <geos/noding/NodedSegmentString.h>
#include <geos/noding/SegmentString.h>
#include <geos/noding/Octant.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/util.h>
// std
#include <memory>

using geos::io::WKTReader;
using geos::geom::CoordinateSequence;
using geos::geom::Geometry;
using geos::geom::GeometryFactory;
using geos::noding::SegmentString;

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_nodedsegmentstring_data {

    typedef std::unique_ptr<geos::geom::CoordinateSequence> \
    CoordinateSequenceAutoPtr;

    typedef std::unique_ptr<geos::noding::NodedSegmentString> \
    SegmentStringAutoPtr;

    WKTReader r;

    SegmentStringAutoPtr
    makeSegmentString(geos::geom::CoordinateSequence* cs, void* d = nullptr)
    {
        return SegmentStringAutoPtr(
                   new geos::noding::NodedSegmentString(cs, true, false, d)
               );
    }

    std::unique_ptr<Geometry>
    toLines(SegmentString::NonConstVect& ss, const GeometryFactory* gf)
    {
        std::vector<std::unique_ptr<Geometry>> lines;
        for (auto s: ss)
        {
            std::unique_ptr<CoordinateSequence> cs = s->getCoordinates()->clone();
            lines.push_back(gf->createLineString(std::move(cs)));
        }
        return gf->createMultiLineString(std::move(lines));
    }

    void
    checkNoding(const std::string& wktLine, const std::string& wktNodes, std::vector<size_t> segmentIndex, const std::string& wktExpected)
    {
        using geos::noding::NodedSegmentString;

        std::unique_ptr<Geometry> line = r.read(wktLine);
        std::unique_ptr<Geometry> pts = r.read(wktNodes);

        NodedSegmentString nss(line->getCoordinates().release(), true, false, 0);
        std::unique_ptr<CoordinateSequence> node = pts->getCoordinates();

        for (std::size_t i = 0, n=node->size(); i < n; ++i) {
          nss.addIntersection(node->getAt(i), segmentIndex.at(i));
        }

        SegmentString::NonConstVect nodedSS;
        nss.getNodeList().addSplitEdges(nodedSS);
        std::unique_ptr<Geometry> result = toLines(nodedSS, line->getFactory());
        //System.out.println(result);
        for (auto ss: nodedSS) {
            delete ss;
        }
        std::unique_ptr<Geometry> expected = r.read(wktExpected);
        ensure_equals_geometry(expected.get(), result.get());
    }

    test_nodedsegmentstring_data()
    {
    }

    ~test_nodedsegmentstring_data()
    {
    }

};

typedef test_group<test_nodedsegmentstring_data> group;
typedef group::object object;

group test_nodedsegmentstring_group("geos::noding::NodedSegmentString");

//
// Test Cases
//

// test constructor with 2 equal points
template<>
template<>
void object::test<1>
()
{
    auto cs = geos::detail::make_unique<geos::geom::CoordinateSequence>(0u, 2u);

    ensure(nullptr != cs.get());

    geos::geom::Coordinate c0(0, 0);
    geos::geom::Coordinate c1(0, 0);

    cs->add(c0);
    cs->add(c1);

    ensure_equals(cs->size(), 2u);

    SegmentStringAutoPtr ss(makeSegmentString(cs.release()));
    ensure(nullptr != ss.get());

    ensure_equals(ss->size(), 2u);

    ensure_equals(ss->getData(), (void*)nullptr);

    ensure_equals(ss->getCoordinate(0), c0);

    ensure_equals(ss->getCoordinate(1), c1);

    ensure_equals(ss->isClosed(), true);

    ensure_equals(ss->getNodeList().size(), 0u);

    ensure_equals(ss->getSegmentOctant(0), 0);
}

// test constructor with 2 different points
template<>
template<>
void object::test<2>
()
{
    auto cs = geos::detail::make_unique<geos::geom::CoordinateSequence>(0u, 2u);

    ensure(nullptr != cs.get());

    geos::geom::Coordinate c0(0, 0);
    geos::geom::Coordinate c1(1, 0);

    cs->add(c0);
    cs->add(c1);

    ensure_equals(cs->size(), 2u);

    SegmentStringAutoPtr ss(makeSegmentString(cs.release()));
    ensure(nullptr != ss.get());

    ensure_equals(ss->size(), 2u);

    ensure_equals(ss->getData(), (void*)nullptr);

    ensure_equals(ss->getCoordinate(0), c0);

    ensure_equals(ss->getCoordinate(1), c1);

    ensure_equals(ss->isClosed(), false);

    ensure_equals(ss->getSegmentOctant(0), 0);

    ensure_equals(ss->getNodeList().size(), 0u);
}

// test constructor with 4 different points forming a ring
template<>
template<>
void object::test<3>
()
{
    auto cs = geos::detail::make_unique<geos::geom::CoordinateSequence>(0u, 2u);

    ensure(nullptr != cs.get());

    geos::geom::Coordinate c0(0, 0);
    geos::geom::Coordinate c1(1, 0);
    geos::geom::Coordinate c2(1, 1);

    cs->add(c0);
    cs->add(c1);
    cs->add(c2);
    cs->add(c0);

    ensure_equals(cs->size(), 4u);

    SegmentStringAutoPtr ss(makeSegmentString(cs.release()));
    ensure(nullptr != ss.get());

    ensure_equals(ss->size(), 4u);

    ensure_equals(ss->getData(), (void*)nullptr);

    ensure_equals(ss->getCoordinate(0), c0);

    ensure_equals(ss->getCoordinate(1), c1);

    ensure_equals(ss->getCoordinate(2), c2);

    ensure_equals(ss->getCoordinate(3), c0);

    ensure_equals(ss->isClosed(), true);

    ensure_equals(ss->getSegmentOctant(2), 4);

    ensure_equals(ss->getSegmentOctant(1), 1);

    ensure_equals(ss->getSegmentOctant(0), 0);

    ensure_equals(ss->getNodeList().size(), 0u);
}

// test Octant class
template<>
template<>
void object::test<4>
()
{
    geos::geom::Coordinate p0(0, 0);
    geos::geom::Coordinate p1(5, -5);
    int octant_rc1 = 0;
    int octant_rc2 = 0;
    int testPassed = true;
    try {
        octant_rc1 = geos::noding::Octant::octant(p0, p1);
        octant_rc2 = geos::noding::Octant::octant(&p0, &p1);
        testPassed = (octant_rc1 == octant_rc2);
    }
    catch(...) {
        testPassed = false;
    }
    ensure(0 != testPassed);
}

// test adding intersections
template<>
template<>
void object::test<5>
()
{
    geos::geom::Coordinate p0(0, 0);
    geos::geom::Coordinate p1(10, 0);


    auto cs = geos::detail::make_unique<geos::geom::CoordinateSequence>(0u, 2u);
    cs->add(p0);
    cs->add(p1);

    SegmentStringAutoPtr ss(makeSegmentString(cs.release()));

    ensure_equals(ss->getNodeList().size(), 0u);

    // the intersection is invalid, but SegmentString trusts us
    ss->addIntersection(p0, 0);
    ensure_equals(ss->getNodeList().size(), 1u);

    // This node is already present, so shouldn't be
    // accepted as a new one
    ss->addIntersection(p0, 0);
    ensure_equals(ss->getNodeList().size(), 1u);

    ss->addIntersection(p1, 0);
    ensure_equals(ss->getNodeList().size(), 2u);

    ss->addIntersection(p1, 0);
    ensure_equals(ss->getNodeList().size(), 2u);

    ss->addIntersection(p0, 0);
    ensure_equals(ss->getNodeList().size(), 2u);

}

/**
 * Tests a case which involves nodes added when using the SnappingNoder.
 * In this case one of the added nodes is relatively "far" from its segment,
 * and "near" the start vertex of the segment.
 * Computing the noding correctly requires the fix to {@link SegmentNode#compareTo(Object)}
 * added in https://github.com/locationtech/jts/pull/399
 *
 * See https://trac.osgeo.org/geos/ticket/1051
 */
template<>
template<>
void object::test<6>
()
{
    std::vector<size_t> segmentIndex;
    segmentIndex.push_back(0);
    segmentIndex.push_back(0);
    segmentIndex.push_back(1);
    segmentIndex.push_back(1);
    checkNoding("LINESTRING(655103.6628454948 1794805.456674405, 655016.20226 1794940.10998, 655014.8317182435 1794941.5196832407)",
        "MULTIPOINT((655016.29615051334 1794939.965427252),(655016.20226531825 1794940.1099718122), (655016.20226 1794940.10998),(655016.20225819293 1794940.1099794197))",
        segmentIndex,
        "MULTILINESTRING ((655014.8317182435 1794941.5196832407,655016.2022581929 1794940.1099794197), (655016.2022581929 1794940.1099794197, 655016.20226 1794940.10998), (655016.20226 1794940.10998, 655016.2022653183 1794940.1099718122), (655016.2022653183 1794940.1099718122, 655016.2961505133 1794939.965427252), (655016.2961505133 1794939.965427252, 655103.6628454948 1794805.456674405))");
  }



// TODO: test getting noded substrings
//  template<>
//  template<>
//  void object::test<6>()
//  {
//      geos::geom::Coordinate cs1p0(0, 0);
//      geos::geom::Coordinate cs1p1(10, 0);
//      CoordinateSequenceAutoPtr cs1(csFactory->create(0, 2));
//      cs1->add(cs1p0);
//      cs1->add(cs1p1);
//
//      geos::geom::Coordinate cs2p0(5, -5);
//      geos::geom::Coordinate cs2p1(5, 5);
//      CoordinateSequenceAutoPtr cs2(csFactory->create(0, 2));
//      cs2->add(cs2p0);
//      cs2->add(cs2p1);
//
//      using geos::noding::SegmentString;
//      using geos::noding::NodedSegmentString;
//
//      SegmentString::NonConstVect inputStrings;
//      inputStrings.push_back(makeSegmentString(cs2.get()).get());
//
//      std::unique_ptr<SegmentString::NonConstVect> nodedStrings(
//          NodedSegmentString::getNodedSubstrings(inputStrings)
//      );
//
//      ensure_equals(nodedStrings->size(), 0u);
//  }



} // namespace tut

