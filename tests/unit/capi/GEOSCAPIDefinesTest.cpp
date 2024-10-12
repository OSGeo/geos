//
// Test Suite for C-API defines

#include <tut/tut.hpp>
// geos
#include <geos_c.h>
// std

#define QUOTE(x) #x
#define EXPAND_AND_QUOTE(y) QUOTE(y)

namespace tut {
//
// Test Group
//

struct basic {};

typedef test_group<basic> group;
typedef group::object object;

group test_capidefines_group("capi::GEOSCAPIDefines");

// Make sure version elements all defined
template<>
template<>
void object::test<1>
()
{
    ensure(std::string(EXPAND_AND_QUOTE(GEOS_VERSION_MAJOR)).length() > 0);
    ensure(std::string(EXPAND_AND_QUOTE(GEOS_VERSION_MINOR)).length() > 0);
    ensure(std::string(EXPAND_AND_QUOTE(GEOS_VERSION_PATCH)).length() > 0);
    ensure(std::string(GEOS_VERSION).length() > 0);

    ensure(std::string(EXPAND_AND_QUOTE(GEOS_CAPI_VERSION_MAJOR)).length() > 0);
    ensure(std::string(EXPAND_AND_QUOTE(GEOS_CAPI_VERSION_MINOR)).length() > 0);
    ensure(std::string(EXPAND_AND_QUOTE(GEOS_CAPI_VERSION_PATCH)).length() > 0);
    ensure(std::string(GEOS_CAPI_VERSION).length() > 0);
}

// Make sure version representations agree
template<>
template<>
void object::test<2>
()
{
    ensure_equals(
        GEOS_VERSION,
        std::string(EXPAND_AND_QUOTE(GEOS_VERSION_MAJOR)) + "." +
        std::string(EXPAND_AND_QUOTE(GEOS_VERSION_MINOR)) + "." +
        std::string(EXPAND_AND_QUOTE(GEOS_VERSION_PATCH)));
}

// Make sure define is consistent with function
template<>
template<>
void object::test<3>
()
{
    ensure_equals(GEOS_CAPI_VERSION, std::string(GEOSversion()));
}

} // namespace tut

