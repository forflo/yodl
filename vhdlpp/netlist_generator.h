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

    // TODO: result should be an argument for NetlistGenerator
    NetlistGenerator() : result(0) {};

    int operator()(Entity *);

    Yosys::RTLIL::Module *result;

private:
    struct stack_element_t {
        virtual ~stack_element_t() = default;
    };

    // for conditions like `if (<c_1> o <clock_edge> o <c_2>)'
    struct dff_complex_netlist_t : stack_element_t {
        dff_complex_netlist_t() = default;
        dff_complex_netlist_t(const Yosys::RTLIL::SigSpec d,
                      const Yosys::RTLIL::SigSpec q,
                      const Yosys::RTLIL::SigSpec clk)
            : rhsInput(d)
            , output(q)
            , clock(clk) {}

        Yosys::RTLIL::SigSpec rhsInput, output, clock;
    };

    // for conditions like `if (<clock_edge>)'
    // or `if (<non-sync condition>)'
    struct flipflop_netlist_t : stack_element_t {
        flipflop_netlist_t() = default;
        flipflop_netlist_t(const Yosys::RTLIL::SigSpec d,
                           const Yosys::RTLIL::SigSpec q,
                           const Yosys::RTLIL::SigSpec clk)
            : input(d)
            , output(q)
            , clock(clk) {}

        Yosys::RTLIL::SigSpec input, output, clock;
    };

    struct muxer_netlist_t : stack_element_t {
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

    struct case_netlist_t : stack_element_t {
        case_netlist_t() = default;
        case_netlist_t(const std::map<string, muxer_netlist_t> &n,
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

private:
    std::map<string const, Expression const *> prevSigAssigns;
    std::map<string const, Expression const *> prevVarAssigns;

    std::vector<stack_element_t> contextStack;

    Entity *working;
    ScopeBase *currentScope;
};

#endif /* IVL_NETLIST_GENERATOR */
