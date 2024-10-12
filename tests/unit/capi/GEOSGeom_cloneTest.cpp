#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
struct test_capigeosgeom_clone : public capitest::utility {};

typedef test_group<test_capigeosgeom_clone> group;
typedef group::object object;

group test_capigeosgeom_clone_group("capi::GEOSGeom_clone");

template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING (3 8, -12 -4)");
    GEOSSetSRID(geom1_, 32145);
    int data = 500;
    GEOSGeom_setUserData(geom1_, &data);

    geom2_ = GEOSGeom_clone(geom1_);

    ensure_geometry_equals(geom1_, geom2_);
    ensure(GEOSGeom_getCoordSeq(geom1_) != GEOSGeom_getCoordSeq(geom2_)); // deep copy
    ensure_equals(GEOSGetSRID(geom2_), 32145);
    ensure(GEOSGeom_getUserData(geom2_) == nullptr); // userData not transferred
}

template<>
template<>
void object::test<2>
()
{
    input_ = GEOSGeomFromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    result_ = GEOSGeom_clone(input_);

    ensure_equals(toWKT(result_), "CIRCULARSTRING (0 0, 1 1, 2 0)");
}

} // namespace tut

