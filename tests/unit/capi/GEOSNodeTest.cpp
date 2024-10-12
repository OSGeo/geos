//
// Test Suite for C-API GEOSNode

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeosnode_data : public capitest::utility {
    test_capigeosnode_data()
    {
    }
};

typedef test_group<test_capigeosnode_data> group;
typedef group::object object;

group test_capigeosnode_group("capi::GEOSNode");

//
// Test Cases
//

/// Self-intersecting line
template<>
template<>
void object::test<1>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING(0 0, 10 10, 10 0, 0 10)");
    geom2_ = GEOSNode(geom1_);
    ensure(nullptr != geom2_);

    GEOSNormalize(geom2_);
    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);
    std::string out(wkt_);

    ensure_equals(out,
        "MULTILINESTRING ((5 5, 10 10, 10 0, 5 5), (0 10, 5 5), (0 0, 5 5))"
        );
}

/// Overlapping lines
template<>
template<>
void object::test<2>
()
{
    geom1_ = GEOSGeomFromWKT("MULTILINESTRING((0 0, 2 0, 4 0),(5 0, 3 0, 1 0))");
    geom2_ = GEOSNode(geom1_);
    ensure(nullptr != geom2_);

    GEOSNormalize(geom2_);
    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);
    std::string out(wkt_);

    ensure_equals(out,
        "MULTILINESTRING ((4 0, 5 0), (3 0, 4 0), (2 0, 3 0), (1 0, 2 0), (0 0, 1 0))"
        );
}

/// Equal lines
template<>
template<>
void object::test<3>
()
{
    geom1_ = GEOSGeomFromWKT("MULTILINESTRING((0 0, 2 0, 4 0),(0 0, 2 0, 4 0))");
    geom2_ = GEOSNode(geom1_);
    ensure(nullptr != geom2_);

    GEOSNormalize(geom2_);
    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);
    std::string out(wkt_);

    ensure_equals(out, "MULTILINESTRING ((2 0, 4 0), (0 0, 2 0))");
}

// https://gis.stackexchange.com/questions/345341/get-location-of-postgis-geos-topology-exception/345482#345482
template<>
template<>
void object::test<4>
()
{
    std::string wkb = "010500000002000000010200000003000000dc874d65fcc25ec176032c6b350c5341b336429ffec25ec1f962bbd"
                      "9480c5341fc849518ffc25ec15be20f5f500c5341010200000006000000fa9bbfd3fcc25ec1b978232f390c5341"
                      "b336429ffec25ec1f962bbd9480c5341a77e6be5fec25ec1357c21334d0c5341c3eba27bfec25ec11be5a4c34a0"
                      "c5341b61d8cacfcc25ec1bcf273143c0c5341fa9bbfd3fcc25ec1b978232f390c5341";

    geom1_ = GEOSGeomFromHEX_buf((const unsigned char*) wkb.c_str(), wkb.size());
    geom2_ = GEOSNode(geom1_);

    // Noding currently fails for this case.
    // ensure(geom2_);
}


// https://github.com/libgeos/geos/issues/601
template<>
template<>
void object::test<5>
()
{
    geom1_ = GEOSGeomFromWKT("LINESTRING EMPTY");
    geom2_ = GEOSNode(geom1_);
    ensure(nullptr != geom2_);

    wkt_ = GEOSWKTWriter_write(wktw_, geom2_);
    std::string out(wkt_);

    ensure_equals(out, "LINESTRING EMPTY");
}

// Noding two XYZ LineStrings
template<>
template<>
void object::test<6>
()
{
    geom1_= GEOSGeomFromWKT("MULTILINESTRING Z ((0 0 0, 1 1 1), (0 1 5, 1 0 10))");
    result_ = GEOSNode(geom1_);
    expected_ = GEOSGeomFromWKT("MULTILINESTRING Z("
                                "(0 0 0, 0.5 0.5 4),"
                                "(0.5 0.5 4, 1 0 10),"
                                "(0 1 5, 0.5 0.5 4),"
                                "(0.5 0.5 4, 1 1 1))");

    GEOSNormalize(result_);
    GEOSNormalize(expected_);

    auto wkt_result = GEOSWKTWriter_write(wktw_, result_);
    auto wkt_expected = GEOSWKTWriter_write(wktw_, expected_);

    ensure_equals(std::string(wkt_result), std::string(wkt_expected));

    GEOSFree(wkt_result);
    GEOSFree(wkt_expected);
}

// Noding two XYM LineStrings
template<>
template<>
void object::test<7>
()
{
    geom1_= GEOSGeomFromWKT("MULTILINESTRING M ((0 0 0, 1 1 1), (0 1 5, 1 0 10))");
    result_ = GEOSNode(geom1_);
    expected_ = GEOSGeomFromWKT("MULTILINESTRING M("
                                "(0 0 0, 0.5 0.5 4),"
                                "(0.5 0.5 4, 1 0 10),"
                                "(0 1 5, 0.5 0.5 4),"
                                "(0.5 0.5 4, 1 1 1))");

    GEOSNormalize(result_);
    GEOSNormalize(expected_);

    auto wkt_result = GEOSWKTWriter_write(wktw_, result_);
    auto wkt_expected = GEOSWKTWriter_write(wktw_, expected_);

    ensure_equals(std::string(wkt_result), std::string(wkt_expected));

    GEOSFree(wkt_result);
    GEOSFree(wkt_expected);
}

// Noding two XYZ and XYM LineStrings
template<>
template<>
void object::test<8>
()
{
    geom1_= GEOSGeomFromWKT("GEOMETRYCOLLECTION (LINESTRING Z(0 0 0, 1 1 1), LINESTRING M(0 1 5, 1 0 10))");
    result_ = GEOSNode(geom1_);
    expected_ = GEOSGeomFromWKT("MULTILINESTRING ZM("
                                "(0 0 0 NaN, 0.5 0.5 0.5 7.5),"
                                "(0.5 0.5 0.5 7.5, 1 0 NaN 10),"
                                "(0 1 NaN 5, 0.5 0.5 0.5 7.5),"
                                "(0.5 0.5 0.5 7.5, 1 1 1 NaN))");

    GEOSNormalize(result_);
    GEOSNormalize(expected_);

    auto wkt_result = GEOSWKTWriter_write(wktw_, result_);
    auto wkt_expected = GEOSWKTWriter_write(wktw_, expected_);

    ensure_equals(std::string(wkt_result), std::string(wkt_expected));

    GEOSFree(wkt_result);
    GEOSFree(wkt_expected);
}

template<>
template<>
void object::test<9>()
{
    input_ = fromWKT("MULTICURVE (CIRCULARSTRING (0 0, 1 1, 2 0), CIRCULARSTRING (0 1, 1 0, 2 1))");
    ensure(input_);

    result_ = GEOSNode(input_);

    ensure("curved geometries not supported", result_ == nullptr);
}

} // namespace tut

