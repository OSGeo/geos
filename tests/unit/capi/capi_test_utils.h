
#pragma once

#include <tut/tut.hpp>

#include <geos_c.h>

#include <cstdarg>
#include <cstdio>
#include <cfenv>


namespace capitest {

    struct utility {
        GEOSWKTWriter* wktw_ = nullptr;
        GEOSGeometry* geom1_ = nullptr;
        GEOSGeometry* geom2_ = nullptr;
        GEOSGeometry* geom3_ = nullptr;
        GEOSGeometry* input_ = nullptr;
        GEOSGeometry* result_ = nullptr;
        GEOSGeometry* expected_ = nullptr;
        char* wkt_ = nullptr;
       char* str_ = nullptr;

        utility()
        {
            initGEOS(notice, notice);
            wktw_ = GEOSWKTWriter_create();
            GEOSWKTWriter_setRoundingPrecision(wktw_, 10);

            std::feclearexcept(FE_ALL_EXCEPT);
        }

        ~utility()
        {
            if (wktw_)     GEOSWKTWriter_destroy(wktw_);
            if (geom1_)    GEOSGeom_destroy(geom1_);
            if (geom2_)    GEOSGeom_destroy(geom2_);
            if (geom3_)    GEOSGeom_destroy(geom3_);
            if (input_)    GEOSGeom_destroy(input_);
            if (result_)   GEOSGeom_destroy(result_);
            if (expected_) GEOSGeom_destroy(expected_);
            if (wkt_)      GEOSFree(wkt_);
            if (str_)      GEOSFree(str_);
            finishGEOS();
        }


        static void notice(GEOS_PRINTF_FORMAT const char* fmt, ...) GEOS_PRINTF_FORMAT_ATTR(1, 2)
        {
            std::fprintf(stdout, "NOTICE: ");

            va_list ap;
            va_start(ap, fmt);
            #ifdef __MINGW32__
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
            #endif
            std::vfprintf(stdout, fmt, ap);
            #ifdef __MINGW32__
            #pragma GCC diagnostic pop
            #endif
            va_end(ap);

            std::fprintf(stdout, "\n");
        }

        GEOSGeometry*
        fromWKT(const char* wkt)
        {
            GEOSGeometry* g = GEOSGeomFromWKT(wkt);
            tut::ensure(g != nullptr);
            return g;
        }

        std::string
        toWKT(const GEOSGeometry* g)
        {
            char* wkt = GEOSWKTWriter_write(wktw_, g);
            std::string ret(wkt);
            GEOSFree(wkt);
            return ret;
        }

        void
        ensure_geometry_equals(GEOSGeometry* g1, GEOSGeometry* g2, double tolerance)
        {
            char rslt;
            if (g1 == nullptr || g2 == nullptr) {
                rslt = (g1 == nullptr && g2 == nullptr) ? 1 : 0;
            }
            else {
                GEOSNormalize(g1);
                GEOSNormalize(g2);
                rslt = GEOSEqualsExact(g1, g2, tolerance);
            }
            report_not_equal("ensure_equals_norm", g1, g2, tolerance, rslt);
            tut::ensure_equals("GEOSEqualsExact(g1, g2, tolerance)", rslt, 1);
        }

        void
        ensure_geometry_equals_exact(GEOSGeometry* g1, GEOSGeometry* g2, double tolerance)
        {
            char rslt;
            if (g1 == nullptr || g2 == nullptr) {
                rslt = (g1 == nullptr && g2 == nullptr) ? 1 : 0;
            }
            else {
                rslt = GEOSEqualsExact(g1, g2, tolerance);
            }
            report_not_equal("ensure_equals_exact", g1, g2, tolerance, rslt);
            tut::ensure_equals("GEOSEqualsExact(g1, g2, tolerance)", rslt, 1);
        }

        void
        ensure_geometry_equals_identical(GEOSGeometry* g1, GEOSGeometry* g2)
        {
            char rslt;
            if (g1 == nullptr || g2 == nullptr) {
                rslt = (g1 == nullptr && g2 == nullptr) ? 1 : 0;
            }
            else {
                rslt = GEOSEqualsIdentical(g1, g2);
            }
            report_not_equal("ensure_equals_identical", g1, g2, 1e-12, rslt);
            tut::ensure_equals("GEOSEqualsIdentical(g1, g2)", rslt, 1);
        }

        void
        ensure_geometry_equals(GEOSGeometry* g1, GEOSGeometry* g2)
        {
            return ensure_geometry_equals(g1, g2, 1e-12);
        }

        void
        ensure_geometry_equals(GEOSGeometry* g1, const char* g2str)
        {
            GEOSGeometry* g2 = GEOSGeomFromWKT(g2str);
            tut::ensure(g2 != nullptr);
            GEOSNormalize(g1);
            GEOSNormalize(g2);
            char rslt = GEOSEqualsExact(g1, g2, 1e-12);
            if (rslt != 1)
            {
                char* wkt1 = GEOSWKTWriter_write(wktw_, g1);
                char* wkt2 = GEOSWKTWriter_write(wktw_, g2);
                std::fprintf(stdout, "\n%s != %s\n", wkt1, wkt2);
                GEOSFree(wkt1);
                GEOSFree(wkt2);
            }
            GEOSGeom_destroy(g2);
            tut::ensure_equals("GEOSEqualsExact(g1, g2, 1e-12)", rslt, 1);
        }

        void
        report_not_equal(const char* tag, GEOSGeometry* g1, GEOSGeometry* g2, double tolerance, char rslt)
        {
            if (rslt == 1) return;
            //TODO: handle rslt exception value

            char* wkt1 = nullptr;
            char* wkt2 = nullptr;
            if (g1 != nullptr)
                wkt1 = GEOSWKTWriter_write(wktw_, g1);
            if (g2 != nullptr)
                wkt2 = GEOSWKTWriter_write(wktw_, g2);
            const char* val1 = (g1 == nullptr) ? "null" : wkt1;
            const char* val2 = (g2 == nullptr) ? "null" : wkt2;
            std::fprintf(stdout, "\n%s : %s != %s (tol = %f)\n", tag, val1, val2, tolerance);
            GEOSFree(wkt1);
            GEOSFree(wkt2);
        }

    };

}

