//
// Test Suite for geos::io::ByteOrderValues

// tut
#include <tut/tut.hpp>
// geos
#include <geos/io/ByteOrderValues.h>
#include <geos/constants.h> // for int64

namespace tut {
//
// Test Group
//

// dummy data, not used
struct test_byteordervalues_data {
};

typedef test_group<test_byteordervalues_data> group;
typedef group::object object;

group test_byteordervalues_group("geos::io::ByteOrderValues");


//
// Test Cases
//

// 1 - Read/write an int
template<>
template<>
void object::test<1>
()
{
    using geos::io::ByteOrderValues;

    unsigned char buf[4];
    int in = 1;
    int out;

    ByteOrderValues::putInt(in, buf, ByteOrderValues::ENDIAN_BIG);
    ensure("putInt big endian[0]", buf[0] == 0);
    ensure("putInt big endian[1]", buf[1] == 0);
    ensure("putInt big endian[2]", buf[2] == 0);
    ensure("putInt big endian[3]", buf[3] == 1);

    out = ByteOrderValues::getInt(buf,
                                  geos::io::ByteOrderValues::ENDIAN_BIG);
    ensure_equals("getInt big endian", out, in);

    ByteOrderValues::putInt(1, buf, geos::io::ByteOrderValues::ENDIAN_LITTLE);
    ensure("putInt little endian[0]", buf[0] == 1);
    ensure("putInt little endian[1]", buf[1] == 0);
    ensure("putInt little endian[2]", buf[2] == 0);
    ensure("putInt little endian[3]", buf[3] == 0);

    out = ByteOrderValues::getInt(buf,
                                  ByteOrderValues::ENDIAN_LITTLE);
    ensure_equals("getInt little endian", out, in);
}

// 2 - Read/write a double
template<>
template<>
void object::test<2>
()
{
    using geos::io::ByteOrderValues;

    unsigned char buf[8];
    double in = 2;
    double out;

    ByteOrderValues::putDouble(in, buf,
                               ByteOrderValues::ENDIAN_BIG);
    ensure("putDouble big endian[0]", buf[0] == 64);
    ensure("putDouble big endian[1]", buf[1] == 0);
    ensure("putDouble big endian[2]", buf[2] == 0);
    ensure("putDouble big endian[3]", buf[3] == 0);
    ensure("putDouble big endian[4]", buf[4] == 0);
    ensure("putDouble big endian[5]", buf[5] == 0);
    ensure("putDouble big endian[6]", buf[6] == 0);
    ensure("putDouble big endian[7]", buf[7] == 0);

    out = ByteOrderValues::getDouble(buf,
                                     ByteOrderValues::ENDIAN_BIG);
    ensure_equals("getDouble big endian", out, in);

    ByteOrderValues::putDouble(in, buf,
                               ByteOrderValues::ENDIAN_LITTLE);
    ensure("putDouble little endian[0]", buf[0] == 0);
    ensure("putDouble little endian[1]", buf[1] == 0);
    ensure("putDouble little endian[2]", buf[2] == 0);
    ensure("putDouble little endian[3]", buf[3] == 0);
    ensure("putDouble little endian[4]", buf[4] == 0);
    ensure("putDouble little endian[5]", buf[5] == 0);
    ensure("putDouble little endian[6]", buf[6] == 0);
    ensure("putDouble little endian[7]", buf[7] == 64);

    out = ByteOrderValues::getDouble(buf,
                                     ByteOrderValues::ENDIAN_LITTLE);
    ensure_equals("getDouble little endian", out, in);
}

// 3 - Read/write a long
template<>
template<>
void object::test<3>
()
{
    using geos::io::ByteOrderValues;

    unsigned char buf[8];
    long in = 2;
    long out;

    ByteOrderValues::putLong(in, buf,
                             ByteOrderValues::ENDIAN_BIG);
    ensure("putLong big endian[0]", buf[0] == 0);
    ensure("putLong big endian[1]", buf[1] == 0);
    ensure("putLong big endian[2]", buf[2] == 0);
    ensure("putLong big endian[3]", buf[3] == 0);
    ensure("putLong big endian[4]", buf[4] == 0);
    ensure("putLong big endian[5]", buf[5] == 0);
    ensure("putLong big endian[6]", buf[6] == 0);
    ensure("putLong big endian[7]", buf[7] == 2);

    out = static_cast<long>(ByteOrderValues::getLong(buf, ByteOrderValues::ENDIAN_BIG));
    ensure_equals("getLong big endian", out, in);

    ByteOrderValues::putLong(in, buf,
                             ByteOrderValues::ENDIAN_LITTLE);
    ensure("putLong little endian[0]", buf[0] == 2);
    ensure("putLong little endian[1]", buf[1] == 0);
    ensure("putLong little endian[2]", buf[2] == 0);
    ensure("putLong little endian[3]", buf[3] == 0);
    ensure("putLong little endian[4]", buf[4] == 0);
    ensure("putLong little endian[5]", buf[5] == 0);
    ensure("putLong little endian[6]", buf[6] == 0);
    ensure("putLong little endian[7]", buf[7] == 0);

    out = static_cast<long>(ByteOrderValues::getLong(buf, ByteOrderValues::ENDIAN_LITTLE));
    ensure_equals("getLong little endian", out, in);
}

// 4 - Read/write an unsigned int
template<>
template<>
void object::test<4>()
{
    using geos::io::ByteOrderValues;

    unsigned char buf[4];
    uint32_t in = 3210000003;
    uint32_t out;

    ByteOrderValues::putUnsigned(in, buf, ByteOrderValues::ENDIAN_BIG);
    ensure("putUnsigned big endian[0]", buf[0] == 0xbf);
    ensure("putUnsigned big endian[1]", buf[1] == 0x54);
    ensure("putUnsigned big endian[2]", buf[2] == 0xb6);
    ensure("putUnsigned big endian[3]", buf[3] == 0x83);

    out = ByteOrderValues::getUnsigned(buf,
                                  geos::io::ByteOrderValues::ENDIAN_BIG);
    ensure_equals("getInt big endian", out, in);

    ByteOrderValues::putUnsigned(in, buf, geos::io::ByteOrderValues::ENDIAN_LITTLE);
    ensure("putUnsigned little endian[0]", buf[0] == 0x83);
    ensure("putUnsigned little endian[1]", buf[1] == 0xb6);
    ensure("putUnsigned little endian[2]", buf[2] == 0x54);
    ensure("pUtnsigned little endian[3]", buf[3] == 0xbf);

    out = ByteOrderValues::getUnsigned(buf,
                                  ByteOrderValues::ENDIAN_LITTLE);
    ensure_equals("getInt little endian", out, in);
}

} // namespace tut

