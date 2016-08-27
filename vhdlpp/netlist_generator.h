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
    struct muxer_netlist_t {
        muxer_netlist_t(
            const std::map<std::string, Yosys::RTLIL::SigSpec> &i,
            Yosys::RTLIL::SigSpec o, unsigned int w)
            : inputPaths(i)
            , muxerOutput(o)
            , muxerWidth(w) { }
        std::map<std::string, Yosys::RTLIL::SigSpec> inputPaths;
        Yosys::RTLIL::SigSpec muxerOutput;
        unsigned int muxerWidth;
    };

    struct case_stack_element_t {
        case_stack_element_t(const std::map<perm_string, muxer_netlist_t> &n,
                             const Yosys::RTLIL::SigSpec &s)
            : netlist(n)
            , curWhenAlternative(s) {}

        std::map<perm_string, muxer_netlist_t> netlist;
        Yosys::RTLIL::SigSpec curWhenAlternative;
    };

private:
    int traverseConcStmts(std::list<Architecture::Statement*> *);
    int traverseBlockStatement(BlockStatement *);
    int traverseProcessStatement(ProcessStatement *);

    int traverseAssignment(SignalSeqAssignment const *);


    int generateMuxerH(int, Yosys::RTLIL::Cell *,
                       std::vector<Yosys::RTLIL::SigBit> const &,
                       string,
                       std::map<std::string, Yosys::RTLIL::SigSpec> &);
    muxer_netlist_t generateMuxer(CaseSeqStmt const *);

    int executeCaseStmt(CaseSeqStmt const *);
    int executeSequentialStmt(SequentialStmt const *);
    int executeSignalAssignment(SignalSeqAssignment const *);
    Yosys::RTLIL::SigSpec executeExpression(Expression const *);

    std::map<perm_string const, Expression const *> prevSigAssigns;
    std::map<perm_string const, Expression const *> prevVarAssigns;

    std::vector<case_stack_element_t> caseStack;

    // save current synchronized state and appropriate clock wire

    Entity *working;
    ScopeBase *currentScope;
};

#endif /* IVL_NETLIST_GENERATOR */
