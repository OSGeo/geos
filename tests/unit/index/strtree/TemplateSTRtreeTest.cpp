#include <tut/tut.hpp>
// geos
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineSegment.h>
#include <geos/index/strtree/TemplateSTRtree.h>
#include <geos/index/ItemVisitor.h>
#include <geos/io/WKTReader.h>

using namespace geos;
using geos::index::strtree::TemplateSTRtree;
using geos::geom::Geometry;

namespace tut {
// dummy data, not used
struct test_templatestrtree_data {
    struct Grid {
        double x0 = 0;
        double y0 = 0;
        double dx = 1;
        double dy = 1;
        std::size_t nx = 10;
        std::size_t ny = 10;

        geom::Envelope getEnvelope() const {
            return {x0, x0+dx*static_cast<double>(nx),
                    y0, y0+dy*static_cast<double>(ny)};
        }
    };

    static std::vector<std::unique_ptr<geom::Point>> pointGrid(const Grid & grid) {
        std::vector<std::unique_ptr<geom::Point>> ret;

        auto gf = geom::GeometryFactory::create();
        for (std::size_t i = 0; i < grid.nx; ++i) {
            for (std::size_t j = 0; j < grid.ny; ++j) {
                geom::Coordinate c(grid.x0 + grid.dx*static_cast<double>(i),
                                   grid.y0 + grid.dy*static_cast<double>(j));
                auto pt = gf->createPoint(c);
                ret.emplace_back(std::move(pt));
            }
        }

        return ret;
    }

    static std::vector<std::unique_ptr<Geometry>> boxGrid(const Grid & grid) {
        std::vector<std::unique_ptr<Geometry>> ret;

        auto gf = geom::GeometryFactory::create();
        for (std::size_t i = 0; i < grid.nx; ++i) {
            for (std::size_t j = 0; j < grid.ny; ++j) {
                geom::CoordinateXY ll(grid.x0 + grid.dx*static_cast<double>(i),
                                      grid.y0 + grid.dy*static_cast<double>(j));
                geom::CoordinateXY ur(grid.x0 + grid.dx*static_cast<double>(i+1),
                                      grid.y0 + grid.dy*static_cast<double>(j+1));

                geom::Envelope env(ll, ur);

                auto poly = gf->toGeometry(&env);

                ret.emplace_back(std::move(poly));
            }
        }

        return ret;
    }

    template<typename TreeItemType, typename VectorItemType>
    static TemplateSTRtree<TreeItemType> makeTree(const std::vector<VectorItemType> & items) {
        TemplateSTRtree<TreeItemType> t(10);
        for (auto& g : items) {
            t.insert(g.get());
        }
        return t;
    }
};

using group = test_group<test_templatestrtree_data>;
using object = group::object;
group test_templatestrtree_group("geos::index::strtree::TemplateSTRtree");

//
// Test Cases
//

template<>
template<>
void object::test<1>() {
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 10;

    auto geoms = pointGrid(grid);
    auto tree = makeTree<const geom::Point*>(geoms);

    // Query by collecting items into a vector
    geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
    std::vector<const geom::Point*> matches;
    tree.query(qe, matches);
    ensure(matches.size() == 4);
}

template<>
template<>
void object::test<2>() {
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 10;

    auto geoms = pointGrid(grid);
    auto tree = makeTree<const geom::Point*>(geoms);

    // Query using visitor
    class SimpleTestVisitor : public index::ItemVisitor {
        public:
            std::size_t count;

            SimpleTestVisitor()
                : count(0)
                {}

            void
            visitItem(void* item) override
            {
                auto pt = static_cast<geom::Point*>(item);
                if (!pt->isEmpty())
                    count++;
            }
    };
    SimpleTestVisitor vis;
    geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
    tree.query(&qe, vis);
    ensure(vis.count == 4);
}

template<>
template<>
void object::test<3>
()
{
    Grid grid1, grid2;
    grid1.x0 = grid1.y0 = 0;
    grid1.dx = grid1.dy = 1;
    grid1.nx = grid1.ny = 10;

    grid2.x0 = grid2.y0 = 11;
    grid2.dx = grid2.dy = 1;
    grid2.nx = grid2.ny = 10;

    auto geoms1 = pointGrid(grid1);
    auto geoms2 = pointGrid(grid2);

    auto tree1 = makeTree<const geom::Point*>(geoms1);
    auto tree2 = makeTree<const geom::Point*>(geoms2);

    struct GeometryDistance {
        double operator()(const Geometry* a, const Geometry* b) {
            return a->distance(b);
        };
    };
    auto rslt = tree1.nearestNeighbour<GeometryDistance>(tree2);

    ensure_equals(rslt.first->getX(), 9.0);
    ensure_equals(rslt.first->getY(), 9.0);
    ensure_equals(rslt.second->getX(), 11.0);
    ensure_equals(rslt.second->getY(), 11.0);
}

template<>
template<>
void object::test<4>
()
{
    auto gf = geom::GeometryFactory::create();
    geos::io::WKTReader wkt(*gf);
    std::vector<std::unique_ptr<geom::Geometry>> geoms;
    geoms.emplace_back(wkt.read("LINESTRING(0 0, 10 10)"));
    geoms.emplace_back(wkt.read("LINESTRING(5 5, 15 15)"));
    geoms.emplace_back(wkt.read("LINESTRING(10 10, 20 20)"));
    geoms.emplace_back(wkt.read("LINESTRING(15 15, 25 25)"));

    auto tree = makeTree<Geometry*>(geoms);

    const std::size_t leaf_before = tree.getRoot()->getNumLeafNodes();
    const std::size_t all_before = tree.getRoot()->getNumNodes();
    ensure_equals(leaf_before, 4u);
    ensure_equals(all_before, 5u);

    tree.remove(geoms[3]->getEnvelopeInternal(), geoms[3].get());

    const std::size_t leaf_after = tree.getRoot()->getNumLeafNodes();
    const std::size_t all_after = tree.getRoot()->getNumNodes();
    ensure_equals(leaf_after, 3u);
    ensure_equals(all_after, 4u);
}

template<>
template<>
void object::test<5>
()
{
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 20;

    auto geoms = pointGrid(grid);

    // storing integers instead of geometry pointers
    TemplateSTRtree<size_t> tree;
    for (std::size_t i = 0; i < geoms.size(); i++) {
        tree.insert(*geoms[i]->getEnvelopeInternal(), i);
    }

    // Query into vector
    std::vector<size_t> hits;
    geom::Envelope queryEnv(2.5, 4.5, 2.5, 4.5);
    tree.query(queryEnv, hits);
    ensure_equals(hits.size(), 4u);

    // Get items in tree order
    {
        std::vector<size_t> orderedItems(tree.items().begin(), tree.items().end());
        ensure_equals(orderedItems.size(), geoms.size());
    }

    // Remove an item and get items in tree order
    {
        auto removed = tree.remove(grid.getEnvelope(), 17);
        ensure(removed);
        std::vector<size_t> orderedItems(tree.items().begin(), tree.items().end());
        ensure_equals(orderedItems.size(), geoms.size() - 1);
    }
}

template<>
template<>
void object::test<6>()
{
    TemplateSTRtree<geom::LineSegment> tree;

    for (double i = 0; i < 100; i += 1.0) {
        geom::Coordinate p0(i, i);
        geom::Coordinate p1(i + 1, i + 1);

        geom::LineSegment ls(p0, p1);
        geom::Envelope e(p0, p1);
        tree.insert(e, ls);
    }

    geom::Envelope qe(35.5, 38.5, 35.5, 38.5);
    std::vector<geom::LineSegment> hits;
    tree.query(qe, hits);

    ensure_equals(hits.size(), 4u);
}

template<>
template<>
void object::test<7>() {
    struct FloatBox {
        FloatBox(double p_xmin, double p_xmax, double p_ymin, double p_ymax) :
            xmin(static_cast<float>(p_xmin)),
            xmax(static_cast<float>(p_xmax)),
            ymin(static_cast<float>(p_ymin)),
            ymax(static_cast<float>(p_ymax)) {

            if (static_cast<double>(xmax) < p_xmax) {
                xmax = std::nextafter(xmax, std::numeric_limits<float>::infinity());
            }
            if (static_cast<double>(xmin) > p_xmin) {
                xmin = std::nextafter(xmin, -std::numeric_limits<float>::infinity());
            }
            if (static_cast<double>(ymax) < p_ymax) {
                ymax = std::nextafter(ymax, std::numeric_limits<float>::infinity());
            }
            if (static_cast<double>(ymin) > p_ymin) {
                ymin = std::nextafter(ymin, -std::numeric_limits<float>::infinity());
            }
        }

        void expandToInclude(const FloatBox & other) {
            xmin = std::min(xmin, other.xmin);
            xmax = std::max(xmax, other.xmax);
            ymin = std::min(ymin, other.ymin);
            ymax = std::max(ymax, other.ymax);
        }

        bool intersects(const FloatBox & other) const {
            return !(other.xmin > xmax ||
                     other.xmax < xmin ||
                     other.ymin > ymax ||
                     other.ymax < ymin);
        }

        float xmin;
        float xmax;
        float ymin;
        float ymax;
    };

    struct BoxTraits {
        using BoundsType = FloatBox;
        using TwoDimensional = std::true_type;

        // Quiet incorrect gcc warning about unused local typedef
        TwoDimensional doNothing() {
            return {};
        }

        static bool intersects(const BoundsType & a, const BoundsType & b) {
            return a.intersects(b);
        }

        static double getX(const BoundsType& a) {
            return 0.5 * static_cast<double>(a.xmin + a.xmax);
        }

        static double getY(const BoundsType& a) {
            return 0.5 * static_cast<double>(a.ymin + a.ymax);
        }

        static void expandToInclude(BoundsType& a, const BoundsType& b) {
            a.expandToInclude(b);
        }

        static bool isNull(const BoundsType & a) {
            (void) a;
            return false;
        }
    };

    TemplateSTRtree<geom::LineSegment, BoxTraits> tree;

    for (double i = 0; i < 100; i += 1.0) {
        geom::Coordinate p0(i, i);
        geom::Coordinate p1(i + 1, i + 1);

        geom::LineSegment ls(p0, p1);
        FloatBox e(p0.x, p1.x, p0.y, p1.y);
        tree.insert(e, ls);
    }

    FloatBox qe(35.5, 38.5, 35.5, 38.5);
    std::vector<geom::LineSegment> hits;
    tree.query(qe, hits);

    ensure_equals(hits.size(), 4u);
}

// Test visitor short-circuiting
template<>
template<>
void object::test<8>() {
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 10;

    auto geoms = pointGrid(grid);
    auto tree = makeTree<const geom::Point*>(geoms);

    std::vector<const geom::Point*> matches;
    auto visitor = [&matches](const geom::Point* pt) {
        matches.push_back(pt);
        return matches.size() < 2; // stop the query after we've found two items.
    };

    // Query by collecting items into a vector
    geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
    tree.query(qe, visitor);
    ensure(matches.size() == 2);
}


#if !defined(_MSC_VER) || _MSC_VER >= 1910
// Test bounds-and-item visitor. Method not defined in MSVC 2015.
template<>
template<>
void object::test<9>() {
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 10;

    auto geoms = pointGrid(grid);
    auto tree = makeTree<const geom::Point*>(geoms);

    // Collect the envelopes instead of the items
    std::vector<geom::Envelope> matches;
    auto visitor = [&matches](const geom::Envelope& e, const geom::Point* pt) {
        (void) pt;
        matches.push_back(e);
    };

    geom::Envelope qe(-0.5, 1.5, -0.5, 1.5);
    tree.query(qe, visitor);
    ensure(matches.size() == 4);
}
#endif

// Test short-circuiting by returning false from query callback
// https://github.com/libgeos/geos/issues/577
template<>
template<>
void object::test<10>() {
    TemplateSTRtree<void*> tree;

    for (int i = 0; i < 10; i++)
    {
        tree.insert(geos::geom::Envelope(i * 10, i * 10 + 10, i * 10, i * 10 + 10), nullptr);
    }

    for (int i = 0; i < 10; i++)
    {
        tree.insert(geos::geom::Envelope(i * 10, i * 10 + 10, -(i * 10), -(i * 10 + 10)), nullptr);
    }

    std::vector<const void*> hits;
    tree.query(geos::geom::Envelope(0, 1000, 0, 1000), [&hits](const void* ptr) {
        hits.push_back(ptr);
        return false;
    });

    ensure_equals(hits.size(), 1u);
}

template<>
template<>
void object::test<11>()
{
    Grid grid;
    grid.x0 = grid.y0 = 0;
    grid.dx = grid.dy = 1;
    grid.nx = grid.ny = 10;

    auto geoms = boxGrid(grid);
    auto tree = makeTree<const geom::Geometry*>(geoms);

    std::size_t pairCount1 = 0;
    std::size_t pairCount2 = 0;
    std::size_t pairCount3 = 0;

    for (const auto& g1 : geoms) {
        tree.query(*g1->getEnvelopeInternal(), [&g1, &pairCount1](const Geometry* g2) {
            if (g2 <= g1.get()) {
                return;
            }
            pairCount1++;
        });
    }

    // test with bool-returning callback
    tree.queryPairs([&pairCount2](const Geometry* g1, const Geometry* g2) {
        (void) g1; (void) g2;
        pairCount2++;
        return true;
    });

    // test with void callback
    tree.queryPairs([&pairCount3](const Geometry* g1, const Geometry* g2) {
        (void) g1; (void) g2;
        pairCount3++;
    });


    ensure_equals("same number of pairs visited (bool-returning callback)", pairCount1, pairCount2);
    ensure_equals("same number of pairs visited (void callback)", pairCount1, pairCount3);
}


} // namespace tut

