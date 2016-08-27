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
    int traverseConcStmts(std::list<Architecture::Statement*> *);
    int traverseBlockStatement(BlockStatement *);
    int traverseProcessStatement(ProcessStatement *);

    int traverseAssignment(SignalSeqAssignment const *);


    int generateMuxerH(int, Yosys::RTLIL::Cell *,
                       std::vector<Yosys::RTLIL::SigBit> const &,
                       string,
                       std::map<std::string, Yosys::RTLIL::SigSpec> &);
    int generateMuxer(CaseSeqStmt const *);

    int executeCaseStmt(CaseSeqStmt const *);
    int executeSequentialStmt(SequentialStmt const *);
    int executeSignalAssignment(SignalSeqAssignment const *);
    Yosys::RTLIL::SigSpec executeExpression(Expression const *);

    std::map<perm_string const, Expression const *> prevSigAssigns;
    std::map<perm_string const, Expression const *> prevVarAssigns;

    std::stack<std::pair<bool, Yosys::RTLIL::Cell *>> caseStack;

    // save current synchronized state and appropriate clock wire

    Entity *working;
    ScopeBase *currentScope;
};

#endif /* IVL_NETLIST_GENERATOR */
