//
// Test Suite for C-API GEOSCluster*

#include <tut/tut.hpp>
// geos
#include <geos_c.h>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capicluster_data : public capitest::utility {
    static GEOSGeometry* construct_clusters(const GEOSGeometry* input,
                                            const GEOSClusterInfo* clusters)
    {
        auto ngeoms = static_cast<std::size_t>(GEOSGetNumGeometries(input));
        auto numClusters = GEOSClusterInfo_getNumClusters(clusters);

        std::vector<GEOSGeometry*> cluster_components(ngeoms);
        std::vector<GEOSGeometry*> cluster_geoms(numClusters);

        for (std::size_t cluster_id = 0; cluster_id < numClusters; cluster_id++) {
            auto sz = GEOSClusterInfo_getClusterSize(clusters, cluster_id);
            const std::size_t* indices = GEOSClusterInfo_getInputsForClusterN(clusters, cluster_id);
            for (std::size_t i = 0; i < sz; i++) {
                cluster_components[i] = GEOSGeom_clone(GEOSGetGeometryN(input, static_cast<int>(indices[i])));
            }
            cluster_geoms[cluster_id] = GEOSGeom_createCollection(GEOS_GEOMETRYCOLLECTION,
                                                                  cluster_components.data(),
                                                                  static_cast<unsigned>(sz));
        }

        // combine the clusters into a single nested GeometryCollection
        return GEOSGeom_createCollection(GEOS_GEOMETRYCOLLECTION,
                                         cluster_geoms.data(),
                                         static_cast<unsigned>(cluster_geoms.size()));
    }
};

typedef test_group<test_capicluster_data> group;
typedef group::object object;

group test_capicluster_group("capi::GEOSCluster");

//
// Test Cases
//
template<>
template<> void object::test<1>
()
{
    input_ = fromWKT(
                 "GEOMETRYCOLLECTION ("
                 "POINT (0 1),"
                 "LINESTRING (0 0, 0 0.1),"
                 "LINESTRING (0 0, 1.0 1.0),"
                 "POINT (0.9 1.0),"
                 "POINT (0 7))");

    {
        GEOSClusterInfo* clusters = GEOSClusterEnvelopeIntersects(input_);
        ensure_equals("two clusters by envelope intersection", GEOSClusterInfo_getNumClusters(clusters), 2u);

        GEOSGeometry* geom_result = construct_clusters(input_, clusters);
        GEOSGeometry* geom_expected = fromWKT("GEOMETRYCOLLECTION ("
                                              "  GEOMETRYCOLLECTION ("
                                              "    POINT (0 1),"
                                              "    LINESTRING (0 0, 0 0.1),"
                                              "    LINESTRING (0 0, 1.0 1.0),"
                                              "    POINT (0.9 1.0)),"
                                              "  GEOMETRYCOLLECTION ("
                                              "    POINT (0 7)))");

        ensure_geometry_equals_identical(geom_expected, geom_result);
        GEOSGeom_destroy(geom_result);
        GEOSGeom_destroy(geom_expected);

        GEOSClusterInfo_destroy(clusters);
    }

    {
        GEOSClusterInfo* clusters = GEOSClusterEnvelopeDistance(input_, 6);
        ensure_equals("one cluster by envelope distance", GEOSClusterInfo_getNumClusters(clusters), 1u);
        GEOSClusterInfo_destroy(clusters);
    }

    {
        GEOSClusterInfo* clusters = GEOSClusterGeometryIntersects(input_);
        ensure_equals("four clusters by geometry intersection", GEOSClusterInfo_getNumClusters(clusters), 4u);
        GEOSClusterInfo_destroy(clusters);
    }

    {
        GEOSClusterInfo* clusters = GEOSClusterGeometryDistance(input_, 0.2);
        ensure_equals("three clusters by distance", GEOSClusterInfo_getNumClusters(clusters), 3u);
        GEOSClusterInfo_destroy(clusters);
    }
}

template<>
template<>
void object::test<2>()
{
    input_ = fromWKT(
                 "GEOMETRYCOLLECTION ("
                 "POINT (0 0),"
                 "POINT (-1 0),"
                 "POINT (-1 -0.1),"
                 "POINT (-1 0.1),"
                 "POINT (1 0),"
                 "POINT (2 0),"
                 "POINT (3  0),"
                 "POINT ( 3 -0.1),"
                 "POINT ( 3 0.1)"
                 ")");

    {
        GEOSClusterInfo* clusters = GEOSClusterDBSCAN(input_, 1.01, 5);
        ensure_equals("two clusters with minPoints = 5", GEOSClusterInfo_getNumClusters(clusters), 2u);
        GEOSClusterInfo_destroy(clusters);
    }

    {
        GEOSClusterInfo* clusters = GEOSClusterDBSCAN(input_, 1.01, 2);
        ensure_equals("one cluster with minPoints = 2", GEOSClusterInfo_getNumClusters(clusters), 1u);
        GEOSClusterInfo_destroy(clusters);
    }

    {
        GEOSClusterInfo* clusters = GEOSClusterDBSCAN(input_, 1.01, 20);
        ensure_equals("no clusters with minPoints = 20", GEOSClusterInfo_getNumClusters(clusters), 0u);

        auto* cluster_ids = GEOSClusterInfo_getClustersForInputs(clusters);
        for (int i = 0; i < GEOSGetNumGeometries(input_); i++) {
            ensure_equals(cluster_ids[i], GEOS_CLUSTER_NONE);
        }
        GEOSClusterInfo_destroy(clusters);
        GEOSFree(cluster_ids);
    }
}


} // namespace tut

