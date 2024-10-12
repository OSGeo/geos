//
// Test Suite for geos::util::UniqueCoordinateArrayFilter class.

// geos

#include <geos/profiler.h>
#include <geos/math/DD.h>

// tut
#include <tut/tut.hpp>
#include <utility.h>

using namespace geos::math;

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_dd_data {

    double eps;
    DD pi;
    DD e;


    void ensure_dd_equals(const char *str, const DD &d1, const DD &d2, double tolerance)
    {
        DD delta = DD::abs(d1 - d2);
        double diff = delta.doubleValue();
        ensure(str, diff <= tolerance);
    }

    void checkTrunc(const DD &x, const DD &expected)
    {
        DD trunc = DD::trunc(x);
        ensure("checkTrunc", trunc == expected);
    }

    void checkDeterminant(double x1, double y1, double x2, double y2, double expected, double errBound)
    {
        DD det = DD::determinant(x1, y1, x2, y2);
        //ensure_equals("1", Angle::angle(Coordinate(10, 0)), 0.0, TOL);
        ensure_dd_equals("checkDeterminant", det, DD(expected), errBound);
    }

    void checkDeterminantDD(double x1, double y1, double x2, double y2, double expected, double errBound)
    {
        DD det = DD::determinant(DD(x1), DD(y1), DD(x2), DD(y2));
        ensure_dd_equals("checkDeterminantDD", det, DD(expected), errBound);
    }

    void checkAddMult2(const DD &dd)
    {
        DD sum = dd + dd;
        DD prod = dd * DD(2.0);
        ensure_dd_equals("checkAddMult2", sum, prod, 0.0);
    }

    void checkMultiplyDivide(const DD &a, const DD &b, double errBound)
    {
        DD a2 = (a * b) / b;
        ensure_dd_equals("checkMultiplyDivide", a, a2, errBound);
    }

    void checkDivideMultiply(const DD &a, const DD &b, double errBound)
    {
        DD a2 = (a / b) * b;
        ensure_dd_equals("checkDivideMultiply", a, a2, errBound);
    }

    /**
    * Computes (a+b)^2 in two different ways and compares the result.
    * For correct results, a and b should be integers.
    */
    void checkBinomialSquare(double a, double b)
    {
        // binomial square
        DD add(a);
        DD bdd(b);
        DD aPlusb = add + bdd;
        DD abSq = aPlusb * aPlusb;

        // expansion
        DD a2dd = add * add;
        DD b2dd = bdd * bdd;
        DD ab = add * bdd;
        DD sum = b2dd + ab + ab;
        DD diff = abSq - a2dd;
        DD delta = diff - sum;

        ensure("isSame", diff == sum);
        ensure("isDeltaZero", delta.isZero());
    }

    void checkBinomial2(double a, double b)
    {
        // binomial product
        DD add(a);
        DD bdd(b);
        DD aPlusb = add + bdd;
        DD aSubb = add - bdd;
        DD abProd = aPlusb * aSubb;

        // expansion
        DD a2dd = add * add;
        DD b2dd = bdd * bdd;

        // this should equal b^2
        DD diff = (abProd - a2dd).negate();
        DD delta = diff - b2dd;

        ensure("isSame", diff == b2dd);
        ensure("isDeltaZero", delta.isZero());
    }

    void checkReciprocal(double x, double errBound)
    {
        DD xdd(x);
        DD rr = xdd.reciprocal().reciprocal();
        double err = (xdd - rr).doubleValue();
        ensure("checkReciprocal", err <= errBound);
    }

    DD slowPow(const DD &x, int exp)
    {
        if (exp == 0)
            return DD(1.0);

        int n = std::abs(exp);
        // MD - could use binary exponentiation for better precision & speed
        DD pow(x);
        for (int i = 1; i < n; i++) {
            pow = pow * x;
        }
        if (exp < 0) {
            return pow.reciprocal();
        }
        return pow;
    }

    void checkPow(double x, int exp, double errBound)
    {
        DD xdd(x);
        DD pow = DD::pow(xdd, exp);
        DD pow2 = slowPow(xdd, exp);
        double err = (pow - pow2).doubleValue();
        ensure("checkPow", err <= errBound);
    }


    DD arctan(DD x)
    {
        DD t = x;
        DD t2 = t*t;
        DD at(0.0);
        DD two(2.0);
        int k = 0;
        DD d(1.0);
        int sign = 1;
        while (t.doubleValue() > eps) {
            k = k + 1;
            if (sign < 0)
                at = at - (t / d);
            else
                at = at + (t / d);

            d = d + two;
            t = t * t2;
            sign = -sign;
        }
        return at;
    }

    /**
     * Uses Taylor series to compute e
     *
     * e = 1 + 1 + 1/2! + 1/3! + 1/4! + ...
     */
    DD computeEByTaylorSeries()
    {
        DD s(2.0);
        DD t(1.0);
        double n = 1.0;
        int i = 0;
        while(t.doubleValue() > eps)
        {
            i = i + 1;
            n += 1.0;
            t = t / DD(n);
            s = s + t;
        }
        return s;
    }

    /**
     * Uses Machin's arctangent formula to compute Pi:
     *
     *    Pi / 4  =  4 arctan(1/5) - arctan(1/239)
     */

    DD computePiByMachine()
    {
        DD t1 = DD(1.0) / DD(5.0);
        DD t2 = DD(1.0) / DD(239.0);
        DD pie4 = (DD(4.0) * arctan(t1)) - arctan(t2);
        DD pie = DD(4.0) * pie4;
        return pie;
    }

    test_dd_data():
        eps(1.23259516440783e-32), /* = 2^-106 */
        pi(DD(3.141592653589793116e+00, 1.224646799147353207e-16)),
        e(DD(2.718281828459045091e+00, 1.445646891729250158e-16))
        {}

};

typedef test_group<test_dd_data> group;
typedef group::object object;

group test_dd_group("geos::math::DD");

//
// Test Cases
//

// Test PI calculation
template<>
template<>
void object::test<1>
()
{
    DD testPi = computePiByMachine();
    double err = std::abs((testPi - pi).doubleValue());
    // std::cout << "Difference from PI = " << err << std::endl;
    ensure("Test PI calculation", err < 8*eps);
}

// Test E calculation
template<>
template<>
void object::test<2>
()
{
    DD testE = computeEByTaylorSeries();
    double err = std::abs((testE - e).doubleValue());
    // std::cout << "Difference from E = " << err << std::endl;
    ensure("Test E calculation", err < eps);
}


// Test NaN
template<>
template<>
void object::test<3>
()
{
    DD nan = DD(1.0) / DD(0.0);
    ensure("isNan", nan.isNaN());
    ensure("isNan", (DD(1.0) * nan).isNaN());
}

// testAddMult2
template<>
template<>
void object::test<4>
()
{
    checkAddMult2(DD(3.0));
    checkAddMult2(DD(pi));
}


// testMultiplyDivide
template<>
template<>
void object::test<5>
()
{
    checkMultiplyDivide(DD(pi), DD(e), 1e-30);
    checkMultiplyDivide(DD(DD(2.0)*pi), DD(e), 1e-30);
    checkMultiplyDivide(DD(DD(0.5)*pi), DD(e), 1e-30);
    checkMultiplyDivide(DD(39.4), DD(10), 1e-30);
}


// testDivideMultiply
template<>
template<>
void object::test<6>
()
{
    checkDivideMultiply(DD(pi), DD(e), 1e-30);
    checkDivideMultiply(DD(39.4), DD(10), 1e-30);
}


// testTrunc
template<>
template<>
void object::test<7>
()
{
    checkTrunc(DD(1e16) - DD(1), DD(1e16) - DD(1));
    // the appropriate error bound is determined empirically
    checkTrunc(DD(pi), DD(3));
    checkTrunc(DD(999.999), DD(999));

    checkTrunc(DD(e).negate(), DD(-2));
    checkTrunc(DD(-999.999), DD(-999));
}

// testPow
template<>
template<>
void object::test<8>
()
{
    checkPow(0, 3, 16 * eps);
    checkPow(0, 3, 16 * eps);
    checkPow(14, 3, 16 * eps);
    checkPow(3, -5, 16 * eps);
    checkPow(-3, 5, 16 * eps);
    checkPow(-3, -5, 16 * eps);
    checkPow(0.12345, -5, 1e5 * eps);
}


// testReciprocal
template<>
template<>
void object::test<9>
()
{
    checkReciprocal(3.0, 0);
    checkReciprocal(99.0, 1e-29);
    checkReciprocal(999.0, 0);
    checkReciprocal(314159269.0, 0);
}

// testDeterminant
template<>
template<>
void object::test<10>
()
{
    checkDeterminant(3, 8, 4, 6, -14, 0);
    checkDeterminantDD(3, 8, 4, 6, -14, 0);
}

// testDeterminantRobust
template<>
template<>
void object::test<11>
()
{
    checkDeterminant(1.0e9, 1.0e9 - 1, 1.0e9 - 1, 1.0e9 - 2, -1, 0);
    checkDeterminantDD(1.0e9, 1.0e9 - 1, 1.0e9 - 1, 1.0e9 - 2, -1, 0);
}


// testBinom
template<>
template<>
void object::test<12>
()
{
    checkBinomialSquare(100.0, 1.0);
    checkBinomialSquare(1000.0, 1.0);
    checkBinomialSquare(10000.0, 1.0);
    checkBinomialSquare(100000.0, 1.0);
    checkBinomialSquare(1000000.0, 1.0);
    checkBinomialSquare(1e8, 1.0);
    checkBinomialSquare(1e10, 1.0);
    checkBinomialSquare(1e14, 1.0);
    // Following call will fail, because it requires 32 digits of precision
    // checkBinomialSquare(1e16, 1.0);

    checkBinomialSquare(1e14, 291.0);
    checkBinomialSquare(5e14, 291.0);
    checkBinomialSquare(5e14, 345291.0);
}

// testBinom2
template<>
template<>
void object::test<13>
()
{
    checkBinomial2(100.0, 1.0);
    checkBinomial2(1000.0, 1.0);
    checkBinomial2(10000.0, 1.0);
    checkBinomial2(100000.0, 1.0);
    checkBinomial2(1000000.0, 1.0);
    checkBinomial2(1e8, 1.0);
    checkBinomial2(1e10, 1.0);
    checkBinomial2(1e14, 1.0);

    checkBinomial2(1e14, 291.0);

    checkBinomial2(5e14, 291.0);
    checkBinomial2(5e14, 345291.0);
}



} // namespace tut

