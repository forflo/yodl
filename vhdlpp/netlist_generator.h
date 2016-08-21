#ifndef IVL_NETLIST_GENERATOR
#define IVL_NETLIST_GENERATOR

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <list>
#include <map>
#include <stack>

#include <StringHeap.h>
#include <entity.h>
#include <architec.h>
#include <sequential.h>

class NetlistGenerator {
public:
    enum class edge_spec { FALLING, RISING, UNDEF };

    NetlistGenerator() : result(0) {};

    int operator()(Entity *);

    Yosys::RTLIL::Module *result;

private:
    std::pair<bool, NetlistGenerator::edge_spec>
    containsSyncCondition(const Expression *e);

    int traverseConcStmts(std::list<Architecture::Statement*> *);
    int traverseBlockStatement(BlockStatement *);
    int traverseProcessStatement(ProcessStatement *);

    int traverseCase(CaseSeqStmt *);
    int traverseAssignment(SignalSeqAssignment *);

    int executeSignalAssignment(SignalSeqAssignment *);
    Yosys::RTLIL::SigSpec executeExpression(Expression *exp);


    std::map<const char *,
             std::stack<Yosys::RTLIL::SigSpec *>> previousAssigns;

    Entity *working;
    ScopeBase *currentScope;
};

#endif /* IVL_NETLIST_GENERATOR */
