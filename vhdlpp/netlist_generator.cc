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
#include <sync_condition_predicate.h>

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
        result->addWire(string("\\") + i->name.str());
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

int NetlistGenerator::executeSignalAssignment(SignalSeqAssignment const *a){
    const VType *ltype = a->lval_->probe_type(
        working, currentScope);

    if (!ltype->type_match(
            &working->context_->
            global_types->primitive_STDLOGIC)){
        std::cout << "traverseProcessStatement" << endl;
        std::cout << "Type error in netlist generator"
                  << endl;
        std::cout << "Types other than std_logic "
            "are not supported" << endl;
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

    const char *signalId = dynamic_cast<ExpName*>(a->lval_)->name_.str();
    string s = signalId;
    Expression *tmp = *a->waveform_.begin();
    using namespace mch;

    SigSpec res = executeExpression(tmp);

    result->connect(result->wire("\\" + s), res);

    return 0;
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

            return SigSpec(t->value_);
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
        Case(C<ExpName>()){
            const ExpName *n = dynamic_cast<ExpName const *>(exp);
            // ugly, but neccessary because of the perm_strings are
            string strT = n->name_.str();
            Wire *w = result->wire("\\" + strT);

            if (w){
                return SigSpec(w);
            } else {
                return SigSpec(result->addWire("\\" + strT));
            }
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

int NetlistGenerator::executeCaseStmtSync(stmt){

}

int NetlistGenerator::executeCaseStmtNonsync(stmt){

}

int NetlistGenerator::generateMuxerH(
    int curSelectorIdx, Cell *orig,
    std::vector<SigBit> const &selector,
    string path,
    map<string, SigSpec> &paths)
{
    if (curSelectorIdx >= 0 &&
        (unsigned long) curSelectorIdx < selector.size()) {

        orig->setPort("\\S", selector[curSelectorIdx]);

        if (curSelectorIdx != selector.size() - 1){
            Cell *a = result->addCell(NEW_ID, "$mux"),
                 *b = result->addCell(NEW_ID, "$mux");

            a->setPort("\\Y", orig->getPort("\\A"));
            b->setPort("\\Y", orig->getPort("\\B"));

            generateMuxerH(curSelectorIdx + 1, a, selector, path + '0', paths);
            generateMuxerH(curSelectorIdx + 1, b, selector, path + '1', paths);
        } else {
            paths[path + '0'] = orig->getPort("\\A");
            paths[path + '1'] = orig->getPort("\\B");
        }
    } else {
        return 0;
    }
}

int NetlistGenerator::generateMuxer(CaseSeqStmt const *c){
    Expression const *condition = c->cond_;
    vector<SigSpec> selVec;
    int selectorBits = 0;
    if (condition->probe_type(
            working,currentScope)->type_match(
                &working->context_->global_types->
                primitive_STDLOGIC)){
        selectorBits = 1;
    } else if (condition->probe_type(
                   working,currentScope)->type_match(
                       &working->context_->global_types->
                       primitive_STDLOGIC_VECTOR)){
        selectorBits = 1;
    } else {
        std::cout << "[Semantic error]\n"
                  << "condition in Case Statement has inappropriate type!\n";
    }

    Cell *muxOrigin = result->addCell(NEW_ID, "$mux");
    SigSpec out = muxOrigin->getPort("\\Y");

    SigSpec evaledCond = executeExpression(condition);

    map<string, SigSpec> inputs;
    generateMuxerH(selectorBits, muxOrigin, evaledCond.bits(), string(""), inputs);

    std::cout << "[generateMuxer Debug]\n";
    for (auto &i : inputs){
        std::cout << i.first << " " << i.second.as_string() << std::endl;
    }
}

int NetlistGenerator::executeCaseStmt(CaseSeqStmt const *stmt){
    Expression *condition = stmt->cond_->clone();
    bool inSyncContext = false;

    SyncCondPredicate isSync(working, currentScope);
    // 1) Modify condition so that all clock edge specs get deleted

    if(isSync(condition)){
        ClockEdgeRecognizer reco;
        reco(condition);
        inSyncContext = true;
    }

    for (auto &i : stmt->alt_){
        if (i->exp_ && i->exp_->size() > 1){
            std::cout << "[Semantic error!]\n"
                      << "Each alternative can only contain one expression!";
            return 1;
        }
    }

    if (stmt->alt_.size() == 2){
        Cell *c = result->addCell(NEW_ID, "$mux");
        c->setPort("\\S", executeExpression(condition));

        caseStack.push(std::pair<bool, Cell*>(inSyncContext, c));

        for (auto &i : stmt->alt_){
            executeSequentialStmt(i->);
        }

        c->fixup_parameters();
    } else {
        if (inSyncContext){
            std::cout << "[Semantic error!]\n";
            std::cout << "more than 2 case branches with sync "
                "condition is not allowed!\n";
            return 1;
        }

        generateMuxer(stmt);
    }

    inSyncContext = false;

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
}
