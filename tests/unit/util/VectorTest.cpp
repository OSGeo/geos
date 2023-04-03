//
// Test Suite for geos::util::Vector class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/util/Vector.h>

#include <iostream>

using geos::util::Vector;

namespace tut {
//
// Test Group
//

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v) {
    for (const auto& item : v) {
        os << item << " ";
    }
    os << std::endl;
    return os;
}

// Common data used in test cases.
struct test_vector_data {
};


typedef test_group<test_vector_data> group;
typedef group::object object;

group test_vector_group("geos::util::Vector");

//
// Test Cases
//

// Construct empty
template<>
template<>
void object::test<1>()
{
    Vector<double> v;

    ensure_equals(v.size(), 0u);
    ensure_equals(v.capacity(), 0u);
    ensure("empty()", v.empty());
    ensure("owned()", v.owned());
}

// Construct fixed-size
template<>
template<>
void object::test<2>()
{
    Vector<int> v(5);

    ensure_equals(v.size(), 5u);
    ensure_equals(v.capacity(), 5u);
    ensure("empty()", !v.empty());
    ensure("owned()", v.owned());
}

// Construct fixed-size (empty)
template<>
template<>
void object::test<3>()
{
    Vector<int> v(0);

    ensure_equals(v.size(), 0u);
    ensure_equals(v.capacity(), 0u);
    ensure("empty()", v.empty());
    ensure("owned()", v.owned());
}

// Construct from external
template<>
template<>
void object::test<4>()
{
    int items[] = {1, 2, 3};

    Vector<int> v(3, items);

    ensure_equals(v.size(), 3u);
    ensure_equals(v.capacity(), 0u);
    ensure("empty()", !v.empty());
    ensure("owned()", !v.owned());
}

// Construct from external nullptr
template<>
template<>
void object::test<5>()
{
    Vector<int> v(0, nullptr);

    ensure_equals(v.size(), 0u);
    ensure_equals(v.capacity(), 0u);
    ensure("empty()", v.empty());
    ensure("owned()", v.owned());
}

// Add single items
template<>
template<>
void object::test<6>()
{
    Vector<int> v;
    for (int i = 0; i < 100; i++) {
        v.push_back(i);
    }

    ensure_equals(v.size(), 100u);
    ensure("capacity()", v.capacity() >= 100);
    ensure("empty()", !v.empty());
    ensure("owned()", v.owned());

    for (std::size_t i = 0; i < v.size(); i++) {
        ensure_equals(v[i], static_cast<int>(i));
    }
}

// Add single items, starting from external buffer
template<>
template<>
void object::test<7>() {
    int items[] = {0, 1, 2, 3, 4};
    Vector<int> v(5, items);

    for (int i = 5; i < 20; i++) {
        v.push_back(i);
    }

    ensure_equals(v.size(), 20u);
    ensure("capacity()", v.capacity() >= 20);
    ensure("empty()", !v.empty());
    ensure("owned()", v.owned());

    for (std::size_t i = 0; i < v.size(); i++) {
        ensure_equals(v[i], static_cast<int>(i));
    }
}

// Add multiple items at end
template<>
template<>
void object::test<8>() {
    Vector<int> v;
    for (int i = 0; i < 10; i++) {
        v.push_back(i);
    }

    v.insert(v.end(), 5u, 999);

    ensure_equals(v.size(), 15u);
    ensure("capacity()", v.capacity() >= 15);
    ensure("empty()", !v.empty());
    ensure("owned()", v.owned());

    for (std::size_t i = 0; i < v.size(); i++) {
        if (i < 10) {
            ensure_equals(v[i], static_cast<int>(i));
        } else {
            ensure_equals(v[i], 999);
        }
    }
}

// Add multiple items in middle
template<>
template<>
void object::test<9>() {
    int items[] = {0, 1, 2};

    Vector<int> v(3u, items);

    v.insert(v.begin() + 1, 2u, 999);

    ensure_equals(v.size(), 5u);
    ensure("capacity()", v.capacity() >= 5);
    ensure("empty()", !v.empty());
    ensure("owned()", v.owned());

    ensure_equals(v[0], 0);
    ensure_equals(v[1], 999);
    ensure_equals(v[2], 999);
    ensure_equals(v[3], 1);
    ensure_equals(v[4], 2);
}

// Copy contructor (empty)
template<>
template<>
void object::test<10>() {
    Vector<int> v1;
    Vector<int> v2(v1);
}

// Copy constructor (owned, non-empty)
template<>
template<>
void object::test<11>() {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    Vector<int> v2(v1);

    ensure_equals(v1.size(), v2.size());
    ensure(v1.data() != v2.data());
}

// Copy contructor (non-owned)
template<>
template<>
void object::test<12>() {
    int items[] = {0, 1, 2};
    Vector<int> v1(3u, items);

    Vector<int> v2(v1);

    ensure_equals(v1.size(), v2.size());
    ensure(v1.data() != v2.data());
    ensure("owned()", v2.owned());
}

// Copy assignment (empty)
template<>
template<>
void object::test<13>() {
    Vector<int> v1;
    Vector<int> v2(5);
    v2 = v1;

    ensure("empty()", v2.empty());
}

// Copy assignment (owned, non-empty)
template<>
template<>
void object::test<14>() {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    Vector<int> v2(5);
    v2 = v1;

    ensure_equals(v1.size(), v2.size());
    ensure(v1.data() != v2.data());
}

// Copy assignment (non-owned)
template<>
template<>
void object::test<15>() {
    int items[] = {0, 1, 2};
    Vector<int> v1(3u, items);

    Vector<int> v2(5);
    v2 = v1;

    ensure_equals(v1.size(), v2.size());
    ensure(v1.data() != v2.data());
    ensure("owned()", v2.owned());
}

// Move construction (empty)
template<>
template<>
void object::test<16>() {
    Vector<int> v1;
    Vector<int> v2(std::move(v1));

    ensure("empty()", v2.empty());
}

// Move construction (owned, non-empty)
template<>
template<>
void object::test<17>() {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    auto ptr = v1.data();

    Vector<int> v2(std::move(v1));

    ensure_equals(v2.size(), 2u);
    ensure("owned()", v2.owned());
    ensure_equals(v2.data(), ptr);
}

// Move construction (non-owned)
template<>
template<>
void object::test<18>() {
    int items[] = {0, 1, 2};
    Vector<int> v1(3u, items);
    Vector<int> v2(std::move(v1));

    ensure_equals(v2.size(), 3u);
    ensure("owned()", !v2.owned());
    ensure_equals(v2.data(), items);
}

// Move assignment (empty)
template<>
template<>
void object::test<19>() {
    Vector<int> v1;
    Vector<int> v2(5);

    v2 = std::move(v1);

    ensure("empty()", v2.empty());
}

// Move assignment (owned, non-empty)
template<>
template<>
void object::test<20>() {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    auto ptr = v1.data();

    Vector<int> v2(5);
    v2 = std::move(v1);

    ensure_equals(v2.size(), 2u);
    ensure("owned()", v2.owned());
    ensure_equals(v2.data(), ptr);
}

// Move assignment (non-owned)
template<>
template<>
void object::test<21>() {
    int items[] = {0, 1, 2};
    Vector<int> v1(3u, items);
    Vector<int> v2(5);

    v2 = std::move(v1);

    ensure_equals(v2.size(), 3u);
    ensure("owned()", !v2.owned());
    ensure_equals(v2.data(), items);
}

// No invalidation if we are within existing capacity
template<>
template<>
void object::test<22>() {
    Vector<int> v;
    v.reserve(8);

    auto ptr = v.data();
    v.insert(v.begin(), 8u, 999);

    ensure_equals(v.data(), ptr);
}


}
