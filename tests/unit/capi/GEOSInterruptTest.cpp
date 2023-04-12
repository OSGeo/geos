//
// Test Suite for C-API custom allocators

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <thread>

namespace tut {
//
// Test Group
//

// Common data used in test cases.
struct test_capiinterrupt_data {
    static int numcalls;
    static int maxcalls;
    static GEOSInterruptCallback* nextcb;

    static void
    notice(const char* fmt, ...)
    {
        std::fprintf(stdout, "NOTICE: ");

        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(stdout, fmt, ap);
        va_end(ap);

        std::fprintf(stdout, "\n");
    }

    test_capiinterrupt_data()
    {
    }

    ~test_capiinterrupt_data()
    {
    }

    static void
    interruptNow()
    {
        GEOS_interruptRequest();
    }

    static void
    countCalls()
    {
        ++numcalls;
        if(nextcb) {
            (*nextcb)();
        }
    }

    static void
    interruptAfterMaxCalls(void* data)
    {
        if (++*static_cast<int*>(data) >= maxcalls) {
            GEOS_interruptThread();
        }
    }

};

int test_capiinterrupt_data::numcalls = 0;
int test_capiinterrupt_data::maxcalls = 0;
GEOSInterruptCallback* test_capiinterrupt_data::nextcb = nullptr;

typedef test_group<test_capiinterrupt_data> group;
typedef group::object object;

group test_capiinterrupt_group("capi::GEOSInterrupt");

//
// Test Cases
//

/// Test interrupt callback being called
template<>
template<>
void object::test<1>
()
{
    numcalls = 0;

    initGEOS(notice, notice);

    GEOS_interruptRegisterCallback(countCalls);

    ensure_equals(numcalls, 0);

    GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING(0 0, 1 0)");

    ensure("GEOSGeomFromWKT failed", nullptr != geom1);

    GEOSGeometry* geom2 = GEOSBuffer(geom1, 1, 8);

    ensure("GEOSBufferWithStyle failed", nullptr != geom2);

    ensure("interrupt callback never called", numcalls > 0);

    GEOSGeom_destroy(geom1);
    GEOSGeom_destroy(geom2);

    GEOS_interruptRegisterCallback(nullptr); /* unregister */


    finishGEOS();
}

/// Test interrupt callback being called XXX
template<>
template<>
void object::test<2>
()
{
    numcalls = 0;

    initGEOS(notice, notice);

    GEOS_interruptRegisterCallback(countCalls);

    ensure_equals(numcalls, 0);

    GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING(0 0, 1 1, 2 2, 4 4)");
    GEOSGeometry* geom2 = GEOSGeomFromWKT("LINESTRING(0 0, 1 1.01, 4 4.001)");

    ensure("GEOSGeomFromWKT failed", nullptr != geom1);

    GEOSGeometry* geom3 = GEOSSnap(geom1, geom2, 0.1);

    ensure("GEOSSnap failed", nullptr != geom3);

    ensure("interrupt callback never called", numcalls > 0);

    GEOSGeom_destroy(geom1);
    GEOSGeom_destroy(geom2);
    GEOSGeom_destroy(geom3);

    GEOS_interruptRegisterCallback(nullptr); /* unregister */

    finishGEOS();
}

/// Test interrupt callback being NOT reset by initGEOS
template<>
template<>
void object::test<3>
()
{
    numcalls = 0;

    GEOS_interruptRegisterCallback(countCalls);

    initGEOS(notice, notice);

    ensure_equals(numcalls, 0);

    GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING(0 0, 1 0)");

    ensure("GEOSGeomFromWKT failed", nullptr != geom1);

    GEOSGeometry* geom2 = GEOSBuffer(geom1, 1, 8);

    ensure("GEOSBufferWithStyle failed", nullptr != geom2);

    ensure("interrupt callback never called", numcalls > 0);

    GEOSGeom_destroy(geom1);
    GEOSGeom_destroy(geom2);

    GEOS_interruptRegisterCallback(nullptr);

    finishGEOS();
}

/// Test interrupting from callback
template<>
template<>
void object::test<4>
()
{
    initGEOS(notice, notice);

    GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING(0 0, 1 0)");

    ensure("GEOSGeomFromWKT failed", nullptr != geom1);

    GEOS_interruptRegisterCallback(interruptNow);
    GEOSGeometry* geom2 = GEOSBuffer(geom1, 1, 8);
    ensure("GEOSBuffer wasn't interrupted", nullptr == geom2);
    GEOS_interruptRegisterCallback(nullptr);  /* unregister */

    // TODO: check the actual exception ? (sent to notice() callback)

    GEOSGeom_destroy(geom1);

    finishGEOS();
}

/// Test chaining interrupt callbacks
template<>
template<>
void object::test<5>
()
{
    numcalls = 0;

    initGEOS(notice, notice);

    GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING(0 0, 1 0)");

    ensure("GEOSGeomFromWKT failed", nullptr != geom1);

    GEOS_interruptRegisterCallback(interruptNow);
    nextcb = GEOS_interruptRegisterCallback(countCalls);
    GEOSGeometry* geom2 = GEOSBuffer(geom1, 1, 8);
    ensure("GEOSBuffer wasn't interrupted", nullptr == geom2);
    ensure_equals(numcalls, 1);
    GEOS_interruptRegisterCallback(nullptr);  /* unregister */
    nextcb = nullptr;

    GEOSGeom_destroy(geom1);

    finishGEOS();
}


// Test callback is thread-local
template<>
template<>
void object::test<6>
()
{
    maxcalls = 3;
    int calls_1 = 0;
    int calls_2 = 0;

    initGEOS(notice, notice);

    auto buffer = [](GEOSInterruptThreadCallback* cb, void* data) {
        GEOSGeometry* geom1 = GEOSGeomFromWKT("LINESTRING (0 0, 1 0)");

        GEOS_interruptRegisterThreadCallback(cb, data);

        GEOSGeometry* geom2 = GEOSBuffer(geom1, 1, 8);
        GEOSGeom_destroy(geom2);
        GEOSGeom_destroy(geom1);
    };

    std::thread t1(buffer, interruptAfterMaxCalls, &calls_1);
    std::thread t2(buffer, interruptAfterMaxCalls, &calls_2);

    t1.join();
    t2.join();

    ensure_equals(calls_1, maxcalls);
    ensure_equals(calls_2, maxcalls);

    finishGEOS();
}


} // namespace tut

