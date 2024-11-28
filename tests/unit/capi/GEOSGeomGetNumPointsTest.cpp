#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

struct test_geosgeomgetnumpoints_data : public capitest::utility {};

typedef test_group<test_geosgeomgetnumpoints_data> group;
typedef group::object object;

group test_geosgeomgetnumpoints("capi::GEOSGeomGetNumPoints");

template<>
template<>
void object::test<1>()
{
    input_ = GEOSGeomFromWKT("LINESTRING EMPTY");
    ensure_equals(GEOSGeomGetNumPoints(input_), 0);
}

template<>
template<>
void object::test<2>()
{
    input_ = GEOSGeomFromWKT("LINESTRING (1 1, 2 2)");
    ensure_equals(GEOSGeomGetNumPoints(input_), 2);
}

template<>
template<>
void object::test<3>()
{
    input_ = GEOSGeomFromWKT("POINT (0 0)");
    ensure_equals(GEOSGeomGetNumPoints(input_), -1);
}

template<>
template<>
void object::test<4>()
{
    input_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure_equals(GEOSGeomGetNumPoints(input_), 3);
}

template<>
template<>
void object::test<5>()
{
    input_ = fromWKT("LINEARRING (0 0, 1 1, 2 0, 0 0)");
    ensure_equals(GEOSGeomGetNumPoints(input_), 4);
}

} // namespace tut

