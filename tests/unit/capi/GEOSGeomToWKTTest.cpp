//
// Test Suite for C-API GEOSGeomToWKT

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <string>
#include <cstdlib>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeomtowkt_data : public capitest::utility {
    void
    test_wkt(std::string const& wkt)
    {
        GEOSGeometry* geom1 = GEOSGeomFromWKT(wkt.c_str());
        ensure(nullptr != geom1);

        char* wkt_c = GEOSGeomToWKT(geom1);
        std::string out(wkt_c);
        free(wkt_c);
        GEOSGeom_destroy(geom1);

        ensure_equals(out, wkt);
    }

    void
    test_wkt(std::string const& wkt, std::string::size_type n)
    {
        GEOSGeometry* geom1 = GEOSGeomFromWKT(wkt.c_str());
        ensure(nullptr != geom1);

        char* wkt_c = GEOSGeomToWKT(geom1);
        std::string out(wkt_c);
        free(wkt_c);
        GEOSGeom_destroy(geom1);

        ensure_equals(out.substr(0, n), wkt.substr(0, n));
    }
};

typedef test_group<test_capigeosgeomtowkt_data> group;
typedef group::object object;

group test_capigeosgeomtowkt_group("capi::GEOSGeomToWKT");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    test_wkt("POINT EMPTY");
}

template<>
template<>
void object::test<2>
()
{
    test_wkt("LINESTRING EMPTY");
}

template<>
template<>
void object::test<3>
()
{
    test_wkt("POLYGON EMPTY");
}

template<>
template<>
void object::test<4>
()
{
    test_wkt("MULTIPOINT EMPTY");
}

template<>
template<>
void object::test<5>
()
{
    test_wkt("MULTILINESTRING EMPTY");
}

template<>
template<>
void object::test<6>
()
{
    test_wkt("MULTIPOLYGON EMPTY");
}

// Comparing string based on float-point numbers does not make sense,
// so make poor-man comparison of WKT type tag and first few numbers

template<>
template<>
void object::test<7>
()
{
    test_wkt("POINT (1.234000 5.678)", 15);

    // check default OutputDimension(2) with higher dimension points
    test_wkt("POINT (1.234000 5.678 9)", 15); // POINT Z
    test_wkt("POINT (1.234000 5.678 9 10)", 15); // POINT ZM
}

template<>
template<>
void object::test<8>
()
{
    test_wkt("LINESTRING (0.000 0, 5 5, 10 5, 10 10)", 17);
}

template<>
template<>
void object::test<9>
()
{
    test_wkt("POLYGON ((0.000 10, 5 5, 10 5, 15 10, 10 15, 5 15, 0 10))", 15);
}

template<>
template<>
void object::test<10>
()
{
    test_wkt("MULTIPOINT ((0.000 0), (5 5), (10 10), (15 15), (20 20))", 17);
}

template<>
template<>
void object::test<11>
()
{
    test_wkt("MULTILINESTRING ((0.000 0, 10 0, 10 10, 0 10, 10 20),(2 2, 2 6, 6 4, 20 2))", 23);
}

template<>
template<>
void object::test<12>
()
{
    test_wkt("MULTIPOLYGON (((0.000 0, 10 0, 10 10, 0 10, 0 0),(2 2, 2 6, 6 4, 2 2)),((60 60, 60 50, 70 40, 60 60)))", 21);
}

} // namespace tut

