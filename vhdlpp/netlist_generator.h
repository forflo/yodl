#ifndef IVL_NETLIST_GENERATOR
#define IVL_NETLIST_GENERATOR

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <list>

#include <entity.h>
#include <architec.h>
#include <sequential.h>

class NetlistGenerator {
public:
    NetlistGenerator() : result(0) {};

    int operator()(Entity *);

private:
    int traverseConcStmts(std::list<Architecture::Statement*> *);
    int traverseBlockStatement(BlockStatement *);
    int traverseProcessStatement(ProcessStatement *);

    int traverseCase(CaseSeqStmt *);
    int traverseAssignment(SignalSeqAssignment *);

    Yosys::RTLIL::Module *result;
};

#endif /* IVL_NETLIST_GENERATOR */
