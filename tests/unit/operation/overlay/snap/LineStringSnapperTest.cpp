//
// Test Suite for geos::operation::overlay::snap::LineStringSnapper class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/overlay/snap/LineStringSnapper.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateList.h>
#include <geos/geom/CoordinateSequence.h>
// std
#include <vector>

using geos::geom::CoordinateSequence;

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_linestringsnapper_data {

    test_linestringsnapper_data() {}
};

typedef test_group<test_linestringsnapper_data> group;
typedef group::object object;

group test_linestringsnapper_group("geos::operation::overlay::snap::LineStringSnapper");

//
// Test Cases
//

// Test vertices snapping
template<>
template<>
void object::test<1>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates
    Coordinate src_a(0, 0);
    Coordinate src_b(10, 10);

    // snap coordinates
    Coordinate snp_a(0.1, 0);
    Coordinate snp_b(10, 10.1);

    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);

    std::vector<const Coordinate*> snpCoords;
    snpCoords.push_back(&snp_a);
    snpCoords.push_back(&snp_b);

    LineStringSnapper snapper(srcCoords, 0.4);

    auto ret(snapper.snapTo(snpCoords));

    // both points should have been snapped
    ensure_equals(ret->size(), 2u);
    ensure_equals(ret->operator[](0), snp_a);
    ensure_equals(ret->operator[](1), snp_b);

}

// Test segment snapping
template<>
template<>
void object::test<2>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates
    Coordinate src_a(0, 0);
    Coordinate src_b(10, 10);

    // snap coordinates
    Coordinate snp_a(0.4, 0);
    Coordinate snp_b(10, 10.4);

    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);

    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);
    snpCoords.push_back(&snp_b);

    LineStringSnapper snapper(srcCoords, 0.3);

    auto ret(snapper.snapTo(snpCoords));

    // snap point a should be inserted
    ensure_equals(ret->size(), 3u);
    ensure_equals(ret->operator[](0), src_a);
    ensure_equals(ret->operator[](1), snp_a);
    ensure_equals(ret->operator[](2), src_b);

}

// Test vertices snapping in a closed ring
template<>
template<>
void object::test<3>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates

    Coordinate src_a(0, 0);
    Coordinate src_b(10, 10);
    Coordinate src_c(0, 10);

    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);
    srcCoords.add(src_c);
    srcCoords.add(src_a);

    // snap coordinates

    Coordinate snp_a(0.1, 0);
    Coordinate snp_b(10, 10.1);

    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);
    snpCoords.push_back(&snp_b);

    LineStringSnapper snapper(srcCoords, 0.4);

    auto ret(snapper.snapTo(snpCoords));

    // Points A and B should be snapped
    ensure_equals(ret->size(), 4u);
    ensure_equals(ret->operator[](0), snp_a);
    ensure_equals(ret->operator[](1), snp_b);
    ensure_equals(ret->operator[](2), src_c);
    ensure_equals(ret->operator[](3), snp_a);

}

// Test vertices snapping in a short sequence
template<>
template<>
void object::test<4>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates

    Coordinate src_a(0, 0);
    CoordinateSequence srcCoords;
    srcCoords.add(src_a);

    // snap coordinates

    Coordinate snp_a(0.1, 0);
    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);

    LineStringSnapper snapper(srcCoords, 0.4);

    auto ret(snapper.snapTo(snpCoords));

    ensure_equals(ret->size(), 1u);
    ensure_equals(ret->operator[](0), snp_a);

}

// Test vertices snapping in an empty sequence
template<>
template<>
void object::test<5>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates

    CoordinateSequence srcCoords;

    // snap coordinates

    Coordinate::ConstVect snpCoords;

    LineStringSnapper snapper(srcCoords, 0.4);

    auto ret(snapper.snapTo(snpCoords));

    ensure_equals(ret->size(), 0u);

}

// Test snapping an empty sequence
template<>
template<>
void object::test<6>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // source coordinates

    CoordinateSequence srcCoords;

    // snap coordinates

    Coordinate snp_a(0.1, 0);
    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);

    LineStringSnapper snapper(srcCoords, 0.4);

    auto ret(snapper.snapTo(snpCoords));

    ensure_equals(ret->size(), 0u);

}

// Test allow snapping to source vertices
template<>
template<>
void object::test<7>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // Source: (0 0, 10 0, 0 1)
    Coordinate src_a(0, 0);
    Coordinate src_b(10, 0);
    Coordinate src_c(0, 1);
    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);
    srcCoords.add(src_c);

    // Snap: (0 0)
    Coordinate snp_a(0, 0);
    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);

    // Snap with tolerance of 1
    // (both first and second point could be snapped)
    LineStringSnapper snapper(srcCoords, 1);

    // Allow source-snapping, expect: (0 0, 5 0, 0 0, 10 0)
    snapper.setAllowSnappingToSourceVertices(true);
    auto ret(snapper.snapTo(snpCoords));

    ensure_equals(ret->size(), 4u);
    ensure_equals(ret->operator[](0), src_a);
    ensure_equals(ret->operator[](1), src_b);
    ensure_equals(ret->operator[](2), snp_a);
    ensure_equals(ret->operator[](3), src_c);

    // Do not allow source-snapping, expect: (0 0, 5 0, 10 0)
    snapper.setAllowSnappingToSourceVertices(false);
    ret = snapper.snapTo(snpCoords);
    ensure_equals(ret->size(), 3u);
    ensure_equals(ret->operator[](0), src_a);
    ensure_equals(ret->operator[](1), src_b);
    ensure_equals(ret->operator[](2), src_c);
}

// Test two candidate vertices snaps
template<>
template<>
void object::test<8>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // Source: (0 0, 1 0, 1 1)
    Coordinate src_a(0, 0);
    Coordinate src_b(1, 0);
    Coordinate src_c(1, 1);
    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);
    srcCoords.add(src_c);

    // Snap: (0.5, 0)
    Coordinate snp_a(0.5, 0);
    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);

    // Snap with tolerance of 1
    // (both first and second point could be snapped)
    LineStringSnapper snapper(srcCoords, 1);

    // Expect: (0.5 0, 1 0, 1 1)
    auto ret(snapper.snapTo(snpCoords));
    ensure_equals(ret->size(), 3u);
    ensure_equals(ret->operator[](0), snp_a);
    ensure_equals(ret->operator[](1), src_b);
    ensure_equals(ret->operator[](2), src_c);
}

// Snap of last segment in closed linestring
// See https://trac.osgeo.org/geos/ticket/758
template<>
template<>
void object::test<9>
()
{
    using geos::geom::Coordinate;
    using geos::operation::overlay::snap::LineStringSnapper;

    // Source: (1 1, 5 9, 9 1, 1 1)
    Coordinate src_a(1, 1);
    Coordinate src_b(5, 9);
    Coordinate src_c(9, 1);
    CoordinateSequence srcCoords;
    srcCoords.add(src_a);
    srcCoords.add(src_b);
    srcCoords.add(src_c);
    srcCoords.add(src_a);

    // Snap: (0 0, 10 0, 1 0.5)
    Coordinate snp_a(0, 0);
    Coordinate snp_b(10, 0);
    Coordinate snp_c(1, 0.5);
    Coordinate::ConstVect snpCoords;
    snpCoords.push_back(&snp_a);
    snpCoords.push_back(&snp_b);
    snpCoords.push_back(&snp_c);

    // Snap with tolerance of 2
    // (both first and second point could be snapped)
    LineStringSnapper snapper(srcCoords, 2);

    // Expect: (0 0, 5 9, 10 0, 1 0.5, 0 0)
    auto ret(snapper.snapTo(snpCoords));
    ensure_equals(ret->size(), 5u);
    ensure_equals(ret->operator[](0), snp_a); //  0 0
    ensure_equals(ret->operator[](1), src_b); //  5 9
    ensure_equals(ret->operator[](2), snp_b); // 10 0
    ensure_equals(ret->operator[](3), snp_c); //  1 0.5
    ensure_equals(ret->operator[](4), snp_a); //  0 0
}

} // namespace tut
