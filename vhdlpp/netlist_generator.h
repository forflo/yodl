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
    // I'm emulating algebraic data types here by using inheritance
    // for building variants. C++17 will provide std::varaint that will
    // render the following classes obsolete.
    // FUTURE-TODO: Adapt to new C++ standard

    /* netlist elements hold inputs and outputs of circuit parts */
    struct netlist_element_t {
        virtual ~netlist_element_t() = default;
    };

    // for conditions like `if (<c_1> o <clock_edge> o <c_2>)'
    struct dff_complex_netlist_t : netlist_element_t {
        dff_complex_netlist_t(const Yosys::RTLIL::SigSpec d,
                              const Yosys::RTLIL::SigSpec q)
            : input(d)
            , output(q) {}

        Yosys::RTLIL::SigSpec input, output;
    };

    // for conditions like `if (<clock_edge>)'
    // or `if (<non-sync condition>)'
    struct flipflop_netlist_t : netlist_element_t {
        flipflop_netlist_t(const Yosys::RTLIL::SigSpec d,
                           const Yosys::RTLIL::SigSpec q)
            : input(d)
            , output(q) {}

        Yosys::RTLIL::SigSpec input, output;
    };

    struct muxer_netlist_t : netlist_element_t {
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

    /* stack elements hold mappings from signals to netlist elements */
    struct stack_element_t {
        virtual ~stack_element_t() = default;
        stack_element_t(const std::map<string, netlist_element_t> &n,
                        const std::set<string> &o)
            : netlist(n)
            , occuringSignals(o) { }

        std::map<string, netlist_element_t> netlist;
        std::set<string> occuringSignals;
    };

    struct case_t : stack_element_t {
        case_t(const std::map<string, netlist_element_t> &n,
                       const Yosys::RTLIL::SigSpec &s,
                       const std::set<string> &o)
            : stack_element_t(n, o)
            , curWhenAlternative(s) {}

        Yosys::RTLIL::SigSpec curWhenAlternative;
    };

    struct if_dff_t : stack_element_t {
        if_dff_t(const std::map<string, netlist_element_t> &n,
                 const std::set<string> &o)
            : stack_element_t(n, o) {}
    };

    struct if_latch_t : stack_element_t {
        if_latch_t(const std::map<string, netlist_element_t> &n,
                   const std::set<string> &o)
            : stack_element_t(n, o) {}
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
    std::set<string> extractLhs(std::list<SequentialStmt *> const &l);

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
