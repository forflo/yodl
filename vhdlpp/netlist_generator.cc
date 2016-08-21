#include <netlist_generator.h>
#include <parse_context.h>

#include <functional>
#include <stateful_lambda.h>

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <sstream>
#include <mach7_includes.h>

using namespace Yosys::RTLIL;

namespace mch {
    template <> struct bindings<ExpUNot> {
        Members(ExpUNot::operand1_);
    };

    template <> struct bindings<ExpCharacter> {
        Members(ExpCharacter::value_);
    };

    template <> struct bindings<ExpAttribute> {
        Members(ExpAttribute::name_);
    };

    template <> struct bindings<ExpLogical> {
        Members(ExpLogical::operand1_,
                ExpLogical::operand2_);
    };

    template <> struct bindings<ExpFunc> {
        Members(ExpFunc::name_,
                ExpFunc::def_,
                ExpFunc::argv_);
    };

    template <> struct bindings<ExpName> {
        //TODO: add prefix
        Members(//ExpName::prefix_, // unique_ptr<ExpName>
                ExpName::name_,
                ExpName::indices_);
    };

    template <> struct bindings<ExpRelation> {
        Members(ExpRelation::operand1_,
                ExpRelation::operand2_);
    };
};

bool NetlistGenerator::containsSyncCondition(const Expression *e){
    using namespace mch;

    StatefulLambda<bool> visitor(
        false, static_cast<function<int (const AstNode *, bool &)>>(
            [](const AstNode *n, bool &env) -> int {
                var<perm_string> attrName, name;
                var<char> charVal;

                Match(n){
                    Case(C<ExpFunc>()){
                        // test if func only has one argument with type ExpName
                        // and if func's name either is "rising_edge" or
                        // "falling_edge"
                        break;
                    }
                    // case 1
                    Case(C<ExpLogical>(
                             C<ExpRelation>(
                                 C<ExpName>(name),
                                 C<ExpCharacter>(charVal)),
                             C<ExpAttribute>(attrName))){

                        // intentional fallthrough!
                    }
                    Case(C<ExpLogical>(
                             C<ExpRelation>(
                                 C<ExpCharacter>(charVal),
                                 C<ExpName>(name)),
                             C<ExpAttribute>(attrName))){

                        if ((charVal == '0' || charVal == '1') &&
                            (!strcmp(attrName, "event"))){
                            env = true;
                        }

                        break;
                    }

                    // case 2
                    Case(C<ExpLogical>(
                             C<ExpRelation>(
                                 C<ExpName>(name),
                                 C<ExpCharacter>(charVal)),
                             C<ExpUNot>(
                                 C<ExpAttribute>(attrName)))){

                        // intentional fallthough
                    }
                    Case(C<ExpLogical>(
                             C<ExpRelation>(
                                 C<ExpCharacter>(charVal),
                                 C<ExpName>(name)),
                             C<ExpUNot>(
                                 C<ExpAttribute>(attrName)))){

                        if ((charVal == '0' || charVal == '1') &&
                            (!strcmp(attrName, "stable"))){
                            env = true;
                        }

                        break;
                    }

                    // mirror cases for case 1
                    Case(C<ExpLogical>(
                             C<ExpAttribute>(attrName),
                             C<ExpRelation>(
                                 C<ExpName>(name),
                                 C<ExpCharacter>(charVal)))){

                        // intentional fallthrough!
                    }
                    Case(C<ExpLogical>(
                             C<ExpAttribute>(attrName),
                             C<ExpRelation>(
                                 C<ExpCharacter>(charVal),
                                 C<ExpName>(name)))){

                        if ((charVal == '0' || charVal == '1') &&
                            (!strcmp(attrName, "event"))){
                            env = true;
                        }

                        break;
                    }

                    // mirror cases for case 2
                    Case(C<ExpLogical>(
                             C<ExpUNot>(
                                 C<ExpAttribute>(attrName)),
                             C<ExpRelation>(
                                 C<ExpName>(name),
                                 C<ExpCharacter>(charVal)))){
                    }
                    Case(C<ExpLogical>(
                             C<ExpUNot>(
                                 C<ExpAttribute>(attrName)),
                             C<ExpRelation>(
                                 C<ExpCharacter>(charVal),
                                 C<ExpName>(name)))){

                        if ((charVal == '0' || charVal == '1') &&
                            (!strcmp(attrName, "stable"))){
                            env = true;
                        }
                        break;
                    }
                    Otherwise(){
                        return 0;
                    }
                } EndMatch;
                return 0;
            }));

    return false;
    //GenericTraverser();
}

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

    SigSpec res = executeExpression(tmp);

    result->connect(result->wire("\\" + s), res);

    return 0;
}

SigSpec NetlistGenerator::executeExpression(Expression *exp){
    using namespace mch;

    Match(exp){
        Case(C<ExpCharacter>()){
            SigSpec tmpSig;
            switch(dynamic_cast<ExpCharacter *>(exp)->value_){
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
            ExpUNot *t = dynamic_cast<ExpUNot *>(exp);

            Cell *c = result->addCell(NEW_ID, "$not");
            Wire *out = result->addWire(NEW_ID);

            c->setPort("\\A", executeExpression(t->operand1_));
            c->setPort("\\Y", out);

            c->fixup_parameters();
            return SigSpec(out);
            break;
        }
        Case(C<ExpRelation>()){
            ExpRelation *t = dynamic_cast<ExpRelation *>(exp);

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
        Case(C<ExpLogical>()){
            ExpLogical *t = dynamic_cast<ExpLogical *>(exp);

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
            ExpName *n = dynamic_cast<ExpName *>(exp);
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
