#include <netlist_generator.h>
#include <parse_context.h>

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <sstream>
#include <mach7_includes.h>

using namespace Yosys::RTLIL;

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

int NetlistGenerator::executeSignalAssignment(SignalSeqAssignment *a){
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

    SigSpec *res = executeExpression(tmp);

    result->connect(result->wire("\\" + s), res);

    return 0;
}

SigSpec *NetlistGenerator::executeExpression(Expression *exp){
    using namespace mch;

    Match(exp){
        Case(C<ExpCharacter>()){
            SigSpec *tmpSig;
            switch(dynamic_cast<ExpCharacter *>(exp)->value_){
            case '0':
                tmpSig = new SigSpec(Const(State::S0));
                break;
            case '1':
                tmpSig = new SigSpec(Const(State::S1));
                break;
            case 'z':
                tmpSig = new SigSpec(Const(State::Sz));
                break;
            case '-':
                tmpSig = new SigSpec(Const(State::Sa));
                break;
            }

            return tmpSig;

            break;
        }
        Case(C<ExpLogical>()){
            ExpLogical *t = dynamic_cast<ExpLogical *>(exp);

            Cell *c;
            SigSpec *out = new SigSpec();

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

            return out;
            break;
        }
        Case(C<ExpName>()){
            ExpName *n = dynamic_cast<ExpName *>(exp);
            // ugly, but neccessary because of the perm_strings are
            string strT = n->name_.str();
            Wire *w = result->wire("\\" + strT);

            if (w){
                return new SigSpec(w);
            } else {
                return new SigSpec(result->addWire("\\" + strT));
            }
        }
        Otherwise(){
            std::cout << "This kind of expression fails exec Expr!"
                      << std::endl;
            return NULL;
            break;
        }
    } EndMatch;

    return 0;
}

int NetlistGenerator::traverseProcessStatement(ProcessStatement *proc){
    using namespace mch;
    currentScope = proc;

    int errors = 0;

    for (auto &i : proc->statements_){
        Match(i){
            Case(C<SignalSeqAssignment>()){
                SignalSeqAssignment *tmp =
                    dynamic_cast<SignalSeqAssignment*>(i);

                errors += executeSignalAssignment(tmp);

                break;
            }
            Case(C<CaseSeqStmt>()){

                break;
            }
            Otherwise(){
                std::cout << "This statement type is not supported"
                          << endl;
                break;
            }
        } EndMatch;
    }

    return errors;
}
