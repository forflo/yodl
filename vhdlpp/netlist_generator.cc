#include <netlist_generator.h>
#include <parse_context.h>
#include <predicate_generators.h>

#include <functional>
#include <stateful_lambda.h>

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <sstream>
#include <mach7_includes.h>
#include <generic_traverser.h>
#include <clock_edge_recognizer.h>
#include <sequential.h>
#include <ifelse_case_converter.h>
#include <sync_condition_predicate.h>
#include <simple_match/include/simple_match/simple_match.hpp>

namespace simple_match {
    namespace customization {
        template<> struct tuple_adapter<NetlistGenerator::dff_complex_netlist_t> {
            enum { tuple_len = 2 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.input, t.output)); }};

        template<> struct tuple_adapter<NetlistGenerator::flipflop_netlist_t> {
            enum { tuple_len = 2 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.input, t.output)); }};

        template<> struct tuple_adapter<NetlistGenerator::muxer_netlist_t> {
            enum { tuple_len = 3 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.inputPaths, t.output, t.muxerWidth)); }};

        template<> struct tuple_adapter<NetlistGenerator::case_t> {
            enum { tuple_len = 3 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.netlist, t.occuringSignals, t.curWhenAlternative)); }};

        template<> struct tuple_adapter<NetlistGenerator::if_dff_t> {
            enum { tuple_len = 2 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.netlist, t.occuringSignals)); }};

        template<> struct tuple_adapter<NetlistGenerator::stack_element_t> {
            enum { tuple_len = 2 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.netlist, t.occuringSignals)); }};

        template<> struct tuple_adapter<NetlistGenerator::if_latch_t> {
            enum { tuple_len = 2 };
            template<size_t I, typename T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.netlist, t.occuringSignals)); }};
    }
}

using namespace Yosys::RTLIL;
using namespace std;

int NetlistGenerator::operator()(Entity *entity){
    Yosys::log_streams.push_back(&std::cout);
    Yosys::log_error_stderr = true;

    working = entity;

    if (result == NULL)
        result = new Module();

    for (auto &i : entity->ports_){
        if (!i->type->type_match(
                &entity->context_->global_types->primitive_STDLOGIC)){
            std::cout << "Type error in netlist generator"
                      << endl;
            std::cout << "Types other than std_logic are not supported"
                      << endl;
            return 1;
        }

        if (i->expr) {
            std::cout
                << "Netlist generator doesn't support default values";
            return 1;
        }
    }

    auto iter = entity->arch_.begin();
    auto arch = dynamic_cast<Architecture*>(iter->second);

    for (auto &i : entity->ports_){
        const VType *type = i->type;

        int vectorWidth = 0;

        if (!type->type_match(
                &working->context_->
                global_types->primitive_STDLOGIC_VECTOR)){
            vectorWidth = 0;
        } else if (!type->type_match(
                       &working->context_->
                       global_types->primitive_STDLOGIC_VECTOR)){
            VTypeArray const *tmp = dynamic_cast<VTypeArray const *>(type);
            if (tmp->ranges_.size() != 1) {
                std::cout << "Currently only 1-dim signal vectors allowed"
                          << std::endl;
                exit(1);
            }

            VTypeArray::range_t range = tmp->ranges_[0];
            if (range.msb_ == NULL || range.lsb_ == NULL){
                std::cout << "Invalid range"
                          << std::endl;
                exit(1);
            }

            int64_t evalMsb, evalLsb;
            bool evaluable = true ;
            evaluable = evaluable &&
                range.msb_->evaluate(working, currentScope, evalMsb);
            evaluable = evaluable &&
                range.lsb_->evaluate(working, currentScope, evalLsb);

            if (!evaluable){
                std::cout << "Non-static range";
                exit(1);
            }

            if (range.direction_){
                vectorWidth = evalMsb - evalLsb;
            }
            else {
                vectorWidth = evalLsb - evalMsb;
            }

            std::cout << "vectorWidth for entity port: "
                      << vectorWidth << std::endl;
        }

        result->addWire(string("\\") + i->name.str(), vectorWidth);
    }

    return traverseConcStmts(&arch->statements_);
}

int NetlistGenerator::traverseConcStmts(
    list<Architecture::Statement *> *stmts){
    using namespace mch;

    int errors = 0;

    for (auto &i : *stmts){
        Match(i){
            Case(C<BlockStatement>()){
                errors += traverseBlockStatement(
                    dynamic_cast<BlockStatement *>(i));
                break;
            }
            Case(C<ProcessStatement>()){
                errors += traverseProcessStatement(
                    dynamic_cast<ProcessStatement *>(i));
                break;
            }
            Otherwise(){
                std::cout << "Concurrent statement type not supported"
                          << endl;
                std::cout << "Will ignore"
                          << endl;
                break;
            }
        } EndMatch;
    }

    return errors;
}

int NetlistGenerator::traverseBlockStatement(BlockStatement *block){
    // traverse decl part
    currentScope = block;

    return traverseConcStmts(block->concurrent_stmts_);
}

// TODO: refactor. Another name! NoContext
int NetlistGenerator::executeSignalAssignmentNonContext(
    SignalSeqAssignment const *a){

    const char *signalId = dynamic_cast<ExpName*>(a->lval_)->name_.str();
    string s = signalId;
    Expression *tmp = *a->waveform_.begin();

    SigSpec res = executeExpression(tmp);

    result->connect(result->wire("\\" + s), res);

    return 0;
}

int NetlistGenerator::executeSignalAssignmentContextInit(
    stack_element_t *youngest, string const signalId, SigSpec const &res){

    using namespace simple_match;
    using namespace simple_match::placeholders;

    match(
        youngest,

        some<NetlistGenerator::case_t>(ds(_x, _x, _x)),
        [this,&signalId,&res](auto &netlistsMap, auto &, auto &currentChoice){
            netlist_element_t *tmp = &netlistsMap[signalId];

            muxer_netlist_t *muxer = dynamic_cast<muxer_netlist_t *>(tmp);
            if (muxer) {
                result->connect(muxer->inputPaths[currentChoice], res);
            }
            else {
                std::cout << "[executeSignalAssignmentCase] "
                          << "Impossible Nullpointer!"
                          << std::endl;
            }
        },

        some<NetlistGenerator::stack_element_t>(ds(_x, _x)),
        [this,&signalId,&res](auto &netlistMap, auto &){
            netlist_element_t *tmp = &netlistMap[signalId];

            match(
                tmp,

                some<NetlistGenerator::flipflop_netlist_t>(ds(_x, _x)),
                [this,&res](auto &input, auto &){
                    result->connect(input, res);
                },

                some<NetlistGenerator::dff_complex_netlist_t>(ds(_x, _x)),
                [this,&res](auto &input, auto &){
                    result->connect(input, res);
                },

                some<NetlistGenerator::muxer_netlist_t>(ds(_x, _x, _x)),
                [](auto &, auto &, auto &){
                    std::cout << "[executeSignalAssignmentCase] "
                              << "Impossible condition!"
                              << std::endl;
                    exit(1);
                },

                some(), [](NetlistGenerator::netlist_element_t const &){
                    std::cout << "[executeSignalAssignmentCase] Got "
                              << "stuck in default!"
                              << std::endl;
                    exit(1);
                },

                none(), [](){
                    std::cout << "[executeSignalAssignmentCase] Nullpointer "
                              << "in nested!"
                              << std::endl;
                    exit(1);
                });
        },

        some(), [](stack_element_t const &){
            std::cout << "[executeSignalAssignmentCase] Don't know!"
                      << std::endl;
            exit(1);
        },

        none(), [](){
            std::cout << "[executeSignalAssignmentCase] Nullpointer was given!"
                      << std::endl;
            exit(1);
        });

    return 0;
}

int NetlistGenerator::executeSignalAssignmentContextConnect(
    stack_element_t *moreNested, stack_element_t *lessNested,
    string const signalId){

    using namespace simple_match;
    using namespace simple_match::placeholders;

    match(
        lessNested,

        some<NetlistGenerator::case_t>(ds(_x, _x, _x)),
        [this,&signalId,moreNested](auto &netlistsMap, auto &,
                                    auto &currentChoice){
            netlist_element_t *tmp = &netlistsMap[signalId];
            match(
                tmp,

                // connect input of less nested flipflop with
                // output of the more nested netlist's output appropriate for
                // signal id
                some<NetlistGenerator::flipflop_netlist_t>(ds(_x, _x)),
                [this,moreNested,&signalId](auto &input, auto &){
                    result->connect(
                        input, moreNested->netlist[signalId].output);
                },

                some<NetlistGenerator::dff_complex_netlist_t>(ds(_x, _x)),
                [this,moreNested,&signalId](auto &input, auto &){
                    result->connect(
                        input, moreNested->netlist[signalId].output);
                },

                some<NetlistGenerator::muxer_netlist_t>(ds(_x, _x, _x)),
                [this,currentChoice,moreNested,&signalId](
                    auto &inputPaths, auto &, auto &){

                    result->connect(
                        inputPaths[currentChoice],
                        moreNested->netlist[signalId].output);
                },

                // default error handling
                some(), [](NetlistGenerator::netlist_element_t const &){
                    std::cout << "[executeSignalAssignmentCaseConnect] "
                              << "Got stuck in default!"
                              << std::endl;
                    exit(1);
                },

                none(), [](){
                    std::cout << "[executeSignalAssignmentCaseConnect] "
                              << "Nullpointer in nested!"
                              << std::endl;
                    exit(1);
                });
        },

        some<NetlistGenerator::stack_element_t>(ds(_x, _x)),
        [this,&signalId,moreNested](auto &netlistsMap,
                                    auto &){
            netlist_element_t *tmp = &netlistsMap[signalId];
            match(
                tmp,

                // connect input of less nested flipflop with
                // output of the more nested netlist's output appropriate for
                // signal id
                some<NetlistGenerator::flipflop_netlist_t>(ds(_x, _x)),
                [this,moreNested,&signalId](auto &input, auto &){
                    result->connect(
                        input, moreNested->netlist[signalId].output);
                },

                some<NetlistGenerator::dff_complex_netlist_t>(ds(_x, _x)),
                [this,moreNested,&signalId](auto &input, auto &){
                    result->connect(
                        input, moreNested->netlist[signalId].output);
                },

                some<NetlistGenerator::muxer_netlist_t>(ds(_x, _x, _x)),
                [this,moreNested,&signalId](auto &, auto &, auto &){
                    std::cout << "[executeSignalAssignmentCaseConnect] "
                              << "impossible condition detected!"
                              << std::endl;
                    exit(1);
                },

                // default error handling
                some(), [](NetlistGenerator::netlist_element_t const &){
                    std::cout << "[executeSignalAssignmentCaseConnect] "
                              << "Got stuck in default!"
                              << std::endl;
                    exit(1);
                },

                none(), [](){
                    std::cout << "[executeSignalAssignmentCaseConnect] "
                              << "Nullpointer in nested!"
                              << std::endl;
                    exit(1);
                });
        },

        // default error handling
        some(), [](stack_element_t const &){
            std::cout << "[executeSignalAssignmentCaseConnect] Don't know!"
                      << std::endl;
            exit(1);
        },

        none(), [](){
            std::cout << "[executeSignalAssignmentCaseConncet] "
                      << "Nullpointer was given!"
                      << std::endl;
            exit(1);
        });

    return 0;
}

int NetlistGenerator::executeSignalAssignmentContextFinalize(
    stack_element_t *oldest, string const &signalId){

    result->connect(
        result->wire("\\" + signalId),
        oldest->netlist[signalId].output);

    return 0;
}

// TODO: refactor. Another name! ..Context
int NetlistGenerator::executeSignalAssignmentContext(SignalSeqAssignment const *a){
    const char *signalId = dynamic_cast<ExpName*>(a->lval_)->name_.str();
    string s = signalId;
    Expression *tmp = *a->waveform_.begin();
    SigSpec res = executeExpression(tmp);

    stack_element_t *youngest = &contextStack[contextStack.size() - 1];

    executeSignalAssignmentContextInit(youngest, signalId, res);

    for (int i = contextStack.size() - 1; i > 0; i--){
        stack_element_t *moreNested = &contextStack[i];
        stack_element_t *lessNested = &contextStack[i - 1];

        executeSignalAssignmentContextConnect(moreNested, lessNested, signalId);
    }

    executeSignalAssignmentContextFinalize(&contextStack[0], s);
    return 0;
}

int NetlistGenerator::executeSignalAssignment(SignalSeqAssignment const *a){
    const VType *ltype = a->lval_->probe_type(
        working, currentScope);

    if (!ltype->type_match(
            &working->context_->
            global_types->primitive_STDLOGIC)){
        std::cout << "traverseProcessStatement"
                  << endl;
        std::cout << "Type error in netlist generator"
                  << endl;
        std::cout << "Types other than std_logic "
                  << "are not supported" << endl;
        return 1;
    }

    if (a->waveform_.size() != 1){
        std::cout << "waveform only allowed to contain one expression"
                  << endl;
        return 1;
    }

    if (!(*a->waveform_.begin())->probe_type(
            working,currentScope)->type_match(
                &working->context_->global_types->
                primitive_STDLOGIC)){
        std::cout << "rval must be STDLOGIC" << endl;
        return 1;
    }

    if (contextStack.size() != 0){
        return executeSignalAssignmentContext(a);
    } else {
        return executeSignalAssignmentNonContext(a);
    }
}

SigSpec NetlistGenerator::executeExpression(Expression const *exp){
    using namespace mch;

    Match(exp){
        Case(C<ExpCharacter>()){
            SigSpec tmpSig;
            switch(dynamic_cast<ExpCharacter const *>(exp)->value_){
            case '0':
                tmpSig = SigSpec(State::S0);
                break;
            case '1':
                tmpSig = SigSpec(State::S1);
                break;
            case 'z':
                tmpSig = SigSpec(State::Sz);
                break;
            case '-':
                tmpSig = SigSpec(State::Sa);
                break;
            }

            return tmpSig;

            break;
        }
        Case(C<ExpUNot>()){
            ExpUNot const *t = dynamic_cast<ExpUNot const *>(exp);

            Cell *c = result->addCell(NEW_ID, "$not");
            Wire *out = result->addWire(NEW_ID);

            c->setPort("\\A", executeExpression(t->operand1_));
            c->setPort("\\Y", out);

            c->fixup_parameters();
            return SigSpec(out);
            break;
        }
        Case(C<ExpRelation>()){
            ExpRelation const *t = dynamic_cast<ExpRelation const *>(exp);

            Cell *c;
            Wire *out = result->addWire(NEW_ID);

            switch(t->fun_){
            case ExpRelation::fun_t::EQ:
                c = result->addCell(NEW_ID, "$eq");
                break;
            case ExpRelation::fun_t::LT:
                c = result->addCell(NEW_ID, "$lt");
                break;
            case ExpRelation::fun_t::GT:
                c = result->addCell(NEW_ID, "$gt");
                break;
            case ExpRelation::fun_t::NEQ:
                c = result->addCell(NEW_ID, "$ne"); // or $nex??
                break;
            case ExpRelation::fun_t::LE:
                c = result->addCell(NEW_ID, "$le");
                break;
            case ExpRelation::fun_t::GE:
                c = result->addCell(NEW_ID, "$ge");
                break;
            }

            c->setPort("\\A", executeExpression(t->operand1_));
            c->setPort("\\B", executeExpression(t->operand2_));

            c->setPort("\\Y", out);

            c->fixup_parameters();

            return SigSpec(out);
            break;
        }
        Case(C<ExpString>()){
            ExpString const *t = dynamic_cast<ExpString const *>(exp);
            return sigSpecFromString(t->value_);
        }
        Case(C<ExpLogical>()){
            ExpLogical const *t = dynamic_cast<ExpLogical const *>(exp);

            Cell *c;
            Wire *out = result->addWire(NEW_ID);

            switch(t->fun_){
            case ExpLogical::fun_t::AND:
                c = result->addCell(NEW_ID, "$and");
                break;
            case ExpLogical::fun_t::OR:
                c = result->addCell(NEW_ID, "$or");
                break;
            case ExpLogical::fun_t::NAND:
                c = result->addCell(NEW_ID, "$nand");
                break;
            case ExpLogical::fun_t::NOR:
                c = result->addCell(NEW_ID, "$nor");
                break;
            case ExpLogical::fun_t::XOR:
                c = result->addCell(NEW_ID, "$xor");
                break;
            case ExpLogical::fun_t::XNOR:
                c = result->addCell(NEW_ID, "$xnor");
                break;
            }

            c->setPort("\\A", executeExpression(t->operand1_));
            c->setPort("\\B", executeExpression(t->operand2_));

            c->setPort("\\Y", out);

            c->fixup_parameters();

            return SigSpec(out);
            break;
        }
        Case(C<ExpShift>()){
            ExpShift const *n = dynamic_cast<ExpShift const *>(exp);
            ExpShift::shift_t operation = n->shift_;

            Cell *c;
            Wire *out = result->addWire(NEW_ID);

            //TODO: implement
            switch(operation){
            case ExpShift::shift_t::SRL:
                c = result->addCell(NEW_ID, "");
                break;
            case ExpShift::shift_t::SLL: break;
            case ExpShift::shift_t::SRA: break;
            case ExpShift::shift_t::SLA: break;
            case ExpShift::shift_t::ROL: break;
            case ExpShift::shift_t::ROR: break;
            }

            c->setPort("\\A", executeExpression(n->operand1_));
            c->setPort("\\B", executeExpression(n->operand2_));

            c->setPort("\\Y", out);

            c->fixup_parameters();

            return SigSpec(out);
            break;
        }
        Case(C<ExpName>()){
            const ExpName *n = dynamic_cast<ExpName const *>(exp);

            string strT = n->name_.str();

            // Ad-hoc implementation of true and false enums
            if (strT == "true"){ return SigSpec(State::S1); break; }
            if (strT == "false"){ return SigSpec(State::S0); break; }

            Wire *w = result->wire("\\" + strT);

            if (w){
                return SigSpec(w);
            } else {
                std::cout << "Usage of non existent signal!"
                          << "\n";
                exit(1);
                return SigSpec();
//                return SigSpec(result->addWire("\\" + strT), vectorWidth);
            }

            break;
        }
        Case(C<ExpConcat>()){
            ExpConcat const *n = dynamic_cast<ExpConcat const *>(exp);

            SigSpec left  = executeExpression(n->operand1_),
                    right = executeExpression(n->operand2_);

            Cell *c = result->addCell(NEW_ID, "$concat");
            c->setPort("\\B", left);
            c->setPort("\\A", right);

            Wire *out = result->wire(NEW_ID);
            c->setPort("\\Y", out);

            c->fixup_parameters();
            return SigSpec(out);

            break;
        }
        Otherwise(){
            std::cout << "This kind of expression fails exec Expr!"
                      << std::endl;
            return SigSpec(State::S0);
            break;
        }
    } EndMatch;

    return SigSpec(State::S0);
}

class IndependencePredicate {
public:
    IndependencePredicate(Expression const *r)
        : rhs(r), count(0) {}

    // checks whether the lhs occurs in rhs
    bool isLhsIndependent(ExpName const *lhs){
        GenericTraverser nameSearcher(
            makeTypePredicate<ExpName>(),
            static_cast<function<int (AstNode const *)>>(
                [this, lhs](AstNode const *n) -> int {
                    // TODO: Make this more general with value compare on
                    //       AstNode objects. This comparison
                    //       is not yet implemented, though...
                    if (lhs->name_ == dynamic_cast<ExpName const *>(n)->name_){
                        count++;
                    }
                    return 0;
                }),
            GenericTraverser::NONRECUR);

        nameSearcher(rhs);

        return (count > 0 ? false : true);
    }

    void reset(Expression const *r){
        rhs = r;
        count = 0;
    }

private:
    Expression const *rhs;
    int count;
};

//int NetlistGenerator::executeCaseStmtSync(stmt){
//
//}
//
//int NetlistGenerator::executeCaseStmtNonsync(stmt){
//
//}

SigSpec NetlistGenerator::sigSpecFromString(const string &s){
    vector<SigBit> bitlist;
    for (int i = s.length() - 1; i >= 0; i--){
        switch(s[i]){
        case '0':
            bitlist.push_back(SigSpec(State::S0));
            break;
        case '1':
            bitlist.push_back(SigSpec(State::S1));
            break;
        case 'z':
            bitlist.push_back(SigSpec(State::Sz));
            break;
        case '-':
            bitlist.push_back(SigSpec(State::Sa));
            break;
        }
    }

    return SigSpec(bitlist);
}

int NetlistGenerator::generateMuxerH(
    int curSelectorIdx, Cell *orig,
    std::vector<SigBit> const &selector,
    string path,
    map<SigSpec, SigSpec> &paths)
{
    if (curSelectorIdx >= 0 &&
        (unsigned long) curSelectorIdx < selector.size()) {

        orig->setPort("\\S", selector[curSelectorIdx]);
        orig->setPort("\\A", result->addWire(NEW_ID));
        orig->setPort("\\B", result->addWire(NEW_ID));

        if ((unsigned long) curSelectorIdx != selector.size() - 1){
            Cell *a = result->addCell(NEW_ID, "$mux"),
                 *b = result->addCell(NEW_ID, "$mux");

            a->setPort("\\Y", orig->getPort("\\A"));
            b->setPort("\\Y", orig->getPort("\\B"));

            generateMuxerH(curSelectorIdx + 1, a, selector, '0' + path, paths);
            generateMuxerH(curSelectorIdx + 1, b, selector, '1' + path, paths);
        } else {
            paths[sigSpecFromString('0' + path)] = orig->getPort("\\A");
            paths[sigSpecFromString('1' + path)] = orig->getPort("\\B");
        }
    }

    return 0;
}

NetlistGenerator::muxer_netlist_t
NetlistGenerator::generateMuxer(CaseSeqStmt const *c){
    Expression const *condition = c->cond_;
    vector<SigSpec> selVec;
    if (! (condition->probe_type(working,currentScope)->type_match(
               &working->context_->global_types->
               primitive_STDLOGIC) ||

           condition->probe_type(working,currentScope)->type_match(
               &working->context_->global_types->
               primitive_STDLOGIC_VECTOR))){

        std::cout << "[Semantic error]\n"
                  << "Condition in Case Statement has inappropriate type!\n";
    }

    Cell *muxOrigin = result->addCell(NEW_ID, "$mux");

    Wire *output = result->addWire(NEW_ID);
    muxOrigin->setPort("\\Y", output);

    SigSpec evaledCond = executeExpression(condition);

    map<SigSpec, SigSpec> inputs;
    generateMuxerH(0, muxOrigin, evaledCond.bits(), string(""), inputs);

//    result->connect(inputs[sigSpecFromString("101")], SigSpec(State::S1));
//    result->connect(inputs[sigSpecFromString("010")], SigSpec(State::S1));
//    result->connect(inputs[sigSpecFromString("110")], SigSpec(State::S1));

    return muxer_netlist_t(inputs, output, inputs.begin()->first.size());
}

set<string> NetlistGenerator::extractLhs(list<SequentialStmt *> const &l){
    set<string> leftHandSides;
    set<string> tmp;

    for (auto &i : l){
        tmp = extractLhs(i);
        for (auto &j : tmp)
            leftHandSides.insert(j);
    }

    return leftHandSides;
}

set<string> NetlistGenerator::extractLhs(AstNode const *stmt){
    set<string> leftHandSides;

    GenericTraverser traverser(
        makeTypePredicate<SignalSeqAssignment>(),
        static_cast<function<int (AstNode const *)>>(
            [&leftHandSides](AstNode const *n) -> int {
                leftHandSides.insert(
                    dynamic_cast<ExpName const *>(
                        dynamic_cast<SignalSeqAssignment const *>(
                            n)-> lval_)->name_.str());
                return 0;
            }),
        GenericTraverser::NONRECUR);
    traverser(stmt);

    return leftHandSides;
}

//int NetlistGenerator::finishUpUnassignedSignals(){
//
//}

int NetlistGenerator::executeCaseStmt(CaseSeqStmt const *stmt){
    Expression *condition = stmt->cond_->clone();

    SyncCondPredicate isSync(working, currentScope);

    if(isSync(condition)){
        std::cout << "[Semantic error!]\n";
        std::cout << "Synchronized conditions not allowed in "
                  << "case statement condition";
    }

    for (auto &i : stmt->alt_){
        if (i->exp_ && i->exp_->size() > 1){
            std::cout << "[Semantic error!]\n"
                      << "Each alternative can only contain one expression!";
            return 1;
        }
    }

    set<string> lhsOfCase = extractLhs(stmt);

    map<string, netlist_element_t> sigMuxerMapping;
    for (auto &i : lhsOfCase)
        sigMuxerMapping[i] = generateMuxer(stmt);

    for (auto &i : stmt->alt_){
        SigSpec choice = executeExpression(*i->exp_->begin());
        set<string> lhsOfAlt = extractLhs(i);

        // first push new netlist environment at the end
        contextStack.push_back(
            case_t(sigMuxerMapping, choice, lhsOfAlt));

        // execute every sequential stmt from current choice
        for (auto &i : i->stmts_){
            executeSequentialStmt(i);
        }

        // then erase now obsolete environment
        contextStack.erase(contextStack.end());
    }

    return 0;
}

int NetlistGenerator::executeIfStmtH1(ClockEdgeRecognizer const &findEdgeSpecs,
                                      Expression const *condition,
                                      IfSequential const *s){
    stack_element_t newTop;

    std::set<string> drivenSignals;
    std::map<string, netlist_element_t> sigsToNetsMap;
    drivenSignals = extractLhs(s->if_);

    for (auto &i : drivenSignals){

        netlist_element_t flipflop;
        if (findEdgeSpecs.fullClockSpecs[0] == condition) {
            // condition consits only of the clock edge specification
            std::cout << "condition consists only of clock edge spec"
                      << std::endl;

            Cell *dff = result->addCell(NEW_ID, "$dff");
            Wire *out = result->addWire(NEW_ID);
            Wire *in = result->addWire(NEW_ID);


            dff->setPort("\\CLK", executeExpression(findEdgeSpecs.clockNameExp));
            dff->setPort("\\D", SigSpec(in));
            dff->setPort("\\Q", SigSpec(out));

            flipflop = flipflop_netlist_t(SigSpec(in), SigSpec(out));

        } else {
            std::cout << "condition contains other subexpressions "
                      << "besides a edge spec" << std::endl;

            Cell *mux = result->addCell(NEW_ID, "$mux");
            Wire *muxZero = result->addWire(NEW_ID);
            Wire *muxOne = result->addWire(NEW_ID);
            Wire *muxSelector = result->addWire(NEW_ID);
            Wire *muxOut = result->addWire(NEW_ID);

            Cell *dff = result->addCell(NEW_ID, "$dff");
            Wire *dffOut = result->addWire(NEW_ID);

            dff->setPort("\\CLK", executeExpression(findEdgeSpecs.clockNameExp));
            dff->setPort("\\D", SigSpec(muxOut));
            dff->setPort("\\Q", SigSpec(dffOut));

            mux->setPort("\\S", SigSpec(muxSelector));
            mux->setPort("\\A", SigSpec(muxZero));
            mux->setPort("\\B", SigSpec(muxOne));
            mux->setPort("\\Y", SigSpec(muxOut));

            result->connect(SigSpec(muxZero), SigSpec(dffOut));

            //TODO: muxOne must acutally be conected with the
            //      condition from the if, where clock edge is replaced by true

            flipflop = dff_complex_netlist_t(SigSpec(muxSelector), SigSpec());
        }

        sigsToNetsMap[i] = flipflop;
    }

    newTop = if_dff_t(sigsToNetsMap, drivenSignals);
    executeIfStmtHRecurse(s, newTop);

    return 0;
}

int NetlistGenerator::executeIfStmtH2(Expression const *condition,
                                      IfSequential const *s){
    stack_element_t newTop;

    std::set<string> drivenSignals;
    std::map<string, netlist_element_t> sigsToNetsMap;
    drivenSignals = extractLhs(s->if_);

    for (auto &i : drivenSignals){
        netlist_element_t latch;
        Cell *dff = result->addCell(NEW_ID, "$dlatch");
        Wire *out = result->addWire(NEW_ID);
        Wire *in = result->addWire(NEW_ID);

        dff->setPort("\\EN", executeExpression(condition));
        dff->setPort("\\D", SigSpec(in));
        dff->setPort("\\Q", SigSpec(out));

        latch = flipflop_netlist_t(SigSpec(in), SigSpec(out));
        sigsToNetsMap[i] = latch;
    }

    newTop = if_latch_t(sigsToNetsMap, drivenSignals);

    executeIfStmtHRecurse(s, newTop);

    return 0;
}

int NetlistGenerator::executeIfStmtHRecurse(IfSequential const *s,
                                            stack_element_t const &newTop){

    contextStack.push_back(newTop);

    // traverse all statements inside the if block
    for (auto &i : s->if_)
        executeSequentialStmt(i);

    contextStack.erase(contextStack.end());

    return 0;
}

int NetlistGenerator::executeIfStmt(IfSequential const *s){
    Expression *condition = s->cond_->clone();
    bool syncCondition;

    if (s->elsif_.size() > 0) {
        std::cout << "Elsifs not supported!\n"
                  << "run elsif to if-else converter instead!\n"
                  << std::endl;
        exit(1);
    }

    SyncCondPredicate isSync(working, currentScope);
    // 1) Modify condition so that all clock edge specs get deleted

    syncCondition = isSync(condition);

    Expression *condClone = condition->clone();

    ClockEdgeRecognizer findEdgeSpecs;
    findEdgeSpecs(condClone);

    if (findEdgeSpecs.numberClockEdges > 1){
        std::cout << "Currently only one clock edge supported!" << std::endl;
        exit(1);
    }

    //TODO: Handle cases where if has else blocks...
    //      done for thirt path!
    if (syncCondition) {
        // clock edge synchronized => dff on top of the stack
        if (findEdgeSpecs.clockNameExp == NULL){
            std::cout << "clock name expression was null. ABORT"
                      << std::endl;
            exit(1);
        }

        if (s->else_.size() > 0) {
            std::cout << "Else statements in if with sync condition not allowed!"
                      << std::endl;
            exit(1);
        }

        executeIfStmtH1(findEdgeSpecs, condition, s);
    } else if ((!syncCondition) && findEdgeSpecs.numberClockEdges > 1) {
        std::cout << "Can't currently synthesize conditions that"
                  << "are no sync_condition but contain a clock edge"
                  << std::endl;

    } else if ((!syncCondition) && findEdgeSpecs.numberClockEdges == 0) {
        if (s->else_.size() > 0) {
            set<string> lhsInIf, lhsInElse;
            lhsInIf = extractLhs(s->if_);
            lhsInElse = extractLhs(s->else_);

            if (lhsInIf == lhsInElse){
                // every signal get's driven at least once in both paths
                // => no dlatch but normal muxer circuit
                // TODO:
                BranchesToCases caseConverter;
                CaseSeqStmt *caseTemp =
                    caseConverter.makeCaseSeq(s->cond_, s->if_, s->else_);

                executeCaseStmt(caseTemp);

                delete caseTemp;

                std::cout << "Every signal driven in both paths!" << std::endl;
            }
        } else {
            // level sensitive => put dlatch on top

            executeIfStmtH2(condition, s);
        }
    } else {
        std::cout << "Unknown combination of sync condition and clock edge\n";
    }

    return 0;
}


int NetlistGenerator::executeSequentialStmt(SequentialStmt const *s){
    using namespace mch;
    int errors = 0;

    Match(s){
        Case(C<SignalSeqAssignment>()){
            SignalSeqAssignment const *tmp =
                dynamic_cast<SignalSeqAssignment const *>(s);

            errors += executeSignalAssignment(tmp);

            break;
        }
        Case(C<CaseSeqStmt>()){
            CaseSeqStmt const *tmp =
                dynamic_cast<CaseSeqStmt const *>(s);
            errors += executeCaseStmt(tmp);
            break;
        }
        Case(C<IfSequential>()){
            IfSequential const *tmp =
                dynamic_cast<IfSequential const *>(s);
            errors += executeIfStmt(tmp);
            break;
        }
        Otherwise(){
            std::cout << "This statement type is not supported"
                      << endl;
            break;
        }
    } EndMatch;

    return errors;
}

int NetlistGenerator::traverseProcessStatement(ProcessStatement *proc){
    currentScope = proc;

    for (auto &i : proc->statements_){
        executeSequentialStmt(i);
    }

    return 0;
}
