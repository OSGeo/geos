//
// Test Suite for geos::io::WKTWriter

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/CircularString.h>
#include <geos/geom/CompoundCurve.h>
#include <geos/geom/CurvePolygon.h>
#include <geos/geom/MultiCurve.h>
#include <geos/geom/MultiSurface.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/util.h>
// std
#include <string>
#include <memory>

namespace tut {
//
// Test Group
//

using geos::geom::CoordinateSequence;
using geos::geom::Coordinate;
using geos::geom::CoordinateXY;
using geos::geom::CoordinateXYM;
using geos::geom::CoordinateXYZM;

// dummy data, not used
struct test_wktwriter_data {
    typedef geos::geom::PrecisionModel PrecisionModel;
    typedef geos::geom::GeometryFactory GeometryFactory;
    typedef geos::geom::Geometry Geometry;
    typedef geos::geom::GeometryCollection GeometryCollection;
    typedef geos::io::WKTReader WKTReader;
    typedef geos::io::WKTWriter WKTWriter;
    typedef std::unique_ptr<geos::geom::Geometry> GeomPtr;

    PrecisionModel pm;
    GeometryFactory::Ptr gf;
    WKTReader wktreader;
    WKTWriter wktwriter;

    test_wktwriter_data()
        :
        pm(1000.0),
        gf(GeometryFactory::create(&pm)),
        wktreader(gf.get())
    {}

};

typedef test_group<test_wktwriter_data> group;
typedef group::object object;

group test_wktwriter_group("geos::io::WKTWriter");


//
// Test Cases
//

// 1 - Test the trim capability.
template<>
template<>
void object::test<1>
()
{
    GeomPtr geom(wktreader.read("POINT(-117 33)"));
    std::string  result;

    wktwriter.setTrim(false);
    result = wktwriter.write(geom.get());

    ensure_equals(result, "POINT (-117.000 33.000)");

    wktwriter.setTrim(true);
    result = wktwriter.write(geom.get());

    ensure_equals(result, "POINT (-117 33)");
}

// 2 - Test the output precision capability
template<>
template<>
void object::test<2>
()
{
    GeomPtr geom(wktreader.read("POINT(-117.1234567 33.1234567)"));
    std::string  result;

    wktwriter.setTrim(false);
    result = wktwriter.write(geom.get());
    ensure_equals(result, "POINT (-117.123 33.123)");

    wktwriter.setRoundingPrecision(2);
    result = wktwriter.write(geom.get());
    ensure_equals(result, "POINT (-117.12 33.12)");

    geom = wktreader.read("POINT(-117.000001 33.000001)");
    wktwriter.setRoundingPrecision(2);
    wktwriter.setTrim(true);
    result = wktwriter.write(geom.get());
    ensure_equals(result, "POINT (-117 33)");

    geom = wktreader.read("POINT(-0.000001 -33.000001)");
    result = wktwriter.write(geom.get());
    ensure_equals(result, "POINT (0 -33)");

    geom = wktreader.read("POINT(-10000000.000001 -100000033.000001)");
    result = wktwriter.write(geom.get());
    ensure_equals(result, "POINT (-10000000 -100000033)");

}

// 3 - Test 3D generation from a 3D geometry.
template<>
template<>
void object::test<3>
()
{
    GeomPtr geom(wktreader.read("POINT Z (-117 33 120)"));
    std::string  result;

    wktwriter.setOutputDimension(3);
    wktwriter.setTrim(true);
    wktwriter.setOld3D(false);

    result = wktwriter.write(geom.get());

    ensure_equals(result, std::string("POINT Z (-117 33 120)"));

    wktwriter.setOld3D(true);
    result = wktwriter.write(geom.get());

    ensure_equals(result, std::string("POINT (-117 33 120)"));

}

// 4 - Test 2D generation from a 3D geometry.
template<>
template<>
void object::test<4>
()
{
    GeomPtr geom(wktreader.read("POINT(-117 33 120)"));
    std::string  result;

    wktwriter.setOutputDimension(2);
    wktwriter.setTrim(true);
    wktwriter.setOld3D(false);

    result = wktwriter.write(geom.get());

    ensure_equals(result, std::string("POINT (-117 33)"));
}

// 5 - Test negative number of digits in precision model
template<>
template<>
void object::test<5>
()
{
    PrecisionModel pm3(0.001);
    GeometryFactory::Ptr gf3(GeometryFactory::create(&pm3));
    WKTReader wktreader3(gf3.get());
    GeomPtr geom(wktreader3.read("POINT(123456 654321)"));

    std::string  result = wktwriter.write(geom.get());
    ensure_equals(result, std::string("POINT (123000 654000)"));
}


// 6 - Test writing out a multipoint with an empty member
template<>
template<>
void object::test<6>
()
{
    PrecisionModel pm3(PrecisionModel::FLOATING);
    GeometryFactory::Ptr gf3(GeometryFactory::create(&pm3));
    std::unique_ptr<Geometry> empty_point(gf3->createPoint());
    ensure(empty_point != nullptr);

    geos::geom::Coordinate coord(1, 2);
    std::unique_ptr<Geometry> point(gf3->createPoint(coord));
    ensure(point != nullptr);

    std::vector<const Geometry*> geoms{empty_point.get(), point.get()};
    std::unique_ptr<Geometry> col(gf3->createMultiPoint(geoms));
    ensure(col != nullptr);

    ensure(col->getCoordinate() != nullptr);
    ensure_equals(col->getCoordinate()->x, 1);
    ensure_equals(col->getCoordinate()->y, 2);

    wktwriter.setRoundingPrecision(2);
    wktwriter.setTrim(true);
    std::string result = wktwriter.write(col.get());
    ensure_equals(result, std::string("MULTIPOINT (EMPTY, (1 2))"));
}


template<>
template<>
void object::test<7>
()
{
    using geos::geom::Coordinate;
    using geos::geom::CoordinateSequence;
    using geos::geom::Point;

    PrecisionModel pmLocal;
    auto factory_ = GeometryFactory::create(&pmLocal);
    auto coords = geos::detail::make_unique<CoordinateSequence>();
    ensure(coords != nullptr);

    coords->add(Coordinate(geos::DoubleNotANumber, geos::DoubleNotANumber));
    auto point = factory_->createPoint(std::move(coords));

    std::string result = wktwriter.write(point.get());
    ensure_equals(result, std::string("POINT (NaN NaN)"));
}


// 5 - Test negative number of digits in precision model
template<>
template<>
void object::test<8>
()
{
    const char* gctxt = "GEOMETRYCOLLECTION (LINESTRING EMPTY, POLYGON EMPTY)";
    PrecisionModel gcpm;
    GeometryFactory::Ptr gcgf(GeometryFactory::create(&gcpm));
    WKTReader gcwktreader(gcgf.get());
    GeomPtr gcgeom(gcwktreader.read(gctxt));
    std::string result = wktwriter.write(gcgeom.get());
    ensure_equals(result, std::string(gctxt));
}

// Test writing XYZM
template<>
template<>
void object::test<9>
()
{
    auto coords = geos::detail::make_unique<CoordinateSequence>(2u, true, true);
    coords->setAt(CoordinateXYZM(1, 2, 3, 4), 0);
    coords->setAt(CoordinateXYZM(5, 6, 7, 8), 1);

    auto ls = gf->createLineString(std::move(coords));

    wktwriter.setTrim(true);
    wktwriter.setOutputDimension(4);

    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING ZM (1 2 3 4, 5 6 7 8)"));

    wktwriter.setOld3D(true);
    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING (1 2 3 4, 5 6 7 8)"));


    // If only 3 dimensions are allowed we pick Z instead of M
    wktwriter.setOld3D(false);
    wktwriter.setOutputDimension(3);

    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING Z (1 2 3, 5 6 7)"));

    wktwriter.setOld3D(true);
    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING (1 2 3, 5 6 7)"));
}

// Test writing XYM
template<>
template<>
void object::test<10>
()
{
    auto coords = geos::detail::make_unique<CoordinateSequence>(2u, false, true);
    coords->setAt(CoordinateXYM(1, 2, 3), 0);
    coords->setAt(CoordinateXYM(4, 5, 6), 1);

    auto ls = gf->createLineString(std::move(coords));

    wktwriter.setTrim(true);
    wktwriter.setOutputDimension(3);

    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING M (1 2 3, 4 5 6)"));

    // Same output
    wktwriter.setOld3D(true);
    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING M (1 2 3, 4 5 6)"));
}

// Test writing XY
template<>
template<>
void object::test<11>
()
{
    auto coords = geos::detail::make_unique<CoordinateSequence>(2u, false, false);
    coords->setAt(CoordinateXY(1, 2), 0);
    coords->setAt(CoordinateXY(3, 4), 1);

    auto ls = gf->createLineString(std::move(coords));

    wktwriter.setTrim(true);

    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING (1 2, 3 4)"));

    // Same output
    wktwriter.setOld3D(true);
    ensure_equals(wktwriter.write(*ls),
                  std::string("LINESTRING (1 2, 3 4)"));
}

// Test writing 3D/4D EMPTY geometries
// https://trac.osgeo.org/geos/ticket/1129
template<>
template<>
void object::test<12>
()
{
    wktwriter.setOutputDimension(4);

    CoordinateSequence coords_xyz(0u, true, false);
    CoordinateSequence coords_xym(0u, false, true);
    CoordinateSequence coords_xyzm(0u, true, true);

    auto pt_xyz = gf->createPoint(coords_xyz.clone());
    auto pt_xym = gf->createPoint(coords_xym.clone());
    auto pt_xyzm = gf->createPoint(coords_xyzm.clone());

    ensure_equals(wktwriter.write(*pt_xyz), std::string("POINT Z EMPTY"));
    ensure_equals(wktwriter.write(*pt_xym), std::string("POINT M EMPTY"));
    ensure_equals(wktwriter.write(*pt_xyzm), std::string("POINT ZM EMPTY"));

    auto ls_xyz = gf->createLineString(coords_xyz.clone());
    auto ls_xym = gf->createLineString(coords_xym.clone());
    auto ls_xyzm = gf->createLineString(coords_xyzm.clone());

    ensure_equals(wktwriter.write(*ls_xyz), std::string("LINESTRING Z EMPTY"));
    ensure_equals(wktwriter.write(*ls_xym), std::string("LINESTRING M EMPTY"));
    ensure_equals(wktwriter.write(*ls_xyzm), std::string("LINESTRING ZM EMPTY"));

    auto lr_xyz = gf->createLinearRing(coords_xyz.clone());
    auto lr_xym = gf->createLinearRing(coords_xym.clone());
    auto lr_xyzm = gf->createLinearRing(coords_xyzm.clone());

    auto poly_xyz = gf->createPolygon(std::move(lr_xyz));
    auto poly_xym = gf->createPolygon(std::move(lr_xym));
    auto poly_xyzm = gf->createPolygon(std::move(lr_xyzm));

    ensure_equals(wktwriter.write(*poly_xyz), std::string("POLYGON Z EMPTY"));
    ensure_equals(wktwriter.write(*poly_xym), std::string("POLYGON M EMPTY"));
    ensure_equals(wktwriter.write(*poly_xyzm), std::string("POLYGON ZM EMPTY"));
}

// Test writing an explicitly-created XYZ geometry where Z is NaN
// https://github.com/libgeos/geos/issues/808
template<>
template<>
void object::test<13>
()
{
    wktwriter.setOutputDimension(3);
    wktwriter.setTrim(true);

    CoordinateSequence xyz(1, true, false);
    xyz.setAt(Coordinate(1, 2, std::numeric_limits<double>::quiet_NaN()), 0);
    auto pt = gf->createPoint(std::move(xyz));

    ensure_equals(wktwriter.write(*pt), std::string("POINT Z (1 2 NaN)"));

    wktwriter.setRemoveEmptyDimensions(true);

    ensure_equals(wktwriter.write(*pt), std::string("POINT (1 2)"));
}

// Test removal of empty dimensions
template<>
template<>
void object::test<14>
()
{
    wktwriter.setOutputDimension(4);
    wktwriter.setTrim(true);

    auto g = wktreader.read("LINESTRING ZM (1 2 NaN 3, 4 5 NaN NaN)");

    ensure_equals(wktwriter.write(*g), "LINESTRING ZM (1 2 NaN 3, 4 5 NaN NaN)");

    wktwriter.setRemoveEmptyDimensions(true);

    ensure_equals(wktwriter.write(*g), "LINESTRING M (1 2 3, 4 5 NaN)");
}

// Test multi-part geometries with zero or more empty parts
// https://github.com/libgeos/geos/issues/951
template<>
template<>
void object::test<15>
()
{
    // zero empties -- but don't check dim types
    // https://github.com/libgeos/geos/issues/888
    std::vector<std::string> variants0{
        "MULTIPOINT EMPTY", "MULTILINESTRING EMPTY",
        "MULTIPOLYGON EMPTY", "GEOMETRYCOLLECTION EMPTY",
        "MULTICURVE EMPTY", "MULTISURFACE EMPTY"
    };
    for (const auto& wkt : variants0) {
        const auto g = wktreader.read(wkt);
        ensure_equals(wktwriter.write(*g), wkt);
        ensure_equals(g->getNumGeometries(), 0u);
    }

    // single empty
    std::vector<std::string> variants1{
        "MULTIPOINT (EMPTY)", "MULTIPOINT Z (EMPTY)",
        "MULTIPOINT M (EMPTY)", "MULTIPOINT ZM (EMPTY)",
        "MULTILINESTRING (EMPTY)", "MULTILINESTRING Z (EMPTY)",
        "MULTILINESTRING M (EMPTY)", "MULTILINESTRING ZM (EMPTY)",
        "MULTIPOLYGON (EMPTY)", "MULTIPOLYGON Z (EMPTY)",
        "MULTIPOLYGON M (EMPTY)", "MULTIPOLYGON ZM (EMPTY)",
        "GEOMETRYCOLLECTION (MULTIPOINT EMPTY)",
        "GEOMETRYCOLLECTION Z (POINT Z EMPTY)",
        "GEOMETRYCOLLECTION M (LINESTRING M EMPTY)",
        "GEOMETRYCOLLECTION ZM (POLYGON ZM EMPTY)",
        "MULTICURVE (EMPTY)", "MULTICURVE Z (EMPTY)",
        "MULTICURVE M (EMPTY)", "MULTICURVE ZM (EMPTY)",
        "MULTICURVE (CIRCULARSTRING EMPTY)", "MULTICURVE Z (CIRCULARSTRING Z EMPTY)",
        "MULTICURVE M (CIRCULARSTRING M EMPTY)", "MULTICURVE ZM (CIRCULARSTRING ZM EMPTY)",
        "MULTISURFACE (EMPTY)", "MULTISURFACE Z (EMPTY)",
        "MULTISURFACE M (EMPTY)", "MULTISURFACE ZM (EMPTY)",
        "MULTISURFACE (EMPTY)", "MULTISURFACE Z (CURVEPOLYGON Z EMPTY)",
        "MULTISURFACE M (CURVEPOLYGON M EMPTY)", "MULTISURFACE ZM (CURVEPOLYGON ZM EMPTY)",
    };
    for (const auto& wkt : variants1) {
        const auto g = wktreader.read(wkt);
        ensure_equals(wktwriter.write(*g), wkt);
        ensure_equals(g->getNumGeometries(), 1u);
    }

    // two empties
    std::vector<std::string> variants2{
        "MULTIPOINT (EMPTY, EMPTY)", "MULTIPOINT Z (EMPTY, EMPTY)",
        "MULTIPOINT M (EMPTY, EMPTY)", "MULTIPOINT ZM (EMPTY, EMPTY)",
        "MULTILINESTRING (EMPTY, EMPTY)", "MULTILINESTRING Z (EMPTY, EMPTY)",
        "MULTILINESTRING M (EMPTY, EMPTY)", "MULTILINESTRING ZM (EMPTY, EMPTY)",
        "MULTIPOLYGON (EMPTY, EMPTY)", "MULTIPOLYGON Z (EMPTY, EMPTY)",
        "MULTIPOLYGON M (EMPTY, EMPTY)", "MULTIPOLYGON ZM (EMPTY, EMPTY)",
        "GEOMETRYCOLLECTION (POLYGON EMPTY, LINESTRING EMPTY)",
        "GEOMETRYCOLLECTION Z (LINESTRING Z EMPTY, POINT Z EMPTY)",
        "GEOMETRYCOLLECTION M (POINT M EMPTY, LINESTRING M EMPTY)",
        "GEOMETRYCOLLECTION ZM (POINT ZM EMPTY, LINESTRING ZM EMPTY)",
        "MULTICURVE (EMPTY, CIRCULARSTRING EMPTY)", "MULTICURVE Z (EMPTY, CIRCULARSTRING Z EMPTY)",
        "MULTICURVE M (EMPTY, CIRCULARSTRING M EMPTY)", "MULTICURVE ZM (EMPTY, CIRCULARSTRING ZM EMPTY)",
        "MULTISURFACE (EMPTY, EMPTY)", "MULTISURFACE Z (EMPTY, CURVEPOLYGON Z EMPTY)",
        "MULTISURFACE M (EMPTY, CURVEPOLYGON M EMPTY)", "MULTISURFACE ZM (EMPTY, CURVEPOLYGON ZM EMPTY)",
    };
    for (const auto& wkt : variants2) {
        const auto g = wktreader.read(wkt);
        ensure_equals(wktwriter.write(*g), wkt);
        ensure_equals(g->getNumGeometries(), 2u);
    }

}

// Test big, small, and non-finite values
// https://github.com/libgeos/geos/issues/970
template<>
template<>
void object::test<16>
()
{
    PrecisionModel pmf(PrecisionModel::FLOATING);
    GeometryFactory::Ptr gff(GeometryFactory::create(&pmf));
    WKTReader wktreaderf(gff.get());

    // Big values
    auto big = wktreaderf.read("POINT (-1.234e+15 1.234e+16 1.234e+17 -1.234e+18)");

    // Check precision from 0 to 5
    wktwriter.setRoundingPrecision(0);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1e+17 -1e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 123400000000000000 -1234000000000000000)");

    wktwriter.setRoundingPrecision(1);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1.2e+17 -1.2e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000.0 12340000000000000.0 123400000000000000.0 -1234000000000000000.0)");

    wktwriter.setRoundingPrecision(2);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1.23e+17 -1.23e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000.00 12340000000000000.00 123400000000000000.00 -1234000000000000000.00)");

    wktwriter.setRoundingPrecision(3);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1.234e+17 -1.234e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000.000 12340000000000000.000 123400000000000000.000 -1234000000000000000.000)");

    wktwriter.setRoundingPrecision(4);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1.234e+17 -1.234e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000.0000 12340000000000000.0000 123400000000000000.0000 -1234000000000000000.0000)");

    wktwriter.setRoundingPrecision(5);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000 12340000000000000 1.234e+17 -1.234e+18)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*big), "POINT ZM (-1234000000000000.00000 12340000000000000.00000 123400000000000000.00000 -1234000000000000000.00000)");

    // Small values
    auto small = wktreaderf.read("POINT (-1.234e-3 2.234e-4 1.234e-5 -1.234e-6)");

    // Check precision from 0 to 5
    wktwriter.setRoundingPrecision(0);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.001 0.0002 1e-5 -1e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0 0 0 -0)");

    wktwriter.setRoundingPrecision(1);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.001 0.0002 1.2e-5 -1.2e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.0 0.0 0.0 -0.0)");

    wktwriter.setRoundingPrecision(2);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.001 0.0002 1.23e-5 -1.23e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.00 0.00 0.00 -0.00)");

    wktwriter.setRoundingPrecision(3);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.001 0.0002 1.234e-5 -1.234e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.001 0.000 0.000 -0.000)");

    wktwriter.setRoundingPrecision(4);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.0012 0.0002 1.234e-5 -1.234e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.0012 0.0002 0.0000 -0.0000)");

    wktwriter.setRoundingPrecision(5);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.00123 0.00022 1.234e-5 -1.234e-6)");
    wktwriter.setTrim(false);
    ensure_equals(wktwriter.write(*small), "POINT ZM (-0.00123 0.00022 0.00001 -0.00000)");

    // Extremely small and big
    auto extreme = wktreaderf.read("POINT (-1.2e-208 9.1e-191 3.8e+221 4.9e+154)");
    wktwriter.setRoundingPrecision(5);
    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*extreme), "POINT ZM (-1.2e-208 9.1e-191 3.8e+221 4.9e+154)");
    // Skip non-trim, as this may vary between compilers
    // wktwriter.setTrim(false);
    // ensure_equals(wktwriter.write(*extreme), "POINT ZM (-0.00000 0.00000 ...)");

    // Non-finite values
    auto nonfinite = wktreaderf.read("POINT(-inf inf nan)");

    wktwriter.setTrim(true);
    ensure_equals(wktwriter.write(*nonfinite), "POINT Z (-Infinity Infinity NaN)");
    // Skip non-trim, as this may vary between compilers
    // wktwriter.setTrim(false);
    // ensure_equals(wktwriter.write(*nonfinite), "POINT Z (-inf inf nan)");

}

// test CircularString
template<>
template<>
void object::test<17>()
{
    CoordinateSequence seq{
        CoordinateXY(0, 0),
        CoordinateXY(1, 1),
        CoordinateXY(2, 0)
    };
    auto geom = gf->createCircularString(std::move(seq));

    ensure_equals(wktwriter.write(*geom), "CIRCULARSTRING (0 0, 1 1, 2 0)");
}

// test CompoundCurve
template<>
template<>
void object::test<18>()
{
    std::vector<std::unique_ptr<geos::geom::SimpleCurve>> curves;

    curves.emplace_back(gf->createCircularString({
        CoordinateXY(0, 0),
        CoordinateXY(1, 1),
        CoordinateXY(2, 0)
    }));

    curves.emplace_back(gf->createLineString({
         CoordinateXY(2, 0),
         CoordinateXY(2, 2)
     }));

    auto geom = gf->createCompoundCurve(std::move(curves));

    ensure_equals(wktwriter.write(*geom), "COMPOUNDCURVE (CIRCULARSTRING (0 0, 1 1, 2 0), (2 0, 2 2))");
}

// test CurvePolygon
template<>
template<>
void object::test<19>()
{
    std::vector<std::unique_ptr<geos::geom::Curve>> holes;

    std::vector<std::unique_ptr<geos::geom::SimpleCurve>> shell_sections;
    shell_sections.emplace_back(
         gf->createCircularString({
             CoordinateXY(0, 0),
             CoordinateXY(2, 0),
             CoordinateXY(2, 1),
             CoordinateXY(2, 3),
             CoordinateXY(4, 3)
    }));
    shell_sections.emplace_back(
         gf->createLineString({
             CoordinateXY(4, 3),
             CoordinateXY(4, 5),
             CoordinateXY(1, 4),
             CoordinateXY(0, 0)
    }));

    auto shell = gf->createCompoundCurve(std::move(shell_sections));

    holes.emplace_back(gf->createCircularString({
        CoordinateXY(1.7, 1),
        CoordinateXY(1.4, 0.4),
        CoordinateXY(1.6, 0.4),
        CoordinateXY(1.6, 0.5),
        CoordinateXY(1.7, 1)
    }));

    auto geom = gf->createCurvePolygon(std::move(shell), std::move(holes));

    ensure_equals(wktwriter.write(*geom), "CURVEPOLYGON (COMPOUNDCURVE (CIRCULARSTRING (0 0, 2 0, 2 1, 2 3, 4 3), (4 3, 4 5, 1 4, 0 0)), CIRCULARSTRING (1.7 1, 1.4 0.4, 1.6 0.4, 1.6 0.5, 1.7 1))");
}

// test MultiCurve
template<>
template<>
void object::test<20>()
{
    std::vector<std::unique_ptr<geos::geom::Curve>> curves;

    // Add a CompoundCurve
    std::vector<std::unique_ptr<geos::geom::SimpleCurve>> cc_sections;
    cc_sections.emplace_back(
         gf->createCircularString({
             CoordinateXY(0, 0),
             CoordinateXY(2, 0),
             CoordinateXY(2, 1),
             CoordinateXY(2, 3),
             CoordinateXY(4, 3)
    }));
    cc_sections.emplace_back(
         gf->createLineString({
             CoordinateXY(4, 3),
             CoordinateXY(4, 5),
             CoordinateXY(1, 4),
             CoordinateXY(0, 0)
    }));

    curves.emplace_back(gf->createCompoundCurve(std::move(cc_sections)));

    // Add a LineString
    curves.emplace_back(gf->createLineString({CoordinateXY(8, 9), CoordinateXY(10, 11)}));

    // Add a CircularString
    curves.emplace_back(gf->createCircularString({
        CoordinateXY(1.7, 1),
        CoordinateXY(1.4, 0.4),
        CoordinateXY(1.6, 0.4),
        CoordinateXY(1.6, 0.5),
        CoordinateXY(1.7, 1)
    }));

    auto geom = gf->createMultiCurve(std::move(curves));

    ensure_equals(wktwriter.write(*geom), "MULTICURVE (COMPOUNDCURVE (CIRCULARSTRING (0 0, 2 0, 2 1, 2 3, 4 3), (4 3, 4 5, 1 4, 0 0)), (8 9, 10 11), CIRCULARSTRING (1.7 1, 1.4 0.4, 1.6 0.4, 1.6 0.5, 1.7 1))");
}

// test MultiSurface
template<>
template<>
void object::test<21>()
{
    std::vector<std::unique_ptr<geos::geom::Surface>> surfaces;

    surfaces.emplace_back(
        gf->createPolygon(
            gf->createLinearRing({
                 CoordinateXY(0, 0),
                 CoordinateXY(1, 0),
                 CoordinateXY(1, 1),
                 CoordinateXY(0, 1),
                 CoordinateXY(0, 0)
    })));

    surfaces.emplace_back(
        gf->createCurvePolygon(
            gf->createCircularString({
                 CoordinateXY(10, 10),
                 CoordinateXY(11, 11),
                 CoordinateXY(12, 10),
                 CoordinateXY(11, 9),
                 CoordinateXY(10, 10)
    })));

    auto geom = gf->createMultiSurface(std::move(surfaces));

    ensure_equals(wktwriter.write(*geom), "MULTISURFACE (((0 0, 1 0, 1 1, 0 1, 0 0)), CURVEPOLYGON (CIRCULARSTRING (10 10, 11 11, 12 10, 11 9, 10 10)))");
}

// test formatted output
template<>
template<>
void object::test<22>()
{
    auto geom = wktreader.read("POINT (1 1)");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "POINT (1 1)");

    geom = wktreader.read("LINESTRING (1 2, 3 4)");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "LINESTRING (1 2, 3 4)");

    geom = wktreader.read("LINEARRING (0 0, 1 0, 1 1, 0 0)");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "LINEARRING (0 0, 1 0, 1 1, 0 0)");

    geom = wktreader.read("CIRCULARSTRING (0 0, 1 1, 2 0)");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "CIRCULARSTRING (0 0, 1 1, 2 0)");

    geom = wktreader.read("COMPOUNDCURVE((0 10, 0 5), CIRCULARSTRING (0 5, 1 1, 2 0), (2 0, 3 0))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "COMPOUNDCURVE ((0 10, 0 5), \n"
                                                        "  CIRCULARSTRING (0 5, 1 1, 2 0), \n"
                                                        "  (2 0, 3 0))");

    geom = wktreader.read("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 4, 4 4, 4 3, 3 3))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), \n"
                                                        "  (1 1, 1 2, 2 2, 2 1, 1 1), \n"
                                                        "  (3 3, 3 4, 4 4, 4 3, 3 3))");

    geom = wktreader.read("CURVEPOLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), CIRCULARSTRING (3 3, 3 4, 5 3, 3 2, 3 3))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "CURVEPOLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), \n"
                                                        "  (1 1, 1 2, 2 2, 2 1, 1 1), \n"
                                                        "  CIRCULARSTRING (3 3, 3 4, 5 3, 3 2, 3 3))");

    geom = wktreader.read("MULTIPOINT ((0 0), (1 1), (2 2))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "MULTIPOINT ((0 0), (1 1), (2 2))");

    geom = wktreader.read("MULTILINESTRING ((0 0, 1 1), (2 2, 3 3), (4 4, 5 5))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "MULTILINESTRING ((0 0, 1 1), \n"
                                                        "  (2 2, 3 3), \n"
                                                        "  (4 4, 5 5))");

    geom = wktreader.read("MULTICURVE ((0 0, 1 1), COMPOUNDCURVE ((2 2, 4 4), CIRCULARSTRING (4 4, 5 5, 6 4), (6 4, 7 4)), (100 100, 200 200))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "MULTICURVE ((0 0, 1 1), \n"
                                                        "  COMPOUNDCURVE ((2 2, 4 4), \n"
                                                        "    CIRCULARSTRING (4 4, 5 5, 6 4), \n"
                                                        "    (6 4, 7 4)), \n"
                                                        "  (100 100, 200 200))");

    geom = wktreader.read("MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 4, 4 4, 4 3, 3 3)), ((100 100, 200 100, 200 200, 100 100)))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), \n"
                                                        "  (1 1, 1 2, 2 2, 2 1, 1 1), \n"
                                                        "  (3 3, 3 4, 4 4, 4 3, 3 3)), \n"
                                                        "  ((100 100, 200 100, 200 200, 100 100)))");

    geom = wktreader.read("MULTISURFACE (CURVEPOLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), CIRCULARSTRING (3 3, 3 4, 5 3, 3 2, 3 3)), ((100 100, 200 100, 200 200, 100 100)))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "MULTISURFACE (CURVEPOLYGON ((0 0, 10 0, 10 10, 0 10, 0 0), \n"
                                                        "  (1 1, 1 2, 2 2, 2 1, 1 1), \n"
                                                        "  CIRCULARSTRING (3 3, 3 4, 5 3, 3 2, 3 3)), \n"
                                                        "  ((100 100, 200 100, 200 200, 100 100)))");

    geom = wktreader.read("GEOMETRYCOLLECTION (LINESTRING (0 0, 1 1), MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1), (3 3, 3 4, 4 4, 4 3, 3 3)), ((100 100, 200 100, 200 200, 100 100))), POINT (2 2))");
    ensure_equals(wktwriter.writeFormatted(geom.get()), "GEOMETRYCOLLECTION (LINESTRING (0 0, 1 1), \n"
                                                        "  MULTIPOLYGON (((0 0, 10 0, 10 10, 0 10, 0 0), \n"
                                                        "    (1 1, 1 2, 2 2, 2 1, 1 1), \n"
                                                        "    (3 3, 3 4, 4 4, 4 3, 3 3)), \n"
                                                        "    ((100 100, 200 100, 200 200, 100 100))), \n"
                                                        "  POINT (2 2))");
}



} // namespace tut
