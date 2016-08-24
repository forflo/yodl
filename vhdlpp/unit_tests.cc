#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

// YOSYS specific headers
#include <kernel/yosys.h>
#include <kernel/rtlil.h>
#include <backends/ilang/ilang_backend.h>

// code base specific includes
#include "generate_graph.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "entity.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "stateful_lambda.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "generic_traverser.h"
#include "vtype.h"
#include "std_types.h"
#include "std_funcs.h"
#include "parse_context.h"
#include "root_class.h"
#include "mach7_includes.h"
#include "path_finder.h"
#include "predicate_generators.h"
#include "signal_extractor.h"
#include "elsif_eliminator.h"
#include "clock_edge_recognizer.h"
#include "csa_lifter.h"

#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

// Where to dump design entities
const char *work_path = "ivl_vhdl_work";
const char *dump_design_entities_path = 0;
const char *dump_libraries_path       = 0;
const char *debug_log_path            = 0;

bool verbose_flag = false;
bool debug_elaboration = false;
ofstream debug_log_file;

TEST_GROUP(FirstTestGroup)
{

};

TEST(FirstTestGroup, FirstTest){
    Expression *clock_edge_f1 = new ExpFunc(
        perm_string::literal("rising_edge"),
        { new ExpName(perm_string::literal("fooclk")) });

    Expression *clock_edge_f2 = new ExpFunc(
        perm_string::literal("falling_edge"),
        { new ExpName(perm_string::literal("foobarclk")) });

    // correct
    Expression *e1 = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpName(perm_string::literal("fnordclock")),
            new ExpCharacter('0')),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));

    // incorrect
    Expression *e2 = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::LE,
            new ExpName(perm_string::literal("fnordclock")),
            new ExpCharacter('0')
            ),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));

    // correct
    Expression *e3 = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpCharacter('1'),
            new ExpName(perm_string::literal("fnordclock"))
            ),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));

    // incorrect
    Expression *e4 = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpCharacter('0'),
            new ExpName(perm_string::literal("fnordclock"))
            ),
        new ExpObjAttribute(NULL, perm_string::literal("fneeevent"), NULL));

    Expression *complicated = new ExpArithmetic(
        ExpArithmetic::fun_t::PLUS,
        e4, e2);

    ClockEdgeRecognizer clockEdges;

    clockEdges.reset();

    clockEdges(clock_edge_f1);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::RISING);

    clockEdges.reset();

    clockEdges(clock_edge_f2);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::FALLING);

    clockEdges.reset();

    clockEdges(e1);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::FALLING);

    clockEdges.reset();

    clockEdges(e2);
    CHECK(clockEdges.containsClockEdge == false);
    CHECK(clockEdges.numberClockEdges == 0);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::UNDEF);

    clockEdges.reset();

    clockEdges(e3);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::RISING);

    clockEdges.reset();

    clockEdges(e4);
    CHECK(clockEdges.containsClockEdge == false);
    CHECK(clockEdges.numberClockEdges == 0);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::UNDEF);

    clockEdges.reset();

    clockEdges(complicated);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 2);
}

int main(int ac, char** av)
{
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return CommandLineTestRunner::RunAllTests(ac, av);
}