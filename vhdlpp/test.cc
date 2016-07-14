// FM. MA
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>

// YOSYS specific headers
#include <kernel/yosys.h>
#include <kernel/rtlil.h>

// code base specific includes
#include "generate_graph.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "entity.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "generic_traverser.h"
#include "vtype.h"
#include "std_types.h"
#include "std_funcs.h"
#include "parse_context.h"
#include "test.h"
#include "root_class.h"
#include "mach7_includes.h"
#include "path_finder.h"


bool verbose_flag = false;
// Where to dump design entities
const char *work_path = "ivl_vhdl_work";
const char *dump_design_entities_path = 0;
const char *dump_libraries_path       = 0;
const char *debug_log_path            = 0;

bool     debug_elaboration = false;
ofstream debug_log_file;

TEST_CASE("Yosys RTLIL construction", "[rtlil usage]"){

}


TEST_CASE("Simple block", "[ast]"){
    int rc;

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_sorrys == 0);
}

TEST_CASE("Simple for loop", "[ast]"){
    int rc;

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file(
        "vhdl_testfiles/for_loop_simple.vhd",
        perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_sorrys == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    auto entity1 = iterator->second;

    //emit_dotgraph(std::cout, "foo", entity1->emit_strinfo_tree());
}

TEST_CASE("Multiple parses", "[ast]"){
    int rc1, rc2;

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    StandardTypes *std_types1 = (new StandardTypes())->init();
    StandardFunctions *std_funcs1 = (new StandardFunctions())->init();
    ParserContext *context1 = (new ParserContext(std_types1, std_funcs1))->init();

    rc1 = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                        perm_string(), context);
    rc2 = ParserUtil::parse_source_file("vhdl_testfiles/adder.vhd",
                                        perm_string(), context1);

    REQUIRE(rc1 == 0);
    REQUIRE(rc2 == 0);
    REQUIRE(context->parse_errors  == 0);
    REQUIRE(context->parse_errors  == 0);
    REQUIRE(context1->parse_sorrys == 0);
    REQUIRE(context1->parse_sorrys == 0);
}

TEST_CASE("Simple clone test", "[clone]"){
    int rc;

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors  == 0);
    REQUIRE(context->parse_errors  == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    REQUIRE(iterator->second != NULL);

    auto cloned_entity = iterator->second->clone();
    REQUIRE(cloned_entity != NULL);
}

TEST_CASE("Simple clone test with dot generation", "[clone]"){
    int rc;

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_errors == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    auto entity1 = iterator->second;
    REQUIRE(entity1 != NULL);

    auto entity2 = iterator->second->clone();
    REQUIRE(entity2 != NULL);

    stringstream a{};
    stringstream b{};

    auto tree1 = entity1->emit_strinfo_tree();
    auto tree2 = entity2->emit_strinfo_tree();

    DotGraphGenerator()(a, "foo", entity1->emit_strinfo_tree());
    DotGraphGenerator()(b, "foo", entity2->emit_strinfo_tree());

    REQUIRE((*tree1 == *tree2) == true);
    REQUIRE(a.str() == b.str());
}

TEST_CASE("Test equality of simple tree", "[simple tree]"){
    auto tree1 = new SimpleTree<map<string, string>>(
        map<string, string>({
                {"foo", "bar"},
                {"bar", "foo"}}),
        empty_simple_tree());

    auto tree2 = new SimpleTree<map<string, string>>(
        map<string, string>({
                {"foo", "bar"},
                {"bar", "foo"}}),
        empty_simple_tree());

    REQUIRE((*tree1 == *tree2) == true);

    tree1->root["bar"] = "fnord";
    REQUIRE((*tree1 == *tree2) == false);
}

TEST_CASE("Test simple generic traversal", "[generic traverser]"){
    using namespace mch;

    int rc;
    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_errors == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    auto entity1 = iterator->second;
    REQUIRE(entity1 != NULL);

    const AstNode *root = entity1;

    StatefulLambda<int> state = StatefulLambda<int>(
        0,
        static_cast<function <int (const AstNode *, int &)>>(
        [](const AstNode *, int &env) -> int {
            cout << "[VISITOR] Found node!"  << endl;
            env++;
            return 0;
        }));

    GenericTraverser traverser(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        static_cast<function<int (const AstNode *)>>(
            [&state](const AstNode *a) -> int { return state(a); }),
        GenericTraverser::RECUR);

    traverser(root);
    REQUIRE(state.environment == 2);
}

TEST_CASE("GenericTraverser class constructor test", "[generic traverser]"){
    using namespace mch;

    GenericTraverser traverserConst(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        static_cast<function<int (const AstNode *)>>(
            [](const AstNode *) -> int { return 0; }),
        GenericTraverser::RECUR);

    REQUIRE(traverserConst.isNaryTraverser() == false);
    REQUIRE(traverserConst.wasError() == false);
    REQUIRE(traverserConst.isMutatingTraverser() == false);

    GenericTraverser traverserMutating(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        static_cast<function<int (AstNode *)>>([](AstNode *) -> int {
                return 0; }),
        GenericTraverser::RECUR);

    REQUIRE(traverserMutating.isNaryTraverser() == false);
    REQUIRE(traverserMutating.wasError() == false);
    REQUIRE(traverserMutating.isMutatingTraverser() == true);

    GenericTraverser traverserNary(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        static_cast<function<int (
            const AstNode *, const std::vector<const AstNode *> &)>>(
                [](const AstNode *, const std::vector<const AstNode *> &)
                -> int { return 0; }),
        GenericTraverser::RECUR);

    REQUIRE(traverserNary.isNaryTraverser() == true);
    REQUIRE(traverserNary.wasError() == false);
    REQUIRE(traverserNary.isMutatingTraverser() == false);

    GenericTraverser traverserNaryMutating(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        static_cast<function<int (
            AstNode *, const std::vector<AstNode *> &)>>(
                [](AstNode *, const std::vector<AstNode *> &)
                -> int { return 0; }),
        GenericTraverser::RECUR);

    REQUIRE(traverserNaryMutating.isNaryTraverser() == true);
    REQUIRE(traverserNaryMutating.wasError() == false);
    REQUIRE(traverserNaryMutating.isMutatingTraverser() == true);

}

TEST_CASE("Test simple generic traversal on cloned AST", "[generic traverser]"){
    using namespace mch;

    int rc;
    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_errors == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    auto entity1 = iterator->second;
    REQUIRE(entity1 != NULL);

    const AstNode *root = entity1->clone();

    StatefulLambda<int> state = StatefulLambda<int>(
        0,
        static_cast<function<int (const AstNode *, int &)>>(
            [](const AstNode *, int &env) -> int {
                cout << "[VISITOR] Found node!"  << endl;
                env++;
                return 0;
            }));

    GenericTraverser traverser(
        [=](const AstNode *node){
            Match(node){
                Case(C<BlockStatement>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        // static_cast needed here in order to resolve the
        // overload resolution ambiguity arising from the use
        // of std::function
        static_cast<function<int (const AstNode *)>>(
            [&state](const AstNode *a) -> int { return state(a); }),
        GenericTraverser::RECUR);

    traverser(root);
    REQUIRE(state.environment == 2);
    traverser.emitTraversalMessages(cout, "\n");
    traverser.emitErrorMessages(cout, "\n");
}

TEST_CASE("Test path finder", "[path finder]"){
    vector<vector<AstNode *>> res1, res2;

    ExpInteger *int1 = new ExpInteger(100);
    ExpInteger *int2 = new ExpInteger(101);
    ExpInteger *int3 = new ExpInteger(102);
    ExpInteger *int4 = new ExpInteger(103);

    ExpArithmetic *arith1 = new ExpArithmetic(ExpArithmetic::PLUS, int1, int2);
    ExpArithmetic *arith2 = new ExpArithmetic(ExpArithmetic::PLUS, int3, int4);

    ExpArithmetic *arith = new ExpArithmetic(ExpArithmetic::MULT, arith1, arith2);
    ExpArithmetic *aUnb = new ExpArithmetic(ExpArithmetic::MULT, arith1, int4);

    cout << "arith: " <<  static_cast<AstNode *>(arith) << endl;
    cout << "aUnb: " <<  static_cast<AstNode *>(aUnb) << endl;
    cout << "arith1: " << static_cast<AstNode *>(arith1) << endl;
    cout << "arith2: " << static_cast<AstNode *>(arith2) << endl;
    cout << "int1: " <<   static_cast<AstNode *>(int1) << endl;
    cout << "int2: " <<   static_cast<AstNode *>(int2) << endl;
    cout << "int3: " <<   static_cast<AstNode *>(int3) << endl;
    cout << "int4: " <<   static_cast<AstNode *>(int4) << endl;

    // check function PathFinder::getListOfchilds
    const std::list<AstNode *> childs1 =
        PathFinder::getListOfChilds(static_cast<AstNode *>(arith));
    REQUIRE(childs1.front() == arith1);
    REQUIRE(childs1.back() == arith2);

    const std::list<AstNode *> childs2 =
        PathFinder::getListOfChilds(static_cast<AstNode *>(arith1));
    REQUIRE(childs2.front() == int1);
    REQUIRE(childs2.back() == int2);

    const std::list<AstNode *> childs3 =
        PathFinder::getListOfChilds(static_cast<AstNode *>(arith2));
    REQUIRE(childs3.front() == int3);
    REQUIRE(childs3.back() == int4);

    // check pathFinder::findPath
    PathFinder pathFinder(1);

    pathFinder.findPath(arith);
    cout << pathFinder;

    PathFinder pathFinder2(2);
    pathFinder2.findPath(arith);
    cout << pathFinder2;

    PathFinder pathFinder3(3);
    pathFinder3.findPath(arith);
    cout << pathFinder3;

    PathFinder pathFinder4(4);
    pathFinder4.findPath(arith);
    cout << pathFinder4;

    PathFinder pathFinderU(3);
    pathFinderU.findPath(aUnb);
    cout << pathFinderU;

    // 1-ary pathFinder
    REQUIRE(pathFinder.getPaths().size() == 1);
    REQUIRE(pathFinder.getPaths()[0].size() == 1);
    REQUIRE(pathFinder.getPaths()[0][0] == arith);

    // 2-ary pathFinder
    REQUIRE(pathFinder2.getPaths().size() == 2);
    REQUIRE(pathFinder2.getPaths()[0].size() == 2);
    REQUIRE(pathFinder2.getPaths()[1].size() == 2);

    REQUIRE(pathFinder2.getPaths()[0][0] == arith);
    REQUIRE(pathFinder2.getPaths()[0][1] == arith1);

    REQUIRE(pathFinder2.getPaths()[1][0] == arith);
    REQUIRE(pathFinder2.getPaths()[1][1] == arith2);

    // 3-ary pathFinder
    REQUIRE(pathFinder3.getPaths().size() == 4);
    for (int i = 0 ; i< 3; i++){
        REQUIRE(pathFinder3.getPaths()[i].size() == 3);
    }

    REQUIRE(pathFinder3.getPaths()[0][0] == arith);
    REQUIRE(pathFinder3.getPaths()[1][0] == arith);
    REQUIRE(pathFinder3.getPaths()[2][0] == arith);
    REQUIRE(pathFinder3.getPaths()[3][0] == arith);

    REQUIRE(pathFinder3.getPaths()[0][1] == arith1);
    REQUIRE(pathFinder3.getPaths()[1][1] == arith1);
    REQUIRE(pathFinder3.getPaths()[2][1] == arith2);
    REQUIRE(pathFinder3.getPaths()[3][1] == arith2);

    REQUIRE(pathFinder3.getPaths()[0][2] == int1);
    REQUIRE(pathFinder3.getPaths()[1][2] == int2);
    REQUIRE(pathFinder3.getPaths()[2][2] == int3);
    REQUIRE(pathFinder3.getPaths()[3][2] == int4);

    // 4-ary pathFinder
    REQUIRE(pathFinder4.getPaths().size() == 0);

    // 3-ary pathFinder on unbalanced tree
    REQUIRE(pathFinderU.getPaths()[0].size() == 3);

    REQUIRE(pathFinderU.getPaths()[0][0] == aUnb);
    REQUIRE(pathFinderU.getPaths()[1][0] == aUnb);
    REQUIRE(pathFinderU.getPaths()[0][1] == arith1);
    REQUIRE(pathFinderU.getPaths()[1][1] == arith1);
    REQUIRE(pathFinderU.getPaths()[0][2] == int1);
    REQUIRE(pathFinderU.getPaths()[1][2] == int2);
}

TEST_CASE("Test nary traverser", "[generic traverser]"){
    ExpInteger *int1 = new ExpInteger(100);
    ExpInteger *int2 = new ExpInteger(101);
    ExpInteger *int3 = new ExpInteger(102);
    ExpInteger *int4 = new ExpInteger(103);

    ExpArithmetic *arith1 = new ExpArithmetic(ExpArithmetic::PLUS, int1, int2);
    ExpArithmetic *arith2 = new ExpArithmetic(ExpArithmetic::PLUS, int3, int4);

    ExpArithmetic *arith = new ExpArithmetic(ExpArithmetic::MULT, arith1, arith2);
    ExpArithmetic *aUnb = new ExpArithmetic(ExpArithmetic::MULT, arith1, int4);

    cout << "arith: " <<  static_cast<AstNode *>(arith) << endl;
    cout << "aUnb: " <<  static_cast<AstNode *>(aUnb) << endl;
    cout << "arith1: " << static_cast<AstNode *>(arith1) << endl;
    cout << "arith2: " << static_cast<AstNode *>(arith2) << endl;
    cout << "int1: " <<   static_cast<AstNode *>(int1) << endl;
    cout << "int2: " <<   static_cast<AstNode *>(int2) << endl;
    cout << "int3: " <<   static_cast<AstNode *>(int3) << endl;
    cout << "int4: " <<   static_cast<AstNode *>(int4) << endl;

    struct functor_t {
        bool alreadyCalled = false;

        AstNode *i1;
        AstNode *i2;
        AstNode *i3;
        AstNode *i4;
        AstNode *ar;
        AstNode *ar1;
        AstNode *ar2;
        AstNode *au;

        functor_t(AstNode *a, AstNode *b,
                  AstNode *c, AstNode *d,
                  AstNode *e, AstNode *f,
                  AstNode *g, AstNode *h)
            : i1(a), i2(b), i3(c), i4(d)
            , ar(e), ar1(f), ar2(g), au(h) {}

        int operator()(AstNode *node, const std::vector<AstNode *> &parents){
            if (node == i1){
                REQUIRE(parents[0] == ar1);
                REQUIRE(parents[1] == ar);
            }

            if (node == i2){
                REQUIRE(parents[0] == ar1);
                REQUIRE(parents[1] == ar);
            }

            if (node == i3) {
                REQUIRE(parents[0] == ar2);
                REQUIRE(parents[1] == ar);
            }

            if (node == i4) {
                REQUIRE(parents[0] == ar2);
                REQUIRE(parents[1] == ar);
            }

            return 0;
        }
    };

    GenericTraverser traverserNaryMutating(
        [=](const AstNode *node){
            Match(node){
                Case(mch::C<ExpInteger>()){ return true; }
                Otherwise(){ return false; }
            } EndMatch;
            return false; //without: compiler warning
        },
        functor_t(int1, int2, int3, int4, arith, arith1, arith2, aUnb),
        GenericTraverser::RECUR);

    traverserNaryMutating(arith);
}

TEST_CASE("Higher order traverser", "[generic traverser]"){
    int rc;
    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *context = (new ParserContext(std_types, std_funcs))->init();

    rc = ParserUtil::parse_source_file("vhdl_testfiles/block_simple.vhd",
                                       perm_string(), context);

    REQUIRE(rc == 0);
    REQUIRE(rc == 0);
    REQUIRE(context->parse_errors == 0);
    REQUIRE(context->parse_errors == 0);

    REQUIRE(context->design_entities.size() == 1);

    auto iterator = context->design_entities.begin();
    auto entity1 = iterator->second;
    REQUIRE(entity1 != NULL);

    struct counter_t {
        int count;

        int operator()(const AstNode *n){
            const SignalAssignment *sig = dynamic_cast<const SignalAssignment*>(n);
            count += sig->rval_.size();
            return 0;
        }
    };

    GenericTraverser trav(
        [](const AstNode *n) -> bool {
            Match(n){ Case(mch::C<BlockStatement>()){ return true; } }
            EndMatch; return false;
        },
        static_cast<function<int (const AstNode *)>>(
        GenericTraverser(
            [](const AstNode *n) -> bool {
                Match(n){ Case(mch::C<SignalAssignment>()){ return true; } }
                EndMatch; return false;
            },
            static_cast<function<int (const AstNode *)>>(counter_t()),
            GenericTraverser::NONRECUR)),
        GenericTraverser::NONRECUR);
}
