#include <netlist_generator.h>
#include <parse_context.h>

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <sstream>
#include <mach7_includes.h>

using namespace Yosys::RTLIL;

int NetlistGenerator::operator()(Entity *entity){
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

int NetlistGenerator::traverseProcessStatement(ProcessStatement *proc){
    using namespace mch;
    currentScope = proc;

    int errors = 0;

    for (auto &i : proc->statements_){
        Match(i){
            Case(C<SignalSeqAssignment>()){
                SignalSeqAssignment *a =
                    dynamic_cast<SignalSeqAssignment*>(i);

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
