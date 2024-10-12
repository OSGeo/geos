#include <tut/tut.hpp>
// geos
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/algorithm/locate/IndexedPointInAreaLocator.h>

// std
#include <memory>

using geos::geom::CoordinateXY;
using geos::geom::CoordinateSequence;
using geos::geom::GeometryFactory;
using geos::geom::Location;
using geos::algorithm::locate::IndexedPointInAreaLocator;

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_indexedpointinarealocator_data {
    const GeometryFactory& factory_ = *GeometryFactory::getDefaultInstance();
};

typedef test_group<test_indexedpointinarealocator_data> group;
typedef group::object object;

group test_indexedpointinarealocator_group("geos::algorithm::locate::IndexedPointInAreaLocator");

// Test all CoordinateSequence dimensions
template<>
template<>
void object::test<1>
()
{
    // XY
    CoordinateSequence seq_xy = CoordinateSequence::XY(0);
    seq_xy.add(0.0, 0.0);
    seq_xy.add(1.0, 0.0);
    seq_xy.add(1.0, 1.0);
    seq_xy.add(0.0, 1.0);
    seq_xy.add(0.0, 0.0);
    auto ls_xy = factory_.createLineString(seq_xy.clone());
    IndexedPointInAreaLocator ipa_xy(*ls_xy);

    CoordinateXY pt_boundary(0.5, 0);
    CoordinateXY pt_interior(0.5, 0.5);
    CoordinateXY pt_exterior(1.5, 0.5);

    ensure_equals(ipa_xy.locate(&pt_boundary), Location::BOUNDARY);
    ensure_equals(ipa_xy.locate(&pt_interior), Location::INTERIOR);
    ensure_equals(ipa_xy.locate(&pt_exterior), Location::EXTERIOR);

    // XYZ
    CoordinateSequence seq_xyz = CoordinateSequence::XYZ(0);
    seq_xyz.add(seq_xy);
    auto ls_xyz = factory_.createLineString(seq_xyz.clone());

    IndexedPointInAreaLocator ipa_xyz(*ls_xy);
    ensure_equals(ipa_xyz.locate(&pt_boundary), Location::BOUNDARY);
    ensure_equals(ipa_xyz.locate(&pt_interior), Location::INTERIOR);
    ensure_equals(ipa_xyz.locate(&pt_exterior), Location::EXTERIOR);

    // XYM
    CoordinateSequence seq_xym = CoordinateSequence::XYM(0);
    seq_xym.add(seq_xy);
    auto ls_xym = factory_.createLineString(seq_xym.clone());

    IndexedPointInAreaLocator ipa_xym(*ls_xy);
    ensure_equals(ipa_xym.locate(&pt_boundary), Location::BOUNDARY);
    ensure_equals(ipa_xym.locate(&pt_interior), Location::INTERIOR);
    ensure_equals(ipa_xym.locate(&pt_exterior), Location::EXTERIOR);

    // XYZM
    CoordinateSequence seq_xyzm = CoordinateSequence::XYZM(0);
    seq_xyzm.add(seq_xy);
    auto ls_xyzm = factory_.createLineString(seq_xyzm.clone());

    IndexedPointInAreaLocator ipa_xyzm(*ls_xy);
    ensure_equals(ipa_xyzm.locate(&pt_boundary), Location::BOUNDARY);
    ensure_equals(ipa_xyzm.locate(&pt_interior), Location::INTERIOR);
    ensure_equals(ipa_xyzm.locate(&pt_exterior), Location::EXTERIOR);
}

}

