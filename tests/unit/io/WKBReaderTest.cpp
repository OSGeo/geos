// 
// Test Suite for geos::io::WKBReader 
// Uses geos::io::WKTReader to check correctness.
// Uses geos::io::WKBWriter to check correctness.
// Currently only tests 2D geoms of all (7) types.
// Tests NDR and XDR input and output .

// tut
#include <tut.hpp>
// geos
#include <geos/io/WKBReader.h>
#include <geos/io/WKBConstants.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/util/GEOSException.h>
// std
#include <sstream>
#include <string>
#include <memory>

namespace tut
{
	//
	// Test Group
	//

	// dummy data, not used
	struct test_wkbreader_data
	{
		geos::geom::PrecisionModel pm;
		geos::geom::GeometryFactory::unique_ptr gf;
		geos::io::WKBReader wkbreader;
		geos::io::WKBWriter xdrwkbwriter;
		geos::io::WKBWriter ndrwkbwriter;
		geos::io::WKTReader wktreader;

		typedef std::auto_ptr<geos::geom::Geometry> GeomPtr;

		test_wkbreader_data()
			:
			pm(1.0),
			gf(geos::geom::GeometryFactory::create(&pm)),
			wkbreader(*gf),
			// 2D only, XDR (big endian)
			xdrwkbwriter(2, geos::io::WKBConstants::wkbXDR),
			// 2D only, NDR (little endian)
			ndrwkbwriter(2, geos::io::WKBConstants::wkbNDR),
			wktreader(gf.get())
		{}

		void testInputOutput(const std::string& WKT,
				const std::string& ndrWKB,
				const std::string& xdrWKB)
		{
			GeomPtr gWKT(wktreader.read(WKT));

			// NDR input
			std::stringstream ndr_in(ndrWKB);
			GeomPtr gWKB_ndr(wkbreader.readHEX(ndr_in));
			ensure("NDR input",
				gWKB_ndr->equalsExact(gWKT.get()) );

			// XDR input
			std::stringstream xdr_in(xdrWKB);
			GeomPtr gWKB_xdr(wkbreader.readHEX(xdr_in));
			ensure("XDR input",
				gWKB_xdr->equalsExact(gWKT.get()) );

			// Compare geoms read from NDR and XDR
			ensure( gWKB_xdr->equalsExact(gWKB_ndr.get()) );

			// NDR output
			std::stringstream ndr_out;
			ndrwkbwriter.writeHEX(*gWKT, ndr_out);
			ensure_equals("NDR output",
				ndr_out.str(), ndr_in.str());

			// XDR output
			std::stringstream xdr_out;
			xdrwkbwriter.writeHEX(*gWKT, xdr_out);
			ensure_equals("XDR output",
				xdr_out.str(), xdr_in.str());

		}

	};

	typedef test_group<test_wkbreader_data> group;
	typedef group::object object;

	group test_wkbreader_group("geos::io::WKBReader");


	//
	// Test Cases
	//

	// 1 - Read/write a point in XDR and NDR format
	template<>
	template<>
	void object::test<1>()
	{         
		testInputOutput(

			// WKT
			"POINT(0 0)",

			// NDR HEXWKB
			"010100000000000000000000000000000000000000",

			// XDR HEXWKB
			"000000000100000000000000000000000000000000"

		);
	}

	// 2 - Read a linestring
	template<>
	template<>
	void object::test<2>()
	{         

		testInputOutput(

			// WKT
			"LINESTRING(1 2, 3 4)",

			// NDR HEXWKB
			"010200000002000000000000000000F03F000000000000004000000000000008400000000000001040",

			// XDR HEXWKB
			"0000000002000000023FF0000000000000400000000000000040080000000000004010000000000000"

		);

	}

	// 3 - Read a polygon
	template<>
	template<>
	void object::test<3>()
	{         
		testInputOutput(

			// WKT
			"POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(2 2, 2 6, 6 4, 2 2))",

			// NDR HEXWKB
			"0103000000020000000500000000000000000000000000000000000000000000000000244000000000000000000000000000002440000000000000244000000000000000000000000000002440000000000000000000000000000000000400000000000000000000400000000000000040000000000000004000000000000018400000000000001840000000000000104000000000000000400000000000000040",

			// XDR HEXWKB
			"0000000003000000020000000500000000000000000000000000000000402400000000000000000000000000004024000000000000402400000000000000000000000000004024000000000000000000000000000000000000000000000000000440000000000000004000000000000000400000000000000040180000000000004018000000000000401000000000000040000000000000004000000000000000"

		);

	}

	// 4 - Read a multipoint
	template<>
	template<>
	void object::test<4>()
	{         

		testInputOutput(

			// WKT
			"MULTIPOINT(0 0, 10 0, 10 10, 0 10, 0 0)",

			// NDR HEXWKB
			"010400000005000000010100000000000000000000000000000000000000010100000000000000000024400000000000000000010100000000000000000024400000000000002440010100000000000000000000000000000000002440010100000000000000000000000000000000000000",

			// XDR HEXWKB
			"000000000400000005000000000100000000000000000000000000000000000000000140240000000000000000000000000000000000000140240000000000004024000000000000000000000100000000000000004024000000000000000000000100000000000000000000000000000000"

		);

	}

	// 5 - Read a multilinestring
	template<>
	template<>
	void object::test<5>()
	{         

		testInputOutput(

			// WKT
			"MULTILINESTRING((0 0, 10 0, 10 10, 0 10, 10 20),(2 2, 2 6, 6 4, 20 2))",

			// NDR HEXWKB
			"010500000002000000010200000005000000000000000000000000000000000000000000000000002440000000000000000000000000000024400000000000002440000000000000000000000000000024400000000000002440000000000000344001020000000400000000000000000000400000000000000040000000000000004000000000000018400000000000001840000000000000104000000000000034400000000000000040",

			// XDR HEXWKB
			"000000000500000002000000000200000005000000000000000000000000000000004024000000000000000000000000000040240000000000004024000000000000000000000000000040240000000000004024000000000000403400000000000000000000020000000440000000000000004000000000000000400000000000000040180000000000004018000000000000401000000000000040340000000000004000000000000000"

		);

	}

	// 6 - Read a multipolygon
	template<>
	template<>
	void object::test<6>()
	{         

		testInputOutput(

			// WKT
			"MULTIPOLYGON(((0 0, 10 0, 10 10, 0 10, 0 0),(2 2, 2 6, 6 4, 2 2)),((60 60, 60 50, 70 40, 60 60)))",

			// NDR HEXWKB
			"0106000000020000000103000000020000000500000000000000000000000000000000000000000000000000244000000000000000000000000000002440000000000000244000000000000000000000000000002440000000000000000000000000000000000400000000000000000000400000000000000040000000000000004000000000000018400000000000001840000000000000104000000000000000400000000000000040010300000001000000040000000000000000004E400000000000004E400000000000004E400000000000004940000000000080514000000000000044400000000000004E400000000000004E40",

			// XDR HEXWKB
			"000000000600000002000000000300000002000000050000000000000000000000000000000040240000000000000000000000000000402400000000000040240000000000000000000000000000402400000000000000000000000000000000000000000000000000044000000000000000400000000000000040000000000000004018000000000000401800000000000040100000000000004000000000000000400000000000000000000000030000000100000004404E000000000000404E000000000000404E000000000000404900000000000040518000000000004044000000000000404E000000000000404E000000000000"

		);

	}

	// 7 - Read a collection 
	template<>
	template<>
	void object::test<7>()
	{         

		testInputOutput(

			// WKT
			"GEOMETRYCOLLECTION(POINT(0 0),LINESTRING(1 2,3 4),POLYGON((0 0,10 0,10 10,0 10,0 0),(2 2,2 6,6 4,2 2)),MULTIPOINT(0 0,10 0,10 10,0 10,0 0),MULTILINESTRING((0 0,10 0,10 10,0 10,10 20),(2 2,2 6,6 4,20 2)),MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0),(2 2,2 6,6 4,2 2)),((60 60,60 50,70 40,60 60))))",

			// NDR HEXWKB
			"010700000006000000010100000000000000000000000000000000000000010200000002000000000000000000F03F00000000000000400000000000000840000000000000104001030000000200000005000000000000000000000000000000000000000000000000002440000000000000000000000000000024400000000000002440000000000000000000000000000024400000000000000000000000000000000004000000000000000000004000000000000000400000000000000040000000000000184000000000000018400000000000001040000000000000004000000000000000400104000000050000000101000000000000000000000000000000000000000101000000000000000000244000000000000000000101000000000000000000244000000000000024400101000000000000000000000000000000000024400101000000000000000000000000000000000000000105000000020000000102000000050000000000000000000000000000000000000000000000000024400000000000000000000000000000244000000000000024400000000000000000000000000000244000000000000024400000000000003440010200000004000000000000000000004000000000000000400000000000000040000000000000184000000000000018400000000000001040000000000000344000000000000000400106000000020000000103000000020000000500000000000000000000000000000000000000000000000000244000000000000000000000000000002440000000000000244000000000000000000000000000002440000000000000000000000000000000000400000000000000000000400000000000000040000000000000004000000000000018400000000000001840000000000000104000000000000000400000000000000040010300000001000000040000000000000000004E400000000000004E400000000000004E400000000000004940000000000080514000000000000044400000000000004E400000000000004E40",

			// XDR HEXWKB
			"0000000007000000060000000001000000000000000000000000000000000000000002000000023FF00000000000004000000000000000400800000000000040100000000000000000000003000000020000000500000000000000000000000000000000402400000000000000000000000000004024000000000000402400000000000000000000000000004024000000000000000000000000000000000000000000000000000440000000000000004000000000000000400000000000000040180000000000004018000000000000401000000000000040000000000000004000000000000000000000000400000005000000000100000000000000000000000000000000000000000140240000000000000000000000000000000000000140240000000000004024000000000000000000000100000000000000004024000000000000000000000100000000000000000000000000000000000000000500000002000000000200000005000000000000000000000000000000004024000000000000000000000000000040240000000000004024000000000000000000000000000040240000000000004024000000000000403400000000000000000000020000000440000000000000004000000000000000400000000000000040180000000000004018000000000000401000000000000040340000000000004000000000000000000000000600000002000000000300000002000000050000000000000000000000000000000040240000000000000000000000000000402400000000000040240000000000000000000000000000402400000000000000000000000000000000000000000000000000044000000000000000400000000000000040000000000000004018000000000000401800000000000040100000000000004000000000000000400000000000000000000000030000000100000004404E000000000000404E000000000000404E000000000000404900000000000040518000000000004044000000000000404E000000000000404E000000000000"

		);

	}

  // 8 - Invalid HEXWKB for missing HEX char (#675)
  template<>
  template<>
  void object::test<8>()
  {         
    std::stringstream hexwkb;
    // NOTE: add a 0 to make valid
    hexwkb << "01010000000000000000000000000000000000000";
    //hexwkb << "0";
    std::string err;
    try {
    GeomPtr gWKB_ndr(wkbreader.readHEX(hexwkb));
    } catch (const geos::util::GEOSException& ex) {
      err = ex.what();
    }
    ensure("Missing expected error", !err.empty());
    ensure_equals(err, "ParseException: Premature end of HEX string");
  }

	template<>
	template<>
	void object::test<9>()
	{

		testInputOutput(

			// WKT
			"LINESTRING Z (0 1 2, 1 2 3, 2 3 4)",

			// NDR HEXWKB
			"01ea030000030000000000000000000000000000000000f03f0000000000000040000000000000f03f00000000000000400000000000000840000000000000004000000000000008400000000000001040",

			// XDR HEXWKB
			"00000003ea0000000300000000000000003ff000000000000040000000000000003ff000000000000040000000000000004008000000000000400000000000000040080000000000004010000000000000"

		);

	}

	template<>
	template<>
	void object::test<10>()
	{

		testInputOutput(

			// WKT
			"LINESTRING M (1 2 3, 2 3 4, 3 4 5)",

			// NDR HEXWKB
			"01d207000003000000000000000000f03f00000000000000400000000000000840000000000000004000000000000008400000000000001040000000000000084000000000000010400000000000001440",

			// XDR HEXWKB
			"00000007d2000000033ff000000000000040000000000000004008000000000000400000000000000040080000000000004010000000000000400800000000000040100000000000004014000000000000"

		);

	}

	template<>
	template<>
	void object::test<11>()
	{

		testInputOutput(

			// WKT
			"LINESTRING ZM (2 3 4 5, 3 4 5 6, 4 5 6 7)",

			// NDR HEXWKB
			"01ba0b000003000000000000000000004000000000000008400000000000001040000000000000144000000000000008400000000000001040000000000000144000000000000018400000000000001040000000000000144000000000000018400000000000001c40",

			// XDR HEXWKB
			"0000000bba0000000340000000000000004008000000000000401000000000000040140000000000004008000000000000401000000000000040140000000000004018000000000000401000000000000040140000000000004018000000000000401c000000000000"

		);

	}

	template<>
	template<>
	void object::test<12>()
	{

		testInputOutput(

			// WKT
			"POLYGON Z ((0 1 2, 1 2 3, 2 3 4, 0 1 2))",

			// NDR HEXWKB
			"01eb03000001000000040000000000000000000000000000000000f03f0000000000000040000000000000f03f000000000000004000000000000008400000000000000040000000000000084000000000000010400000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"00000003eb000000010000000400000000000000003ff000000000000040000000000000003ff00000000000004000000000000000400800000000000040000000000000004008000000000000401000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<13>()
	{

		testInputOutput(

			// WKT
			"POLYGON M ((1 2 3, 2 3 4, 3 4 5, 1 2 3))",

			// NDR HEXWKB
			"01d30700000100000004000000000000000000f03f00000000000000400000000000000840000000000000004000000000000008400000000000001040000000000000084000000000000010400000000000001440000000000000f03f00000000000000400000000000000840",

			// XDR HEXWKB
			"00000007d300000001000000043ff0000000000000400000000000000040080000000000004000000000000000400800000000000040100000000000004008000000000000401000000000000040140000000000003ff000000000000040000000000000004008000000000000"

		);

	}

	template<>
	template<>
	void object::test<14>()
	{

		testInputOutput(

			// WKT
			"POLYGON ZM ((2 3 4 5, 3 4 5 6, 4 5 6 7, 2 3 4 5))",

			// NDR HEXWKB
			"01bb0b00000100000004000000000000000000004000000000000008400000000000001040000000000000144000000000000008400000000000001040000000000000144000000000000018400000000000001040000000000000144000000000000018400000000000001c400000000000000040000000000000084000000000000010400000000000001440",

			// XDR HEXWKB
			"0000000bbb000000010000000440000000000000004008000000000000401000000000000040140000000000004008000000000000401000000000000040140000000000004018000000000000401000000000000040140000000000004018000000000000401c0000000000004000000000000000400800000000000040100000000000004014000000000000"

		);

	}

	template<>
	template<>
	void object::test<15>()
	{

		testInputOutput(

			// WKT
			"POLYGON Z ((0 0 1, 2 0 1, 0 2 1, 0 0 1), (0 0 2, 1 0 2, 0 1 2, 0 0 2))",

			// NDR HEXWKB
			"01eb030000020000000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f04000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000003eb0000000200000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff0000000000000000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<16>()
	{

		testInputOutput(

			// WKT
			"POLYGON M ((0 0 1, 2 0 1, 0 2 1, 0 0 1), (0 0 2, 1 0 2, 0 1 2, 0 0 2))",

			// NDR HEXWKB
			"01d3070000020000000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f04000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000007d30000000200000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff0000000000000000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<17>()
	{

		testInputOutput(

			// WKT
			"POLYGON ZM ((0 0 1 2, 2 0 1 2, 0 2 1 2, 0 0 1 2), (0 0 1 2, 1 0 1 2, 0 1 1 2, 0 0 1 2))",

			// NDR HEXWKB
			"01bb0b0000020000000400000000000000000000000000000000000000000000000000f03f000000000000004000000000000000400000000000000000000000000000f03f000000000000004000000000000000000000000000000040000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f00000000000000400400000000000000000000000000000000000000000000000000f03f0000000000000040000000000000f03f0000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"0000000bbb0000000200000004000000000000000000000000000000003ff00000000000004000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000040000000000000003ff00000000000004000000000000000000000000000000000000000000000003ff0000000000000400000000000000000000004000000000000000000000000000000003ff000000000000040000000000000003ff000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff00000000000003ff00000000000004000000000000000000000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<18>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING Z ((0 1 2, 1 2 3, 2 3 4, 0 1 2))",

			// NDR HEXWKB
			"01ed0300000100000001ea030000040000000000000000000000000000000000f03f0000000000000040000000000000f03f000000000000004000000000000008400000000000000040000000000000084000000000000010400000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"00000003ed0000000100000003ea0000000400000000000000003ff000000000000040000000000000003ff00000000000004000000000000000400800000000000040000000000000004008000000000000401000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<19>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING M ((1 2 3, 2 3 4, 3 4 5, 1 2 3))",

			// NDR HEXWKB
			"01d50700000100000001d207000004000000000000000000f03f00000000000000400000000000000840000000000000004000000000000008400000000000001040000000000000084000000000000010400000000000001440000000000000f03f00000000000000400000000000000840",

			// XDR HEXWKB
			"00000007d50000000100000007d2000000043ff0000000000000400000000000000040080000000000004000000000000000400800000000000040100000000000004008000000000000401000000000000040140000000000003ff000000000000040000000000000004008000000000000"

		);

	}

	template<>
	template<>
	void object::test<20>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING ZM ((2 3 4 5, 3 4 5 6, 4 5 6 7, 2 3 4 5))",

			// NDR HEXWKB
			"01bd0b00000100000001ba0b000004000000000000000000004000000000000008400000000000001040000000000000144000000000000008400000000000001040000000000000144000000000000018400000000000001040000000000000144000000000000018400000000000001c400000000000000040000000000000084000000000000010400000000000001440",

			// XDR HEXWKB
			"0000000bbd000000010000000bba0000000440000000000000004008000000000000401000000000000040140000000000004008000000000000401000000000000040140000000000004018000000000000401000000000000040140000000000004018000000000000401c0000000000004000000000000000400800000000000040100000000000004014000000000000"

		);

	}

	template<>
	template<>
	void object::test<21>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING Z ((0 0 1, 2 0 1, 0 2 1, 0 0 1), (0 0 2, 1 0 2, 0 1 2, 0 0 2))",

			// NDR HEXWKB
			"01ed0300000200000001ea0300000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f01ea03000004000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000003ed0000000200000003ea00000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff000000000000000000003ea000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<22>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING M ((0 0 1, 2 0 1, 0 2 1, 0 0 1), (0 0 2, 1 0 2, 0 1 2, 0 0 2))",

			// NDR HEXWKB
			"01d50700000200000001d20700000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f01d207000004000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000007d50000000200000007d200000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff000000000000000000007d2000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<23>()
	{

		testInputOutput(

			// WKT
			"MULTILINESTRING ZM ((0 0 1 2, 2 0 1 2, 0 2 1 2, 0 0 1 2), (0 0 1 2, 1 0 1 2, 0 1 1 2, 0 0 1 2))",

			// NDR HEXWKB
			"01bd0b00000200000001ba0b00000400000000000000000000000000000000000000000000000000f03f000000000000004000000000000000400000000000000000000000000000f03f000000000000004000000000000000000000000000000040000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f000000000000004001ba0b00000400000000000000000000000000000000000000000000000000f03f0000000000000040000000000000f03f0000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"0000000bbd000000020000000bba00000004000000000000000000000000000000003ff00000000000004000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000040000000000000003ff00000000000004000000000000000000000000000000000000000000000003ff000000000000040000000000000000000000bba00000004000000000000000000000000000000003ff000000000000040000000000000003ff000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff00000000000003ff00000000000004000000000000000000000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<24>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON Z (((0 1 2, 1 2 3, 2 3 4, 0 1 2)))",

			// NDR HEXWKB
			"01ee0300000100000001eb03000001000000040000000000000000000000000000000000f03f0000000000000040000000000000f03f000000000000004000000000000008400000000000000040000000000000084000000000000010400000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"00000003ee0000000100000003eb000000010000000400000000000000003ff000000000000040000000000000003ff00000000000004000000000000000400800000000000040000000000000004008000000000000401000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<25>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON M (((1 2 3, 2 3 4, 3 4 5, 1 2 3)))",

			// NDR HEXWKB
			"01d60700000100000001d30700000100000004000000000000000000f03f00000000000000400000000000000840000000000000004000000000000008400000000000001040000000000000084000000000000010400000000000001440000000000000f03f00000000000000400000000000000840",

			// XDR HEXWKB
			"00000007d60000000100000007d300000001000000043ff0000000000000400000000000000040080000000000004000000000000000400800000000000040100000000000004008000000000000401000000000000040140000000000003ff000000000000040000000000000004008000000000000"

		);

	}

	template<>
	template<>
	void object::test<26>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON ZM (((2 3 4 5, 3 4 5 6, 4 5 6 7, 2 3 4 5)))",

			// NDR HEXWKB
			"01be0b00000100000001bb0b00000100000004000000000000000000004000000000000008400000000000001040000000000000144000000000000008400000000000001040000000000000144000000000000018400000000000001040000000000000144000000000000018400000000000001c400000000000000040000000000000084000000000000010400000000000001440",

			// XDR HEXWKB
			"0000000bbe000000010000000bbb000000010000000440000000000000004008000000000000401000000000000040140000000000004008000000000000401000000000000040140000000000004018000000000000401000000000000040140000000000004018000000000000401c0000000000004000000000000000400800000000000040100000000000004014000000000000"

		);

	}

	template<>
	template<>
	void object::test<27>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON Z (((0 0 1, 2 0 1, 0 2 1, 0 0 1)), ((0 0 2, 1 0 2, 0 1 2, 0 0 2)))",

			// NDR HEXWKB
			"01ee0300000200000001eb030000010000000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f01eb0300000100000004000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000003ee0000000200000003eb0000000100000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff000000000000000000003eb00000001000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<28>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON M (((0 0 1, 2 0 1, 0 2 1, 0 0 1)), ((0 0 2, 1 0 2, 0 1 2, 0 0 2)))",

			// NDR HEXWKB
			"01d60700000200000001d3070000010000000400000000000000000000000000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f00000000000000000000000000000040000000000000f03f00000000000000000000000000000000000000000000f03f01d30700000100000004000000000000000000000000000000000000000000000000000040000000000000f03f000000000000000000000000000000400000000000000000000000000000f03f0000000000000040000000000000000000000000000000000000000000000040",

			// XDR HEXWKB
			"00000007d60000000200000007d30000000100000004000000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff0000000000000000000000000000040000000000000003ff0000000000000000000000000000000000000000000003ff000000000000000000007d300000001000000040000000000000000000000000000000040000000000000003ff00000000000000000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000000000000000000004000000000000000"

		);

	}

	template<>
	template<>
	void object::test<29>()
	{

		testInputOutput(

			// WKT
			"MULTIPOLYGON ZM (((0 0 1 2, 2 0 1 2, 0 2 1 2, 0 0 1 2)), ((0 0 1 2, 1 0 1 2, 0 1 1 2, 0 0 1 2)))",

			// NDR HEXWKB
			"01be0b00000200000001bb0b0000010000000400000000000000000000000000000000000000000000000000f03f000000000000004000000000000000400000000000000000000000000000f03f000000000000004000000000000000000000000000000040000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f000000000000004001bb0b0000010000000400000000000000000000000000000000000000000000000000f03f0000000000000040000000000000f03f0000000000000000000000000000f03f00000000000000400000000000000000000000000000f03f000000000000f03f000000000000004000000000000000000000000000000000000000000000f03f0000000000000040",

			// XDR HEXWKB
			"0000000bbe000000020000000bbb0000000100000004000000000000000000000000000000003ff00000000000004000000000000000400000000000000000000000000000003ff00000000000004000000000000000000000000000000040000000000000003ff00000000000004000000000000000000000000000000000000000000000003ff000000000000040000000000000000000000bbb0000000100000004000000000000000000000000000000003ff000000000000040000000000000003ff000000000000000000000000000003ff0000000000000400000000000000000000000000000003ff00000000000003ff00000000000004000000000000000000000000000000000000000000000003ff00000000000004000000000000000"

		);

	}

} // namespace tut

