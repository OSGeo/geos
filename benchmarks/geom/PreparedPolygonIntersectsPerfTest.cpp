/******************************************************
 *   Performance tests for spatial predicates
 * 
 * Usage: perf_prepared_polygon_intersects [ intersects | contains ]
******************************************************/

#include <geos/geom/util/SineStarFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/profiler.h>
#include <geos/geom/IntersectionMatrix.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/operation/relateng/RelateNG.h>
#include <geos/operation/relateng/RelatePredicate.h>
#include <geos/io/WKBWriter.h>
#include <BenchmarkUtils.h>

#include <iomanip>

using namespace geos::geom;

std::size_t MAX_ITER = 10;
std::size_t NUM_LINES = 10000;
std::size_t NUM_LINES_PTS = 100;

#define INTERSECTS 0
#define CONTAINS 1

int predicateOp = INTERSECTS;

int testRelateOpIntersects(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    for (const auto& geom : geoms) {
        auto im = g.relate(geom.get());
        count += im->isIntersects();
    }
    return count;
}

int testRelateOpContains(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    for (const auto& geom : geoms) {
        auto im = g.relate(geom.get());
        count += im->isContains();
    }
    return count;
}

int testGeometryIntersects(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    for (const auto& geom : geoms) {
        count += g.intersects(geom.get());
    }
    return count;
}

int testGeometryContains(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    for (const auto& geom : geoms) {
        count += g.contains(geom.get());
    }
    return count;
}

int testPrepGeomIntersects(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    auto prep = prep::PreparedGeometryFactory::prepare(&g);
    for (const auto& geom : geoms) {
        count += prep->intersects(geom.get());
    }
    return count;
}

int testPrepGeomContains(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    auto prep = prep::PreparedGeometryFactory::prepare(&g);
    for (const auto& geom : geoms) {
        count += prep->contains(geom.get());
    }
    return count;
}

int testRelateNGPreparedIntersects(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    auto prep = geos::operation::relateng::RelateNG::prepare(&g);
    for (const auto& line : geoms) {
        count += prep->evaluate(line.get(), *geos::operation::relateng::RelatePredicate::intersects());
    }
    return count;
}

int testRelateNGPreparedContains(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms) {
    int count = 0;
    auto prep = geos::operation::relateng::RelateNG::prepare(&g);
    for (const auto& line : geoms) {
        count += prep->evaluate(line.get(), *geos::operation::relateng::RelatePredicate::contains());
    }
    return count;
}

template<typename F>
double test(const Geometry& g, const std::vector<std::unique_ptr<Geometry>>& geoms, const std::string& method, F&& fun, double baseTime)
{
    geos::util::Profile sw("PreparedPolygonIntersects");
    sw.start();

    int count = 0;
    for (std::size_t i = 0; i < MAX_ITER; i++) {
        count += fun(g, geoms);
    }

    sw.stop();
    double tot = sw.getTot();
    double timesFaster = baseTime == 0 ? 1 : baseTime / tot;
    std::cout << std::fixed << std::setprecision(0);
    std::cout << g.getNumPoints() << "," 
        << MAX_ITER * geoms.size() << "," 
        << count << "," << geoms[0]->getGeometryType() << "," 
        << geoms[0]->getNumPoints() << "," 
        << method << "," 
        << tot << ","
        << timesFaster 
        << std::endl;
    return tot;
}

void test(int dim, std::size_t npts) {

    auto target = geos::benchmark::createSineStar({0, 0}, 100, npts);
    std::vector<std::unique_ptr<Geometry>> geoms;
    switch (dim) {
    case 0:
        geoms = geos::benchmark::createPoints(*target->getEnvelopeInternal(), NUM_LINES);
        break;
    case 1:
        geoms = geos::benchmark::createLines(*target->getEnvelopeInternal(), NUM_LINES, 1.0, NUM_LINES_PTS);
        break;
    case 2:
        geoms = geos::benchmark::createPolygons(*target->getEnvelopeInternal(), NUM_LINES, 1.0, NUM_LINES_PTS);
        break;
    }
    double baseTime;
    switch (predicateOp) {
    case INTERSECTS:
        baseTime = test(*target, geoms, "RelateOp intersects", testRelateOpIntersects, 0);
        test(*target, geoms, "Geometry::intersects", testGeometryIntersects, baseTime);
        test(*target, geoms, "PreparedGeom", testPrepGeomIntersects, baseTime);
        test(*target, geoms, "RelateNGPrepared", testRelateNGPreparedIntersects, baseTime);
        break;
    case CONTAINS:
        baseTime = test(*target, geoms, "RelateOp contains", testRelateOpIntersects, 0);
        test(*target, geoms, "Geometry::contains", testGeometryIntersects, baseTime);
        test(*target, geoms, "PreparedGeom contains", testPrepGeomContains, baseTime);
        test(*target, geoms, "RelateNGPrepared contains", testRelateNGPreparedContains, baseTime);
    }
}

void testAll(int dim)
{
    test(dim, 5);
    test(dim, 10);
    test(dim, 500);
    test(dim, 1000);
    test(dim, 2000);
    test(dim, 4000);
    test(dim, 8000);
    test(dim, 16000);
}

int main(int argc, char** argv) {
    predicateOp = INTERSECTS;
    if (argc >= 2) {
        std::string op{argv[1]};
        if (op == "contains") {
            predicateOp = CONTAINS;
        }
    }

    std::cout << "target_points,num_tests,num_hits,test_type,pts_in_test,method,time,factor" << std::endl;
    testAll(0);
    testAll(1);
    testAll(2);
}
