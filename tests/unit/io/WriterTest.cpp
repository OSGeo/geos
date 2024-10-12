//
// Test Suite for geos::io::Writer

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/Writer.h>
// std
#include <string>

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_writer_data {
    test_writer_data()
    {
    }
};

typedef test_group<test_writer_data> group;
typedef group::object object;

group test_writer_group("geos::io::Writer");


//
// Test Cases
//

template<>
template<>
void object::test<1>
()
{
    geos::io::Writer writer;

    writer.write("Hello ");
    writer.write("World!");
    ensure_equals(writer.toString(), "Hello World!");
}

template<>
template<>
void object::test<2>
()
{
    geos::io::Writer writer;

    writer.reserve(512);
    writer.write("Hello ");
    writer.write("World!");
    ensure_equals(writer.toString(), "Hello World!");
}

template<>
template<>
void object::test<3>
()
{
    geos::io::Writer writer;

    writer.reserve(1);
    writer.write("Hello ");
    writer.write("World!");
    ensure_equals(writer.toString(), "Hello World!");
}

template<>
template<>
void object::test<4>
()
{
    geos::io::Writer writer;

    writer.reserve(512);
    writer.write("Hello World!");
    writer.reserve(1);
    ensure_equals(writer.toString(), "Hello World!");
}

} // namespace tut


