/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/geom/GeometryFactory.h>
#include <geos/geom/PrecisionModel.h>
#include <geos/profiler.h>
#include <tinyxml2.h>

using namespace geos;
using geos::geom::Geometry;

class XMLTester {

private:
    enum {
        SHOW_RUN_INFO = 1,
        SHOW_CASE,
        SHOW_TEST,
        SHOW_RESULT,
        SHOW_GEOMS,
        SHOW_GEOMS_FULL,
        PRED
    };

    void parsePrecisionModel(const tinyxml2::XMLElement* el);
    void parseRun(const tinyxml2::XMLNode* node);
    void parseCase(const tinyxml2::XMLNode* node);
    void runPredicates(const geom::Geometry* a, const geom::Geometry* b);

    void runTest(const tinyxml2::XMLNode* node);

    Geometry* gA;
    Geometry* gB;

    bool usePrepared;
    std::unique_ptr<geom::PrecisionModel> pm;
    geom::GeometryFactory::Ptr factory;
    std::unique_ptr<io::WKTReader> wktreader;
    std::unique_ptr<io::WKTWriter> wktwriter;
    std::unique_ptr<io::WKBReader> wkbreader;
    std::unique_ptr<io::WKBWriter> wkbwriter;
    tinyxml2::XMLDocument xml;

    int verbose;

    int failed;
    int succeeded;
    int caseCount;
    int testCount;
    int testLineNum;
    std::string opSignature;

    int testFileCount;
    int totalTestCount;

    const std::string* curr_file;
    std::string curr_case_desc;

    bool testValidOutput;
    bool testValidInput;
    bool sqlOutput;
    bool HEXWKB_output;


public:
    XMLTester();
    ~XMLTester();
    void run(const std::string& testFile);
    void resultSummary(std::ostream& os) const;
    void resetCounters();

    Geometry* parseGeometry(const std::string& in, const char* label = "parsed");
    std::string printGeom(const geom::Geometry* g);
    void printTest(bool success, const std::string& op, const std::string& expected_result, const std::string& actual_result);
    bool testValid(const geom::Geometry* g, const std::string& label);
    std::string testcaseRef();
    
    /*
     * Values:
     *	0: Show case description, run tests, show result
     *	1: Show parsed geometry values
     *	2: Run predicates
     *
     * Return previously set verbosity level
     */
    int setVerbosityLevel(int val);

    int isVerbose() { return verbose; }

    bool isTestValidOutput() { return testValidOutput; }

    bool isUsePrepared() { return usePrepared; }

    geom::GeometryFactory* getFactory() { return factory.get(); }

    int
    getFailuresCount()
    {
        return failed;
    }

    void
    testOutputValidity(bool val)
    {
        testValidOutput = val;
    }
    void
    testInputValidity(bool val)
    {
        testValidInput = val;
    }
    void
    setSQLOutput(bool val)
    {
        sqlOutput = val;
    }
    void
    setHEXWKBOutput(bool val)
    {
        HEXWKB_output = val;
    }

};

class Test {
private:
    XMLTester& tester;

    std::string opName;
    std::string opArg1;
    std::string opArg2;
    std::string opArg3;
    std::string opArg4;
    std::string opRes;
    std::string opSignature;

    //TODO: make these functions on XMLTester?
    int verbose;
    bool testValidOutput;

    bool isSuccess;

    std::string actual_result;
    std::string expected_result;

    void parse(const tinyxml2::XMLNode* node);
    void execute(Geometry* geomA, Geometry* geomB);
    void executeOp(Geometry* geomA, Geometry* geomB);
    void checkResult( Geometry * res );
    void checkUnionResult( Geometry * res );
    void checkResult( double res );
    void checkResult( bool res );

    static int checkBufferSuccess(Geometry const& gRes, Geometry const& gRealRes, double dist);
    static int checkSingleSidedBufferSuccess(Geometry& gRes, Geometry& gRealRes, double dist);
    static double areaDelta(const geom::Geometry* a, const geom::Geometry* b, std::string& rsltMaxDiffOp, double maxDiff, std::stringstream& ss);

public:
    Test(XMLTester& xmlTester) 
        : tester(xmlTester),
        isSuccess(false),
        actual_result("NONE")
    {
        verbose = tester.isVerbose();
        testValidOutput = tester.isTestValidOutput();
        //usePrepared = tester.isUsePrepared();
    }
    bool run(const tinyxml2::XMLNode* node, Geometry* geomA, Geometry* geomB);


};

