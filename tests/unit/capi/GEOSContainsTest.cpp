//
// Test Suite for C-API GEOSContains

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/io/WKBReader.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
// std

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeoscontains_data : public capitest::utility {
};

typedef test_group<test_capigeoscontains_data> group;
typedef group::object object;

group test_capigeoscontains_group("capi::GEOSContains");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geom1_ = fromWKT("POLYGON EMPTY");
    geom2_ = fromWKT("POLYGON EMPTY");

    char const r1 = GEOSContains(geom1_, geom2_);
    ensure_equals(r1, 0);

    char const r2 = GEOSContains(geom2_, geom1_);
    ensure_equals(r2, 0);
}

template<>
template<>
void object::test<2>
()
{
    geom1_ = fromWKT("POLYGON((1 1,1 5,5 5,5 1,1 1))");
    geom2_ = fromWKT("POINT(2 2)");

    char const r1 = GEOSContains(geom1_, geom2_);
    ensure_equals(int(r1), 1);

    char const r2 = GEOSContains(geom2_, geom1_);
    ensure_equals(int(r2), 0);
}

template<>
template<>
void object::test<3>
()
{
    geom1_ = fromWKT("MULTIPOLYGON(((0 0,0 10,10 10,10 0,0 0)))");
    geom2_ = fromWKT("POLYGON((1 1,1 2,2 2,2 1,1 1))");

    char const r1 = GEOSContains(geom1_, geom2_);
    ensure_equals(int(r1), 1);

    char const r2 = GEOSContains(geom2_, geom1_);
    ensure_equals(int(r2), 0);
}

// Test outer polygon contains inner polygon with two coincident vertices
// with results compared depending on precision used with FIXED PMs.
template<>
template<>
void object::test<4>
()
{
    // Coincident vertices of both polygons at
    // -700.67089999181 93743.4218587986, -713.450135807349 93754.1677576647,
    std::string const
    outer("01030000800100000009000000af9dd0005ee585c0f802efbff6e2f6400000000000000000955acde0994b86c039a922afa2e3f64000000000000000002af6fb4f5d1887c07adb1c4071e3f6400000000000000000e5962b388d4f87c0bd3aeda7bae2f640000000000000000087c61344030887c07d585e6ff6e1f6400000000000000000fc8a31b5166186c0230588b20ae1f640000000000000000034733daf050186c0ed9f3ac98ae1f6400000000000000000f190aef659b385c0df2876538ce2f6400000000000000000af9dd0005ee585c0f802efbff6e2f6400000000000000000");
    std::string const
    inner("0103000080010000000a000000ac21f88bbaff86c05f45d8c7b4e2f6400000000000000000467f1177ebf386c05de1971187e2f6400000000000000000fcf677888fc886c04e855a544be2f6400000000000000000c61226e540b686c0c0662d1fe7e1f640000000000000000042dc1bece8a486c09b85529f8ae1f6400000000000000000891047cde55e86c038cfa59c4ee1f6400000000000000000ae9dd0005ee585c0fa02efbff6e2f6400000000000000000975acde0994b86c038a922afa2e3f6400000000000000000287e339b09f986c01b1a083a10e3f6400000000000000000ac21f88bbaff86c05f45d8c7b4e2f6400000000000000000");

    // A contains B if precision is limited to 1e+10
    {
        geos::geom::PrecisionModel pm(1e+10);
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);

        int ret = GEOSContains(geom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSContains(geom2_, geom1_);
        ensure_equals(ret, 0);
        GEOSGeom_destroy(geom1_);
        GEOSGeom_destroy(geom2_);
    }

    // A does NOT contain B if precision is extended to 1e+11 or beyond
    {
        geos::geom::PrecisionModel pm(1e+11);
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);

        int ret = GEOSContains(geom1_, geom2_);
        ensure_equals(ret, 0);
        ret = GEOSContains(geom2_, geom1_);
        ensure_equals(ret, 0);
    }
}

// Test outer rectangle contains inner rectangle with one coincident vertex
// and two vertices of the inner rectangle are on the boundary (lay on segments)
// of the outer rectangle.
// Precision model should not affect the containment test result.
template<>
template<>
void object::test<5>
()
{
    // Coincident vertex at -753.167968418005 93709.4279185742
    //POLYGON ((-753.167968418005 93754.0955183194,-816.392328351464 93754.0955183194,-816.392328351464 93709.4279185742,-753.167968418005 93709.4279185742,-753.167968418005 93754.0955183194))
    std::string const
    outer("01030000800100000005000000bd70d3ff578987c09e373e87a1e3f6400000000000000000a9f60b7d238389c09e373e87a1e3f6400000000000000000a9f60b7d238389c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09e373e87a1e3f6400000000000000000");
    //POLYGON ((-753.167968418005 93747.6909727677,-799.641978447015 93747.6909727677,-799.641978447015 93709.4279185742,-753.167968418005 93709.4279185742,-753.167968418005 93747.6909727677))
    std::string const
    inner("01030000800100000005000000bd70d3ff578987c0f875390e3be3f6400000000000000000579598c522fd88c0f875390e3be3f6400000000000000000579598c522fd88c09625c1d8d6e0f6400000000000000000bd70d3ff578987c09625c1d8d6e0f6400000000000000000bd70d3ff578987c0f875390e3be3f6400000000000000000");

    // A contains B if precision is limited to 1e+10
    {
        geos::geom::PrecisionModel pm(1e+10);
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);

        int ret = GEOSContains(geom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSContains(geom2_, geom1_);
        ensure_equals(ret, 0);

        GEOSGeom_destroy(geom1_);
        GEOSGeom_destroy(geom2_);
    }

    // A contains B if FLOATING PM is used with extended precision
    {
        geos::geom::PrecisionModel pm;
        geos::geom::GeometryFactory::Ptr factory = geos::geom::GeometryFactory::create(&pm);
        geos::io::WKBReader reader(*factory);

        std::istringstream sOuter(outer);
        geom1_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sOuter).release());
        std::istringstream sInner(inner);
        geom2_ = reinterpret_cast<GEOSGeometry*>(reader.readHEX(sInner).release());
        ensure(nullptr != geom1_);
        ensure(nullptr != geom2_);

        int ret = GEOSContains(geom1_, geom2_);
        ensure_equals(ret, 1);
        ret = GEOSContains(geom2_, geom1_);
        ensure_equals(ret, 0);
    }
}

template<>
template<>
void object::test<6>()
{
    geom1_ = fromWKT("CIRCULARSTRING (0 0, 1 1, 2 0)");
    geom2_ = fromWKT("LINESTRING (1 0, 2 0)");

    ensure(geom1_);
    ensure(geom2_);

    ensure_equals("curved geometry not supported", GEOSContains(geom1_, geom2_), 2);
}


} // namespace tut

