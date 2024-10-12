//
// Test Suite for geos::geomgraph::TopologyLocation class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/geomgraph/TopologyLocation.h>
#include <geos/geom/Location.h>

using TopologyLocation = geos::geomgraph::TopologyLocation;
using Location = geos::geom::Location;

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_topologylocation_data {

    test_topologylocation_data() {}
};

typedef test_group<test_topologylocation_data> group;
typedef group::object object;

group test_topologylocation_group("geos::geomgraph::TopologyLocation");

//
// Test Cases
//

// test constructors
template<>
template<>
void object::test<1>
()
{
    TopologyLocation a(Location::INTERIOR);
    ensure(a.isLine());
    ensure_equals(a.get(0), Location::INTERIOR);

    TopologyLocation b(Location::EXTERIOR, Location::INTERIOR, Location::BOUNDARY);
    ensure(b.isArea());
    ensure_equals(b.get(0), Location::EXTERIOR);
    ensure_equals(b.get(1), Location::INTERIOR);
    ensure_equals(b.get(2), Location::BOUNDARY);
}

// test basic setters
template<>
template<>
void object::test<2>
()
{
    TopologyLocation b(Location::EXTERIOR, Location::INTERIOR, Location::BOUNDARY);

    ensure_equals(b.get(0), Location::EXTERIOR);
    ensure_equals(b.get(1), Location::INTERIOR);
    ensure_equals(b.get(2), Location::BOUNDARY);

    b.setLocation(0, Location::INTERIOR);
    b.setLocation(1, Location::BOUNDARY);
    b.setLocation(2, Location::EXTERIOR);

    ensure_equals(b.get(0), Location::INTERIOR);
    ensure_equals(b.get(1), Location::BOUNDARY);
    ensure_equals(b.get(2), Location::EXTERIOR);
}

// test setAllLocations()
template<>
template<>
void object::test<3>
()
{
    TopologyLocation b(Location::EXTERIOR, Location::INTERIOR, Location::BOUNDARY);

    b.setAllLocations(Location::NONE);

    ensure_equals(b.get(0), Location::NONE);
    ensure_equals(b.get(1), Location::NONE);
    ensure_equals(b.get(2), Location::NONE);

    b.setLocation(0, Location::BOUNDARY);

    ensure_equals(b.get(0), Location::BOUNDARY);
    ensure_equals(b.get(1), Location::NONE);
    ensure_equals(b.get(2), Location::NONE);

    b.setAllLocationsIfNull(Location::EXTERIOR);

    ensure_equals(b.get(0), Location::BOUNDARY);
    ensure_equals(b.get(1), Location::EXTERIOR);
    ensure_equals(b.get(2), Location::EXTERIOR);
}

} // namespace tut
