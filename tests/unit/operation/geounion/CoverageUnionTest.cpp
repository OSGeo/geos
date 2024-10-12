//
// Test Suite for geos::operation::geounion::CoverageUnion class.

// tut
#include <tut/tut.hpp>
// geos
#include <geos/operation/union/UnaryUnionOp.h>
#include <geos/operation/union/CoverageUnion.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/util/TopologyException.h>
// std
#include <memory>
#include <string>
#include <vector>

namespace tut {
//
// Test Group
//

// Common data used by tests
    struct test_coverageuniontest_data {
    };

    typedef test_group<test_coverageuniontest_data> group;
    typedef group::object object;

    group test_coverageuniontest_group("geos::operation::geounion::CoverageUnion");

    void checkCoverageUnionEquivalentToUnaryUnion(const std::vector<std::string> &wkt_geoms) {
        using geos::io::WKTReader;
        using geos::geom::Geometry;
        using geos::geom::GeometryFactory;
        using geos::operation::geounion::CoverageUnion;
        using geos::operation::geounion::UnaryUnionOp;

        auto gfact = GeometryFactory::create();

        WKTReader reader(gfact.get());

        std::vector<std::unique_ptr<Geometry>> geoms;

        for (const auto& wkt : wkt_geoms) {
            geoms.push_back(reader.read(wkt));
        }

        std::unique_ptr<Geometry> coll(gfact->createGeometryCollection(std::move(geoms)));

        auto u1 = UnaryUnionOp::Union(*coll);
        auto u2 = CoverageUnion::Union(coll.get());

        ensure( u1->equals(u2.get()) );
    }

    void checkCoverageUnionFails(const std::vector<std::string> & wkt_geoms) {
        using geos::io::WKTReader;
        using geos::geom::Geometry;
        using geos::geom::GeometryFactory;
        using geos::operation::geounion::CoverageUnion;
        auto gfact = GeometryFactory::create();

        WKTReader reader(gfact.get());

        std::vector<std::unique_ptr<Geometry>> geoms;

        for (const auto& wkt : wkt_geoms) {
            geoms.push_back(reader.read(wkt));
        }

        std::unique_ptr<Geometry> coll(gfact->createGeometryCollection(std::move(geoms)));

        try {
            auto u1 = CoverageUnion::Union(coll.get());
            fail();
        } catch(const geos::util::TopologyException &) {
        }
    }

    template<>
    template<>
    void object::test<1>
    ()
    {
        // Adjacent squares
        std::vector<std::string> geoms{
            "POLYGON ((0 0, 0 1, 1 1, 1 0, 0 0))",
            "POLYGON ((1 0, 1 1, 2 1, 2 0, 1 0))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<2>()
    {
        // Nested squares
        std::vector<std::string> geoms{
                "POLYGON ((-0.83 1.06, -0.629 1.06, -0.629 0.875, -0.83 0.875, -0.83 1.06), (-0.675 0.918, -0.78 0.918, -0.78 1.02, -0.675 1.02, -0.675 0.918))",
                "POLYGON ((-0.675 0.918, -0.78 0.918, -0.78 1.02, -0.675 1.02, -0.675 0.918))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<3>()
    {
        // Disconnected components
        std::vector<std::string> geoms{
                "POLYGON ((-0.84 1.18, -0.705 1.18, -0.705 1.121, -0.84 1.121, -0.84 1.18))",
                "POLYGON ((-1.016 1.184, -0.89 1.184, -0.89 1.11, -1.016 1.11, -1.016 1.184))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<4>()
    {
        // Bow-tie
        std::vector<std::string> geoms{
                "POLYGON ((-0.88 1.04, -0.79 1.07, -0.865 1.123, -0.88 1.04))",
                "POLYGON ((-0.865 1.123, -0.935 1.167, -0.863 1.186, -0.865 1.123))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<5>()
    {
        // Polygon inside hole
        std::vector<std::string> geoms{
              "POLYGON ((0 0, 0 20, 40 20, 40 0, 0 0), (30 10, 35 10, 35 15, 5 15, 5 5, 30 5, 30 10))",
              "POLYGON ((20 10, 20 12, 30 12, 29 10, 20 10))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<6>()
    {
        // Polygon inside hole, touching
        std::vector<std::string> geoms{
               "POLYGON ((0 0, 0 20, 40 20, 40 0, 0 0), (30 10, 35 10, 35 15, 5 15, 5 5, 30 5, 30 10))",
               "POLYGON ((20 10, 20 12, 30 12, 30 10, 20 10))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<7>()
    {
        // Multiple nested holes
        std::vector<std::string> geoms{
                "MULTIPOLYGON (((0 0, 0 70, 70 70, 70 0, 0 0), "
                "(20 10, 30 10, 30 20, 40 20, 40 10, 50 10, 50 20, 60 20, 60 30, 50 30, 50 40, 60 40, 60 50, 50 50, 50 60, 40 60, 40 50, 30 50, 30 60, 20 60, 20 50, 10 50, 10 40, 20 40, 20 30, 10 30, 10 20, 20 20, 20 10)),"
                "  ((20 20, 20 30, 30 30, 30 20, 20 20)),"
                "  ((40 20, 40 30, 50 30, 50 20, 40 20)),"
                "  ((30 30, 30 40, 40 40, 40 30, 30 30)),"
                "  ((20 40, 20 50, 30 50, 30 40, 20 40)),"
                "  ((40 40, 40 50, 50 50, 50 40, 40 40)))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<8>()
    {
        // Incorrectly noded input
        std::vector<std::string> geoms {
            "POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))",
            "POLYGON ((1 0, 1 0.5, 1 1, 2 1, 2 0, 1 0))"
        };

        checkCoverageUnionFails(geoms);
    }

    template<>
    template<>
    void object::test<9>()
    {
        // Adjacent polygons with sliver
        std::vector<std::string> geoms {
                "POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))",
                "POLYGON ((1 0, 1.00000008 0.5, 1 1, 2 1, 2 0, 1 0))"
        };

        checkCoverageUnionEquivalentToUnaryUnion(geoms);
    }

    template<>
    template<>
    void object::test<10>()
    {
        // Adjacent polygons with overlap
        std::vector<std::string> geoms {
                "POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))",
                "POLYGON ((1 0, 0.99 0.5, 1 1, 2 1, 2 0, 1 0))"
        };

        checkCoverageUnionFails(geoms);
    }

} // namespace tut


