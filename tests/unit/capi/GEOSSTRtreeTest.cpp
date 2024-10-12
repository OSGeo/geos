//
// Test Suite for C-API GEOSSTRtree

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
#include <geos/constants.h>
// std
#include <cstdio>
#include <cstring>
#include <cmath>
#include <thread>

#include "capi_test_utils.h"

struct INTPOINT {
    INTPOINT(int p_x, int p_y) : x(p_x), y(p_y) {}
    int x;
    int y;
};

static GEOSGeometry*
INTPOINT2GEOS(INTPOINT* p)
{
    GEOSCoordSequence* seq = GEOSCoordSeq_create(1, 2);
    GEOSCoordSeq_setX(seq, 0, p->x);
    GEOSCoordSeq_setY(seq, 0, p->y);
    return GEOSGeom_createPoint(seq);
}

static int
INTPOINT_dist(const void* a, const void* b, double* distance, void*)
{
    INTPOINT* p1 = (INTPOINT*) a;
    INTPOINT* p2 = (INTPOINT*) b;

    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;

    *distance = std::sqrt(static_cast<double>(dx * dx) + dy * dy);
    return 1;
}

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capistrtree_data : public capitest::utility {
};

typedef test_group<test_capistrtree_data> group;
typedef group::object object;

group test_capistrtree_group("capi::GEOSSTRtree");

//
// Test Cases
//

// Test GEOSSTRtree_nearest with a couple of points
template<>
template<>
void object::test<1>
()
{
    GEOSGeometry* g1 = GEOSGeomFromWKT("POINT (3 3)");
    GEOSGeometry* g2 = GEOSGeomFromWKT("POINT (2 7)");
    GEOSGeometry* g3 = GEOSGeomFromWKT("POINT (5 4)");
    GEOSGeometry* g4 = GEOSGeomFromWKT("POINT (3 8)");

    GEOSSTRtree* tree = GEOSSTRtree_create(2);
    GEOSSTRtree_insert(tree, g1, g1);
    GEOSSTRtree_insert(tree, g2, g2);
    GEOSSTRtree_insert(tree, g3, g3);

    const GEOSGeometry* g5 = GEOSSTRtree_nearest(tree, g4);
    ensure(g5 == g2);

    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(g3);
    GEOSGeom_destroy(g4);
    GEOSSTRtree_destroy(tree);
}

// Test GEOSSTRtree_nearest with more points.  This is important because we need to make sure the tree
// actually has a couple of layers of depth.
template<>
template<>
void object::test<2>
()
{
    std::size_t ngeoms = 100;
    std::vector<GEOSGeometry*> geoms;
    std::vector<GEOSGeometry*> queryPoints;
    GEOSSTRtree* tree = GEOSSTRtree_create(8);

    for(std::size_t i = 0; i < ngeoms; i++) {
        GEOSCoordSequence* seq = GEOSCoordSeq_create(1, 2);
        GEOSCoordSeq_setX(seq, 0, std::rand());
        GEOSCoordSeq_setY(seq, 0, std::rand());
        geoms.push_back(GEOSGeom_createPoint(seq));
        GEOSSTRtree_insert(tree, geoms[i], geoms[i]);
    }

    for(std::size_t i = 0; i < ngeoms; i++) {
        GEOSCoordSequence* seq = GEOSCoordSeq_create(1, 2);
        GEOSCoordSeq_setX(seq, 0, std::rand());
        GEOSCoordSeq_setY(seq, 0, std::rand());
        queryPoints.push_back(GEOSGeom_createPoint(seq));
    }

    for(std::size_t i = 0; i < ngeoms; i++) {
        const GEOSGeometry* nearest = GEOSSTRtree_nearest(tree, queryPoints[i]);
        const GEOSGeometry* nearestBruteForce = nullptr;
        double nearestBruteForceDistance = geos::DoubleInfinity;
        for(std::size_t j = 0; j < ngeoms; j++) {
            double distance;
            GEOSDistance(queryPoints[i], geoms[j], &distance);

            if(nearestBruteForce == nullptr || distance < nearestBruteForceDistance) {
                nearestBruteForce = geoms[j];
                nearestBruteForceDistance = distance;
            }
        }

        ensure(nearest == nearestBruteForce || GEOSEquals(nearest, nearestBruteForce));
    }

    for(std::size_t i = 0; i < ngeoms; i++) {
        GEOSGeom_destroy(geoms[i]);
        GEOSGeom_destroy(queryPoints[i]);
    }

    GEOSSTRtree_destroy(tree);
}

// GEOSSTRtree_nearest returns NULL on empty tree
template<>
template<>
void object::test<3>
()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);
    GEOSGeometry* g1 = GEOSGeomFromWKT("POINT (3 3)");
    const GEOSGeometry* g2 = GEOSSTRtree_nearest(tree, g1);

    ensure(g2 == nullptr);

    GEOSGeom_destroy(g1);
    GEOSSTRtree_destroy(tree);
}

// GEOSSTRtree_nearest with a user-defined type
template<>
template<>
void object::test<4>
()
{
    INTPOINT p1(1, 1);
    INTPOINT p2(4, 4);
    INTPOINT p3(3, 3);

    GEOSGeometry* g1 = INTPOINT2GEOS(&p1);
    GEOSGeometry* g2 = INTPOINT2GEOS(&p2);
    GEOSGeometry* g3 = INTPOINT2GEOS(&p3);

    GEOSSTRtree* tree = GEOSSTRtree_create(4);
    GEOSSTRtree_insert(tree, g1, &p1);
    GEOSSTRtree_insert(tree, g2, &p2);

    const INTPOINT* p4 = (const INTPOINT*) GEOSSTRtree_nearest_generic(tree, &p3, g3, &INTPOINT_dist, nullptr);

    ensure(p4 == &p2);

    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(g3);
    GEOSSTRtree_destroy(tree);
}

// GEOSSTRtree_nearest with a tree of empty geometries
template<>
template<>
void object::test<5>
()
{
    GEOSGeometry* g1 = GEOSGeomFromWKT("LINESTRING EMPTY");
    GEOSGeometry* g2 = GEOSGeomFromWKT("POINT (2 7)");

    GEOSSTRtree* tree = GEOSSTRtree_create(4);
    GEOSSTRtree_insert(tree, g1, g1);

    const GEOSGeometry* g3 = GEOSSTRtree_nearest(tree, g2);
    ensure(g3 == nullptr);

    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSSTRtree_destroy(tree);
}

// GEOSSTRtree_nearest with a tree containing some empty geometries
template<>
template<>
void object::test<6>
()
{
    GEOSGeometry* g1 = GEOSGeomFromWKT("LINESTRING EMPTY");
    GEOSGeometry* g2 = GEOSGeomFromWKT("POINT (2 7)");
    GEOSGeometry* g3 = GEOSGeomFromWKT("POINT (12 97)");
    GEOSGeometry* g4 = GEOSGeomFromWKT("LINESTRING (3 8, 4 8)");

    GEOSSTRtree* tree = GEOSSTRtree_create(4);
    GEOSSTRtree_insert(tree, g1, g1);
    GEOSSTRtree_insert(tree, g2, g2);
    GEOSSTRtree_insert(tree, g3, g3);

    const GEOSGeometry* g5 = (const GEOSGeometry*) GEOSSTRtree_nearest(tree, g4);
    ensure(g5 == g2);

    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(g3);
    GEOSGeom_destroy(g4);
    GEOSSTRtree_destroy(tree);
}

// querying empty tree should not crash (see #730)
template<>
template<>
void object::test<7>
()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(16);
    GEOSGeometry* q = GEOSGeomFromWKT("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))");
    GEOSSTRtree_query(tree, q, [](void* item, void* userdata) {
        (void)(item); // make unused parameter warning go away
        (void)(userdata); // make unused parameter warning go away
    }, nullptr);

    GEOSGeom_destroy(q);
    GEOSSTRtree_destroy(tree);
}

// querying tree with box
template<>
template<>
void object::test<8>
()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);

    GEOSGeometry* g = GEOSGeomFromWKT("POINT (2 3)");
    int payload = 876;
    GEOSSTRtree_insert(tree, g, &payload);

    GEOSGeometry* q = GEOSGeomFromWKT("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0))");

    typedef std::vector<int*> IList;
    IList items;
    ensure_equals(items.size(), 0U);
    GEOSSTRtree_query(
        tree,
        q,
        [](void* item, void* userdata) {
            IList* il = (IList*)userdata;
            il->push_back((int*)item);
        },
        &items);

    ensure_equals(items.size(), 1U);

    ensure_equals(*(items[0]), payload);

    GEOSGeom_destroy(q);
    GEOSGeom_destroy(g);
    GEOSSTRtree_destroy(tree);
}


// Index a null pointer
template<>
template<>
void object::test<9>
()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);

    GEOSGeometry* g = GEOSGeomFromWKT("POINT (2 3)");
    GEOSSTRtree_insert(tree, g, (void*)0);

    GEOSGeometry* q = GEOSGeomFromWKT("POLYGON ((0 0, 10 0, 10 10, 0 10, 0 0))");

    typedef std::vector<int*> IList;
    IList items;
    ensure_equals(items.size(), 0U);
    GEOSSTRtree_query(
        tree,
        q,
        [](void* item, void* userdata) {
            IList* il = (IList*)userdata;
            il->push_back((int*)item);
        },
        &items);

    ensure_equals(items.size(), 1U);

    ensure_equals(items[0], (void*)0);

    GEOSGeom_destroy(q);
    GEOSGeom_destroy(g);
    GEOSSTRtree_destroy(tree);
}

// Test that envelopes of inserted objects are owned by the tree.
template<>
template<>
void object::test<10>()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);

    std::vector<size_t> ids(10);
    for (size_t i = 0; i < 10; i++) {
        ids[i] = i;
        GEOSGeometry* point = GEOSGeom_createPointFromXY((double) i, (double) i);
        GEOSSTRtree_insert(tree, point, &ids[i]);
        GEOSGeom_destroy(point);
    }

    GEOSGeometry* queryPoint = GEOSGeom_createPointFromXY(3.0, 3.0);
    size_t hitVal = 0;
    GEOSSTRtree_query(tree, queryPoint, [](void* item, void* data) {
        *((size_t*) data) = *((size_t*) item);
    }, &hitVal);
    GEOSGeom_destroy(queryPoint);
    GEOSSTRtree_destroy(tree);

    ensure_equals(hitVal, 3u);
}

// queries are thread-safe
template<>
template<>
void object::test<11>()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);

    std::vector<GEOSGeometry*> geoms;
    for (size_t i = 0; i < 100; i++) {
        geoms.push_back(GEOSGeom_createPointFromXY((double) i, (double) i));
    }
    for (const auto& geom : geoms) {
        GEOSSTRtree_insert(tree, geom, geom);
    }

    auto query = [tree](const GEOSGeometry* env) {
        GEOSSTRtree_query(tree, env, [](void* g, void* userdata) {
            (void) g;
            (void) userdata;
        }, nullptr);
    };

    GEOSGeometry* q1 = GEOSBuffer(geoms[10], 10, 48);
    GEOSGeometry* q2 = GEOSBuffer(geoms[40], 10, 48);

    std::thread t1(query, q1);
    std::thread t2(query, q2);

    t1.join();
    t2.join();

    GEOSGeom_destroy(q1);
    GEOSGeom_destroy(q2);

    for (auto& geom : geoms) {
        GEOSGeom_destroy(geom);
    }

    GEOSSTRtree_destroy(tree);
}

template<>
template<>
void object::test<12>()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(10);

    GEOSGeometry* g1 = GEOSGeomFromWKT("LINESTRING (0 0, 10 10)");
    GEOSGeometry* g2 = GEOSGeomFromWKT("LINESTRING (20 20, 30 30)");
    GEOSGeometry* g3 = GEOSGeomFromWKT("LINESTRING (20 20, 30 30)");

    GEOSSTRtree_insert(tree, g1, g1);
    GEOSSTRtree_insert(tree, g2, g2);
    GEOSSTRtree_insert(tree, g3, g3);

    GEOSGeometry* p = GEOSGeomFromWKT("POINT (5 5)");

    ensure_equals(GEOSSTRtree_remove(tree, p, g1), 1);

    std::vector<GEOSGeometry*> hits;
    GEOSSTRtree_query(tree, p, [](void* item, void* userdata) {
        auto h = static_cast<std::vector<GEOSGeometry*>*>(userdata);
        h->push_back(static_cast<GEOSGeometry*>(item));
    }, &hits);

    ensure(hits.empty());

    GEOSGeom_destroy(g1);
    GEOSGeom_destroy(g2);
    GEOSGeom_destroy(g3);
    GEOSGeom_destroy(p);

    GEOSSTRtree_destroy(tree);
}

// Test GEOSSTRtree_iterate
template<>
template<>
void object::test<13>()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(4);
    std::vector<GEOSGeometry*> geoms;

    for (int i = 50; i >= 0; i--) {
        geoms.push_back(GEOSGeom_createPointFromXY((double) i, 0));
        GEOSSTRtree_insert(tree, geoms.back(), geoms.back());
    }

    std::vector<GEOSGeometry*> geomsFound;

    // Iterate to collect all tree items
    GEOSSTRtree_iterate(tree, [](void* item, void* userdata) {
        std::vector<GEOSGeometry*>& hits = *static_cast<std::vector<GEOSGeometry*>*>(userdata);
        hits.push_back(static_cast<GEOSGeometry*>(item));
    }, &geomsFound);

    // GEOSSTRtree_iterate does not force tree construction, so input order = iteration order
    ensure_equals(geomsFound.size(), geoms.size());
    for (std::size_t i = 0; i < geomsFound.size(); i++) {
        ensure_equals(geoms[i], geomsFound[i]);
    }

    // Perform a query to force tree construction
    ensure_equals(GEOSSTRtree_build(tree), 1);

    // Iterate to collect all tree items
    std::vector<GEOSGeometry*> geomsFound2;
    GEOSSTRtree_iterate(tree, [](void* item, void* userdata) {
        std::vector<GEOSGeometry*>& hits = *static_cast<std::vector<GEOSGeometry*>*>(userdata);
        hits.push_back(static_cast<GEOSGeometry*>(item));
    }, &geomsFound2);

    ensure_equals(geomsFound2.size(), geoms.size());
    // Tree has been constructed so item order now reflects tree order rather than insertion order
    ensure(geomsFound2[0] != geoms[0]);

    // Cleanup
    for (auto& g : geoms) {
        GEOSGeom_destroy(g);
    }

    GEOSSTRtree_destroy(tree);
}

// Removed items are not returned by GEOSSTRtree_iterate
template<>
template<>
void object::test<14>()
{
    GEOSSTRtree* tree = GEOSSTRtree_create(4);
    std::vector<GEOSGeometry*> geoms;
    std::size_t ngeoms = 50;

    for (std::size_t i = 0; i < ngeoms; i++) {
        geoms.push_back(GEOSGeom_createPointFromXY((double) i, 0));
        GEOSSTRtree_insert(tree, geoms.back(), geoms.back());
    }

    // Remove even numbers
    for (const auto& g : geoms) {
        double x;
        GEOSGeomGetX(g, &x);
        if (static_cast<int>(x) % 2 == 0) {
            GEOSSTRtree_remove(tree, g, g);
        }
    }

    std::vector<const GEOSGeometry*> geomsFound;

    // Iterate to collect all tree items
    GEOSSTRtree_iterate(tree, [](void* item, void* userdata) {
        auto& hits = *static_cast<std::vector<const GEOSGeometry*>*>(userdata);
        hits.push_back(static_cast<const GEOSGeometry*>(item));
    }, &geomsFound);

    ensure_equals(geomsFound.size(), ngeoms / 2);

    // Cleanup
    for (auto& g : geoms) {
        GEOSGeom_destroy(g);
    }

    GEOSSTRtree_destroy(tree);
}


} // namespace tut



