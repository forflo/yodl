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
#include "sync_condition_predicate.h"
#include "propcalc.h"

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

TEST_GROUP(FirstTestGroup) {};

TEST_GROUP(Propcalc){};
TEST_GROUP(SyncPredicate){};


TEST(SyncPredicate, SecondTest){
    // semantically correct clock edge
    Expression *clockEdge = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpCharacter('1'),
            new ExpName(perm_string::literal("fnordclock"))),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));

    // ((1 = 0 and 0 = 1) or ((foo = '0') and (clk = '1' and clk'event)))
    Expression *condition =
        new ExpLogical(
            ExpLogical::fun_t::OR,

            new ExpLogical(
                ExpLogical::fun_t::AND,
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpInteger(1),
                    new ExpInteger(0)),
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpInteger(0),
                    new ExpInteger(1))),

            new ExpLogical(
                ExpLogical::fun_t::AND,
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpName(perm_string::literal("foo")),
                    new ExpCharacter('0')),
                clockEdge));

    // ((foo = '0') and (clk = '1' and clk'event))
    Expression *syncCondition =
        new ExpLogical(
            ExpLogical::fun_t::AND,
            new ExpRelation(
                ExpRelation::fun_t::GT,
                new ExpName(perm_string::literal("foo")),
                new ExpCharacter('0')),
            clockEdge);


    bool result;

    SyncCondPredicate syncPred2(NULL, NULL);
    result = syncPred2(syncCondition);
    CHECK(result == true);

    SyncCondPredicate syncPred(NULL, NULL);
    result = syncPred(condition);
    CHECK(result == false);

    delete condition;
};

// convert an Expression into a propositional calculus formula
TEST(SyncPredicate, FirstTest){
    // correct
    Expression *e3 = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpCharacter('1'),
            new ExpName(perm_string::literal("fnordclock"))
            ),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));


    Expression *e3_left= new ExpLogical(
        ExpLogical::fun_t::OR,
        e3,
        new ExpRelation(
            ExpRelation::fun_t::GT,
            new ExpName(perm_string::literal("fnordclock")),
            new ExpCharacter('0')));

    ClockEdgeRecognizer cer;
    cer(e3);

    SyncCondPredicate s(NULL, NULL);
    PropcalcFormula *r = s.fromExpression(
        dynamic_cast<const Expression *>(
            cer.fullClockSpecs[cer.numberClockEdges - 1]), e3);
    CHECK(PropcalcApi::fromPropcalc(r) == "CLK");

    SyncCondPredicate e3_left_s(NULL, NULL);
    PropcalcFormula *r1 = e3_left_s.fromExpression(e3, e3_left);
    CHECK(PropcalcApi::fromPropcalc(r1) == "(CLK | V0)");

    delete r;
    delete r1;

    Expression *clockE = new ExpLogical(
        ExpLogical::fun_t::AND,
        new ExpRelation(
            ExpRelation::fun_t::EQ,
            new ExpCharacter('1'),
            new ExpName(perm_string::literal("fnordclock"))
            ),
        new ExpObjAttribute(NULL, perm_string::literal("event"), NULL));

    // not ((1 = 0 and 0 = 1) or ((foo = '0') and rising_edge(clk)))
    Expression *e3_right =
        new ExpLogical(
            ExpLogical::fun_t::OR,

            new ExpLogical(
                ExpLogical::fun_t::AND,
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpInteger(1),
                    new ExpInteger(0)),
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpInteger(0),
                    new ExpInteger(1))),

            new ExpLogical(
                ExpLogical::fun_t::AND,
                new ExpRelation(
                    ExpRelation::fun_t::GT,
                    new ExpName(perm_string::literal("foo")),
                    new ExpCharacter('0')),
                clockE));

    SyncCondPredicate complex(NULL, NULL);
    PropcalcFormula *r2 = complex.fromExpression(clockE, e3_right);

    stringstream s2;
    s2 << r2;
    CHECK(s2.str() == "((V0 & V1) | (V2 & CLK))");

    delete r2;

    delete e3_right;
    delete e3_left;
};

TEST(Propcalc, FirstTest){
    PropcalcFormula *n = new PropcalcTerm(
        new PropcalcTerm(
            new PropcalcVar("A"),
            PropcalcTerm::AND,
            new PropcalcVar("B")),
        PropcalcTerm::OR,
        new PropcalcTerm(
            new PropcalcVar("B"),
            PropcalcTerm::IFTHEN,
            new PropcalcVar("A")));

    PropcalcFormula *n2 = new PropcalcTerm(
        new PropcalcConstant(true),
        PropcalcTerm::OR,
        new PropcalcTerm(
            new PropcalcVar("B"),
            PropcalcTerm::IFTHEN,
            new PropcalcVar("A")));

    PropcalcFormula *n3 = new PropcalcTerm(
        new PropcalcVar("D"),
        PropcalcTerm::OR,
        new PropcalcTerm(
            new PropcalcVar("C"),
            PropcalcTerm::OR,
            new PropcalcTerm(
                new PropcalcVar("B"),
                PropcalcTerm::IFTHEN,
                new PropcalcVar("A"))));

    PropcalcFormula *real =
        new PropcalcTerm(
            new PropcalcTerm(
                new PropcalcTerm(
                    new PropcalcVar("CLOCK"),
                    PropcalcTerm::AND,
                    new PropcalcVar("B")),
                PropcalcTerm::XNOR,
                new PropcalcConstant(true)),
            PropcalcTerm::IFTHEN,
            new PropcalcTerm(
                new PropcalcVar("CLOCK"),
                PropcalcTerm::XNOR,
                new PropcalcConstant(true)));

    auto mapping = std::map<string, bool>{{"A", true}, {"B", false}};
    CHECK(PropcalcApi::evaluate(n, mapping) == true);

    CHECK(PropcalcApi::prove(n3) == false);

    CHECK(PropcalcApi::prove(n) == false);
    CHECK(PropcalcApi::prove(n2) == true);

    stringstream a;
    a << real;
    CHECK(a.str() == "(((CLOCK & B) <-> true) -> (CLOCK <-> true))");
    CHECK(PropcalcApi::prove(real) == true);

    stringstream s;
    s << n3;
    CHECK(s.str() == "(D | (C | (B -> A)))");

    delete n;
    delete n2;
    delete n3;
    delete real;
};

IGNORE_TEST(FirstTestGroup, FirstTest){
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
        e3, e1);

    ClockEdgeRecognizer clockEdges;

    clockEdges.reset();

    clockEdges(clock_edge_f1);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::RISING);
    CHECK(clockEdges.fullClockSpecs.size() == 1 &&
          clockEdges.fullClockSpecs[0] == clock_edge_f1);

    clockEdges.reset();

    clockEdges(clock_edge_f2);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::FALLING);
    CHECK(clockEdges.fullClockSpecs.size() == 1 &&
          clockEdges.fullClockSpecs[0] == clock_edge_f2);

    clockEdges.reset();

    clockEdges(e1);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::FALLING);
    CHECK(clockEdges.fullClockSpecs.size() == 1 &&
          clockEdges.fullClockSpecs[0] == e1);

    clockEdges.reset();

    clockEdges(e2);
    CHECK(clockEdges.containsClockEdge == false);
    CHECK(clockEdges.numberClockEdges == 0);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::UNDEF);
    CHECK(clockEdges.fullClockSpecs.size() == 0);

    clockEdges.reset();

    clockEdges(e3);
    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 1);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::RISING);
    CHECK(clockEdges.fullClockSpecs.size() == 1 &&
          clockEdges.fullClockSpecs[0] == e3);

    clockEdges.reset();

    clockEdges(e4);
    CHECK(clockEdges.containsClockEdge == false);
    CHECK(clockEdges.numberClockEdges == 0);
    CHECK(clockEdges.direction == NetlistGenerator::edge_spec::UNDEF);
    CHECK(clockEdges.fullClockSpecs.size() == 0);

    clockEdges.reset();

    // TODO: requires GenericTraverser
    GenericTraverser traverser(
        makeTypePredicate<Expression>(),
        static_cast<function<int (const AstNode *)>>(
            [&clockEdges](const AstNode *n) -> int {
                return clockEdges(n); }),
        GenericTraverser::RECUR);

    traverser(complicated);

    CHECK(clockEdges.containsClockEdge == true);
    CHECK(clockEdges.numberClockEdges == 2);

    delete e2;
    delete e4;

    delete complicated;

    delete clock_edge_f1;
    delete clock_edge_f2;
}

int main(int ac, char** av)
{
//    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    return CommandLineTestRunner::RunAllTests(ac, av);
}
