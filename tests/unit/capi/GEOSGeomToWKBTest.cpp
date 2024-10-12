//
// Test Suite for C-API GEOSGeomToWKB

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <string>
#include <cstdio>
#include <cstdlib>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosgeomtowkb_data : public capitest::utility {

    void
    test_wkb(const std::string& wkt, int flavor)
    {
        geom1_ = GEOSGeomFromWKT(wkt.c_str());
        ensure(nullptr != geom1_);

        GEOSWKBWriter* wkbwriter = GEOSWKBWriter_create();
        GEOSWKBWriter_setOutputDimension(wkbwriter, 3);
        GEOSWKBWriter_setFlavor(wkbwriter, flavor);
        std::size_t wkb_size;
        unsigned char* wkb = GEOSWKBWriter_write(wkbwriter, geom1_, &wkb_size);
        GEOSWKBWriter_destroy(wkbwriter);

        GEOSWKBReader* wkbreader = GEOSWKBReader_create();
        geom2_ = GEOSWKBReader_read(wkbreader, wkb, wkb_size);
        GEOSWKBReader_destroy(wkbreader);
        free(wkb);

        GEOSWKTWriter* wktwriter = GEOSWKTWriter_create();
        GEOSWKTWriter_setRoundingPrecision(wktwriter, 3);
        GEOSWKTWriter_setOutputDimension(wktwriter, 3);
        char* wkt_c = GEOSWKTWriter_write(wktwriter, geom2_);
        GEOSWKTWriter_destroy(wktwriter);
        std::string out(wkt_c);
        free(wkt_c);

        ensure_equals(out, wkt);
    }

    void
    test_wkb(const std::string& wkt)
    {
        test_wkb(wkt, GEOS_WKB_EXTENDED);
    }


};

typedef test_group<test_capigeosgeomtowkb_data> group;
typedef group::object object;

group test_capigeosgeomtowkb_group("capi::GEOSGeomToWKB");

//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    test_wkb("POINT EMPTY");
}

template<>
template<>
void object::test<2>
()
{
    test_wkb("LINESTRING EMPTY");
}

template<>
template<>
void object::test<3>
()
{
    test_wkb("POLYGON EMPTY");
}

template<>
template<>
void object::test<4>
()
{
    test_wkb("MULTIPOINT EMPTY");
}

template<>
template<>
void object::test<5>
()
{
    test_wkb("MULTILINESTRING EMPTY");
}

template<>
template<>
void object::test<6>
()
{
    test_wkb("MULTIPOLYGON EMPTY");
}

// Comparing string based on float-point numbers does not make sense,
// so make poor-man comparison of WKT type tag.

template<>
template<>
void object::test<7>
()
{
    test_wkb("POINT (1 2)");
}

template<>
template<>
void object::test<8>
()
{
    test_wkb("LINESTRING (0 0, 5 5, 10 5, 10 10)");
}

template<>
template<>
void object::test<9>
()
{
    test_wkb("POLYGON ((0 10, 5 5, 10 5, 15 10, 10 15, 5 15, 0 10))");
}

template<>
template<>
void object::test<10>
()
{
    test_wkb("MULTIPOINT ((0 0), (5 5), (10 10), (15 15), (20 20))");
}

template<>
template<>
void object::test<11>
()
{
    test_wkb("MULTILINESTRING ((0 0, 10 0, 10 10, 0 10, 10 20), (2 2, 2 6, 6 4, 20 2))");
}

template<>
template<>
void object::test<12>
()
{
    test_wkb("MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (2 2, 2 6, 6 4, 2 2)), ((60 60, 60 50, 70 40, 60 60)))");
}

template<>
template<>
void object::test<13>
()
{
    test_wkb("POINT Z (1 2 3)");
}

template<>
template<>
void object::test<14>
()
{
    test_wkb("LINESTRING Z (1 2 3, 4 5 6, 7 8 9)");
}

template<>
template<>
void object::test<15>
()
{
    test_wkb("POINT Z (1 2 3)", GEOS_WKB_ISO);
}

template<>
template<>
void object::test<16>
()
{
    test_wkb("LINESTRING Z (1 2 3, 4 5 6, 7 8 9)", GEOS_WKB_ISO);
}

} // namespace tut

