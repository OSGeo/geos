//
// Test Suite for geos::operation::buffer::BufferOp class.

// tut
#include <tut/tut.hpp>
#include <utility.h>
// geos
#include <geos/operation/buffer/BufferOp.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/constants.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/Polygon.h>
#include <geos/algorithm/PointLocator.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/CoordinateSequence.h>
// std
#include <memory>
#include <string>

namespace tut {
//
// Test Group
//

// Common data used by tests
struct test_bufferop_data {
    const geos::geom::GeometryFactory& gf;
    geos::io::WKTReader wktreader;
    geos::io::WKTWriter wktwriter;
    int const default_quadrant_segments;

    typedef geos::geom::Geometry::Ptr GeomPtr;
    typedef std::unique_ptr<geos::geom::CoordinateSequence> CSPtr;

    test_bufferop_data()
        : gf(*geos::geom::GeometryFactory::getDefaultInstance())
        , wktreader(&gf)
        , default_quadrant_segments(geos::operation::buffer::BufferParameters::DEFAULT_QUADRANT_SEGMENTS)
    {
        ensure_equals(default_quadrant_segments, int(8));
    }

    void checkBufferEmpty(const std::string& wkt, double dist, bool isEmpty)
    {
        std::unique_ptr<Geometry> geom = wktreader.read(wkt);
        std::unique_ptr<Geometry> actual = geom->buffer(dist);
        ensure_equals(actual->isEmpty(), isEmpty);
    }

    void checkBuffer(const std::string& wkt, double dist, double tolerance, const std::string& wktExpected)
    {
        std::unique_ptr<Geometry> geom = wktreader.read(wkt);
        std::unique_ptr<Geometry> actual = geom->buffer(dist);
        std::unique_ptr<Geometry> expected = wktreader.read(wktExpected);
        ensure_equals_geometry(expected.get(), actual.get(), tolerance);
    }

private:
    // noncopyable
    test_bufferop_data(test_bufferop_data const& other) = delete;
    test_bufferop_data& operator=(test_bufferop_data const& rhs) = delete;
};

typedef test_group<test_bufferop_data> group;
typedef group::object object;

group test_bufferop_group("geos::operation::buffer::BufferOp");

//
// Test Cases
//
template<>
template<>
void object::test<1>
()
{
    using geos::operation::buffer::BufferOp;

    std::string wkt0("POINT(0 0)");
    GeomPtr g0(wktreader.read(wkt0));

    double const distance = 0.0;
    BufferOp op(g0.get());
    GeomPtr gBuffer = op.getResultGeometry(distance);

    ensure(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure_equals(gBuffer->getNumPoints(), std::size_t(0));
}

template<>
template<>
void object::test<2>
()
{
    using geos::operation::buffer::BufferOp;

    std::string wkt0("POINT(0 0)");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with default buffering parameters
    double const distance = 1.0;
    BufferOp op(g0.get());
    GeomPtr gBuffer = op.getResultGeometry(distance);

    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure_equals(gBuffer->getNumPoints(), 33u);
    auto coords = gBuffer->getCoordinates();
    ensure_equals(coords->getSize(), 33u);

    // Check four sides to check they are exactly on unit circle
    auto coord = coords->getAt(0);
    ensure_equals(coord.x, 1.0);
    ensure_equals(coord.y, 0.0);
    coord = coords->getAt(8);
    ensure_equals(coord.x, 0.0);
    ensure_equals(coord.y, -1.0);
    coord = coords->getAt(16);
    ensure_equals(coord.x, -1.0);
    ensure_equals(coord.y, 0.0);
    coord = coords->getAt(24);
    ensure_equals(coord.x, 0.0);
    ensure_equals(coord.y, 1.0);

}

template<>
template<>
void object::test<3>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt0("POINT(0 0)");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with custom parameters: 32 quadrant segments
    int const segments = 32;
    BufferParameters params(segments);

    BufferOp op(g0.get(), params);

    double const distance = 1.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);

    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(129));
}

template<>
template<>
void object::test<4>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's buffer.xml, test case #25
    std::string
    wkt0("MULTIPOLYGON(((708258.754920656 2402197.91172757,708257.029447455 2402206.56901508,708652.961095455 2402312.65463437,708657.068786251 2402304.6356364,708258.754920656 2402197.91172757)),((708653.498611049 2402311.54647056,708708.895756966 2402203.47250014,708280.326454234 2402089.6337791,708247.896591321 2402252.48269854,708367.379593851 2402324.00761653,708248.882609455 2402253.07294874,708249.523621829 2402244.3124463,708261.854734465 2402182.39086576,708262.818392579 2402183.35452387,708653.498611049 2402311.54647056)))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with custom parameters: 24 quadrant segments
    {
        int const segments = default_quadrant_segments * 3;
        BufferParameters params(segments);
        BufferOp op(g0.get(), params);
        double const distance = 0.0001;
        GeomPtr gBuffer = op.getResultGeometry(distance);
        ensure_not(gBuffer->isEmpty());
        ensure(gBuffer->isValid());
        ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
        ensure(gBuffer->getNumPoints() >= std::size_t(243));
    }

    // Buffer point with custom parameters: 32 quadrant segments
    {
        int const segments = default_quadrant_segments * 4;
        BufferParameters params(segments);
        BufferOp op(g0.get(), params);
        double const distance = 0.0001;
        GeomPtr gBuffer = op.getResultGeometry(distance);
        ensure_not(gBuffer->isEmpty());
        ensure(gBuffer->isValid());
        ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
        ensure(gBuffer->getNumPoints() >= std::size_t(317));
    }
}

template<>
template<>
void object::test<5>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's TestBufferExternal2.xml, test case #31
    std::string
    wkt0("POLYGON((167187.935985 527215.129985,167246.663985 527205.580985,167280.107985 527210.602985,167314.083985 527213.149985,167354.196985 527218.563985,167363.609985 527204.834985,167358.459985 527177.204985,167344.405985 527172.240985,167323.325985 527164.793985,167304.557985 527167.748985,167285.789985 527170.703985,167260.854985 527176.007985,167211.198985 527149.591985,167165.408985 527140.439985,167141.660985 527146.308985,167119.062985 527137.503985,167119.967985 527139.010985,167078.296985 527188.560985,167105.238985 527267.140985,167090.957985 527316.213985,167094.770985 527321.819985,167082.593985 527377.599985,167065.952985 527417.668985,167056.312985 527449.383985,167043.546985 527485.510985,167019.927985 527525.916985,166982.769985 527539.710985,166961.717985 527606.388985,166933.263985 527657.009985,166899.206985 527710.191985,166916.325985 527722.287985,166973.067985 527744.810985,166978.160985 527695.099985,167043.308985 527684.770985,167080.550985 527644.941985,167101.716985 527610.077985,167109.672985 527576.692985,167099.068985 527561.394985,167097.537985 527527.027985,167105.044985 527510.384985,167137.888985 527502.771985,167160.386985 527500.228985,167174.041985 527468.108985,167205.365985 527466.936985,167200.090985 527446.692985,167182.286985 527416.893985,167214.905985 527399.163985,167245.980985 527379.346985,167258.755985 527325.736985,167261.469985 527296.920985,167237.119985 527273.314985,167201.524985 527264.407985,167192.491985 527252.494985,167172.090985 527216.630985,167187.935985 527215.129985))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with quadrant segments value (x4)
    BufferParameters params(default_quadrant_segments * 4);
    BufferOp op(g0.get(), params);
    double const distance = -75.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);
    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(8));
}

template<>
template<>
void object::test<6>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's TestBufferExternal2.xml, test case #41
    std::string
    wkt0("POLYGON((148410.699985 522834.902985,148442.757985 522825.725985,148460.309985 522827.514985,148487.864985 522844.888985,148500.821985 522849.224985,148522.555985 522842.060985,148549.245985 522836.360985,148568.572985 522826.852985,148584.252985 522812.743985,148609.816985 522787.145985,148614.009985 522767.881985,148612.795985 522752.758985,148612.563985 522743.189985,148618.720985 522735.053985,148625.781585 522726.296485,148618.662985 522724.739985,148612.278985 522723.775985,148605.112985 522714.525985,148611.687985 522677.987985,148611.797985 522647.963985,148594.739985 522625.727985,148584.565985 522622.805985,148562.730985 522628.324985,148543.283985 522615.292985,148536.597985 522557.684985,148536.223985 522555.223985,148534.274985 522546.378985,148533.665985 522541.970985,148533.794985 522540.094985,148535.172985 522535.741985,148538.094985 522529.403985,148538.611985 522526.777985,148539.936985 522513.990985,148540.169985 522509.496985,148540.192985 522500.589985,148540.851985 522495.877985,148540.474985 522491.607985,148539.391985 522489.734985,148536.121985 522486.259985,148532.239985 522483.953985,148529.487985 522487.690985,148527.557985 522489.161985,148523.269985 522490.157985,148512.547985 522491.130985,148508.089985 522491.855985,148503.708985 522492.860985,148499.171985 522494.926985,148493.577985 522498.401985,148488.746985 522500.495985,148483.626985 522501.803985,148480.736985 522503.655985,148464.117985 522534.086985,148427.843985 522552.590985,148408.790985 522571.476985,148398.223985 522596.340985,148407.833985 522615.400985,148416.753985 522662.875985,148420.236985 522689.856985,148421.081985 522713.013985,148415.508985 522722.819985,148391.177985 522725.213985,148358.228985 522724.834985,148355.953985 522741.826985,148341.696985 522767.846985,148328.778985 522795.425985,148312.269985 522806.114985,148322.213985 522836.436985,148339.137985 522884.626985,148351.181985 522896.189985,148376.682985 522893.652985,148389.158985 522851.634985,148410.699985 522834.902985))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with quadrant segments value (x4)
    int const segments = default_quadrant_segments * 4;
    BufferParameters params(segments);
    BufferOp op(g0.get(), params);
    double const distance = -75.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);
    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(51));
}

template<>
template<>
void object::test<7>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's TestBufferExternal2.xml, test case #42
    std::string
    wkt0("POLYGON((150213.068985 524020.273985,150226.206985 524020.218985,150245.019985 524022.421985,150248.570985 524017.633985,150243.628985 523991.402985,150233.942985 523969.968985,150203.852985 523929.546985,150189.509985 523905.746985,150179.578985 523905.795985,150163.996985 523891.573985,150150.529985 523872.591985,150158.960985 523848.710985,150150.727985 523827.265985,150129.075985 523796.394985,150110.126985 523782.119985,150064.853985 523787.149985,150051.774985 523791.993985,150035.273985 523796.784985,150034.124985 523803.948985,150047.317985 523842.088985,150048.538985 523846.850985,150048.758985 523856.362985,150044.002985 523858.774985,150033.285985 523861.216985,150022.584985 523866.044985,150013.239985 523875.626985,150010.897985 523882.801985,150007.322985 523904.295985,150015.725985 523913.798985,150028.883985 523920.894985,150036.292985 523937.604985,150033.171985 523964.012985,150028.264985 524013.973985,150020.417985 524042.804985,150014.532985 524064.428985,150004.476985 524083.491985,149987.717985 524115.262985,149981.881985 524139.242985,149991.382985 524146.196985,150012.547985 524165.288985,150017.553385 524169.126585,150024.575985 524166.982985,150037.645985 524157.385985,150054.301985 524147.777985,150067.231985 524142.754985,150080.313985 524135.548985,150096.808985 524132.911985,150108.662985 524120.938985,150113.586985 524111.551985,150113.285985 524097.054985,150114.403985 524085.116985,150121.501985 524075.543985,150134.308985 524061.036985,150143.802985 524053.844985,150159.042985 524051.270985,150177.151985 524046.558985,150188.764985 524039.234985,150195.842985 524027.285985,150213.068985 524020.273985))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with quadrant segments value (x4)
    int const segments = default_quadrant_segments * 4;
    BufferParameters params(segments);
    BufferOp op(g0.get(), params);
    double const distance = -75.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);
    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(24));
}

template<>
template<>
void object::test<8>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's TestBufferExternal2.xml, test case #98
    std::string
    wkt0("POLYGON((146978.395985 514128.105985,146993.088985 514109.724985,146990.935985 514109.919985,146986.515985 514108.676985,146984.834985 514107.491985,146981.559985 514104.355985,146978.605985 514100.856985,146976.054985 514097.475985,146971.011985 514090.004985,146967.745985 514085.474985,146964.701985 514082.154985,146959.542985 514075.969985,146956.337985 514072.894985,146952.680985 514071.027985,146947.749985 514069.211985,146939.485985 514065.648985,146927.297985 514062.022985,146919.855985 514059.063985,146915.584985 514057.875985,146911.533985 514057.834985,146907.047985 514058.932985,146902.995985 514060.801985,146899.081985 514063.093985,146893.310985 514066.882985,146889.708985 514069.594985,146885.365985 514073.314985,146874.977985 514084.255985,146871.396985 514088.322985,146864.758985 514096.561985,146858.269985 514103.545985,146851.525985 514109.507985,146845.834985 514113.792985,146843.682985 514115.048985,146833.562985 514120.296985,146827.089985 514122.419985,146822.686985 514123.116985,146818.808985 514122.219985,146814.499985 514120.648985,146810.509985 514118.510985,146805.983985 514115.339985,146799.004985 514109.608985,146789.184985 514100.497985,146782.878985 514094.071985,146773.731985 514084.130985,146772.704985 514082.828985,146770.140985 514078.765985,146767.239985 514075.730985,146765.038985 514075.353985,146760.639985 514077.133985,146756.696985 514093.552985,146755.166985 514104.428985,146754.236985 514107.679985,146747.054985 514128.282985,146745.452985 514132.485985,146740.637985 514142.592985,146736.292985 514152.151985,146732.113985 514160.830985,146729.632985 514165.603985,146720.451985 514182.099985,146718.255985 514185.582985,146715.669985 514189.265985,146708.041985 514198.572985,146700.623985 514208.288985,146697.764985 514212.325985,146695.357985 514216.136985,146693.225985 514220.099985,146691.843985 514223.389985,146688.846985 514231.402985,146687.411985 514235.659985,146686.889985 514238.098985,146686.285985 514244.852985,146686.270985 514253.857985,146686.450985 514255.277985,146686.301985 514260.300985,146683.033985 514264.144985,146683.155985 514265.983985,146682.338985 514271.113985,146682.374985 514276.109985,146680.337985 514280.147985,146677.579985 514283.544985,146668.796985 514293.186985,146665.431985 514297.393985,146661.798985 514300.600985,146659.382985 514304.403985,146657.401985 514310.058985,146656.451985 514315.110985,146657.862985 514320.479985,146660.328985 514325.311985,146664.346985 514327.493985,146671.146985 514330.078985,146674.727985 514332.813985,146678.721985 514337.242985,146681.490985 514340.795985,146686.032985 514347.673985,146690.888985 514355.863985,146692.808985 514359.916985,146693.698985 514363.305985,146695.616985 514373.416985,146698.231985 514382.035985,146699.836985 514386.527985,146701.571985 514390.707985,146703.974985 514394.998985,146706.750985 514398.590985,146710.002985 514401.691985,146712.889985 514403.299985,146717.072985 514405.089985,146726.061985 514410.373985,146734.615985 514414.500985,146745.317985 514418.731985,146749.574985 514420.166985,146751.177985 514420.590985,146755.638985 514421.241985,146756.964985 514414.062985,146760.786985 514395.658985,146762.453985 514390.190985,146763.644985 514384.449985,146765.274985 514379.265985,146767.350985 514375.290985,146772.077985 514367.066985,146782.052985 514344.326985,146787.377985 514332.973985,146796.553985 514316.678985,146800.956985 514309.903985,146805.131985 514302.406985,146810.612985 514294.080985,146815.038985 514286.445985,146817.815985 514282.583985,146823.033985 514277.958985,146826.565985 514274.275985,146829.279985 514270.683985,146831.879985 514268.007985,146838.691985 514261.283985,146846.154985 514255.376985,146849.511985 514252.379985,146854.426985 514246.995985,146866.975985 514234.461985,146873.174985 514227.718985,146875.892985 514224.130985,146883.032985 514216.049985,146888.426985 514211.137985,146895.661985 514205.813985,146898.228985 514203.079985,146900.862985 514199.416985,146903.718985 514196.294985,146910.227985 514189.724985,146918.475985 514180.160985,146921.638985 514177.199985,146925.123985 514174.344985,146929.916985 514170.920985,146942.656985 514162.441985,146948.828985 514158.846985,146952.598985 514156.414985,146955.585985 514153.824985,146964.483985 514144.873985,146967.826985 514140.249985,146969.666985 514137.975985,146978.395985 514128.105985))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with quadrant segments value (x4)
    int const segments = default_quadrant_segments * 4;
    BufferParameters params(segments);
    BufferOp op(g0.get(), params);
    double const distance = -75.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);
    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(7));
}

template<>
template<>
void object::test<9>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // Geometry from XMLTester's TestBufferExternal2.xml, test case #99
    std::string
    wkt0("POLYGON((144130.790985 514542.589985,144136.106985 514541.268985,144140.618985 514541.819985,144146.580985 514542.271985,144155.308985 514545.689985,144159.779985 514546.073985,144165.774985 514546.027985,144170.614985 514545.336985,144174.457985 514542.248985,144180.591985 514535.602985,144185.031985 514531.945985,144188.930985 514529.699985,144202.240985 514526.170985,144204.522985 514525.434985,144208.647985 514523.675985,144209.506985 514523.064985,144212.582985 514519.733985,144214.710985 514516.736985,144217.305985 514512.269985,144225.821985 514492.742985,144230.222985 514479.998985,144238.048985 514458.756985,144239.749985 514453.779985,144242.237985 514445.130985,144246.840985 514428.052985,144247.696985 514425.401985,144249.538985 514420.578985,144253.385985 514411.927985,144260.058985 514394.218985,144264.745985 514384.187985,144266.213985 514379.927985,144267.402985 514375.585985,144266.909985 514372.870985,144264.565985 514368.375985,144255.527985 514363.949985,144249.852985 514362.074985,144245.934985 514359.844985,144246.620985 514355.223985,144248.164985 514351.697985,144250.544985 514347.859985,144251.479985 514346.801985,144258.417985 514340.953985,144264.816985 514336.013985,144266.807985 514330.309985,144266.470985 514325.834985,144264.754985 514315.479985,144263.754985 514311.093985,144261.914985 514304.532985,144260.268985 514300.367985,144258.258985 514296.998985,144255.314985 514293.564985,144253.224985 514291.582985,144249.714985 514288.669985,144245.918985 514286.308985,144244.887985 514285.843985,144240.548985 514284.624985,144234.199985 514283.756985,144225.189985 514283.542985,144216.921985 514283.099985,144200.074985 514283.128985,144195.587985 514283.291985,144186.779985 514284.886985,144177.834985 514285.878985,144169.973985 514286.948985,144165.504985 514287.444985,144158.597985 514287.500985,144150.714985 514288.363985,144146.268985 514289.065985,144138.749985 514290.519985,144129.998985 514292.618985,144122.836985 514294.931985,144118.663985 514296.619985,144116.254985 514315.702985,144113.154985 514338.329985,144110.932985 514350.449985,144109.642985 514359.358985,144103.633985 514384.235985,144100.698985 514392.568985,144099.732985 514399.419985,144098.307985 514417.516985,144097.345985 514425.834985,144096.877985 514434.147985,144095.446985 514450.289985,144095.470985 514467.498985,144095.977985 514472.854985,144095.698985 514478.235985,144090.849985 514512.499985,144086.867985 514532.967985,144086.837985 514540.297985,144083.986985 514558.486985,144082.564985 514573.165985,144082.617985 514579.549985,144083.019985 514582.853985,144084.070985 514587.218985,144088.382985 514601.055985,144090.700985 514599.014985,144094.407985 514596.448985,144099.296985 514594.867985,144103.775985 514594.417985,144104.772985 514593.932985,144108.205985 514590.960985,144109.332985 514588.480985,144110.651985 514584.097985,144115.058985 514573.308985,144115.953985 514570.340985,144117.115985 514565.480985,144117.981985 514561.064985,144120.043985 514555.351985,144121.899985 514551.257985,144123.475985 514548.300985,144126.738985 514544.570985,144130.790985 514542.589985))");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with quadrant segments value (x4)
    int const segments = default_quadrant_segments * 4;
    BufferParameters params(segments);
    BufferOp op(g0.get(), params);
    double const distance = -75.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);
    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(5));
}

// Test two ways of executing buffer operation
// Problems with BufferOp when using BufferParameters by Markus Meyer
// http://lists.osgeo.org/pipermail/geos-devel/2011-October/005507.html
template<>
template<>
void object::test<10>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    // TODO: Replace with Markus Meyer's test geometry once received
    std::string
    wkt0("POLYGON((144130.790985 514542.589985,144136.106985 514541.268985,144140.618985 514541.819985,144146.580985 514542.271985,144155.308985 514545.689985,144159.779985 514546.073985,144165.774985 514546.027985,144170.614985 514545.336985,144174.457985 514542.248985,144180.591985 514535.602985,144185.031985 514531.945985,144188.930985 514529.699985,144202.240985 514526.170985,144204.522985 514525.434985,144208.647985 514523.675985,144209.506985 514523.064985,144212.582985 514519.733985,144214.710985 514516.736985,144217.305985 514512.269985,144225.821985 514492.742985,144230.222985 514479.998985,144238.048985 514458.756985,144239.749985 514453.779985,144242.237985 514445.130985,144246.840985 514428.052985,144247.696985 514425.401985,144249.538985 514420.578985,144253.385985 514411.927985,144260.058985 514394.218985,144264.745985 514384.187985,144266.213985 514379.927985,144267.402985 514375.585985,144266.909985 514372.870985,144264.565985 514368.375985,144255.527985 514363.949985,144249.852985 514362.074985,144245.934985 514359.844985,144246.620985 514355.223985,144248.164985 514351.697985,144250.544985 514347.859985,144251.479985 514346.801985,144258.417985 514340.953985,144264.816985 514336.013985,144266.807985 514330.309985,144266.470985 514325.834985,144264.754985 514315.479985,144263.754985 514311.093985,144261.914985 514304.532985,144260.268985 514300.367985,144258.258985 514296.998985,144255.314985 514293.564985,144253.224985 514291.582985,144249.714985 514288.669985,144245.918985 514286.308985,144244.887985 514285.843985,144240.548985 514284.624985,144234.199985 514283.756985,144225.189985 514283.542985,144216.921985 514283.099985,144200.074985 514283.128985,144195.587985 514283.291985,144186.779985 514284.886985,144177.834985 514285.878985,144169.973985 514286.948985,144165.504985 514287.444985,144158.597985 514287.500985,144150.714985 514288.363985,144146.268985 514289.065985,144138.749985 514290.519985,144129.998985 514292.618985,144122.836985 514294.931985,144118.663985 514296.619985,144116.254985 514315.702985,144113.154985 514338.329985,144110.932985 514350.449985,144109.642985 514359.358985,144103.633985 514384.235985,144100.698985 514392.568985,144099.732985 514399.419985,144098.307985 514417.516985,144097.345985 514425.834985,144096.877985 514434.147985,144095.446985 514450.289985,144095.470985 514467.498985,144095.977985 514472.854985,144095.698985 514478.235985,144090.849985 514512.499985,144086.867985 514532.967985,144086.837985 514540.297985,144083.986985 514558.486985,144082.564985 514573.165985,144082.617985 514579.549985,144083.019985 514582.853985,144084.070985 514587.218985,144088.382985 514601.055985,144090.700985 514599.014985,144094.407985 514596.448985,144099.296985 514594.867985,144103.775985 514594.417985,144104.772985 514593.932985,144108.205985 514590.960985,144109.332985 514588.480985,144110.651985 514584.097985,144115.058985 514573.308985,144115.953985 514570.340985,144117.115985 514565.480985,144117.981985 514561.064985,144120.043985 514555.351985,144121.899985 514551.257985,144123.475985 514548.300985,144126.738985 514544.570985,144130.790985 514542.589985))");
    GeomPtr g0(wktreader.read(wkt0));
    ensure_equals(g0->getNumPoints(), std::size_t(112));

    double const distance = -75.0;
    int const segments = 16;

    BufferParameters params1(segments, BufferParameters::CAP_ROUND);
    BufferOp op1(g0.get(), params1);
    GeomPtr gBuffer1 = op1.getResultGeometry(distance);
    ensure_not(gBuffer1->isEmpty());
    ensure(gBuffer1->isValid());
    ensure_equals(gBuffer1->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer1->getNumPoints() >= std::size_t(5));

    GeomPtr gBuffer2(BufferOp::bufferOp(g0.get(), distance, segments, BufferParameters::CAP_ROUND));
    ensure_not(gBuffer2->isEmpty());
    ensure(gBuffer2->isValid());
    ensure_equals(gBuffer2->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer2->getNumPoints() >= std::size_t(5));

    ensure(gBuffer1->equals(gBuffer2.get()));
    ensure(gBuffer2->equals(gBuffer1.get()));
}

// Test for ticket #473
template<>
template<>
void object::test<11>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt0("\
MULTILINESTRING(  \
 (-22720.6801580484 130376.223341197, \
  -22620.6136206117 130339.222540348, \
  -22620.6133224902 130339.333510463), \
 (-22720.3807106115 130487.193473695, \
  -22620.3154956134 130450.192663993, \
  -22620.3151974850 130450.303634126), \
 (-22620.6133224902 130339.333510463, -22620.6127262471 130339.555450692),  \
 (-22620.1376011539 130450.303157004, -22620.3151974850 130450.303634126),  \
 (-22620.3151974850 130450.303634126, -22620.3146012281 130450.525574392),  \
 (-21480.3713729115 130150.471377565, \
  -21481.6134583498 130150.918429232, \
  -21482.5899891895 130151.031891269, \
  -21480.9946803241 130149.807142948),  \
 (-21477.6185334698 130150.464355720,\
  -21478.0611246018 130151.020338484,  \
  -21377.8977465929 130114.034129489)  \
)  \
      ");

    GeomPtr g0(wktreader.read(wkt0));

    BufferParameters params(8, BufferParameters::CAP_SQUARE,
                            BufferParameters::JOIN_MITRE,
                            1.0);
    const double distance = 5.0;
    BufferOp op(g0.get(), params);
    GeomPtr gBuffer = op.getResultGeometry(distance);

    // We're basically only interested an rough sense of a
    // meaningful result.
    ensure_equals(gBuffer->getNumPoints(), std::size_t(46));
    ensure_equals(int(gBuffer->getArea()), 3567);
}

template<>
template<>
void object::test<12>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt0("POINT(100 100)");
    GeomPtr g0(wktreader.read(wkt0));

    // Buffer point with custom parameters: 32 quadrant segments
    int const segments = 53;
    int expected_segments = 4 * segments + 1;
    BufferParameters params(segments);

    BufferOp op(g0.get(), params);

    double const distance = 80.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);

    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
    ensure(gBuffer->getNumPoints() >= std::size_t(expected_segments));
}

// Test for ticket #960
template<>
template<>
void object::test<13>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt0("MULTILINESTRING ((10 10, 20 20, 10 40), (40 40, 30 30, 40 20, 30 10))");

    GeomPtr g0(wktreader.read(wkt0));

    BufferParameters param;
    param.setEndCapStyle(geos::operation::buffer::BufferParameters::CAP_FLAT);
    param.setQuadrantSegments(6);

    BufferOp op(g0.get(), param);

    double const distance = 40.0;
    GeomPtr gBuffer = op.getResultGeometry(distance);

    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_POLYGON);
}

// Test for GEOSwift regression failure
template<>
template<>
void object::test<14>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt0("GEOMETRYCOLLECTION (POINT (1 2), MULTIPOINT ((1 2), (3 4)), LINESTRING (1 2, 3 4), MULTILINESTRING ((1 2, 3 4), (5 6, 7 8)), POLYGON ((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), MULTIPOLYGON (((2 2, -2 2, -2 -2, 2 -2, 2 2), (1 1, 1 -1, -1 -1, -1 1, 1 1)), ((7 2, 3 2, 3 -2, 7 -2, 7 2))))");

    GeomPtr g0(wktreader.read(wkt0));
    BufferOp op(g0.get());

    double const distance = 0.5;
    GeomPtr gBuffer = op.getResultGeometry(distance);

    // std::cout << wktwriter.write(gBuffer.get()) << std::endl;

    ensure_not(gBuffer->isEmpty());
    ensure(gBuffer->isValid());
    ensure_equals(gBuffer->getGeometryTypeId(), geos::geom::GEOS_MULTIPOLYGON);
}


// This now works since buffer ring orientation is changed to use signed-area test.
// testBowtiePolygonLargestAreaRetained
template<>
template<>
void object::test<15>
()
{
    std::string wkt0("POLYGON ((10 10, 50 10, 25 35, 35 35, 10 10))");
    GeomPtr g0(wktreader.read(wkt0));
    GeomPtr gresult = g0->buffer(0.0);
    std::string wkt1("POLYGON ((10 10, 30 30, 50 10, 10 10))");
    GeomPtr gexpected(wktreader.read(wkt1));
    ensure_equals_geometry(gresult.get(), gexpected.get());
}

// Test for https://trac.osgeo.org/geos/ticket/1101 - Non-empty negative buffer of 4-pt convex polygon
template<>
template<>
void object::test<16>
()
{
    std::string wkt0("POLYGON ((666360.09 429614.71, 666344.4 429597.12, 666358.47 429584.52, 666374.5 429602.33, 666360.09 429614.71))");
    checkBufferEmpty(wkt0, -9, false);
    checkBufferEmpty(wkt0, -10, true);
    checkBufferEmpty(wkt0, -15, true);
    checkBufferEmpty(wkt0, -18, true);
}

// Test for https://trac.osgeo.org/geos/ticket/1101 - Non-empty negative buffer of 5-pt convex polygon
template<>
template<>
void object::test<17>
()
{
    std::string wkt0("POLYGON ((6 20, 16 20, 21 9, 9 0, 0 10, 6 20))");
    checkBufferEmpty(wkt0, -8, false);
    checkBufferEmpty(wkt0, -8.6, true);
    checkBufferEmpty(wkt0, -9.6, true);
    checkBufferEmpty(wkt0, -11, true);
}

// Test for https://trac.osgeo.org/geos/ticket/1101 - Buffer of Polygon with hole with hole eroded
template<>
template<>
void object::test<18>
()
{
    std::string wkt0("POLYGON ((-6 26, 29 26, 29 -5, -6 -5, -6 26), (6 20, 16 20, 21 9, 9 0, 0 10, 6 20))");
    GeomPtr g0(wktreader.read(wkt0));

    GeomPtr result1 = g0->buffer( -8 );
    ensure( 0 == dynamic_cast<const geos::geom::Polygon*>(result1.get())->getNumInteriorRing() );

    GeomPtr result2 = g0->buffer( -8.6 );
    ensure( 0 == dynamic_cast<const geos::geom::Polygon*>(result2.get())->getNumInteriorRing() );

    GeomPtr result3 = g0->buffer( -9.6 );
    ensure( 0 == dynamic_cast<const geos::geom::Polygon*>(result2.get())->getNumInteriorRing() );

    GeomPtr result4 = g0->buffer( -11 );
    ensure( 0 == dynamic_cast<const geos::geom::Polygon*>(result2.get())->getNumInteriorRing() );
}

// Test for https://trac.osgeo.org/geos/ticket/1101 - Non-empty negative buffer of 5-pt convex polygon
template<>
template<>
void object::test<19>
()
{
    std::string wkt0("MULTIPOLYGON (((30 18, 14 0, 0 13, 16 30, 30 18)), ((180 210, 60 50, 154 6, 270 40, 290 130, 250 190, 180 210)))");
    GeomPtr g0(wktreader.read(wkt0));

    ensure( 2 == GeomPtr(g0->buffer( -9 ))->getNumGeometries() );
    ensure( 1 == GeomPtr(g0->buffer( -10 ))->getNumGeometries() );
    ensure( 1 == GeomPtr(g0->buffer( -15 ))->getNumGeometries() );
    ensure( 1 == GeomPtr(g0->buffer( -18 ))->getNumGeometries() );
}

// Test for buffer inverted ring check optimization
// See https://github.com/locationtech/jts/issues/876
template<>
template<>
void object::test<20>
()
{
    std::string wkt0("LINESTRING (-20 0, 0 20, 20 0, 0 -20, -20 0)");
    GeomPtr g0(wktreader.read(wkt0));

    GeomPtr result1 = g0->buffer( 70 );
    ensure( 0 == dynamic_cast<const geos::geom::Polygon*>(result1.get())->getNumInteriorRing() );
}

// Test for single-sided buffer
// See https://github.com/libgeos/geos/issues/665
template<>
template<>
void object::test<21>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt("LINESTRING (50 50, 150 150, 150 100, 150 0)");
    GeomPtr geom(wktreader.read(wkt));

    geos::operation::buffer::BufferParameters bp;
    bp.setSingleSided(true);
    geos::operation::buffer::BufferOp op(geom.get(), bp);

    std::unique_ptr<Geometry> result = op.getResultGeometry(-21);
    ensure_equals(int(result->getArea()), 5055);
}

// Another test for single-sided buffer
// See https://github.com/libgeos/geos/issues/665
template<>
template<>
void object::test<22>
()
{
    using geos::operation::buffer::BufferOp;
    using geos::operation::buffer::BufferParameters;

    std::string wkt("MULTILINESTRING((0 0,10 0),(20 0,30 0))");
    GeomPtr geom(wktreader.read(wkt));

    geos::operation::buffer::BufferParameters bp;
    bp.setSingleSided(true);
    geos::operation::buffer::BufferOp op(geom.get(), bp);

    std::unique_ptr<Geometry> result = op.getResultGeometry(-10);
    ensure_equals(result->getNumGeometries(), 2u);
    ensure_equals(result->getArea(), 200);
}

// Checks a bug in the inverted-ring-removal heuristic.
// See https://github.com/libgeos/geos/issues/984
template<>
template<>
void object::test<24>
()
{
    std::string wkt("MULTIPOLYGON (((833454.7163917861 6312507.405413097, 833455.3726665961 6312510.208920742, 833456.301153878 6312514.207390314, 833492.2432584754 6312537.770332065, 833493.0901320165 6312536.098774815, 833502.6580673696 6312517.561360772, 833503.9404352929 6312515.0542803425, 833454.7163917861 6312507.405413097)))");

    checkBuffer(wkt, -3.8, 0.1, 
        "POLYGON ((833490.79 6312532.27, 833498.15 6312518, 833459.97 6312512.07, 833490.79 6312532.27))");
    checkBuffer(wkt, -7, 0.1,
        "POLYGON ((833489.57 6312527.65, 833493.27 6312520.48, 833474.09 6312517.5, 833489.57 6312527.65))");
}

// Checks a bug in the inverted-ring-removal heuristic.
// See https://github.com/libgeos/geos/issues/984
template<>
template<>
void object::test<23>
()
{
    std::string wkt("POLYGON ((182719.04521570954238996 224897.14115349075291306, 182807.02887436276068911 224880.64421749324537814, 182808.47314301913138479 224877.25002362736267969, 182718.38701137207681313 224740.00115247094072402, 182711.82697281913715415 224742.08599378637154587, 182717.1393717635946814 224895.61432328051887453, 182719.04521570954238996 224897.14115349075291306))");

    checkBuffer(wkt, -5, 0.1, 
        "POLYGON ((182722 224891.5, 182802 224876.5, 182717 224747, 182722 224891.5))");
    checkBuffer(wkt, -30, 0.1,
        "POLYGON ((182745.98 224861.57, 182760.51 224858.84, 182745.07 224835.33, 182745.98 224861.57))");
}

} // namespace tut
