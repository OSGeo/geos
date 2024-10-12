//
// Test Suite for geos::noding::snapround::MCIndexSnapRounder class.

#include <tut/tut.hpp>
// geos
#include <geos/noding/SegmentNode.h>
#include <geos/noding/Noder.h>
#include <geos/noding/NodedSegmentString.h>
#include <geos/noding/SegmentString.h>
#include <geos/noding/snapround/MCIndexSnapRounder.h>
#include <geos/io/WKTWriter.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKBReader.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiLineString.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by all tests
struct test_mcidxsnprndr_data {

    typedef geos::noding::SegmentString SegmentString;
    typedef geos::noding::NodedSegmentString NodedSegmentString;
    typedef geos::noding::snapround::MCIndexSnapRounder MCIndexSnapRounder;
    typedef geos::noding::Noder Noder;
    typedef geos::io::WKTReader WKTReader;
    typedef geos::io::WKBReader WKBReader;
    typedef geos::io::WKTWriter WKTWriter;
    typedef geos::io::WKBWriter WKBWriter;
    typedef geos::geom::Geometry Geometry;
    typedef geos::geom::PrecisionModel PrecisionModel;
    typedef geos::geom::CoordinateSequence CoordinateSequence;

    typedef std::unique_ptr<CoordinateSequence> CoordSeqPtr;
    typedef std::unique_ptr<Geometry> GeomPtr;

    typedef std::vector<SegmentString*> SegStrVct;
    typedef std::vector<Geometry*> GeomVct;

    const geos::geom::GeometryFactory* gf_;

    test_mcidxsnprndr_data()
        : gf_(geos::geom::GeometryFactory::getDefaultInstance())
    {}

    GeomPtr
    getGeometry(SegStrVct& vct)
    {
        std::vector<std::unique_ptr<Geometry>> lines;
        for(SegStrVct::size_type i = 0, n = vct.size(); i < n; ++i) {
            SegmentString* ss = vct[i];
            lines.push_back(gf_->createLineString(*(ss->getCoordinates())));
        }
        return gf_->createMultiLineString(std::move(lines));
    }

    void
    getSegmentStrings(const Geometry& g, SegStrVct& vct)
    {
        CoordSeqPtr s(g.getCoordinates());
        vct.push_back(new NodedSegmentString(s.release(), g.hasZ(), g.hasM(), nullptr));
    }

    GeomPtr
    readGeometry(const std::string& wkt)
    {
        GeomPtr g;
        if(wkt[0] == '0' || wkt[0] == '1') {
            WKBReader r;
            std::istringstream is(wkt);
            g = r.readHEX(is);
        }
        else {
            WKTReader r;
            g = r.read(wkt);
        }
        return g;
    }

    void
    getSegmentStrings(const std::string& wkt, SegStrVct& vct)
    {
        GeomPtr g = readGeometry(wkt);
        getSegmentStrings(*g, vct);
    }

    void
    freeSegmentStrings(SegStrVct& vct)
    {
        for(SegStrVct::size_type i = 0, n = vct.size(); i < n; ++i) {
            delete vct[i];
        }
    }

private:
    test_mcidxsnprndr_data(test_mcidxsnprndr_data const&); // = delete
    test_mcidxsnprndr_data& operator=(test_mcidxsnprndr_data const&); // = delete
};


typedef test_group<test_mcidxsnprndr_data> group;
typedef group::object object;

group test_mcidxsnprndr_group(
    "geos::noding::snapround::MCIndexSnapRounder"
);

//
// Test Cases
//

// This test would fail before fix in r3528
template<>
template<>
void object::test<1>
()
{
    std::string
    wkt0("LINESTRING(99739.70596 -2239218.0625,99739.85604 -2239218.21258,99739.85605 -2239218.21258, 99739.85605 -2239218.21258,99739.86851 -2239218.21258,99739.86851 -2239218.20012, 99739.86851 -2239218.20012,99739.86851 -2239218.04, 99739.95848 -2239218.11015,99739.86851 -2239218.20012, 99739.86851 -2239218.20012,99739.85605 -2239218.21258, 99739.85605 -2239218.21258,99739.80901 -2239218.25961, 99739.80901 -2239218.25961,99739.68863 -2239218.38, 99739.86204 -2239218.31264,99739.80901 -2239218.25961, 99739.80901 -2239218.25961,99739.66841 -2239218.11901)");
    SegStrVct nodable;
    getSegmentStrings(wkt0, nodable);
    ensure_equals(nodable.size(), 1u);

    PrecisionModel pm(1e-5);
    MCIndexSnapRounder noder(pm);

    ensure_equals(nodable.size(), 1u);
    noder.computeNodes(&nodable);
    std::unique_ptr<SegStrVct> noded(noder.getNodedSubstrings());

    ensure_equals("1e-5", noded->size(), 178u);

    freeSegmentStrings(*noded);
    freeSegmentStrings(nodable);
}


} // namespace tut
