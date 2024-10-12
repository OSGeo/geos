//
// Test Suite for C-API GEOSCoordSeq

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "capi_test_utils.h"

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capigeoscoordseq_data : public capitest::utility {

    GEOSCoordSequence* cs_;

    test_capigeoscoordseq_data() : cs_(nullptr)
    {
    }

    ~test_capigeoscoordseq_data()
    {
        GEOSCoordSeq_destroy(cs_);
        cs_ = nullptr;
    }

};

typedef test_group<test_capigeoscoordseq_data> group;
typedef group::object object;

group test_capigeoscoordseq_group("capi::GEOSCoordSeq");

//
// Test Cases
//

// Test construction and fill of a 3D CoordinateSequence
template<>
template<>
void object::test<1>
()
{
    cs_ = GEOSCoordSeq_create(5, 3);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 5u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals(dims, 3u);

    for(unsigned int i = 0; i < 5; ++i) {
        double x = i * 10;
        double y = i * 10 + 1;
        double z = i * 10 + 2;

        GEOSCoordSeq_setX(cs_, i, x);
        GEOSCoordSeq_setY(cs_, i, y);
        GEOSCoordSeq_setZ(cs_, i, z);

        double xcheck, ycheck, zcheck;
        ensure(0 != GEOSCoordSeq_getX(cs_, i, &xcheck));
        ensure(0 != GEOSCoordSeq_getY(cs_, i, &ycheck));
        ensure(0 != GEOSCoordSeq_getZ(cs_, i, &zcheck));

        ensure_equals(xcheck, x);
        ensure_equals(ycheck, y);
        ensure_equals(zcheck, z);
    }
}

// Test not swapped setX/setY calls (see bug #133, fixed)
template<>
template<>
void object::test<2>
()
{
    cs_ = GEOSCoordSeq_create(1, 3);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals(dims, 3u);

    double x = 10;
    double y = 11;
    double z = 12;

    // X, Y, Z
    GEOSCoordSeq_setX(cs_, 0, x);
    GEOSCoordSeq_setY(cs_, 0, y);
    GEOSCoordSeq_setZ(cs_, 0, z);

    double xcheck, ycheck, zcheck;
    ensure(0 != GEOSCoordSeq_getY(cs_, 0, &ycheck));
    ensure(0 != GEOSCoordSeq_getX(cs_, 0, &xcheck));
    ensure(0 != GEOSCoordSeq_getZ(cs_, 0, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure_equals(zcheck, z);
}

// Test not swapped setOrdinate calls (see bug #133, fixed)
template<>
template<>
void object::test<3>
()
{
    cs_ = GEOSCoordSeq_create(1, 3);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals(dims, 3u);

    double x = 10;
    double y = 11;
    double z = 12;

    // X, Y, Z
    ensure(0 != GEOSCoordSeq_setOrdinate(cs_, 0, 0, x));
    ensure(0 != GEOSCoordSeq_setOrdinate(cs_, 0, 1, y));
    ensure(0 != GEOSCoordSeq_setOrdinate(cs_, 0, 2, z));

    double xcheck, ycheck, zcheck;
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 1, &ycheck));
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 0, &xcheck));
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 2, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure_equals(zcheck, z);

    // correct error on wrong ordinate index
    ensure(0 == GEOSCoordSeq_setOrdinate(cs_, 0, 37, z));
}

// Test swapped setX calls (see bug #133, fixed)
template<>
template<>
void object::test<4>
()
{
    cs_ = GEOSCoordSeq_create(1, 3);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals(dims, 3u);

    double x = 10;
    double y = 11;
    double z = 12;

    // Y, X, Z
    GEOSCoordSeq_setY(cs_, 0, y);
    GEOSCoordSeq_setX(cs_, 0, x);
    GEOSCoordSeq_setZ(cs_, 0, z);

    double xcheck, ycheck, zcheck;
    ensure(0 != GEOSCoordSeq_getY(cs_, 0, &ycheck));
    ensure(0 != GEOSCoordSeq_getX(cs_, 0, &xcheck));
    ensure(0 != GEOSCoordSeq_getZ(cs_, 0, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure_equals(zcheck, z);
}

// Test swapped setOrdinate calls (see bug #133, fixed)
template<>
template<>
void object::test<5>
()
{
    cs_ = GEOSCoordSeq_create(1, 3);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals(dims, 3u);

    double x = 10;
    double y = 11;
    double z = 12;

    // Y, X, Z
    GEOSCoordSeq_setOrdinate(cs_, 0, 1, y);
    GEOSCoordSeq_setOrdinate(cs_, 0, 0, x);
    GEOSCoordSeq_setOrdinate(cs_, 0, 2, z);

    double xcheck, ycheck, zcheck;
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 1, &ycheck));
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 0, &xcheck));
    ensure(0 != GEOSCoordSeq_getOrdinate(cs_, 0, 2, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure_equals(zcheck, z);
}

// Test getDimensions call (see bug #135)
template<>
template<>
void object::test<6>
()
{
    cs_ = GEOSCoordSeq_create(1, 2);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals(size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));

    // The dimension passed to GEOSCoordSeq_create()
    // is a request for a minimum, not a strict mandate
    // for changing actual size.
    //
    ensure(dims >= 2u);

}

template<>
template<>
void object::test<7>
()
{
    // ccw orientation
    cs_ = GEOSCoordSeq_create(4, 2);
    char ccw;

    GEOSCoordSeq_setX(cs_, 0, 0);
    GEOSCoordSeq_setY(cs_, 0, 0);

    GEOSCoordSeq_setX(cs_, 1, 1);
    GEOSCoordSeq_setY(cs_, 1, 0);

    GEOSCoordSeq_setX(cs_, 2, 1);
    GEOSCoordSeq_setY(cs_, 2, 1);

    GEOSCoordSeq_setX(cs_, 3, 0);
    GEOSCoordSeq_setY(cs_, 3, 0);

    ensure_equals(GEOSCoordSeq_isCCW(cs_, &ccw), 1);
    ensure(ccw != 0);
}

template<>
template<>
void object::test<8>
()
{
    // cw orientation
    cs_ = GEOSCoordSeq_create(4, 2);
    char ccw;

    GEOSCoordSeq_setX(cs_, 0, 0);
    GEOSCoordSeq_setY(cs_, 0, 0);

    GEOSCoordSeq_setX(cs_, 1, 1);
    GEOSCoordSeq_setY(cs_, 1, 1);

    GEOSCoordSeq_setX(cs_, 2, 1);
    GEOSCoordSeq_setY(cs_, 2, 0);

    GEOSCoordSeq_setX(cs_, 3, 0);
    GEOSCoordSeq_setY(cs_, 3, 0);

    ensure_equals(GEOSCoordSeq_isCCW(cs_, &ccw), 1);
    ensure(!ccw);
}

template<>
template<>
void object::test<9>
()
{
    // no orientation
    cs_ = GEOSCoordSeq_create(3, 2);
    char ccw;

    GEOSCoordSeq_setX(cs_, 0, 0);
    GEOSCoordSeq_setY(cs_, 0, 0);

    GEOSCoordSeq_setX(cs_, 1, 1);
    GEOSCoordSeq_setY(cs_, 1, 1);

    GEOSCoordSeq_setX(cs_, 2, 1);
    GEOSCoordSeq_setY(cs_, 2, 0);

    ensure_equals(GEOSCoordSeq_isCCW(cs_, &ccw), 1);
    ensure(!ccw);
}

template<>
template<>
void object::test<10>
()
{
    // no orientation
    cs_ = GEOSCoordSeq_create(0, 0);
    char ccw;

    ensure_equals(GEOSCoordSeq_isCCW(cs_, &ccw), 1);
    ensure(!ccw);
}

template<>
template<>
void object::test<11>
()
{
    cs_ = GEOSCoordSeq_create(1, 2);

    unsigned int size;
    unsigned int dims;

    ensure(0 != GEOSCoordSeq_getSize(cs_, &size));
    ensure_equals("Seq has expected size", size, 1u);

    ensure(0 != GEOSCoordSeq_getDimensions(cs_, &dims));
    ensure_equals("seq has expected dim", dims, 2u);

    double x = 10;
    double y = 11;

    GEOSCoordSeq_setXY(cs_, 0, x, y);

    double xcheck, ycheck, zcheck;
    ensure(0 != GEOSCoordSeq_getXY(cs_, 0, &xcheck, &ycheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);

    // Calling getXYZ on a 2D seq gets you NaN for Z
    ensure(0 != GEOSCoordSeq_getXYZ(cs_, 0, &xcheck, &ycheck, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure("z is NaN on 2D seq", std::isnan(zcheck));

    // Calling setXYZ on a 2D seq coerces to 3D
    double z = 12;
    GEOSCoordSeq_setXYZ(cs_, 0, x, y, z);

    ensure(0 != GEOSCoordSeq_getXYZ(cs_, 0, &xcheck, &ycheck, &zcheck));

    ensure_equals(xcheck, x);
    ensure_equals(ycheck, y);
    ensure_equals(zcheck, z);
}

// test 2D from/to buffer
template<>
template<>
void object::test<12>()
{
    unsigned int N = 10;
    unsigned int dim = 2;
    std::vector<double> values(N * dim);
    for (size_t i = 0; i < values.size(); i++) {
        values[i] = static_cast<double>(i);
    }

    cs_ = GEOSCoordSeq_copyFromBuffer(values.data(), N, false, false);

    double x, y;
    ensure(GEOSCoordSeq_getXY(cs_, 0, &x, &y));
    ensure_equals(x, 0.0);
    ensure_equals(y, 1.0);

    ensure(GEOSCoordSeq_getXY(cs_, N - 1, &x, &y));
    ensure_equals(x, static_cast<double>(N-1)*2);
    ensure_equals(y, static_cast<double>(N-1)*2 + 1);

    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, dim);

    std::vector<double> out3(N * 3);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out3.data(), true, false));
    ensure_equals(out3[0], values[0]); // X1
    ensure_equals(out3[1], values[1]); // Y1
    ensure(std::isnan(out3[2])); // Z1
    ensure_equals(out3[3], values[2]); // X2

    std::vector<double> out2(N * 2);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out2.data(), false, false));
    ensure(out2 == values);
}

// test 3D from/to buffer
template<>
template<>
void object::test<13>()
{
    unsigned int N = 10;
    unsigned int dim = 3;
    std::vector<double> values(N * dim);
    for (size_t i = 0; i < values.size(); i++) {
        values[i] = static_cast<double>(i);
    }

    cs_ = GEOSCoordSeq_copyFromBuffer(values.data(), N, true, false);

    double x, y, z;
    ensure(GEOSCoordSeq_getXYZ(cs_, 0, &x, &y, &z));
    ensure_equals(x, 0.0);
    ensure_equals(y, 1.0);
    ensure_equals(z, 2.0);

    ensure(GEOSCoordSeq_getXYZ(cs_, N - 1, &x, &y, &z));
    ensure_equals(x, static_cast<double>(N-1)*3);
    ensure_equals(y, static_cast<double>(N-1)*3 + 1);
    ensure_equals(z, static_cast<double>(N-1)*3 + 2);

    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, dim);

    std::vector<double> out3(N * 3);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out3.data(), true, false));
    ensure(out3 == values);

    // Copy to 2D buffer
    std::vector<double> out2(N * 2);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out2.data(), false, false));
    ensure_equals(out2[0], values[0]); // X1
    ensure_equals(out2[1], values[1]); // Y1
    ensure_equals(out2[2], values[3]); // X2
    ensure_equals(out2[3], values[4]); // Y2

    // Copy to XYZM buffer
    std::vector<double> out4(N * 4);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out4.data(), true, true));
    for (size_t i = 0, j = 0; i < out4.size(); i++) {
        if (i % 4 == 3) {
            // M should be set to NaN
            ensure(std::isnan(out4[i]));
        } else {
            // XYZ should be copied in
            ensure_equals(out4[i], values[j++]);
        }
    }
}

// test 2D from/to arrays
template<>
template<>
void object::test<14>()
{
    unsigned int N = 10;
    unsigned int dim = 2;
    std::vector<double> x(N);
    std::vector<double> y(N);
    for (size_t i = 0; i < N; i++) {
        x[i] = static_cast<double>(i);
        y[i] = static_cast<double>(2 * i);
    }

    cs_ = GEOSCoordSeq_copyFromArrays(x.data(), y.data(), nullptr, nullptr, N);
    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, dim);

    double cx, cy;
    ensure(GEOSCoordSeq_getXY(cs_, 0, &cx, &cy));
    ensure_equals(cx, 0.0);
    ensure_equals(cy, 0.0);

    ensure(GEOSCoordSeq_getXY(cs_, 1, &cx, &cy));
    ensure_equals(cx, 1.0);
    ensure_equals(cy, 2.0);

    ensure(GEOSCoordSeq_getXY(cs_, 2, &cx, &cy));
    ensure_equals(cx, 2.0);
    ensure_equals(cy, 4.0);

    std::vector<double> xout(N), yout(N), zout(N);
    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), nullptr, nullptr));
    ensure(x == xout);
    ensure(y == yout);

    // providing z vector to 2D coordinate sequence populates it with NaN
    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), zout.data(), nullptr));
    ensure(std::all_of(zout.begin(), zout.end(), [](double z) {
        return std::isnan(z);
    }));
}

// test 3D from/to arrays
template<>
template<>
void object::test<15>()
{
    unsigned int N = 10;
    unsigned int dim = 3;
    std::vector<double> x(N);
    std::vector<double> y(N);
    std::vector<double> z(N);
    for (size_t i = 0; i < N; i++) {
        x[i] = static_cast<double>(i);
        y[i] = static_cast<double>(2 * i);
        z[i] = static_cast<double>(3 * i);
    }

    cs_ = GEOSCoordSeq_copyFromArrays(x.data(), y.data(), z.data(), nullptr, N);
    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, dim);

    double cx, cy, cz;
    ensure(GEOSCoordSeq_getXYZ(cs_, 0, &cx, &cy, &cz));
    ensure_equals(cx, 0.0);
    ensure_equals(cy, 0.0);
    ensure_equals(cz, 0.0);

    ensure(GEOSCoordSeq_getXYZ(cs_, 1, &cx, &cy, &cz));
    ensure_equals(cx, 1.0);
    ensure_equals(cy, 2.0);
    ensure_equals(cz, 3.0);

    ensure(GEOSCoordSeq_getXYZ(cs_, 2, &cx, &cy, &cz));
    ensure_equals(cx, 2.0);
    ensure_equals(cy, 4.0);
    ensure_equals(cz, 6.0);

    std::vector<double> xout(N), yout(N), zout(N), mout(N);
    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), nullptr, nullptr));
    ensure(x == xout);
    ensure(y == yout);

    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), zout.data(), mout.data()));
    ensure(x == xout);
    ensure(y == yout);
    ensure(z == zout);
    ensure(std::all_of(mout.begin(), mout.end(), [](double mval) { return std::isnan(mval); }));
}

// test 3DM from/to buffer
template<>
template<>
void object::test<16>()
{
    unsigned int N = 10;
    unsigned int dim = 3;
    std::vector<double> values(N * dim);
    for (size_t i = 0; i < values.size(); i++) {
        values[i] = static_cast<double>(i);
    }

    cs_ = GEOSCoordSeq_copyFromBuffer(values.data(), N, false, true);

    // XYM buffer produces 3D coordinate sequence
    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, 3u);

    // Check first coordinate
    double x, y, z;
    ensure(GEOSCoordSeq_getXYZ(cs_, 0, &x, &y, &z));
    ensure_equals(x, 0.0);
    ensure_equals(y, 1.0);
    ensure(std::isnan(z));

    // Check last coordinate
    ensure(GEOSCoordSeq_getXYZ(cs_, N - 1, &x, &y, &z));
    ensure_equals(x, static_cast<double>(N-1)*3);
    ensure_equals(y, static_cast<double>(N-1)*3 + 1);
    ensure(std::isnan(z));

    // Copy to 2D buffer
    std::vector<double> out2(N * 2);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out2.data(), false, false));
    ensure_equals(out2[0], values[0]); // X1
    ensure_equals(out2[1], values[1]); // Y1
    ensure_equals(out2[2], values[3]); // X2
    ensure_equals(out2[3], values[4]); // Y2

    // Copy to XYM buffer
    std::vector<double> out3m(N * 3);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out3m.data(), false, true));
    for (size_t i =0; i < values.size(); i++) {
        ensure_equals(out3m[i], values[i]);
    }
}

// test 4D from/to buffer
template<>
template<>
void object::test<17>()
{
    unsigned int N = 10;
    unsigned int dim = 4;
    std::vector<double> values(N * dim);
    for (size_t i = 0; i < values.size(); i++) {
        values[i] = static_cast<double>(i);
    }

    cs_ = GEOSCoordSeq_copyFromBuffer(values.data(), N, true, true);

    // XYZM buffer creates a 4D coordinate sequence
    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals(dim_out, 4u);

    // Check first coordinate
    double x, y, z;
    ensure(GEOSCoordSeq_getXYZ(cs_, 0, &x, &y, &z));
    ensure_equals(x, 0.0);
    ensure_equals(y, 1.0);
    ensure_equals(z, 2.0);

    // Check last coordinate
    ensure(GEOSCoordSeq_getXYZ(cs_, N - 1, &x, &y, &z));
    ensure_equals(x, static_cast<double>(N-1)*4);
    ensure_equals(y, static_cast<double>(N-1)*4 + 1);
    ensure_equals(z, static_cast<double>(N-1)*4 + 2);

    // Copy to 4D buffer
    std::vector<double> out4d(N * 4);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out4d.data(), true, true));
    ensure_equals("X1", out4d[0], values[0]);
    ensure_equals("Y1", out4d[1], values[1]);
    ensure_equals("Z1", out4d[2], values[2]);
    ensure_equals("M1", out4d[3], values[3]);
    ensure_equals("X2", out4d[4], values[4]);
    ensure_equals("Y2", out4d[5], values[5]);
    ensure_equals("Z2", out4d[6], values[6]);
    ensure_equals("M2", out4d[7], values[7]);

    // Copy to XYZ buffer
    std::vector<double> out3d(N * 3);
    ensure(GEOSCoordSeq_copyToBuffer(cs_, out3d.data(), true, false));
    ensure_equals("X1", out3d[0], values[0]);
    ensure_equals("Y1", out3d[1], values[1]);
    ensure_equals("Z1", out3d[2], values[2]);
    ensure_equals("X2", out3d[3], values[4]);
    ensure_equals("Y2", out3d[4], values[5]);
    ensure_equals("Z2", out3d[5], values[6]);
}

// test error on invalid dim
template<>
template<>
void object::test<18>()
{
    ensure(!GEOSCoordSeq_create(0, 1));
    ensure(!GEOSCoordSeq_create(0, 5));
}

// test 3d values stored correctly in 4D seq
template<>
template<>
void object::test<19>()
{
    cs_ = GEOSCoordSeq_create(2, 4);
    GEOSCoordSeq_setXYZ(cs_, 0, 1, 2, 3);
    GEOSCoordSeq_setXYZ(cs_, 1, 4, 5, 6);

    double x, y, z;
    GEOSCoordSeq_getXYZ(cs_, 0, &x, &y, &z);
    ensure_equals(x, 1);
    ensure_equals(y, 2);
    ensure_equals(z, 3);

    GEOSCoordSeq_getXYZ(cs_, 1, &x, &y, &z);
    ensure_equals(x, 4);
    ensure_equals(y, 5);
    ensure_equals(z, 6);
}

// test 3DM from/to arrays
template<>
template<>
void object::test<20>()
{
    unsigned int N = 10;
    std::vector<double> x(N);
    std::vector<double> y(N);
    std::vector<double> m(N);
    for (size_t i = 0; i < N; i++) {
        x[i] = static_cast<double>(i);
        y[i] = static_cast<double>(2 * i);
        m[i] = static_cast<double>(3 * i);
    }

    cs_ = GEOSCoordSeq_copyFromArrays(x.data(), y.data(), nullptr, m.data(), N);
    unsigned int dim_out;
    ensure(GEOSCoordSeq_getDimensions(cs_, &dim_out));
    ensure_equals("XYM sequence has dimension = 3", dim_out, 3u);

    std::vector<double> xout(N), yout(N), zout(N), mout(N);
    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), nullptr, mout.data()));
    ensure(x == xout);
    ensure(y == yout);
    ensure(m == mout);

    ensure(GEOSCoordSeq_copyToArrays(cs_, xout.data(), yout.data(), zout.data(), mout.data()));
    ensure(x == xout);
    ensure(y == yout);
    ensure(std::all_of(zout.begin(), zout.end(), [](double zval) { return std::isnan(zval); }));
    ensure(m == mout);
}

template<>
template<>
void object::test<21>()
{
    GEOSContextHandle_t handle = GEOS_init_r();

    const unsigned int sz = 5;
    GEOSCoordSequence* seq1 = GEOSCoordSeq_create_r(handle, sz, 3);
    for (unsigned int i = 0; i < sz; i++)  {
        ensure(GEOSCoordSeq_setX_r(handle, seq1, i, (double) i + 1.0));
        ensure(GEOSCoordSeq_setY_r(handle, seq1, i, (double) i + 1.0));
        ensure(GEOSCoordSeq_setZ_r(handle, seq1, i, (double) i + 1.0));
    }

    GEOSCoordSequence* seq2 = GEOSCoordSeq_clone_r(handle, seq1);
    for (unsigned int i = 0; i < sz; i++)  {
        double x, y, z;
        ensure(GEOSCoordSeq_getX_r(handle, seq2, i, &x));
        ensure(GEOSCoordSeq_getY_r(handle, seq2, i, &y));
        ensure(GEOSCoordSeq_getZ_r(handle, seq2, i, &z));

        ensure_equals(x, (double) i + 1.0);
        ensure_equals(y, (double) i + 1.0);
        ensure_equals(z, (double) i + 1.0);
    }

    GEOSCoordSeq_destroy_r(handle, seq1);
    GEOSCoordSeq_destroy_r(handle, seq2);

    GEOS_finish_r(handle);
}

} // namespace tut
