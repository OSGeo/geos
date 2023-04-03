//
// Test Suite for geos::geom::CoordinateSequence class.

#include <tut/tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/util.h>
#include <utility.h>
// std
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using geos::DoubleNotANumber;
using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::CoordinateXYM;
using geos::geom::CoordinateXYZM;
using geos::geom::CoordinateSequence;

constexpr const std::size_t MAX_NUMBER_OF_TESTS = 100;

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_coordinatearraysequence_data {
    test_coordinatearraysequence_data() {}

    struct Filter : public geos::geom::CoordinateFilter {
        bool is3d;
        Filter() : is3d(false) {}

        void
        filter_rw(geos::geom::CoordinateXY*) const override
        {}

        void
        filter_rw(geos::geom::Coordinate* c) const override
        {
            if(is3d) {
                if(std::isnan(c->z)) {
                    c->z = 0.0;
                }
            }
            else {
                c->z = geos::DoubleNotANumber;
            }
        }
    };
};

typedef test_group<test_coordinatearraysequence_data, MAX_NUMBER_OF_TESTS> group;
typedef group::object object;

group test_coordinatearraysequence_group("geos::geom::CoordinateSequence");

//
// Test Cases
//

// Test of default constructor
template<>
template<>
void object::test<1>
()
{
    const std::size_t size = 0;
    geos::geom::CoordinateSequence sequence;

    ensure("empty sequence is empty", sequence.isEmpty());
    ensure_equals("empty sequence has size zero", sequence.getSize(), size);
    ensure_equals("empty sequence has size zero", sequence.size(), size);

    ensure_equals("empty sequence has dimension 3", sequence.getDimension(), 3u);
    ensure(sequence.hasZ());
    ensure(!sequence.hasM());
}

// Test of overriden constructor
template<>
template<>
void object::test<2>
()
{
    const std::size_t size = 3;
    geos::geom::CoordinateSequence sequence(size);

    ensure("pre-sized sequence is not empty", !sequence.isEmpty());
    ensure_equals("pre-sized sequence has correct size", sequence.getSize(), size);
    ensure_equals("pre-sized sequence has correct size", sequence.size(), size);

    ensure("string rep", sequence.toString() != std::string("()"));

    ensure("Every coodinate in the default sequence should be same.", sequence.hasRepeatedPoints());

    ensure_equals("default/empty coordinates now 2D", sequence.getDimension(), 2u);
    ensure("no Z", !sequence.hasZ());
    ensure("no M", !sequence.hasM());
}

// Test toVector()
template<>
template<>
void object::test<3>
()
{
    CoordinateSequence seq{
        CoordinateXYZM(1, 2, 3, 4),
        CoordinateXYZM(5, 6, 7, 8)};

    std::vector<Coordinate> vec_xyz;
    seq.toVector(vec_xyz);

    ensure_equals(vec_xyz.size(), seq.size());
    ensure(vec_xyz[0].equals3D(seq.getAt<Coordinate>(0)));
    ensure(vec_xyz[1].equals3D(seq.getAt<Coordinate>(1)));

    // Coordinates are appended to existing contents of vector
    std::vector<CoordinateXY> vec_xy{CoordinateXY(0, 3)};
    seq.toVector(vec_xy);

    ensure_equals(vec_xy.size(), seq.size() + 1);
    ensure_equals(vec_xy[0], Coordinate(0, 3));
    ensure_equals(vec_xy[1], seq.getAt<CoordinateXY>(0));
    ensure_equals(vec_xy[2], seq.getAt<CoordinateXY>(1));

    // Optimized copy for XYZ sequence -> XYZ vector
    CoordinateSequence seq_xyz{Coordinate(1, 2, 3), Coordinate(4, 5, 6)};

    std::vector<Coordinate> cvec{Coordinate(7, 8, 9)};
    seq_xyz.toVector(cvec);

    ensure_equals(cvec.size(), 3u);

    ensure(cvec[0].equals3D(Coordinate(7, 8, 9)));
    ensure(cvec[1].equals3D(Coordinate(1, 2, 3)));
    ensure(cvec[2].equals3D(Coordinate(4, 5, 6)));
}

// Test of copy constructor
template<>
template<>
void object::test<4>
()
{
    using geos::geom::Coordinate;

    // Create empty sequence
    const std::size_t sizeEmpty = 0;
    geos::geom::CoordinateSequence empty_original;

    ensure(empty_original.isEmpty());
    ensure_equals(empty_original.size(), sizeEmpty);
    ensure_equals(empty_original.toString(), std::string("()"));

    // Create copy of empty sequence
    geos::geom::CoordinateSequence empty_copy(empty_original);

    ensure(empty_copy.isEmpty());
    ensure_equals(empty_copy.size(), sizeEmpty);
    ensure_equals(empty_copy.toString(), std::string("()"));

    // Create non-empty sequence
    const std::size_t sizeNonEmpty = 2;
    geos::geom::CoordinateSequence non_empty_original;
    non_empty_original.add(Coordinate(1, 2, 3));
    non_empty_original.add(Coordinate(5, 10, 15));

    ensure(!non_empty_original.isEmpty());
    ensure_equals("sequence has expected size", non_empty_original.size(), sizeNonEmpty);

    // Create copy of non-empty sequence
    geos::geom::CoordinateSequence non_empty_copy(non_empty_original);

    ensure(!non_empty_copy.isEmpty());
    ensure_equals(non_empty_copy.size(), sizeNonEmpty);

    // Compare non-empty original and copy using equality operators
    ensure_equals(non_empty_original.getAt(0), non_empty_copy.getAt(0));
    ensure_equals(non_empty_original.getAt(1), non_empty_copy.getAt(1));
    ensure(non_empty_original.getAt(0) != non_empty_copy.getAt(1));
}

// Test of getX() and getY()
template<>
template<>
void object::test<5>
()
{
    using geos::geom::Coordinate;

    // Create non-empty sequence
    geos::geom::CoordinateSequence sequence;
    sequence.add(Coordinate(1, 2));
    sequence.add(Coordinate(5, 10));

    const std::size_t size = 2;

    ensure(!sequence.isEmpty());
    ensure_equals(sequence.size(), size);

    ensure_equals(sequence.getX(0), 1);
    ensure_equals(sequence.getY(0), 2);
    ensure_equals(sequence.getX(1), 5);
    ensure_equals(sequence.getY(1), 10);
}

// Test of getAt()
template<>
template<>
void object::test<6>
()
{
    using geos::geom::Coordinate;

    // Create non-empty sequence
    geos::geom::CoordinateSequence sequence;
    sequence.add(Coordinate(1, 2, 3));
    sequence.add(Coordinate(5, 10, 15));

    ensure(!sequence.isEmpty());
    ensure_equals(sequence.size(), 2u);
    ensure(sequence.getAt(0) != sequence.getAt(1));

    // First version of getAt()
    ensure_equals(sequence.getAt(0).x, 1);
    ensure_equals(sequence.getAt(0).y, 2);
    ensure_equals(sequence.getAt(0).z, 3);
    ensure_equals(sequence.getAt(1).x, 5);
    ensure_equals(sequence.getAt(1).y, 10);
    ensure_equals(sequence.getAt(1).z, 15);

    // Second version of getAt()
    Coordinate buf;

    sequence.getAt(0, buf);
    ensure_equals(buf.x, 1);
    ensure_equals(buf.y, 2);
    ensure_equals(buf.z, 3);

    sequence.getAt(1, buf);
    ensure_equals(buf.x, 5);
    ensure_equals(buf.y, 10);
    ensure_equals(buf.z, 15);

    CoordinateXY bufXY;
    sequence.getAt(1, bufXY);
    ensure_equals("XY X", bufXY.x, 5);
    ensure_equals("XY Y", bufXY.y, 10);

    CoordinateXYM bufXYM;
    sequence.getAt(1, bufXYM);
    ensure_equals("XYM X",bufXYM.x, 5);
    ensure_equals("XYM Y",bufXYM.y, 10);
    ensure("XYM M", std::isnan(bufXYM.m));

    CoordinateXYZM bufXYZM;
    sequence.getAt(1, bufXYZM);
    ensure_equals("XYZM X", bufXYZM.x, 5);
    ensure_equals("XYZM Y", bufXYZM.y, 10);
    ensure_equals("XYZM Z", bufXYZM.z, 15);
    ensure("XYZM M", std::isnan(bufXYZM.m));
}

// Test of add()
template<>
template<>
void object::test<7>
()
{
    using geos::geom::Coordinate;

    // Create empty sequence to fill with coordinates
    const std::size_t size = 0;
    geos::geom::CoordinateSequence sequence;

    ensure(sequence.isEmpty());
    ensure_equals(sequence.size(), size);

    // Add coordinates
    Coordinate tmp(1, 2, 3);
    sequence.add(tmp); // insert copy of tmp
    const std::size_t sizeOne = 1;

    ensure(!sequence.isEmpty());
    ensure_equals(sequence.size(), sizeOne);

    tmp.x = 5;
    tmp.y = 10;
    tmp.z = 15;
    sequence.add(tmp); // insert copy of tmp
    const std::size_t sizeTwo = 2;

    ensure(!sequence.isEmpty());
    ensure_equals(sequence.size(), sizeTwo);

    ensure(!sequence.hasRepeatedPoints());

    // Check elements of sequence
    ensure(sequence.getAt(0) != sequence.getAt(1));

    ensure_equals(sequence.getAt(0).x, 1);
    ensure_equals(sequence.getAt(0).y, 2);
    ensure_equals(sequence.getAt(0).z, 3);
    ensure_equals(sequence.getAt(1).x, 5);
    ensure_equals(sequence.getAt(1).y, 10);
    ensure_equals(sequence.getAt(1).z, 15);
}

// Test of setAt()
template<>
template<>
void object::test<8>
()
{
    // Create sequence with 2 default coordinates
    const std::size_t size = 2;
    geos::geom::CoordinateSequence sequence(size);

    ensure(!sequence.isEmpty());
    ensure_equals(sequence.size(), size);
    ensure(sequence.hasRepeatedPoints());

    // Set new values to first coordinate
    Coordinate first(1, 2, 3);
    sequence.setAt(first, 0);

    ensure_equals(sequence.size(), size);
    ensure_equals(sequence.getAt(0).x, 1);
    ensure_equals(sequence.getAt(0).y, 2);
    ensure_equals(sequence.getAt(0).z, 3);


    // Set new values to second coordinate
    Coordinate second(5, 10, 15);
    sequence.setAt(second, 1);

    ensure_equals(sequence.size(), size);

    ensure_equals(sequence.getAt(1).x, 5);
    ensure_equals(sequence.getAt(1).y, 10);
    ensure_equals(sequence.getAt(1).z, 15);

    ensure(!sequence.hasRepeatedPoints());
}

// Test of pop_back()
template<>
template<>
void object::test<9>
()
{
    CoordinateSequence seq{CoordinateXYZM(1, 2, 3, 4), CoordinateXYZM(5, 6, 7, 8)};
    seq.pop_back();

    ensure_equals(seq.size(), 1u);
    ensure(seq.getAt<CoordinateXYZM>(0).equals4D(CoordinateXYZM(1, 2, 3, 4)));
}

// Test of setPoints()
template<>
template<>
void object::test<10>
()
{
    // Create empty XYZM sequence
    geos::geom::CoordinateSequence sequence(0u, true, true);

    ensure(sequence.isEmpty());
    ensure_equals(sequence.size(), 0u);
    ensure_equals(sequence.getDimension(), 4u);

    // Create collection of points
    std::vector<Coordinate> col;
    col.push_back(Coordinate(1, 2, 3));
    col.push_back(Coordinate(5, 10, 15));
    col.push_back(Coordinate(9, 18, 27));

    // Use setPoints()
    sequence.setPoints(col);

    ensure(!sequence.isEmpty());
    ensure_equals("sequence has expected size", sequence.size(), col.size());
    ensure_equals("sequence dimension", sequence.getDimension(), 3u);
    ensure_equals("hasZ", sequence.hasZ(), true);
    ensure_equals("hasM", sequence.hasM(), false);
    ensure(!sequence.hasRepeatedPoints());

    // Check inserted points
    ensure_equals(sequence.getAt(0).x, 1);
    ensure_equals(sequence.getAt(0).y, 2);
    ensure_equals(sequence.getAt(0).z, 3);

    ensure_equals(sequence.getAt(1).x, 5);
    ensure_equals(sequence.getAt(1).y, 10);
    ensure_equals(sequence.getAt(1).z, 15);

    ensure_equals(sequence.getAt(2).x, 9);
    ensure_equals(sequence.getAt(2).y, 18);
    ensure_equals(sequence.getAt(2).z, 27);
}

// Test of getEnvelope
template<>
template<>
void object::test<11>
()
{
    CoordinateSequence seq1;
    const auto& env1 = seq1.getEnvelope();
    ensure(env1.isNull());

    CoordinateSequence seq2{CoordinateXYZM(1, 2, 3, 4),
                            CoordinateXYZM(5, 6, 7, 8),
                            CoordinateXYZM(9, 10, 11, 12)};

    const auto& env2 = seq2.getEnvelope();

    ensure_equals(env2.getMinX(), 1);
    ensure_equals(env2.getMaxX(), 9);
    ensure_equals(env2.getMinY(), 2);
    ensure_equals(env2.getMaxY(), 10);
}

// Test of equality and inequality operators
template<>
template<>
void object::test<12>
()
{
    using geos::geom::Coordinate;
    using geos::geom::CoordinateSequence;
    using geos::geom::CoordinateSequence;

    Coordinate c1(1, 2, 3);
    Coordinate c2(5, 10, 15);
    Coordinate c3(6, 9, 10);

    CoordinateSequence sequence1;
    CoordinateSequence sequence2;

    sequence1.add(c1);
    sequence1.add(c2);
    sequence1.add(c3);

    sequence2.add(c1);
    sequence2.add(c2);
    sequence2.add(c3);

    ensure_equals(sequence1, sequence2);
    ensure_equals(sequence2, sequence1);

    // Add a new coordinate to sequence2
    sequence2.add(c3);

    ensure(sequence1 != sequence2);
    ensure(sequence2 != sequence1);

    // Add a new coordinate to sequence1
    sequence1.add(c3);

    ensure_equals(sequence1, sequence2);
    ensure_equals(sequence2, sequence1);

    // Add a new coordinate to sequence1
    sequence1.add(c3);
    sequence2.add(c2);

    ensure(sequence1 != sequence2);
    ensure(sequence2 != sequence1);

}

// Test setOrdinate
template<>
template<>
void object::test<13>
()
{
    using geos::geom::Coordinate;
    using geos::geom::CoordinateSequence;

    Coordinate c1(1, 2, 3);

    CoordinateSequence sequence1;

    sequence1.add(c1);

    ensure_equals(sequence1[0], c1);

    sequence1.setOrdinate(0, CoordinateSequence::X, 4);
    ensure_equals(sequence1[0].x, 4);

    sequence1.setOrdinate(0, CoordinateSequence::Y, 5);
    ensure_equals(sequence1[0].y, 5);

    sequence1.setOrdinate(0, CoordinateSequence::Z, 6);
    ensure_equals(sequence1[0].z, 6);

}

// Test setOrdinate setting ordinates in arbitrary order
template<>
template<>
void object::test<14>
()
{
    using geos::geom::Coordinate;
    using geos::geom::CoordinateSequence;

    Coordinate c1(1, 2, 3);

    CoordinateSequence sequence1;

    sequence1.add(c1);

    ensure_equals(sequence1[0], c1);

    // Order: Y, X, Z

    sequence1.setOrdinate(0, CoordinateSequence::Y, 5);
    ensure_equals(sequence1[0].y, 5);

    sequence1.setOrdinate(0, CoordinateSequence::X, 4);
    ensure_equals(sequence1[0].x, 4);

    sequence1.setOrdinate(0, CoordinateSequence::Z, 6);
    ensure_equals(sequence1[0].z, 6);

}

// Test setOrdinate setting ordinates in arbitrary order
// against a non-initialized CoordinateSequence
template<>
template<>
void object::test<15>
()
{
    using geos::geom::Coordinate;
    using geos::geom::CoordinateSequence;

    CoordinateSequence seq(4, 3);

    // Index: 0 - Order: Y, X, Z

    seq.setOrdinate(0, CoordinateSequence::Y,  5);
    ensure_equals(seq[0].y, 5);
    seq.setOrdinate(0, CoordinateSequence::Z,  6);
    ensure_equals(seq[0].z, 6);
    seq.setOrdinate(0, CoordinateSequence::X,  4);
    ensure_equals(seq[0].x, 4);

    // Index: 1 - Order: Z, X, Y

    seq.setOrdinate(1, CoordinateSequence::Z,  9);
    ensure_equals(seq[1].z, 9);
    seq.setOrdinate(1, CoordinateSequence::X,  8);
    ensure_equals(seq[1].x, 8);
    seq.setOrdinate(1, CoordinateSequence::Y,  7);
    ensure_equals(seq[1].y, 7);

    // Index: 2 - Order: X, Y, Z

    seq.setOrdinate(2, CoordinateSequence::X,  34);
    ensure_equals(seq[2].x, 34);
    seq.setOrdinate(2, CoordinateSequence::Y,  -45);
    ensure_equals(seq[2].y, -45);
    seq.setOrdinate(2, CoordinateSequence::Z,  152);
    ensure_equals(seq[2].z, 152);

    // Index: 3 - Order: Y, Z, X

    seq.setOrdinate(3, CoordinateSequence::Y,  63);
    ensure_equals(seq[3].y, 63);
    seq.setOrdinate(3, CoordinateSequence::Z,  13);
    ensure_equals(seq[3].z, 13);
    seq.setOrdinate(3, CoordinateSequence::X,  -65);
    ensure_equals(seq[3].x, -65);

}

// Test of add() in the middle
template<>
template<>
void object::test<16>
()
{
    // Create empty sequence to fill with coordinates
    CoordinateSequence sequence;

    sequence.add(Coordinate(0, 0));
    sequence.add(Coordinate(1, 1));
    sequence.add(Coordinate(2, 2));

    ensure_equals(sequence.size(), std::size_t(3));

    sequence.add(0, Coordinate(4, 4), false); // don't alow repeated
    ensure_equals(sequence.size(), std::size_t(4));
    ensure_equals(sequence.getAt(0).x, 4.0);

    // do not allow repeated
    sequence.add(0, Coordinate(4, 4), false);
    ensure_equals(sequence.size(), std::size_t(4));

    // allow repeated
    sequence.add(0, Coordinate(4, 4), true);
    ensure_equals(sequence.size(), std::size_t(5));

    // Now looks like this: 4,4,0,1,2
    // we'll add at position 4 a 2 (equals to the one after)
    sequence.add(4, Coordinate(2, 2), false);
    ensure_equals(sequence.size(), std::size_t(5));

    // we'll add at position 4 a 1 (equals to the one before)
    sequence.add(4, Coordinate(1, 1), false);
    ensure_equals(sequence.size(), std::size_t(5));

    // we'll add at position 4 a 1 (equals to the one before)
    // but allowing duplicates
    sequence.add(4, Coordinate(1, 1), true);
    ensure_equals(sequence.size(), std::size_t(6));
    ensure_equals(sequence.getAt(3).x, 1);
    ensure_equals(sequence.getAt(4).x, 1);
    ensure_equals(sequence.getAt(5).x, 2);
}

// Test getDimension and filtering (http://trac.osgeo.org/geos/ticket/435)
template<>
template<>
void object::test<17>
()
{
    geos::geom::CoordinateSequence seq(1);
    ensure_equals(seq.getDimension(), 2u);

    Filter f;

    f.is3d = true;
    seq.apply_rw(&f);
    ensure_equals(seq.getDimension(), 3u);

    f.is3d = false;
    seq.apply_rw(&f);
    ensure_equals(seq.getDimension(), 2u);
}

// Test add from iterator
template<>
template<>
void object::test<18>
()
{
    std::vector<Coordinate> coords{{1, 2}, {3, 4}, {5, 6}};

    CoordinateSequence seq;
    seq.add(coords.begin(), coords.end());

    ensure_equals("seq has expected size", seq.size(), 3u);
    ensure(seq[0] == Coordinate(1, 2));
    ensure(seq[1] == Coordinate(3, 4));
    ensure(seq[2] == Coordinate(5, 6));
}

// Test add from iterator (no repeat)
template<>
template<>
void object::test<19>
()
{
    std::vector<Coordinate> coords{{1, 2}, {3, 4}, {3, 4}};

    CoordinateSequence seq;
    seq.add(coords.begin(), coords.end(), false);

    ensure_equals("seq has expected size", seq.size(), 2u);
    ensure(seq[0] == Coordinate(1, 2));
    ensure(seq[1] == Coordinate(3, 4));
}

// Test add from iterator in middle
template<>
template<>
void object::test<20>
()
{
    std::vector<Coordinate> coords{{1, 2}, {3, 4}, {5, 6}};

    CoordinateSequence seq;
    seq.add(coords.begin(), coords.end());
    seq.add(2, coords.begin(), coords.end());

    ensure_equals("seq has expected size", seq.size(), 6u);
    ensure(seq[0] == Coordinate(1, 2));
    ensure(seq[1] == Coordinate(3, 4));
    ensure(seq[2] == Coordinate(1, 2));
    ensure(seq[3] == Coordinate(3, 4));
    ensure(seq[4] == Coordinate(5, 6));
    ensure(seq[5] == Coordinate(5, 6));
}

// Test templated accessors
template<>
template<>
void object::test<21>
()
{
    CoordinateSequence seq(0, 3);
    seq.add(Coordinate{1, 2, 3});
    seq.add(Coordinate{4, 5, 6});

    CoordinateXY c1 = seq.getAt<CoordinateXY>(0);
    ensure(c1.equals2D(CoordinateXY(1, 2)));

    Coordinate c2 = seq.getAt<Coordinate>(1);
    ensure(c2.equals3D(Coordinate(4, 5, 6)));
}

// Test construction and access from XYZM seq
template<>
template<>
void object::test<22>
()
{
    CoordinateSequence seq(2, true, true);
    seq.setAt(CoordinateXYZM{1, 2, 3, 4}, 0);
    seq.setAt(CoordinateXYZM{5, 6, 7, 8}, 1);

    ensure_equals("XYZM seq has dim 4", seq.getDimension(), 4u);
    ensure_equals("XYZM seq has size 2", seq.size(), 2u);

    ensure(seq.getAt<CoordinateXY>(0).equals2D(CoordinateXY(1, 2)));
    ensure(seq.getAt<Coordinate>(0).equals3D(Coordinate(1, 2, 3)));
    ensure(seq.getAt<CoordinateXYZM>(0).equals4D(CoordinateXYZM(1, 2, 3, 4)));
}

// Test construction and access from XYM seq
template<>
template<>
void object::test<23>
()
{
    CoordinateSequence seq(2, false, true);
    seq.setAt(CoordinateXYM{1, 2, 3}, 0);
    seq.setAt(CoordinateXYM{4, 5, 6}, 1);

    ensure_equals("XYM seq has dim 3", seq.getDimension(), 3u);
    ensure_equals("XYM seq has size 2", seq.size(), 2u);

    ensure(seq.getAt<CoordinateXY>(0).equals2D(CoordinateXY(1, 2)));
    auto coord = seq.getAt<CoordinateXYZM>(1);
    ensure_equals(coord.x, 4);
    ensure_equals(coord.y, 5);
    ensure_equals(coord.m, 6);
    ensure(std::isnan(coord.z));
}

// Test construction and access from XY seq
template<>
template<>
void object::test<24>
()
{
    CoordinateSequence seq(2, false, false);
    seq.setAt(CoordinateXY(1, 2), 0);
    seq.setAt(CoordinateXY(3, 4), 1);

    ensure_equals("XY seq has dim 2", seq.getDimension(), 2u);
    ensure_equals("XY seq has size 2", seq.size(), 2u);

    ensure("coord 0 has expected value", seq.getAt<CoordinateXY>(0).equals2D(CoordinateXY(1, 2)));
    ensure("coord 1 has expected value", seq.getAt<CoordinateXY>(1).equals2D(CoordinateXY(3, 4)));
}

// add single coords to XY seq
template<>
template<>
void object::test<25>
()
{
    CoordinateSequence seq(0, false, false);

    ensure_equals("empty XY seq has dim 2", seq.getDimension(), 2u);
    ensure_equals("empty XY seq has size 0", seq.size(), 0u);

    seq.add(Coordinate(1, 2));
    seq.add(Coordinate(3, 4));
    ensure_equals("XY seq has size 2", seq.size(), 2u);

    ensure_equals(seq.getAt<CoordinateXY>(0), Coordinate(1, 2));
    ensure_equals(seq.getAt<CoordinateXY>(1), Coordinate(3, 4));
}

// add multiple coords to XY seq
template<>
template<>
void object::test<26>
()
{
    CoordinateSequence seq(0, false, false);

    std::vector<CoordinateXY> coords;
    coords.emplace_back(1, 2);
    coords.emplace_back(3, 4);

    seq.add(coords.begin(), coords.end());

    ensure_equals("XY seq has size 2", seq.size(), 2u);
    ensure_equals(seq.getAt<CoordinateXY>(0), Coordinate(1, 2));
    ensure_equals(seq.getAt<CoordinateXY>(1), Coordinate(3, 4));
}

// add one XY seq to another
template<>
template<>
void object::test<27>
()
{
    CoordinateSequence seq1(0, false, false);
    seq1.add(Coordinate(1, 2));
    seq1.add(Coordinate(3, 4));


    CoordinateSequence seq2(0, false, false);
    seq2.add(CoordinateXY(5, 6));
    seq2.add(CoordinateXY(7, 8));

    seq1.add(seq2);

    ensure_equals("combined seq has size 4", seq1.size(), 4u);
    ensure_equals("combined seq has dim 2", seq1.getDimension(), 2u);
    ensure_equals(seq1.getAt<CoordinateXY>(0), Coordinate(1, 2));
    ensure_equals(seq1.getAt<CoordinateXY>(1), Coordinate(3, 4));
    ensure_equals(seq1.getAt<CoordinateXY>(2), Coordinate(5, 6));
    ensure_equals(seq1.getAt<CoordinateXY>(3), Coordinate(7, 8));
}

// add XYZ seq to XY seq
template<>
template<>
void object::test<28>
()
{
    CoordinateSequence seq1 = CoordinateSequence::XY(0);
    CoordinateSequence seq2 = CoordinateSequence::XYZ(0);

    seq1.add(CoordinateXY(1, 2));
    seq1.add(CoordinateXY(3, 4));

    seq2.add(Coordinate(5, 6, 6.5));
    seq2.add(Coordinate(7, 8, 8.5));

    seq1.add(seq2);

    ensure_equals("combined seq has size 4", seq1.size(), 4u);
    ensure_equals("combined seq has dim 2", seq1.getDimension(), 2u);
    ensure_equals(seq1.getAt<CoordinateXY>(0), Coordinate(1, 2));
    ensure_equals(seq1.getAt<CoordinateXY>(1), Coordinate(3, 4));
    ensure_equals(seq1.getAt<CoordinateXY>(2), Coordinate(5, 6));
    ensure_equals(seq1.getAt<CoordinateXY>(3), Coordinate(7, 8));
}

// add XY seq to XYZ seq
template<>
template<>
void object::test<29>
()
{
    CoordinateSequence seq1 = CoordinateSequence::XYZ(0);
    CoordinateSequence seq2 = CoordinateSequence::XY(0);

    seq1.add(Coordinate(1, 2, 3));
    seq1.add(Coordinate(4, 5, 6));

    seq2.add(CoordinateXY(7, 8));
    seq2.add(CoordinateXY(9, 10));

    seq1.add(seq2);
    ensure_equals("combined seq has size 4", seq1.size(), 4u);
    ensure_equals("combined seq has dim 3", seq1.getDimension(), 3u);

}

// CoordinateSequence::scroll
template<>
template<>
void object::test<30>
()
{
    CoordinateSequence seq1 = CoordinateSequence::XY(5);
    seq1.setAt(CoordinateXY{1, 2}, 0);
    seq1.setAt(CoordinateXY{3, 4}, 1);
    seq1.setAt(CoordinateXY{5, 6}, 2);
    seq1.setAt(CoordinateXY{7, 8}, 3);
    seq1.setAt(CoordinateXY{9, 10}, 4);

    CoordinateSequence::scroll(&seq1, &seq1.getAt<CoordinateXY>(2));

    ensure_equals(seq1.size(), 5u);

    ensure_equals(seq1.getAt<CoordinateXY>(0), CoordinateXY{5, 6});
    ensure_equals(seq1.getAt<CoordinateXY>(1), CoordinateXY{7, 8});
    ensure_equals(seq1.getAt<CoordinateXY>(2), CoordinateXY{9, 10});
    ensure_equals(seq1.getAt<CoordinateXY>(3), CoordinateXY{1, 2});
    ensure_equals(seq1.getAt<CoordinateXY>(4), CoordinateXY{3, 4});
}

// CoordinateSequence::indexOf
template<>
template<>
void object::test<31>
()
{
    CoordinateSequence seq = CoordinateSequence::XYZ(3);
    seq.setAt(Coordinate{1, 2, 3}, 0);
    seq.setAt(Coordinate{4, 5, 6}, 1);
    seq.setAt(Coordinate{7, 8, 9}, 2);

    Coordinate c1{4, 5, 8};
    ensure_equals("z dimension ignored", CoordinateSequence::indexOf(&c1, &seq), 1u);

    CoordinateXY c2{9, 9};
    ensure_equals("maxint when not found", CoordinateSequence::indexOf(&c2, &seq), std::numeric_limits<std::size_t>::max());
}

// Test add 4D to empty sequence with unspecified dimensionality
template<>
template<>
void object::test<32>
()
{
    CoordinateSequence seq1(0u);
    seq1.add(CoordinateXYZM{1, 2, 3, 4});
    ensure_equals(seq1.size(), 1u);

    CoordinateSequence seq2;
    seq2.add(CoordinateXYZM({1, 2, 3, 4}));
    ensure_equals(seq2.size(), 1u);
}

// Test add(Coordinate) can add a reference to Coordinate inside the container
template<>
template<>
void object::test<33>
()
{
    CoordinateSequence seq;
    seq.add(1.0, 2.0);

    for (std::size_t i = 0; i < 149; i++) {
        seq.add(seq.front());
    }

    ensure_equals(seq.size(), 150u);
}

// Test add(Coordinate, size_t) can add a reference to a coordinate inside the container
template<>
template<>
void object::test<34>
()
{
    CoordinateSequence seq;
    seq.add(1.0, 2.0);

    for(std::size_t i = 0; i < 149; i++) {
        seq.add(seq.front(), i / 10);
    }

    ensure_equals(seq.size(), 150u);
}

// test isRing, closeRing
template<>
template<>
void object::test<35>
()
{
    CoordinateSequence seq;
    seq.add(0.0, 0.0);
    seq.add(1.0, 0.0);
    seq.add(1.0, 1.0);
    seq.add(0.0, 1.0);

    seq.closeRing();

    ensure_equals("size() after first closeRing()" ,seq.size(), 5u);
    ensure("isRing()", seq.isRing());

    seq.closeRing();
    ensure_equals("size() after second closeRing()", seq.size(), 5u);
}

// test initializer_list constructor
template<>
template<>
void object::test<36>
()
{
    CoordinateSequence seq{ Coordinate{1,2,3}, Coordinate{4, 5, 6} };

    ensure_equals(seq.size(), 2u);
    ensure_equals(seq.getDimension(), 3u);
    ensure(seq.hasZ());
}

// test add(CoordinateSequence&, false) with two repeated points in middle
template<>
template<>
void object::test<37>
()
{
    CoordinateSequence seq;

    CoordinateSequence seq1{
        CoordinateXY{1, 2},
        CoordinateXY{4, 5},
        CoordinateXY{4, 5},
        CoordinateXY{8, 9}};

    seq.add(seq1, false);

    ensure_equals("three points added to seq", seq.size(), 3u);
    ensure_equals(seq.getAt<CoordinateXY>(0), CoordinateXY{1, 2});
    ensure_equals(seq.getAt<CoordinateXY>(1), CoordinateXY{4, 5});
    ensure_equals(seq.getAt<CoordinateXY>(2), CoordinateXY{8, 9});
}

// test add(CoordinateSequence&, false) with three repeated points in middle
template<>
template<>
void object::test<38>
()
{
    CoordinateSequence seq;

    CoordinateSequence seq1{
        CoordinateXY{1, 2},
        CoordinateXY{4, 5},
        CoordinateXY{4, 5},
        CoordinateXY{4, 5},
        CoordinateXY{8, 9}};

    seq.add(seq1, false);

    ensure_equals("three points added to seq", seq.size(), 3u);
    ensure_equals(seq.getAt<CoordinateXY>(0), CoordinateXY{1, 2});
    ensure_equals(seq.getAt<CoordinateXY>(1), CoordinateXY{4, 5});
    ensure_equals(seq.getAt<CoordinateXY>(2), CoordinateXY{8, 9});
}

// test add(CoordinateSequence&, false) with three repeated points at end
template<>
template<>
void object::test<39>
()
{
    CoordinateSequence seq;

    CoordinateSequence seq1{
        CoordinateXY{1, 2},
        CoordinateXY{3, 4},
        CoordinateXY{5, 6},
        CoordinateXY{5, 6},
        CoordinateXY{5, 6}};

    seq.add(seq1, false);

    ensure_equals("three points added to seq", seq.size(), 3u);
    ensure_equals(seq.getAt<CoordinateXY>(0), CoordinateXY{1, 2});
    ensure_equals(seq.getAt<CoordinateXY>(1), CoordinateXY{3, 4});
    ensure_equals(seq.getAt<CoordinateXY>(2), CoordinateXY{5, 6});
}

// test add(CoordinateSequence&, false) where repeated points are in different sequences
// and receiving sequence has explicit dimensionality
template<>
template<>
void object::test<40>
()
{
    CoordinateSequence seq{
        CoordinateXY{0, 0},
        CoordinateXY{1, 1}
    };

    CoordinateSequence seq1{
        CoordinateXY{1, 1},
        CoordinateXY{2, 2}};

    seq.add(seq1, false);

    ensure_equals("one point added to seq", seq.size(), 3u);
    ensure_equals(seq.getAt<CoordinateXY>(0), CoordinateXY{0, 0});
    ensure_equals(seq.getAt<CoordinateXY>(1), CoordinateXY{1, 1});
    ensure_equals(seq.getAt<CoordinateXY>(2), CoordinateXY{2, 2});
}

// test add(CoordinateSequence&, false) where repeated points are in different sequences
// and receiving sequence has implicit dimensionality
template<>
template<>
void object::test<41>
()
{
    CoordinateSequence seq;
    seq.add(Coordinate{2, 8});

    CoordinateSequence seq1;
    seq1.add(Coordinate{2, 8});
    seq1.add(Coordinate{4, 8});

    seq.add(seq1, false);

    ensure_equals("one point added to seq", seq.size(), 2u);
    ensure_equals(seq.getAt<CoordinateXY>(0), CoordinateXY{2, 8});
    ensure_equals(seq.getAt<CoordinateXY>(1), CoordinateXY{4, 8});
}

// test CoordinateSequence::reverse
template<>
template<>
void object::test<42>
()
{
    using XY = CoordinateXY;
    using XYZM = CoordinateXYZM;
    using XYZ = Coordinate;

    CoordinateSequence xy5{XY(1, 2), XY(3, 4), XY(5, 6), XY(7, 8), XY(9, 10)};
    CoordinateSequence xy5_rev{XY(9, 10), XY(7, 8), XY(5, 6), XY(3, 4), XY(1, 2)};
    xy5.reverse();

    ensure_equals("XY, n=5", xy5, xy5_rev);

    CoordinateSequence xyz5{XYZ(1, 2), XYZ(3, 4), XYZ(5, 6), XYZ(7, 8), XYZ(9, 10)};
    CoordinateSequence xyz5_rev{XYZ(9, 10), XYZ(7, 8), XYZ(5, 6), XYZ(3, 4), XYZ(1, 2)};
    xyz5.reverse();

    ensure_equals("XYZ, n=5", xyz5, xyz5_rev);

    CoordinateSequence xyzm4{XYZM(1, 2, 3, 4), XYZM(5, 6, 7, 8), XYZM(9, 10, 11, 12), XYZM(13, 14, 15, 16)};
    CoordinateSequence xyzm4_rev{XYZM(13, 14, 15, 16), XYZM(9, 10, 11, 12), XYZM(5, 6, 7, 8), XYZM(1, 2, 3, 4)};
    xyzm4.reverse();

    ensure_equals("XYZM, n=4", xyzm4, xyzm4_rev);
}

// test CoordinateSequence::sort
template<>
template<>
void object::test<43>
()
{
    using XY = CoordinateXY;
    using XYZM = CoordinateXYZM;
    using XYZ = Coordinate;

    CoordinateSequence xy5{XY(1, 2), XY(3, 4), XY(5, 6), XY(7, 8), XY(9, 10)};
    CoordinateSequence xy5_rev{XY(9, 10), XY(7, 8), XY(5, 6), XY(3, 4), XY(1, 2)};
    xy5_rev.sort();

    ensure_equals("XY, n=5", xy5, xy5_rev);

    CoordinateSequence xyz5{XYZ(1, 2), XYZ(3, 4), XYZ(5, 6), XYZ(7, 8), XYZ(9, 10)};
    CoordinateSequence xyz5_rev{XYZ(9, 10), XYZ(7, 8), XYZ(5, 6), XYZ(3, 4), XYZ(1, 2)};
    xyz5_rev.sort();

    ensure_equals("XYZ, n=5", xyz5, xyz5_rev);

    CoordinateSequence xyzm4{XYZM(1, 2, 3, 4), XYZM(5, 6, 7, 8), XYZM(9, 10, 11, 12), XYZM(13, 14, 15, 16)};
    CoordinateSequence xyzm4_rev{XYZM(13, 14, 15, 16), XYZM(9, 10, 11, 12), XYZM(5, 6, 7, 8), XYZM(1, 2, 3, 4)};
    xyzm4_rev.sort();

    ensure_equals("XYZM, n=4", xyzm4, xyzm4_rev);
}

// test no-op add
template<>
template<>
void object::test<44>
()
{
    CoordinateSequence seq1(1);
    seq1.setAt(CoordinateXY{2, 8}, 0);

    CoordinateSequence seq2(2);
    seq2.setAt(CoordinateXY{2, 8}, 0);
    seq2.setAt(CoordinateXY{2, 8}, 1);

    seq1.add(seq2, false);

    ensure_equals(seq1.size(), 1u);
}

// test Z-padded sequence
template<>
template<>
void object::test<45>
()
{
    CoordinateSequence xym_seq(0u, false, true);
    xym_seq.add(CoordinateXYM(1, 2, 3));
    xym_seq.add(CoordinateXYM(4, 5, 6));

    ensure_equals("size", xym_seq.size(), 2u);
    ensure_equals("type", xym_seq.getCoordinateType(), geos::geom::CoordinateType::XYZM);

    // no crash when pulling CoordinateXYZM
    auto c0 = xym_seq.getAt<CoordinateXYZM>(0);
    ensure_equals(c0.x, 1);
    ensure_equals(c0.y, 2);
    ensure_equals(c0.m, 3);
    ensure("z is NaN", std::isnan(c0.z));

    auto c1 = xym_seq.getAt<Coordinate>(1);
    ensure_equals(c1.x, 4);
    ensure_equals(c1.y, 5);
    ensure("z is NaN", std::isnan(c1.z));
}

// Test default-initialized Coordinate values
template<>
template<>
void object::test<46>
()
{
    CoordinateSequence xySeq(1, false, false, true);
    ensure_equals(xySeq.getAt<CoordinateXY>(0), CoordinateXY());

    CoordinateSequence xyzSeq(1, true, false, true);
    ensure(xyzSeq.getAt<Coordinate>(0).equals3D(Coordinate()));

    CoordinateSequence xymSeq(1, false, true, true);
    CoordinateXYM xym;
    xymSeq.getAt(0, xym);
    ensure(xym.equals3D(CoordinateXYM()));

    CoordinateSequence xyzmSeq(1, true, true, true);
    ensure(xyzmSeq.getAt<CoordinateXYZM>(0).equals4D(CoordinateXYZM()));
}

// Declared dimension overrides dimension inferred from Coordinates
template<>
template<>
void object::test<47>
()
{
    CoordinateSequence seq(1, false, false, false);
    seq.setAt(Coordinate{1, 2, 3}, 0);

    ensure_equals(seq.getDimension(), 2u);
}

// Test getOrinate
template<>
template<>
void object::test<48>
()
{
    CoordinateSequence seq{CoordinateXY(1, 2), CoordinateXY(3, 4)};

    ensure_same(seq.getOrdinate(0, CoordinateSequence::Y), 2);
    ensure_same(seq.getOrdinate(1, CoordinateSequence::X), 3);
    ensure_same(seq.getOrdinate(1, CoordinateSequence::Z), DoubleNotANumber);
    ensure_same(seq.getOrdinate(1, CoordinateSequence::M), DoubleNotANumber);

    CoordinateSequence seq2{CoordinateXYZM(1, 2, 3, 4)};

    ensure_same(seq2.getOrdinate(0, CoordinateSequence::X), 1);
    ensure_same(seq2.getOrdinate(0, CoordinateSequence::Y), 2);
    ensure_same(seq2.getOrdinate(0, CoordinateSequence::Z), 3);
    ensure_same(seq2.getOrdinate(0, CoordinateSequence::M), 4);
}

// Test setOrdinate
template<>
template<>
void object::test<49>
()
{
    CoordinateSequence seq{Coordinate(1, 2, 3), Coordinate(4, 5, 6)};

    seq.setOrdinate(0, CoordinateSequence::Y, 6);
    seq.setOrdinate(1, CoordinateSequence::Z, 2);

    const Coordinate& c0 = seq.getAt<Coordinate>(0);
    ensure_same(c0.x, 1);
    ensure_same(c0.y, 6);
    ensure_same(c0.z, 3);

    const Coordinate& c1 = seq.getAt<Coordinate>(1);
    ensure_same(c1.x, 4);
    ensure_same(c1.y, 5);
    ensure_same(c1.z, 2);
}

// Test move contructor
template<>
template<>
void object::test<50>
()
{
    CoordinateSequence seq{CoordinateXYM(1, 2, 3), CoordinateXYM(4, 5, 6)};
    const double* data_ptr = seq.data();

    CoordinateSequence seq2(std::move(seq));
    ensure_equals(seq2.size(), 2u);
    ensure(!seq2.hasZ());
    ensure(seq2.hasM());
    ensure_equals(data_ptr, seq2.data());
}

// Test numeric dimension constructor
template<>
template<>
void object::test<51>
()
{
    try {
        CoordinateSequence seq(0, 1);
        fail();
    } catch (const geos::util::GEOSException&) {}

    CoordinateSequence seq(0, 4);
    ensure(seq.hasZ());
    ensure(seq.hasM());
}

struct AddToSequence : public geos::geom::CoordinateInspector<AddToSequence> {

    AddToSequence(CoordinateSequence& out) : m_seq(out) {}

    template<typename T>
    void filter(const T* c) {
        m_seq.add(*c);
    }

private:
    CoordinateSequence& m_seq;
};

// Test that templated filter preserves dimensionality
template<>
template<>
void object::test<52>
()
{
    // XY
    CoordinateSequence xy({CoordinateXY{1,2}, CoordinateXY{3, 4}});
    CoordinateSequence xy_out = CoordinateSequence::XY(0);
    AddToSequence filter_xy(xy_out);
    xy.apply_ro(&filter_xy);
    ensure_equals(xy.size(), xy_out.size());
    for (std::size_t i = 0; i < xy.size(); i++) {
        ensure(xy.getAt<CoordinateXY>(i).equals2D(xy_out.getAt<CoordinateXY>(i)));
    }

    // XYZ
    CoordinateSequence xyz({Coordinate{1,2,3}, Coordinate{4,5,6}});
    CoordinateSequence xyz_out = CoordinateSequence::XYZ(0);
    AddToSequence filter_xyz(xyz_out);
    xyz.apply_ro(&filter_xyz);
    ensure_equals(xyz.size(), xyz_out.size());
    for (std::size_t i = 0; i < xyz.size(); i++) {
        ensure(xyz.getAt<Coordinate>(i).equals3D(xyz_out.getAt<Coordinate>(i)));
    }

    // XYM
    CoordinateSequence xym({CoordinateXYM{1,2,3}, CoordinateXYM{4,5,6}});
    CoordinateSequence xym_out = CoordinateSequence::XYM(0);
    AddToSequence filter_xym(xym_out);
    xym.apply_ro(&filter_xym);
    ensure_equals(xym.size(), xym_out.size());
    for (std::size_t i = 0; i < xym.size(); i++) {
        ensure(xym.getAt<CoordinateXYM>(i).equals2D(xym_out.getAt<CoordinateXYM>(i)));
    }

    // XYZM
    CoordinateSequence xyzm({CoordinateXYZM{1,2,3,4}, CoordinateXYZM{5,6,7,8}});
    CoordinateSequence xyzm_out = CoordinateSequence::XYZM(0);
    AddToSequence filter_xyzm(xyzm_out);
    xyzm.apply_ro(&filter_xyzm);
    ensure_equals(xyzm.size(), xyzm_out.size());
    for (std::size_t i = 0; i < xyzm.size(); i++) {
        ensure(xyzm.getAt<CoordinateXYZM>(i).equals4D(xyzm_out.getAt<CoordinateXYZM>(i)));
    }
}

// Test type-detecting version of forEach
template<>
template<>
void object::test<53>
()
{
    CoordinateSequence dst(0u, true, true);

    CoordinateSequence src1{{Coordinate{1, 2, 3}, Coordinate{4, 5, 6}}};
    CoordinateSequence src2{{CoordinateXYM{7, 8, 9}, {10, 11, 12}}};

    auto appendToDst = [&dst](auto& coord) {
        dst.add(coord);
    };

    src1.forEach(appendToDst);
    src2.forEach(appendToDst);

    ensure_equals(dst.size(), 4u);
    ensure_equals_xyzm(dst.getAt<CoordinateXYZM>(0), CoordinateXYZM{1, 2, 3, DoubleNotANumber});
    ensure_equals_xyzm(dst.getAt<CoordinateXYZM>(1), CoordinateXYZM{4, 5, 6, DoubleNotANumber});
    ensure_equals_xyzm(dst.getAt<CoordinateXYZM>(2), CoordinateXYZM{7, 8,    DoubleNotANumber, 9});
    ensure_equals_xyzm(dst.getAt<CoordinateXYZM>(3), CoordinateXYZM{10, 11,  DoubleNotANumber, 12});
}

// Test equalsIdentical()
template<>
template<>
void object::test<54>
()
{
    CoordinateSequence xyz2 = CoordinateSequence::XYZ(2);
    CoordinateSequence xyz3 = CoordinateSequence::XYZ(3);
    CoordinateSequence xy3 = CoordinateSequence::XY(3);
    CoordinateSequence xyz3_2 = CoordinateSequence::XYZ(0);

    xyz2.setAt(Coordinate(1, 2, 3), 0);
    xyz2.setAt(Coordinate(4, 5, 6), 1);

    xyz3.setAt(Coordinate(1, 2, 3), 0);
    xyz3.setAt(Coordinate(4, 5, 6), 1);
    xyz3.setAt(Coordinate(7, 8, 9), 2);

    xyz3_2.add(xyz3);

    xy3.setAt(Coordinate(1, 2, 3), 0);
    xy3.setAt(Coordinate(4, 5, 6), 1);
    xy3.setAt(Coordinate(7, 8, 9), 2);

    ensure(xy3.equalsIdentical(xy3));

    ensure(!xyz2.equalsIdentical(xyz3));
    ensure(!xyz3.equalsIdentical(xy3));
    ensure(xyz3.equalsIdentical(xyz3_2));
    ensure(xyz3_2.equalsIdentical(xyz3));
}

// Test creation from external buffer (2D)
template<>
template<>
void object::test<55>
()
{
    double vals[] = {1, 2, 3, 4};

    CoordinateSequence seq(vals, 2, false, false);

    ensure_equals(seq.size(), 2u);
    ensure_equals(seq.getAt(0), Coordinate(1, 2));
    ensure_equals(seq.getAt(1), Coordinate(3, 4));

    seq.add(Coordinate(5, 6));
    ensure_equals(seq.size(), 3u);
    ensure_equals(seq.getAt(0), Coordinate(1, 2));
    ensure_equals(seq.getAt(1), Coordinate(3, 4));
    ensure_equals(seq.getAt(2), Coordinate(5, 6));
}

// Test creation from external buffer (3D)
template<>
template<>
void object::test<56>
()
{
    double vals[] = {1, 2, 3, 4, 5, 6};

    CoordinateSequence seq(vals, 2, true, false);

    ensure_equals(seq.size(), 2u);
    ensure(seq.getAt(0).equals3D(Coordinate(1, 2, 3)));
    ensure(seq.getAt(1).equals3D(Coordinate(4, 5, 6)));
    ensure_equals(seq.data(), vals);

    seq.add(Coordinate(7, 8, 9));
    ensure_equals(seq.size(), 3u);
    ensure(seq.getAt(0).equals3D(Coordinate(1, 2, 3)));
    ensure(seq.getAt(1).equals3D(Coordinate(4, 5, 6)));
    ensure(seq.getAt(2).equals3D(Coordinate(7, 8, 9)));
    ensure(seq.data() != vals);
}

} // namespace tut
