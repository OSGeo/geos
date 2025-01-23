/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifdef _MSC_VER
# if defined(GEOS_DEBUG_MSVC_USE_VLD) && !defined(GEOS_TEST_USE_STACKWALKER)
#  include <vld.h>
# else
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
#  include <crtdbg.h>
# endif
#pragma warning(disable : 4127)
#endif

#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/IntersectionMatrix.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>
#include <geos/geom/util/Densifier.h>
#include <geos/operation/overlay/snap/GeometrySnapper.h>
#include <geos/operation/overlayng/OverlayNG.h>
#include <geos/operation/overlayng/OverlayNGRobust.h>
#include <geos/operation/overlayng/UnaryUnionNG.h>
#include <geos/operation/buffer/BufferBuilder.h>
#include <geos/operation/buffer/BufferParameters.h>
#include <geos/operation/buffer/BufferOp.h>
#include <geos/operation/polygonize/BuildArea.h>
#include <geos/operation/valid/MakeValid.h>
#include <geos/precision/MinimumClearance.h>
#include <geos/simplify/TopologyPreservingSimplifier.h>
#include <geos/simplify/DouglasPeuckerSimplifier.h>
#include <geos/util.h>
#include <geos/util/Interrupt.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/operation/relate/RelateOp.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/linemerge/LineMerger.h>
#include <geos/profiler.h>
#include <geos/unload.h>
#include <geos/operation/valid/IsValidOp.h>
#include "XMLTester.h"
#include "BufferResultMatcher.h"
#include "SingleSidedBufferResultMatcher.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include <signal.h>

#if defined(_MSC_VER) && defined(GEOS_TEST_USE_STACKWALKER)
#include <windows.h>
#include "Stackwalker.h"
#endif

using namespace geos;
using namespace geos::operation::polygonize;
using namespace geos::operation::linemerge;
using namespace geos::geom::prep;
using std::runtime_error;
using geos::operation::overlayng::OverlayNG;
using geos::operation::overlayng::UnaryUnionNG;
using geos::operation::overlayng::OverlayNGRobust;
using operation::valid::TopologyValidationError;

namespace {

using geos::geom::Geometry;

//--------------------------------------------------

std::unique_ptr<const PreparedGeometry>
prepare(const geom::Geometry* g)
{
    return PreparedGeometryFactory::prepare(g);
}

#ifdef not_used
// a utility function defining a very simple method to indent a line of text
const char*
getIndent(unsigned int numIndents)
{
    static const char* pINDENT = "                                      + ";
    static const unsigned int LENGTH = static_cast<unsigned int>(strlen(pINDENT));

    if(numIndents > LENGTH) {
        numIndents = LENGTH;
    }

    return &pINDENT[ LENGTH - numIndents ];
}
#endif


// void dump_to_stdout( const tinyxml2::XMLNode * pParent, unsigned int indent = 0 )
// {
//     if ( !pParent ) return;

//     printf( "%s", getIndent( indent));

//     tinyxml2::XMLPrinter printer;
//     pParent->Accept(&printer);
// }


}

void
tolower(std::string& str)
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](char c){ return (char)std::tolower(c); }
        );
}

void toupper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), 
        [](char c){ return (char)std::toupper(c); }
    );
}

std::string
trimBlanks(const std::string& in)
{
    std::string out;
    std::string::size_type pos = in.find_first_not_of(" \t\n\r");
    if(pos != std::string::npos) {
        out = in.substr(pos);
    }
    pos = out.find_last_not_of(" \t\n\r");
    if(pos != std::string::npos) {
        out = out.substr(0, pos + 1);
    }
    return out;
}

std::string
normalize_filename(const std::string& str)
{
    std::string newstring;

    std::string::size_type last_slash = str.find_last_of('/', str.size());
    if(last_slash == std::string::npos) {
        newstring = str;
    }
    else {
        newstring = str.substr(last_slash + 1);
    }

    for(std::string::iterator i = newstring.begin(), e = newstring.end(); i != e; ++i) {
        if(*i == '.') {
            *i = '_';
        }
    }

    tolower(newstring);

    return newstring;
}

XMLTester::XMLTester()
    :
    gA(nullptr),
    gB(nullptr),
    pm(nullptr),
    factory(nullptr),
    wktreader(nullptr),
    wktwriter(nullptr),
    wkbreader(nullptr),
    wkbwriter(nullptr),
    failedCount(0),
    succeededCount(0),
    caseCount(0),
    testCount(0),
    testLineNum(0),
    testFileCount(0),
    totalTestCount(0),
    curr_file(nullptr),
    testValidOutput(false),
    testValidInput(false),
    sqlOutput(false),
    HEXWKB_output(false)
{
    setVerbosityLevel(0);
}

int
XMLTester::setVerbosityLevel(int value)
{
    int old_value = verbose;

    verbose = value;

    return old_value;
}

std::string 
XMLTester::testcaseRef()
{
    std::stringstream ref;
    ref << *curr_file << " (" << testLineNum << "): ";
    ref << "case " << caseCount;
    ref << ", test " << testCount;
    return ref.str();
}

/*private*/
void
XMLTester::printTestSQL(bool success, const std::string& expected_result, const std::string& actual_result)
{
    std::cout << "INSERT INTO \"" << normalize_filename(*curr_file) << "\" VALUES ("
                << caseCount << ", "
                << testCount << ", "
                //<< "'" << opSignature << "', "
                << "'" << curr_case_desc << "', ";

    std::string geomOut;

    if(gA) {
        std::cout << "'" << printGeom(gA) << "', ";
    }
    else {
        std::cout << "NULL, ";
    }
    if(gB) {
        std::cout << "'" << printGeom(gB) << "', ";
    }
    else {
        std::cout << "NULL, ";
    }
    std::cout << "'" << expected_result << "', "
                << "'" << actual_result << "', ";

    if(success) {
        std::cout << "'t'";
    }
    else {
        std::cout << "'f'";
    }
    std::cout << ");" << std::endl;
}

void
XMLTester::printTest(bool success, const std::string& op, const std::string& expected_result, const std::string& actual_result)
{
    if(sqlOutput) {
        printTestSQL(success, expected_result, actual_result);
        return;
    }
    //-- no output for quiet success
    if (success && verbose == 0)
        return;

    std::cout << testcaseRef() << ": " ;
    std::cout << op << " " << (success ? "ok." : "failed.");

    // print geometry on failure for -v
    // print geometry always for -v -v and above
    if (verbose > 1 || (verbose == 1 && !success)) {
        std::cout << "\tDescription: " << curr_case_desc << std::endl;
        if(gA) {
            std::cout << "\tGeometry A: " << printGeom(gA) << std::endl;
        }
        if(gB) {
            std::cout << "\tGeometry B: " << printGeom(gB) << std::endl;
        }
        std::cout << "\tExpected: " << expected_result << std::endl;
        std::cout << "\tActual:   " << actual_result << std::endl;
    }
    std::cout << std::endl;
}

void
XMLTester::run(const std::string& source)
{
    curr_file = &source;

    if(sqlOutput) {
        std::cout << "CREATE TABLE \"" << normalize_filename(*curr_file) << "\""
                  << "( caseno integer, testno integer, "
                  << " operation varchar, description varchar, "
                  << " a geometry, b geometry, expected geometry, "
                  << " obtained geometry, result bool )"

                  // NOTE: qgis 0.7.4 require oids for proper operations.
                  //       The 'WITH OIDS' parameter is supported back to
                  //       PostgreSQL 7.2, so if you run an older version
                  //       rebuild with the next line commented out.
                  //<< " WITH OIDS"

                  << ";" << std::endl;
    }

    ++testFileCount;

    caseCount = 0;

    tinyxml2::XMLError e = xml.LoadFile(source.c_str());
    if(e) {
        std::stringstream err;
        err << "Could not load " << source << ": " << e << std::endl;
        xml.~XMLDocument(); // Deallocates various internal pools
        throw runtime_error(err.str());
    }

    //dump_to_stdout(&xml);

    const tinyxml2::XMLNode* node = xml.FirstChildElement("run");

    if(! node) {
        throw(runtime_error("Document has no children"));
    }

    parseRun(node);
}

void
XMLTester::resultSummary(std::ostream& os) const
{
    os << "Files: " << testFileCount << std::endl;
    os << "Tests: " << totalTestCount << std::endl;
    os << "Failed: " << failedCount << std::endl;
    os << "Succeeded: " << succeededCount << std::endl;
}

void
XMLTester::resetCounters()
{
    testFileCount = totalTestCount = failedCount = succeededCount = 0;
}

void
XMLTester::parseRun(const tinyxml2::XMLNode* node)
{
    using geos::geom::PrecisionModel;

    assert(node);

    //dump_to_stdout(node);

    // Look for precisionModel element
    const tinyxml2::XMLElement* el = node->FirstChildElement("precisionModel");
    if(el) {
        parsePrecisionModel(el);
    }
    else {
        pm.reset(new PrecisionModel());
    }

    // Look for geometryOperation, if any
    usePrepared = false;
    el = node->FirstChildElement("geometryOperation");
    if(el) {
        const tinyxml2::XMLNode* txt = el->FirstChild();
        if(txt) {
            std::string op = trimBlanks(txt->Value());
            if(op.find("PreparedGeometryOperation")) {
                usePrepared = true;
            }
            else {
                std::cerr << *curr_file
                          << ": WARNING: unknown geometryOperation: "
                          << op << std::endl;
            }
        }
    }

    if(verbose > 1) {
        std::cerr << *curr_file << ": run: Precision Model: " << pm->toString();
        if(usePrepared) {
            std::cerr << " (prepared)";
        }
        std::cerr << std::endl;
    }


    factory = geom::GeometryFactory::create(pm.get());
    wktreader.reset(new io::WKTReader(factory.get()));
    wktwriter.reset(new io::WKTWriter());
    wktwriter->setTrim(true);
    wkbreader.reset(new io::WKBReader(*factory));
    wkbwriter.reset(new io::WKBWriter());

    const tinyxml2::XMLNode* casenode;
    for(casenode = node->FirstChildElement("case");
            casenode;
            casenode = casenode->NextSiblingElement("case")) {
        try {
            parseCase(casenode);
        }
        catch(const std::exception& exc) {
            std::cerr << exc.what() << std::endl;
        }
    }

}

void
XMLTester::parsePrecisionModel(const tinyxml2::XMLElement* el)
{
    using geos::geom::PrecisionModel;

    //dump_to_stdout(el);

    /* This does not seem to work... */
    std::string type;
    const char* typeStr = el->Attribute("type");
    if(typeStr) {
        type = typeStr;
    }

    const char* scaleStr = el->Attribute("scale");

    if(! scaleStr) {
        if(type == "FLOATING_SINGLE") {
            pm.reset(new PrecisionModel(PrecisionModel::FLOATING_SINGLE));
        }
        else {
            pm.reset(new PrecisionModel());
        }
    }
    else {

        char* stopstring;

        double scale = std::strtod(scaleStr, &stopstring);
        double offsetX = 0;
        double offsetY = 2;

        if(! el->QueryDoubleAttribute("offsetx", &offsetX))
        {} // std::cerr << "No offsetx" << std::endl;

        if(! el->QueryDoubleAttribute("offsety", &offsetY))
        {} // std::cerr << "No offsety" << std::endl;

        // NOTE: PrecisionModel discards offsets anyway...
        pm.reset(new PrecisionModel(scale, offsetX, offsetY));
    }
}

bool
XMLTester::testValid(const geom::Geometry* g, const std::string& label)
{
    operation::valid::IsValidOp ivo(g);
    bool valid = ivo.isValid();
    if(! valid) {
        const TopologyValidationError* err = ivo.getValidationError();
        std::cerr << *curr_file << ":"
                  << " case" << caseCount << ":"
                  << " test" << testCount << ": "
                  << " invalid geometry (" << label
                  << "): " << err->toString() << std::endl;
    }
    return valid;
}

/**
 * Parse WKT or HEXWKB
 */
geom::Geometry*
XMLTester::parseGeometry(const std::string& in, const char* label)
{
    if((! wkbreader.get()) || (! wktreader.get())) {
        throw(runtime_error("No precision model specified"));
    }

    std::stringstream is(in, std::ios_base::in);
    char first_char;

    // Remove leading spaces
    while(is.get(first_char) && std::isspace(first_char));
    is.unget();

    std::unique_ptr<geom::Geometry> ret;

    switch(first_char) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        ret = wkbreader->readHEX(is);
        break;
    default:
        ret = wktreader->read(in);
        break;
    }

    if(testValidInput) {
        testValid(ret.get(), std::string(label));
    }

    //ret->normalize();

    return ret.release();
}

void
XMLTester::parseCase(const tinyxml2::XMLNode* node)
{
    assert(node);

    std::string geomAin;
    std::string geomBin;
    std::string thrownException;

    gA = nullptr;
    gB = nullptr;

    curr_case_desc.clear();
    const tinyxml2::XMLNode* txt = node->FirstChildElement("desc");
    if(txt) {
        txt = txt->FirstChild();
        if(txt) {
            curr_case_desc = trimBlanks(txt->Value());
        }
    }
    try {
        const tinyxml2::XMLNode* el = node->FirstChildElement("a");
        geomAin = el->FirstChild()->Value();
        geomAin = trimBlanks(geomAin);
        gA = parseGeometry(geomAin, "Geometry A");

        if(nullptr != (el = node->FirstChildElement("b"))) {
            geomBin = el->FirstChild()->Value();
            geomBin = trimBlanks(geomBin);
            gB = parseGeometry(geomBin, "Geometry B");
        }
    }
    catch(const std::exception& e) {
        thrownException = e.what();
    }
    catch(...) {
        thrownException = "Unknown exception";
    }

//std::cerr << "A: " << geomAin << std::endl;
//std::cerr << "B: " << geomBin << std::endl;


    if(thrownException != "") {
        std::cout << *curr_file << ":";
        std::cout << " case" << caseCount << " (" << node->GetLineNum() << "): ";
        std::cout << " skipped (" << thrownException << ")." << std::endl;
        if (gA) delete gA;
        if (gB) delete gB;
        return;
    }

    ++caseCount;
    testCount = 0;

    const tinyxml2::XMLNode* testnode;
    for(testnode = node->FirstChildElement("test");
            testnode;
            testnode = testnode->NextSiblingElement("test")) {
       runTest(testnode);
    }

    totalTestCount += testCount;

    delete gA;
    delete gB;
}

std::string
XMLTester::printGeom(const geom::Geometry* g)
{
    if(HEXWKB_output) {
        std::stringstream s(std::ios_base::binary | std::ios_base::in | std::ios_base::out);
        wkbwriter->write(*g, s);
        std::stringstream s2;
        wkbreader->printHEX(s, s2);
        return s2.str();
    }
    else {
        wktwriter->setRoundingPrecision(16);
        return wktwriter->write(g);
    }
}

void
XMLTester::runTest(const tinyxml2::XMLNode* node)
{
    Test test(*this); 
    ++testCount;
    testLineNum = node->GetLineNum();

    bool success = test.run(node, gA, gB);

    if(success) {
        ++succeededCount;
    }
    else {
        ++failedCount;
    }
}

void
XMLTester::runPredicates(const geom::Geometry* p_gA, const geom::Geometry* p_gB)
{
    std::cout << "\t    Equals:\tAB=" << (p_gA->equals(p_gB) ? "T" : "F") << ", BA=" << (p_gB->equals(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t  Disjoint:\tAB=" << (p_gA->disjoint(p_gB) ? "T" : "F") << ", BA=" << (p_gB->disjoint(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\tIntersects:\tAB=" << (p_gA->intersects(p_gB) ? "T" : "F") << ", BA=" << (p_gB->intersects(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t   Touches:\tAB=" << (p_gA->touches(p_gB) ? "T" : "F") << ", BA=" << (p_gB->touches(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t   Crosses:\tAB=" << (p_gA->crosses(p_gB) ? "T" : "F") << ", BA=" << (p_gB->crosses(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t    Within:\tAB=" << (p_gA->within(p_gB) ? "T" : "F") << ", BA=" << (p_gB->within(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t  Contains:\tAB=" << (p_gA->contains(p_gB) ? "T" : "F") << ", BA=" << (p_gB->contains(
                  p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t  Overlaps:\tAB=" << (p_gA->overlaps(p_gB) ? "T" : "F") << ", BA=" << (p_gB->overlaps(
                  p_gA) ? "T" : "F") << std::endl;

    std::cout << "\t  Prepared Disjoint:\tAB=" << (prepare(p_gA)->disjoint(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->disjoint(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\tPrepared Intersects:\tAB=" << (prepare(p_gA)->intersects(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->intersects(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t   Prepared Touches:\tAB=" << (prepare(p_gA)->touches(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->touches(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t   Prepared Crosses:\tAB=" << (prepare(p_gA)->crosses(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->crosses(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t    Prepared Within:\tAB=" << (prepare(p_gA)->within(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->within(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t  Prepared Contains:\tAB=" << (prepare(p_gA)->contains(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->contains(p_gA) ? "T" : "F") << std::endl;
    std::cout << "\t Prepared Overlaps:\tAB=" << (prepare(p_gA)->overlaps(p_gB) ? "T" : "F") << ", BA=" << (prepare(
                  p_gB)->overlaps(p_gA) ? "T" : "F") << std::endl;
}

XMLTester::~XMLTester()
{
}
//============================================================================

/* static */
bool
Test::checkOverlaySuccess(Geometry const& gExpected, Geometry const& gActual)
{
    double tol = operation::overlay::snap::GeometrySnapper::computeSizeBasedSnapTolerance(gExpected);
    //-- BUG: this allows all empties to test equal
    if(gExpected.equals(&gActual)) {
        return 1;
    }
    //TODO: is this needed by any tests?
    std::cerr << "Using an overlay tolerance of " << tol << std::endl;
    if(gExpected.equalsExact(&gActual, tol)) {
        return 1;
    }
    return 0;
}

/* static */
bool
Test::checkBufferSuccess(Geometry const& gExpected, Geometry const& gActual, double dist)
{
    using geos::xmltester::BufferResultMatcher;

    if(gExpected.getGeometryTypeId() != gActual.getGeometryTypeId()) {
        std::cerr << "Expected result is of type "
                    << gExpected.getGeometryType()
                    << "; obtained result is of type "
                    << gActual.getGeometryType()
                    << std::endl;
        return false;
    }
    // Is a buffer always an area ?
    if(gExpected.getDimension() != 2) {
        std::cerr << "Don't know how to validate "
                    << "result of buffer operation "
                    << "when expected result is not an "
                    << "areal type."
                    << std::endl;
    }
    if(!BufferResultMatcher::isBufferResultMatch(gActual, gExpected, dist)) {
        std::cerr << "BufferResultMatcher FAILED" << std::endl;
        return false;
    }
    return true;
}

/* static */
bool
Test::checkSingleSidedBufferSuccess(geom::Geometry& gExpected,
                              geom::Geometry& gActual, double dist)
{
    if(gExpected.getGeometryTypeId() != gActual.getGeometryTypeId()) {
        std::cerr << "Expected result is of type "
                    << gExpected.getGeometryType()
                    << "; obtained result is of type "
                    << gActual.getGeometryType()
                    << std::endl;
        return false;
    }
    geos::xmltester::SingleSidedBufferResultMatcher matcher;
    if(! matcher.isBufferResultMatch(gActual,
                                        gExpected,
                                        dist)) {
        std::cerr << "SingleSidedBufferResultMatcher FAILED" << std::endl;
        return false;
    }
    return true;
}

/**
* Computes the maximum area delta value
* resulting from identity equations over the overlay operations.
* The delta value is normalized to the total area of the geometries.
* If the overlay operations are computed correctly
* the area delta is expected to be very small (e.g. < 1e-6).
*/
/* static */
double
Test::areaDelta(const geom::Geometry* a, const geom::Geometry* b, std::string& rsltMaxDiffOp, double maxDiff, std::stringstream& ss)
{
    double areaA = a == nullptr ? 0 : a->getArea();
    double areaB = b == nullptr ? 0 : b->getArea();

    // if an input is non-polygonal delta is 0
    if (areaA == 0 || areaB == 0)
      return 0;

    std::unique_ptr<geom::Geometry> geomU = OverlayNGRobust::Union(a, b);
    std::unique_ptr<geom::Geometry> geomI = OverlayNGRobust::Intersection(a, b);
    std::unique_ptr<geom::Geometry> geomDab = OverlayNGRobust::Difference(a, b);
    std::unique_ptr<geom::Geometry> geomDba = OverlayNGRobust::Difference(b, a);
    std::unique_ptr<geom::Geometry> geomSD = OverlayNGRobust::SymDifference(a, b);

    double areaU   = geomU->getArea();
    double areaI   = geomI->getArea();
    double areaDab = geomDab->getArea();
    double areaDba = geomDba->getArea();
    double areaSD  = geomSD->getArea();


    double maxDelta = 0;

    // & : intersection
    // - : difference
    // + : union
    // ^ : symdifference

    double delta = std::abs(areaA - areaI - areaDab);
    if (delta > maxDelta) {
        rsltMaxDiffOp = "A = ( A & B ) + ( A - B )";
        maxDelta = delta;
    }

    delta = std::abs(areaB - areaI - areaDba);
    if (delta > maxDelta) {
        rsltMaxDiffOp = "B = ( A & B ) + ( B - A )";
        maxDelta = delta;
    }

    delta = std::abs(areaDab + areaDba - areaSD);
    if (delta > maxDelta) {
        maxDelta = delta;
        rsltMaxDiffOp = "( A ^ B ) = ( A - B ) + ( B - A )";
    }

    delta = std::abs(areaI + areaSD - areaU);
    if (delta > maxDelta) {
        maxDelta = delta;
        rsltMaxDiffOp = "( A + B ) = ( A & B ) + ( A ^ B )";
    }

    delta = std::abs(areaU - areaI - areaDab - areaDba);
    if (delta > maxDelta) {
        maxDelta = delta;
        rsltMaxDiffOp = "( A + B ) = ( A & B ) + ( A - B ) + ( A - B )";
    }

    // normalize the area delta value
    double diffScore = maxDelta / (areaA + areaB);
    if (diffScore > maxDiff) {
        ss << std::endl << "A" << std::endl;
        ss << *a;
        ss << std::endl << "B" << std::endl;
        ss << *b;
        ss << std::endl << "geomU" << std::endl;
        ss << *geomU;
        ss << std::endl << "geomI" << std::endl;
        ss << *geomI;
        ss << std::endl << "geomDab" << std::endl;
        ss << *geomDab;
        ss << std::endl << "geomDba" << std::endl;
        ss << *geomDba;
        ss << std::endl << "geomSD" << std::endl;
        ss << *geomSD;
        ss << std::endl;
    }
    return diffScore;
}

void
Test::checkResult( const Geometry& result ) 
{
    checkResult( result, 
    [](Geometry& expected, Geometry& actual) -> bool {
        //TODO: change to equalsExact, since compareTo doesn't check empty type
        return expected.compareTo(&actual) == 0;
    });
}

void
Test::checkResult( const Geometry& result, 
    std::function<bool(Geometry& expected, Geometry& actual)> isMatch ) 
{
    std::string expectedRes = opResult;
    std::unique_ptr<Geometry> gExpectedRes(tester.parseGeometry(expectedRes, "expected"));
    gExpectedRes->normalize();

    std::unique_ptr<Geometry> gActualRes(result.clone());
    gActualRes->normalize();

    isSuccess = isMatch(*gExpectedRes, *gActualRes);

    if (testValidOutput) {
        isSuccess &= tester.testValid(gActualRes.get(), "result");
    }
    actualResultStr = tester.printGeom(gActualRes.get());
}

//TODO: remove this hack when tests are fixed.  Only used for union, and has a bug where empties test equal
void
Test::checkUnionResult( const Geometry& result ) 
{
    checkResult( result, 
        [](Geometry& expected, Geometry& actual) -> bool {
            return checkOverlaySuccess(expected, actual);
    });
}

void
Test::checkResult( bool result ) 
{
    actualResultStr = result ? "true" : "false";
    if (actualResultStr == opResult) {
        isSuccess = true;
    }
}

void
Test::checkResult( double result) 
{
    char* rest;
    double expectedRes = std::strtod(opResult.c_str(), &rest);
    if(rest == opResult.c_str()) {
        throw std::runtime_error("malformed testcase: missing expected double value");
    }
    if (expectedRes == 0.0) {
        if (result == 0.0) {
            isSuccess = true;
        }
    }
    else {
        if (std::abs(expectedRes - result) / expectedRes < 1e-3) {
            isSuccess = true;
        }
    }
    std::stringstream ss;
    ss << result;
    actualResultStr = ss.str();
}

void Test::parse(const tinyxml2::XMLNode* node) {
    const tinyxml2::XMLNode* opnode = node->FirstChildElement("op");
    if(! opnode) {
        throw(runtime_error("test has no op"));
    }

    //dump_to_stdout(opnode);

    const tinyxml2::XMLElement* opel = opnode->ToElement();

    const char* tmp = opel->Attribute("name");
    if(tmp) {
        opName = tmp;
    }

    tmp = opel->Attribute("arg1");
    if(tmp) {
        opArg1 = tmp;
        toupper(opArg1);
    }

    tmp = opel->Attribute("arg2");
    if(tmp) {
        opArg2 = tmp;
        toupper(opArg2);
    }

    tmp = opel->Attribute("arg3");
    if(tmp) {
        opArg3 = tmp;
    }

    tmp = opel->Attribute("arg4");
    if(tmp) {
        opArg4 = tmp;
    }

    const tinyxml2::XMLNode* resnode = opnode->FirstChild();
    if(! resnode) {
        std::stringstream p_tmp;
        p_tmp << tester.testcaseRef() << ": op has no expected result child";
        throw(runtime_error(p_tmp.str()));
    }
    opResult = resnode->Value();

    // trim blanks
    opResult = trimBlanks(opResult);
    opName = trimBlanks(opName);
    tolower(opName);

    std::string opSig = "";

    if(opArg1 != "") {
        opSig = opArg1;
    }
    if(opArg2 != "") {
        if(opSig != "") {
            opSig += ", ";
        }
        opSig += opArg2;
    }
    if(opArg3 != "") {
        if(opSig != "") {
            opSig += ", ";
        }
        opSig += opArg3;
    }
    if(opArg4 != "") {
        if(opSig != "") {
            opSig += ", ";
        }
        opSig += opArg4;
    }
    opSignature = opName + "(" + opSig + ")";
}

bool Test::run(const tinyxml2::XMLNode* node, Geometry* geomA, Geometry* geomB)
{
    parse(node);
    Geometry* argA = geomA;
    Geometry* argB = geomB;
    //-- switch geom args if specified
    if (opArg1 == "B") {
        argA = geomB;
        argB = geomA;
    }
    execute(argA, argB);
    tester.printTest(isSuccess, opSignature, opResult, actualResultStr);
    return isSuccess;
}

void Test::execute(Geometry* geomA, Geometry* geomB)
{
    try {
        executeOp(geomA, geomB);
    }
    catch(const std::exception& e) {
        if (opResult == "exception") {
            isSuccess = true;
            actualResultStr = "exception";
        }
        else {
            std::cerr << "EXCEPTION in " << tester.testcaseRef() << ": " << e.what()
                      << std::endl;
            actualResultStr = e.what();
        }
    }
    catch(...) {
        std::cerr << "Unknown EXCEPTION in "
                  << tester.testcaseRef()
                  << std::endl;
        actualResultStr = "Unknown exception thrown";
    }
}

void Test::executeOp(Geometry* gA, Geometry* gB)
{        
    if(opName == "relate") {
        std::unique_ptr<geom::IntersectionMatrix> im(gA->relate(gB));
        checkResult( im->matches(opArg3) );
    }
    else if(opName == "isvalid") {
        checkResult( gA->isValid() );
    }
    else if(opName == "intersection" || opName == "intersectionng"
            //TODO: remove this opname by fixing test
            || opName == "intersectionsin") {
        checkResult( *(gA->intersection(gB)) );
    }
    //-- in current GEOS all overlay is OverlayNG
    else if(opName == "union"|| opName == "unionng") {
        if (gB) {
            checkUnionResult( *gA->Union(gB) );
        }
        else {
            checkUnionResult( *gA->Union() );
        }
    }
    else if(opName == "difference" || opName == "differenceng") {
        checkResult( *gA->difference(gB) );
    }
    else if(opName == "symdifference" || opName == "symdifferenceng") {
        checkResult( *gA->symDifference(gB) );
    }
    else if(opName == "intersectionsr") {
        double precision = 1.0;
        if(opArg3 != "") {
            precision = std::atof(opArg3.c_str());
        }
        geom::PrecisionModel precMod(precision);
        std::unique_ptr<Geometry> res = OverlayNG::overlay(gA, gB, OverlayNG::INTERSECTION, &precMod);
        checkResult( *res );
    }
    else if(opName == "unionsr") {
        double precision = 1.0;
        std::unique_ptr<Geometry> res;
        if (gB) {
            geom::PrecisionModel precMod(precision);
            if(opArg3 != "") {
                precision = std::atof(opArg3.c_str());
            }
            res = OverlayNG::overlay(gA, gB, OverlayNG::UNION, &precMod);
        }
        else {
            if(opArg2 != "") {
                precision = std::atof(opArg2.c_str());
            }
            geom::PrecisionModel precMod(precision);
            // gRealRes = OverlayNG::geomunion(gA, &precMod);
            res = UnaryUnionNG::Union(gA, precMod);
        }
        checkResult( *res );
    }
    else if(opName == "differencesr") {
        double precision = 1.0;
        if(opArg3 != "") {
            precision = std::atof(opArg3.c_str());
        }
        geom::PrecisionModel precMod(precision);
        std::unique_ptr<Geometry> res = OverlayNG::overlay(gA, gB, OverlayNG::DIFFERENCE, &precMod);
        checkResult( *res );
    }
    else if(opName == "symdifferencesr") {
        double precision = 1.0;
        if(opArg3 != "") {
            precision = std::atof(opArg3.c_str());
        }
        geom::PrecisionModel precMod(precision);
        std::unique_ptr<Geometry> res = OverlayNG::overlay(gA, gB, OverlayNG::SYMDIFFERENCE, &precMod);
        checkResult( *res );
    }
    else if(opName == "densify") {
        double distanceTolerance = std::atof(opArg2.c_str());
        geom::util::Densifier den(gA);
        den.setDistanceTolerance(distanceTolerance);
        checkResult( *den.getResultGeometry() );
    }
    else if(opName == "intersects") {
        bool res = tester.isPrepared() ? prepare(gA)->intersects(gB) : gA->intersects(gB);
        checkResult( res );
    }
    else if(opName == "contains") {
        bool res = tester.isPrepared() ? prepare(gA)->contains(gB) : gA->contains(gB);
        checkResult( res );
    }
    else if(opName == "overlaps") {
        bool res = tester.isPrepared() ? prepare(gA)->overlaps(gB) : gA->overlaps(gB);
        checkResult( res );
    }
    else if(opName == "within") {
        bool res = tester.isPrepared() ? prepare(gA)->within(gB) : gA->within(gB);
        checkResult( res );
    }
    else if(opName == "touches") {
        bool res = tester.isPrepared() ? prepare(gA)->touches(gB) : gA->touches(gB);
        checkResult( res );
    }
    else if(opName == "crosses") {
        bool res = tester.isPrepared() ? prepare(gA)->crosses(gB) : gA->crosses(gB);
        checkResult( res );
    }
    else if(opName == "disjoint") {
        bool res = tester.isPrepared() ? prepare(gA)->disjoint(gB) : gA->disjoint(gB);
        checkResult( res );
    }
    else if(opName == "covers") {
        bool res = tester.isPrepared() ? prepare(gA)->covers(gB) : gA->covers(gB);
        checkResult( res );
    }
    else if(opName == "coveredby") {
        bool res = tester.isPrepared() ? prepare(gA)->coveredBy(gB) : gA->coveredBy(gB);
        checkResult( res );
    }
    else if(opName == "equalstopo") {
        // equalsTopo() is synonym for equals() in JTS
        checkResult( gA->equals(gB));
    }
    else if(opName == "equalsexact") {
        checkResult( gA->equalsExact(gB));
    }
    else if(opName == "equalsnorm") {
        // rather than implementing equalsnorm in the library,
        // we just do it in this one test case for now
        // NOTE: mutates A and B !!!!
        gA->normalize();
        gB->normalize();
        checkResult( gA->equalsExact(gB));
    }
    else if(opName == "getboundary") {
        checkResult( *gA->getBoundary() );
    }
    else if(opName == "getcentroid") {
        checkResult( *gA->getCentroid() );
    }
    else if(opName == "issimple") {
        checkResult( gA->isSimple() );
   }
    else if(opName == "convexhull") {
        checkResult( *gA->convexHull() );
    }
    else if(opName == "buffer") {
        double dist = std::atof(opArg2.c_str());

        operation::buffer::BufferParameters params;
        if(opArg3 != "") {
            params.setQuadrantSegments(std::atoi(opArg3.c_str()));
        }
        operation::buffer::BufferOp op(gA, params);
        std::unique_ptr<Geometry> result = op.getResultGeometry(dist);
        checkResult( *result, 
            [dist](Geometry& expected, Geometry& actual) -> bool {
                return checkBufferSuccess(expected, actual, dist);
            });
    }
    else if(opName == "buffermitredjoin") {
        double dist = std::atof(opArg2.c_str());

        operation::buffer::BufferParameters params;
        params.setJoinStyle(operation::buffer::BufferParameters::JOIN_MITRE);
        if(opArg3 != "") {
            params.setQuadrantSegments(std::atoi(opArg3.c_str()));
        }
        operation::buffer::BufferOp op(gA, params);
        std::unique_ptr<Geometry> result = op.getResultGeometry(dist);

        checkResult( *result, 
            [dist](Geometry& expected, Geometry& actual) -> bool {
                return checkBufferSuccess(expected, actual, dist);
            });
    }
    else if(opName == "buffersinglesided") {
        double dist = std::atof(opArg2.c_str());

        operation::buffer::BufferParameters params;
        params.setJoinStyle(operation::buffer::BufferParameters::JOIN_ROUND);
        if(opArg3 != "") {
            params.setQuadrantSegments(std::atoi(opArg3.c_str()));
        }
        bool leftSide = true ;
        if(opArg4 == "right") {
            leftSide = false ;
        }
        operation::buffer::BufferBuilder bufBuilder(params) ;
        std::unique_ptr<Geometry> result = bufBuilder.bufferLineSingleSided(gA, dist, leftSide);
        checkResult( *result, 
            [dist](Geometry& expected, Geometry& actual) -> bool {
                return checkSingleSidedBufferSuccess(expected, actual, dist);
            });
    }
    else if(opName == "getinteriorpoint") {
        //-- may return null
        std::unique_ptr<Geometry> res(gA->getInteriorPoint());
        if (! res.get()) {
            res = tester.getFactory()->createPoint();
        }
        checkResult( *res );
    }
    else if(opName == "iswithindistance") {
        double dist = std::atof(opArg3.c_str());
        checkResult( gA->isWithinDistance(gB, dist) );
    }
    else if(opName == "polygonize") {
        Polygonizer plgnzr;
        plgnzr.add(gA);
        auto polys = plgnzr.getPolygons();
        std::unique_ptr<Geometry> res(tester.getFactory()->createGeometryCollection(std::move(polys)));
        checkResult( *res );
    }
    else if(opName == "linemerge") {
        LineMerger merger;
        merger.add(gA);
        auto lines = merger.getMergedLineStrings();
        std::unique_ptr<Geometry> res(tester.getFactory()->createGeometryCollection(std::move(lines)));
        checkResult( *res );
    }
    else if(opName == "overlayareatest") {
        std::string maxDiffOp;
        std::stringstream p_tmp;
        double maxDiff = 1e-6;
        double areaDiff = areaDelta(gA, gB, maxDiffOp, maxDiff, p_tmp);
        
        // Debug output of actual geometries returned
        if (areaDiff < maxDiff && false) {
            std::cout << p_tmp.str();
        }
        checkResult(areaDiff < maxDiff);
    }
    else if(opName == "unionlength") {
        checkResult( OverlayNGRobust::Union(gA)->getLength() );
    }
    else if(opName == "unionarea") {
        checkResult( OverlayNGRobust::Union(gA)->getArea() );
    }
    else if(opName == "areatest") {
        char* rest;
        double toleratedDiff = std::strtod(opResult.c_str(), &rest);
        bool validOut = true;

        if(rest == opResult.c_str()) {
            throw std::runtime_error("malformed testcase: missing tolerated area difference in 'areatest' op");
        }

        std::unique_ptr<Geometry> gI(gA->intersection(gB));
        if(testValidOutput) {
            validOut &= tester.testValid(gI.get(), "areatest intersection");
        }

        std::unique_ptr<Geometry> gDab(gA->difference(gB));
        if(testValidOutput) {
            validOut &= tester.testValid(gI.get(), "areatest difference(a,b)");
        }

        std::unique_ptr<Geometry> gDba(gB->difference(gA));
        if(testValidOutput) {
            validOut &= tester.testValid(gI.get(), "areatest difference(b,a)");
        }

        std::unique_ptr<Geometry> gSD(gA->symDifference(gB));
        if(testValidOutput) {
            validOut &= tester.testValid(gI.get(), "areatest symdifference");
        }

        std::unique_ptr<Geometry> gU(gA->Union(gB));

        double areaA = gA->getArea();
        double areaB = gB->getArea();
        double areaI = gI->getArea();
        double areaDab = gDab->getArea();
        double areaDba = gDba->getArea();
        double areaSD = gSD->getArea();
        double areaU = gU->getArea();

        double maxdiff = 0;
        std::string maxdiffop;

        // @ : symdifference
        // - : difference
        // + : union
        // ^ : intersection

        // A == ( A ^ B ) + ( A - B )
        double diff = std::fabs(areaA - areaI - areaDab);
        if(diff > maxdiff) {
            maxdiffop = "A == ( A ^ B ) + ( A - B )";
            maxdiff = diff;
        }

        // B == ( A ^ B ) + ( B - A )
        diff = std::fabs(areaB - areaI - areaDba);
        if(diff > maxdiff) {
            maxdiffop = "B == ( A ^ B ) + ( B - A )";
            maxdiff = diff;
        }

        //  ( A @ B ) == ( A - B ) + ( B - A )
        diff = std::fabs(areaDab + areaDba - areaSD);
        if(diff > maxdiff) {
            maxdiffop = "( A @ B ) == ( A - B ) + ( B - A )";
            maxdiff = diff;
        }

        //  ( A u B ) == ( A ^ B ) + ( A @ B )
        diff = std::fabs(areaI + areaSD - areaU);
        if(diff > maxdiff) {
            maxdiffop = "( A u B ) == ( A ^ B ) + ( A @ B )";
            maxdiff = diff;
        }

        if(maxdiff <= toleratedDiff) {
            isSuccess = validOut;
        }

        std::stringstream p_tmp;
        p_tmp << maxdiffop << ": " << maxdiff;
        actualResultStr = p_tmp.str();
    }
    else if(opName == "distance") {
        checkResult( gA->distance(gB) );
    }
    else if(opName == "minclearance") {
        precision::MinimumClearance mc(gA);
        double minclearanceO = mc.getDistance();
        // Hack for Inf/1.7976931348623157E308 comparison
        if(minclearanceO > 1.7976931348623157E308) {
            minclearanceO = 1.7976931348623157E308;
        }
        checkResult( minclearanceO );
    }
    else if(opName == "minclearanceline") {
        precision::MinimumClearance mc(gA);
        //TODO: could use a checkResult with a tolerance?
        checkResult( *mc.getLine() );
    }
    else if (opName == "buildarea")
    {
        checkResult( *BuildArea().build(gA) );
    }
    else if (opName == "makevalid")
    {
        checkResult( *geos::operation::valid::MakeValid().build(gA) );
    }
    else if(opName == "simplifydp")
    {
        double tolerance = std::atof(opArg2.c_str());
        checkResult( *geos::simplify::DouglasPeuckerSimplifier::simplify(gA, tolerance) );
    }
    else if(opName == "simplifytp")
    {
        double tolerance = std::atof(opArg2.c_str());
        checkResult( *geos::simplify::TopologyPreservingSimplifier::simplify(gA, tolerance) );
    }
    else {
        //TODO: error out here?
        std::cerr << tester.testcaseRef() << " - " << opName;
        std::cerr << ": skipped (unrecognized)." << std::endl;
    }
}

//==================================================================================

static void
usage(char* me, int exitcode, std::ostream& os)
{
    os << "Usage: " << me << " [options] <test> [<test> ...]" << std::endl;
    os << "Options: " << std::endl;
    os << " -v                  Verbose mode "
       << "(multiple -v increment verbosity)" << std::endl
       << "--test-valid-output  Test output validity" << std::endl
       << "--test-valid-input   Test input validity" << std::endl
       << "--sql-output         Produce SQL output" << std::endl
       << "--wkb-output         Print Geometries as HEXWKB" << std::endl;

    std::exit(exitcode);
}

void
request_interrupt(int)
{
    geos::util::Interrupt::request();
}

int
main(int argC, char* argV[])
{
    int verbose = 0;
    bool sql_output = false;

#if defined(_MSC_VER) && defined(GEOS_TEST_USE_STACKWALKER)
    InitAllocCheck();
    {
#endif

        if(argC < 2) {
            usage(argV[0], 1, std::cerr);
        }

        signal(15, request_interrupt);

        XMLTester tester;
        tester.setVerbosityLevel(verbose);

        for(int i = 1; i < argC; ++i) {
            // increment verbosity level
            if(! std::strcmp(argV[i], "-v")) {
                ++verbose;
                tester.setVerbosityLevel(verbose);
                continue;
            }
            if(! std::strcmp(argV[i], "--test-valid-output")) {
                tester.setTestOutputValidity(true);
                continue;
            }
            if(! std::strcmp(argV[i], "--sql-output")) {
                sql_output = true;
                tester.setSQLOutput(sql_output);
                continue;
            }
            if(! std::strcmp(argV[i], "--wkb-output")) {
                sql_output = true;
                tester.setHEXWKBOutput(sql_output);
                continue;
            }
            if(! std::strcmp(argV[i], "--test-valid-input")) {
                tester.setTestInputValidity(true);
                continue;
            }

            std::string source = argV[i];

            try {
                tester.run(source);
            }
            catch(const std::exception& exc) {
                std::cerr << exc.what() << std::endl;
            }
        }

        if(! sql_output) {
            tester.resultSummary(std::cout);
        }
        else {
            tester.resultSummary(std::cerr);
        }

        io::Unload::Release();

        return tester.getFailuresCount();

#if defined(_MSC_VER) && defined(GEOS_TEST_USE_STACKWALKER)
    }
    DeInitAllocCheck();
#endif
}

