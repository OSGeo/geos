#include <tut/tut.hpp>
// geos
#include <geos/constants.h> // for std::isnan
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/operation/valid/MakeValid.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/util.h>

#include <utility.h>

using namespace geos::geom;
using namespace geos::operation::valid;

namespace tut {
//
// Test Group
//

struct test_makevalid_data {
    test_makevalid_data() {}
};

typedef test_group<test_makevalid_data> group;
typedef group::object object;

group test_makevalid_group("geos::operation::valid::MakeValid");

//
// Test Cases
//

// https://github.com/libgeos/geos/issues/265
template<>
template<>
void object::test<1>
()
{
    auto cs = geos::detail::make_unique<CoordinateSequence>();
    cs->add(2.22, 2.28);
    cs->add(7.67, 2.06);
    cs->add(10.98, 7.70);
    cs->add(9.39, 5.00);
    cs->add(7.96, 7.12);
    cs->add(6.77, 5.16);
    cs->add(7.43, 6.24);
    cs->add(3.70, 7.22);
    cs->add(5.72, 5.77);
    cs->add(4.18, 10.74);
    cs->add(2.20, 6.83);
    cs->add(2.22, 2.28);

    auto gf = GeometryFactory::getDefaultInstance();
    auto lr = gf->createLinearRing(std::move(cs));
    auto errplyg = gf->createPolygon(std::move(lr));

    ensure(!errplyg->isValid());

    MakeValid mkvalid;
    auto validGeom = mkvalid.build(errplyg.get());

    ensure(validGeom->isValid());
}

// template<>
// template<>
// void object::test<2>
// ()
// {


//     std::ifstream ifs("GoesBathymetryBug.txt");
//     std::string content((std::istreambuf_iterator<char>(ifs)),
//                        (std::istreambuf_iterator<char>()));

//     geos::io::WKTReader reader;
//     auto geom(reader.read(content));

//     // auto gf = GeometryFactory::getDefaultInstance();

//     // auto cs = gf->getCoordinateSequenceFactory()->create(std::move(v));
//     // auto lr = gf->createLinearRing(std::move(cs));
//     // auto errplyg = gf->createPolygon(std::move(lr));

//     // ensure(!errplyg->isValid());

//     MakeValid mkvalid;
//     auto validGeom = mkvalid.build(geom.get());
//     ensure("MakeValid output is not valid", validGeom->isValid());

//     geos::io::WKTWriter writer;
//     writer.setOutputDimension(2);
//     writer.setTrim(true);
//     std::string result = writer.write(validGeom.get());
//     std::cout << result << std::endl;

// }

template<>
template<>
void object::test<3>()
{
    auto gf = GeometryFactory::getDefaultInstance();

    auto mp = gf->createMultiPolygon();

    MakeValid mv;
    auto result = mv.build(mp.get());

    ensure(result->isValid());
}

template<>
template<>
void object::test<4>()
{
    geos::io::WKBReader reader;
    std::stringstream input;
    // From PostGIS test: https://github.com/postgis/postgis/blob/5e310cf6ad646702e5574eb3aa2391021dcdd8c5/liblwgeom/cunit/cu_geos.c#L147
    input << "0103000000010000000900000062105839207df640378941e09d491c41ced67431387df640c667e7d398491"
             "c4179e92631387df640d9cef7d398491c41fa7e6abcf87df640cdcccc4c70491c41e3a59bc4527df64052b8"
             "1e053f491c41cdcccccc5a7ef640e3a59bc407491c4104560e2da27df640aaf1d24dd3481c41e9263108c67"
             "bf64048e17a1437491c4162105839207df640378941e09d491c41";

    auto g = reader.readHEX(input);

    geos::operation::valid::MakeValid mv;

    auto result = mv.build(g.get());

    ensure_equals_geometry(result.get(),
                           "POLYGON((92127.546 463452.075,92117.173 463439.755,92133.675 463425.942,"
                           "92122.136 463412.826,92092.377 463437.77,92114.014 463463.469,92115.512 463462.207,"
                           "92115.51207431706 463462.2069374289,92127.546 463452.075))");
}


} // namespace tut
