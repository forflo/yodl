#include <netlist_generator.h>
#include <parse_context.h>

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <sstream>
#include <mach7_includes.h>

using namespace Yosys::RTLIL;

int NetlistGenerator::operator()(Entity *entity){
    if (result == NULL)
        result = new Module();

    for (auto &i : entity->ports_){
        if (!i->type->type_match(
                &entity->context_->global_types->primitive_STDLOGIC)){
            std::cout << "Type error in netlist generator";
            std::cout << "Types other than std_logic are not supported";
            return 1;
        }

        if (i->expr) {
            std::cout << "Netlist generator doesn't support default values";
            return 1;
        }
    }

    auto iter = entity->arch_.begin();
    auto arch = dynamic_cast<Architecture*>(iter->second);

    for (auto &i : entity->ports_){
        result->addWire(string("\\") + i->name.str());
    }

    return traverseConcStmts(arch->statements_);
}

int NetlistGenerator::traverseConcStmts(
    list<Architecture::Statement *> *stmts){
    using namespace mch;

    int errors = 0;

    for (auto &i : stmts){
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

}

int NetlistGenerator::traverseProcessStatement(BlockStatement *proc){

}
