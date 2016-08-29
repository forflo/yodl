#ifndef IVL_NETLIST_GENERATOR
#define IVL_NETLIST_GENERATOR

#include <kernel/yosys.h>
#include <kernel/rtlil.h>

#include <list>
#include <map>
#include <stack>

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
        muxer_netlist_t() = default;
        muxer_netlist_t(
            const std::map<Yosys::RTLIL::SigSpec, Yosys::RTLIL::SigSpec> &i,
            Yosys::RTLIL::SigSpec o, unsigned int w)
            : inputPaths(i)
            , muxerOutput(o)
            , muxerWidth(w) { }
        // maps pahts to wires. Key sigspec is used to represent a choice
        // of a case sequential statement
        std::map<Yosys::RTLIL::SigSpec, Yosys::RTLIL::SigSpec> inputPaths;
        Yosys::RTLIL::SigSpec muxerOutput;
        unsigned int muxerWidth;
    };

    struct case_stack_element_t {
        case_stack_element_t() = default;
        case_stack_element_t(const std::map<string, muxer_netlist_t> &n,
                             const Yosys::RTLIL::SigSpec &s,
                             const std::set<string> &o)
            : occuringSignals(o)
            , netlist(n)
            , curWhenAlternative(s) {}

        std::set<string> occuringSignals;
        std::map<string, muxer_netlist_t> netlist;
        Yosys::RTLIL::SigSpec curWhenAlternative;
    };

private:
    Yosys::RTLIL::SigSpec sigSpecFromString(const std::string &);

    int traverseConcStmts(std::list<Architecture::Statement*> *);
    int traverseBlockStatement(BlockStatement *);
    int traverseProcessStatement(ProcessStatement *);

    int traverseAssignment(SignalSeqAssignment const *);



    int generateMuxerH(int, Yosys::RTLIL::Cell *,
                       std::vector<Yosys::RTLIL::SigBit> const &,
                       string,
                       std::map<Yosys::RTLIL::SigSpec,
                                Yosys::RTLIL::SigSpec> &);
    muxer_netlist_t generateMuxer(CaseSeqStmt const *);

    std::set<string> extractLhs(AstNode const *stmt);

    int executeIfStmt(IfSequential const *);

    int executeCaseStmt(CaseSeqStmt const *);
    int executeSequentialStmt(SequentialStmt const *);

    int executeSignalAssignmentCase(SignalSeqAssignment const *a);
    int executeSignalAssignmentNonCase(SignalSeqAssignment const *a);
    int executeSignalAssignment(SignalSeqAssignment const *);
    Yosys::RTLIL::SigSpec executeExpression(Expression const *);

    bool trySetSyncContext(void){
        if (inSyncContext) { return false; }
        else               { return inSyncContext = true; }
    }

private:
    std::map<string const, Expression const *> prevSigAssigns;
    std::map<string const, Expression const *> prevVarAssigns;

    std::vector<case_stack_element_t> caseStack;

    bool inSyncContext = false;

    Entity *working;
    ScopeBase *currentScope;
};

#endif /* IVL_NETLIST_GENERATOR */
