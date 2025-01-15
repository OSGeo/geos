/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2020 Martin Davis
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/profiler.h>

#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/operation/valid/MakeValid.h>
#include <geos/io/GeoJSONWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTStreamReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKBStreamReader.h>
#include <geos/io/WKBWriter.h>

#include <cfenv>
#include <fstream>
#include <iostream>
#include <sstream>

#include "GeometryOp.h"
#include "GeosOp.h"
#include "cxxopts.hpp"

using namespace geos;
using namespace geos::geom;
using namespace geos::io;

void showHelp() {
    std::cout << "geosop executes GEOS geometry operations on inputs." << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: geosop [wktfile] opname args..." << std::endl;
}

static bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

double parseNumber(std::string s) {
    auto val = s;
    /**
     * To get around cmdline parset limitation for parsing neg numbers,
     * allow syntax "Nnum" as well
     */
    if (startsWith(val, "N")) {
        val = val.substr(1, val.size()-1);
    }
    try {
        return std::stod(val);
    } catch (const std::invalid_argument&) {
        std::cerr << "Invalid positional argument '" << val << "' (expected number)" << std::endl;
        exit(1);
    } catch (const std::out_of_range&) {
        std::cerr << "Out of range positional argument '" << val << "' (expected double)" << std::endl;
        exit(1);
    }
}

int main(int argc, char** argv) {
    GeosOpArgs cmdArgs;
    OpArguments opArgs;

    cxxopts::Options options("geosop", "Executes GEOS geometry operations");
    options.add_options()
        ("a", "source for A geometries (WKT, WKB, file, stdin, stdin.wkb)", cxxopts::value<std::string>( cmdArgs.srcA ))
        ("b", "source for B geometries (WKT, WKB, file, stdin, stdin.wkb)", cxxopts::value<std::string>( cmdArgs.srcB ))
        ("l,limita", "Limit number of A geometries read", cxxopts::value<int>( cmdArgs.limitA ))
        ("o,offseta", "Skip reading first N geometries of A", cxxopts::value<int>( cmdArgs.offsetA ) )
        ("c,collect", "Collect input into single geometry (automatic for AGG ops)", cxxopts::value<bool>( cmdArgs.isCollect ))
        ("e,explode", "Explode results into component geometries", cxxopts::value<bool>( cmdArgs.isExplode))
        ("f,format", "Output format (wkt, wkb, txt or geojson)", cxxopts::value<std::string>( ))
        ("p,precision", "Set number of decimal places in output coordinates", cxxopts::value<int>( cmdArgs.precision ) )
        ("q,quiet", "Disable result output", cxxopts::value<bool>( cmdArgs.isQuiet ) )
        ("r,repeat", "Repeat operation N times", cxxopts::value<int>( cmdArgs.repeatNum ) )
        ("select", "Select geometries where op result is true", cxxopts::value<bool>( cmdArgs.isSelect ) )
        ("selectNot", "Select geometries where op result is false", cxxopts::value<bool>( cmdArgs.isSelectNot ) )
        ("t,time", "Print execution time", cxxopts::value<bool>( cmdArgs.isShowTime ) )
        ("v,verbose", "Verbose output", cxxopts::value<bool>( cmdArgs.isVerbose )->default_value("false"))
        ("h,help", "Print help")

        ("opName", "Operation name", cxxopts::value<std::string>()->default_value("no-op"))
        ("opArgs", "Operation arguments ", cxxopts::value<std::vector<std::string>>())
    ;

    options.parse_positional({"opName", "opArgs"});
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch ( cxxopts::option_not_exists_exception& ex ) {
        std::cerr << ex.what() << std::endl;
        exit(1);
    }

    if (argc <= 1 || result.count("help")) {
        std::cout << "geosop - GEOS " << geosversion() << std::endl;
        options.positional_help("opName opArg");
        std::cout << options.help() << std::endl;
        //showHelp();
        if (result.count("help")) {
            std::cout << "Notes:" << std::endl;
            std::cout << "- to use negative numeric op arguments use '--' before the op name" << std::endl;
            std::cout << "- Negative numeric op arguments can be specified with leading N:  e.g. N-0.1" << std::endl;
            std::cout << std::endl;
            std::cout << "Operations:" << std::endl;
            std::vector<std::string> ops = GeometryOp::listOps();
            for (auto opName : ops) {
               std::cout << "  " << opName << std::endl;
            }

        }
        return 0;
    }

    if (result.count("format")) {
        auto fmt = result["format"].as<std::string>();
        // Use lowercase matching
        std::transform(fmt.begin(), fmt.end(), fmt.begin(),
            [](unsigned char c){ return (unsigned char)std::tolower(c); });
        if (fmt == "txt" || fmt == "wkt" ) {
            cmdArgs.format = GeosOpArgs::fmtText;
        }
        else if (fmt == "wkb") {
            cmdArgs.format = GeosOpArgs::fmtWKB;
        }
        else if (fmt == "geojson" || fmt == "json") {
            cmdArgs.format = GeosOpArgs::fmtGeoJSON;
        }
        else {
            std::cerr << "Invalid format value: " << fmt << std::endl;
            exit(1);
        }
    }

    if (result.count("opName")) {
        cmdArgs.opName = result["opName"].as<std::string>();
    }
    //--- parse positional op arg
    // TODO: verify param number against operation
    if (result.count("opArgs")) {
        auto& v = result["opArgs"].as<std::vector<std::string>>();
        if ( v.size() > 2 ) {
            std::cerr << "too many positional arguments: " << v.size() << std::endl;
            exit(1);
        }
        opArgs.nArgs = (int) v.size();
        if (v.size() >= 1) {
            opArgs.arg1 = parseNumber(v[0]);
        }
        if (v.size() >= 2) {
            opArgs.arg2 = parseNumber(v[1]);
        }
    }

    GeosOp geosop(cmdArgs);
    geosop.run(opArgs);
}

GeosOp::GeosOp(GeosOpArgs& arg)
    : args(arg)
{
}

GeosOp::~GeosOp() {
}

//-- Format numbers with comma separators
//   see https://stackoverflow.com/a/30857592/2308716
class comma_numpunct : public std::numpunct<char>
{
public:
   comma_numpunct(char p_thousands_sep, const char* p_grouping)
      :m_thousands_sep(p_thousands_sep),
       m_grouping(p_grouping){}
protected:
   char do_thousands_sep() const override {return m_thousands_sep;}
   std::string do_grouping() const override {return m_grouping;}
private:
   char m_thousands_sep;
   std::string m_grouping;
};

template<typename T>
std::string formatNum(T n)
{
    std::stringstream ss;
    std::locale comma_locale(std::locale(), new comma_numpunct(',', "\03"));
    ss.imbue(comma_locale);
    ss << std::fixed << n;
    return ss.str();
}

std::vector<std::unique_ptr<Geometry>> collect( std::vector<std::unique_ptr<Geometry>>& geoms ) {
    auto gf = GeometryFactory::create();
    auto gc = gf->createGeometryCollection( std::move(geoms) );
    std::vector<std::unique_ptr<Geometry>> geomsColl;
    geomsColl.push_back( std::move( gc ) );
    return geomsColl;
}

bool isWKTLiteral(std::string s) {
    // check for empty geoms (which do not have parens)
    const size_t slen = s.size();
    if (slen < 6) return false;
    auto lastWord = s.substr(slen - 6, slen);
    for (char& c : lastWord)
        c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
    if ( lastWord.compare(" EMPTY") == 0 ) return true;

    // assume if string contains a ( it is WKT
    auto numLParen = std::count(s.begin(), s.end(), '(');
    return numLParen > 0;
}

bool isWKBLiteral(std::string s) {
    // assume WKB if only chars are [0-9] and [a-fA-F]
    const std::string hexChars = "0123456789abcdefABCDEF";
    return s.find_first_not_of(hexChars) == std::string::npos;
}

std::vector<std::unique_ptr<Geometry>>
readWKTFile(std::istream& in, int limit, int offset) {

    WKTStreamReader rdr( in );
    std::vector<std::unique_ptr<Geometry>> geoms;
    int count = 0;
    while (limit < 0 || (int) geoms.size() < limit) {
        auto geom = rdr.next();
        if (geom == nullptr)
            break;
        if (count > offset) {
            geoms.push_back(std::move(geom));
        }
        count++;
    }
    return geoms;
}

std::vector<std::unique_ptr<Geometry>>
readWKTFile(std::string src, int limit, int offset) {
    if (src == "-" || src == "-.wkt" || src == "stdin" || src == "stdin.wkt") {
        return readWKTFile( std::cin, limit, offset );
    }
    std::ifstream f( src );
    auto geoms = readWKTFile( f, limit, offset );
    f.close();
    return geoms;
}

std::vector<std::unique_ptr<Geometry>>
readWKBFile(std::istream& in, int limit, int offset) {
    WKBStreamReader rdr( in );
    std::vector<std::unique_ptr<Geometry>> geoms;
    int count = 0;
    while (limit < 0 || (int) geoms.size() < limit) {
        auto geom = rdr.next();
        if (geom == nullptr)
            break;
        if (count > offset) {
            geoms.push_back(std::move(geom));
        }
        count++;
    }
    return geoms;
}

std::vector<std::unique_ptr<Geometry>>
readWKBFile(std::string src, int limit, int offset) {
    if (src == "-.wkb" || src == "stdin.wkb" ) {
        return readWKBFile( std::cin, limit, offset );
    }
    std::ifstream f( src );
    auto geoms = readWKBFile( f, limit, offset );
    f.close();
    return geoms;
}

void GeosOp::log(std::string s) {
    if (args.isVerbose) {
        std::cout << s << std::endl;
    }

}

std::vector<std::unique_ptr<Geometry>>
GeosOp::readInput(std::string name, std::string src, int limit, int offset) {
    std::vector<std::unique_ptr<Geometry>> geoms;
    std::string srcDesc = "Input " + name + ": ";
    if ( isWKTLiteral(src) ) {
        log(srcDesc + "WKT literal");

        geos::io::WKTReader rdr;
        auto geom = rdr.read( src );
        geoms.push_back( std::move(geom) );
    }
    else if ( isWKBLiteral(src) ) {
        log(srcDesc + "WKB literal");

        geos::io::WKBReader rdr;
        std::istringstream hex(src);
        auto geom = rdr.readHEX( hex );
        geoms.push_back( std::move(geom) );
    }
    else if (endsWith(src, ".wkb")) {
        log(srcDesc + "WKB file " + src);
        geoms = readWKBFile( src, limit, offset );
    }
    else {
        log(srcDesc + "WKT file " + src);
        geoms = readWKTFile( src, limit, offset );
    }
    return geoms;
}

std::string geomStats(int geomCount, int geomVertices) {
    return formatNum( geomCount) + " geometries, "
        + formatNum( geomVertices) + " vertices";

}
std::string summaryStats(std::vector<std::unique_ptr<Geometry>>& geoms) {
    int geomCount = 0;
    int geomPts = 0;
    for (const  auto& geom : geoms) {
        geomCount++;
        geomPts += static_cast<int>(geom->getNumPoints());
    }
    return geomStats(geomCount, geomPts);
}

std::vector<std::unique_ptr<Geometry>>
GeosOp::loadInput(std::string name, std::string src, int limit, int offset) {
    if (src.length() == 0) {
        std::vector<std::unique_ptr<Geometry>> geoms;
        return geoms;
    }
    geos::util::Profile sw( "Read" );
    sw.start();

    std::vector<std::unique_ptr<Geometry>> geoms;
    try {
      geoms = readInput( name, src, limit, offset );
    } catch (geos::util::GEOSException & e) {
      std::cout << e.what() << std::endl;
      exit(1);
    }

    sw.stop();
    auto stats = summaryStats(geoms);
    log("Read " + stats  + "  -- " + formatNum( (long) sw.getTot() ) + " usec");
    return geoms;
}

void GeosOp::run(OpArguments& opArgs) {
    GeometryOp* op = getOp();

    if (opArgs.nArgs != op->nParam()) {
        std::cerr << "wrong number of arguments for operation: " << op->name() << std::endl;
        return;
    }

    // ensure at least one op processed
    if (args.repeatNum < 1) args.repeatNum = 1;

    auto geomsLoadA = loadInput("A", args.srcA, args.limitA, args.offsetA);

    //--- collect input into single geometry collection if required
    bool doCollect = args.isCollect || op->isAggregate();
    if (doCollect && geomsLoadA.size() > 1) {
        geomA = collect( geomsLoadA );
    }
    else {
        geomA = std::move(geomsLoadA);
    }

    geomB = loadInput("B", args.srcB, -1, -1);

    //------------------------

    try {
        std::feclearexcept(FE_ALL_EXCEPT); // clear floating-point status flags

        execute(op, opArgs);

        // Catch everything except for FE_INEXACT, which is usually harmless
        const int fpexp = std::fetestexcept(FE_ALL_EXCEPT ^ FE_INEXACT);
        if (args.isVerbose && (fpexp != 0)) {
            std::cerr << "Operation raised floating-point environment flag(s):";
            if (fpexp & FE_DIVBYZERO)
                std::cerr << " FE_DIVBYZERO";
            if (fpexp & FE_INEXACT)
                std::cerr << " FE_INEXACT";
            if (fpexp & FE_INVALID)
                std::cerr << " FE_INVALID";
            if (fpexp & FE_OVERFLOW)
                std::cerr << " FE_OVERFLOW";
            if (fpexp & FE_UNDERFLOW)
                std::cerr << " FE_UNDERFLOW";
            std::cerr << std::endl;
        }
    }
    catch (std::exception &e) {
        std::cerr << "Run-time exception: " << e.what() << std::endl;
        exit(1);
    }

    if (args.isShowTime || args.isVerbose) {
        std::cerr
            << "Ran " <<  formatNum( opCount ) << " " << args.opName << " ops ( "
            << formatNum( vertexCount ) << " vertices)"
            << "  -- " << formatNum( (long) totalTime ) <<  " usec"
            << "    (GEOS " << geosversion() << ")"
            << std::endl;
    }
}

GeometryOp* GeosOp::getOp() {
    std::string opName = args.opName;

    GeometryOp * op;
    //-- default op is to copy geom
    if (opName == "" || opName == "no-op") {
        opName = "copy";
    }
    op = GeometryOp::find(opName);

    if (op == nullptr) {
        std::cerr << "Unknown operation: " << opName << std::endl;
        exit(1);
    }
    return op;
}

void GeosOp::execute(GeometryOp* op, OpArguments& opArgs) {

    if (op->isBinary()) {
        executeBinary(op, opArgs);
    }
    else {
        executeUnary(op, opArgs);
    }
}

void GeosOp::executeUnary(GeometryOp * op, OpArguments& opArgs) {
    for (unsigned i = 0; i < geomA.size(); i++) {
        vertexCount += geomA[i]->getNumPoints();
        Result* result = executeOpRepeat(op, i, geomA[i], 0, nullptr, opArgs);
        output(result, geomA[i].get());
        delete result;
    }
}

void GeosOp::executeBinary(GeometryOp * op, OpArguments& opArgs) {
    for (unsigned ia = 0; ia < geomA.size(); ia++) {
        for (unsigned ib = 0; ib < geomB.size(); ib++) {
            vertexCount += geomA[ia]->getNumPoints();
            vertexCount += geomB[ib]->getNumPoints();
            Result* result = executeOpRepeat(op, ia, geomA[ia], ib, geomB[ib], opArgs);

            output(result, geomA[ia].get());
            delete result;
        }
    }
}

std::string inputDesc(std::string name, unsigned int index, const std::unique_ptr<Geometry>& geom)
{
    if (geom == nullptr) {
        return "";
    }
    std::string desc = name + "[" + std::to_string(index+1) + "] " + geom->getGeometryType()
        + "( " + std::to_string(geom->getNumPoints()) + " )";
    return desc;
}

Result* GeosOp::executeOpRepeat(GeometryOp * op,
    unsigned int indexA,
    const std::unique_ptr<Geometry>& gA,
    unsigned int indexB,
    const std::unique_ptr<Geometry>& gB,
    OpArguments& opArgs)
{
    Result* res = nullptr;
    for (int i = 0; i < args.repeatNum; i++) {
        res = executeOp(op, indexA, gA, indexB, gB, opArgs);
    }
    return res;
}

Result* GeosOp::executeOp(GeometryOp * op,
    unsigned int indexA,
    const std::unique_ptr<Geometry>& gA,
    unsigned int indexB,
    const std::unique_ptr<Geometry>& gB,
    OpArguments& opArgs) {

    opCount++;
    geos::util::Profile sw( "op" );
    sw.start();

    Result* result = op->execute( gA, gB, opArgs.arg1, opArgs.arg2  );
    sw.stop();
    double time = sw.getTot();
    totalTime += time;

    // avoid cost of logging if not verbose
    if (args.isVerbose) {
        log(
            "[ " + std::to_string(opCount) + "] " + op->name() + ": "
            + inputDesc("A", indexA, gA) + " "
            + inputDesc("B", indexB, gB)
            + " -> " + result->metadata()
            + "  --  " + formatNum( (int) time ) + " usec"
        );
    }

    return result;
}

void GeosOp::output(Result* result, Geometry* geom) {
    //---- print result if format specified
    if (args.isQuiet)
        return;

    if (result->isGeometry() ) {
        if (args.isExplode) {
            outputExplode( result->valGeom );
        }
        else {
            outputGeometry( result->valGeom.get() );
        }
    }
    else if (result->isGeometryList() ) {
        outputGeometryList( result->valGeomList );
    }
    else if (result->isBool() ) {
        if (args.isSelect || args.isSelectNot) {
            bool isSelected = (args.isSelect && result->toBool())
                    || (args.isSelectNot && ! result->toBool());
            if (isSelected) {
                outputGeometry( geom );
            }
        }
        else {
            std::cout << result->toString() << std::endl;
        }
    }
    else {
        // output as text/WKT
        std::cout << result->toString() << std::endl;
    }
}

void GeosOp::outputExplode(std::unique_ptr<Geometry>& geom) {
    for (size_t i = 0; i < geom->getNumGeometries(); i++) {
        auto g = geom->getGeometryN(i);
        outputGeometry( g );
    }
}

void GeosOp::outputGeometry(const Geometry * geom) {
    if (geom == nullptr) {
        std::cout << "null" << std::endl;
        return;
    }

    if (args.format == GeosOpArgs::fmtWKB ) {
        // output as hex-encoded WKB
        WKBWriter writer;
        writer.writeHEX(*geom, std::cout);
        std::cout << std::endl;
    }
    else if (args.format == GeosOpArgs::fmtGeoJSON ) {
        // output as GeoJSON
        // TODO: enable args.precision to output
        geos::io::GeoJSONWriter geojsonwriter;
        std::cout << geojsonwriter.write(geom) << std::endl;
    }
    else {
        // output as text/WKT
        WKTWriter writer;
        if (args.precision >= 0) {
            writer.setRoundingPrecision(args.precision);
            writer.setTrim(false);
        }
        std::cout << writer.write(geom) << std::endl;
    }
}

void GeosOp::outputGeometryList(std::vector<std::unique_ptr<const Geometry>> & list) {
    for (size_t i = 0; i < list.size(); i++) {
        outputGeometry( list[i].get() );
    }
}
