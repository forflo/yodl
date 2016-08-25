#ifndef IVL_CLOCK_EDGE
#define IVL_CLOCK_EDGE

#include <netlist_generator.h>
#include <vector>
#include <expression.h>

class ClockEdgeRecognizer {
public:
    ClockEdgeRecognizer()
        : containsClockEdge(false)
        , numberClockEdges(0)
        , direction(NetlistGenerator::edge_spec::UNDEF)
        { }

    int operator()(const AstNode *);
    void reset(void);

    bool containsClockEdge;
    int numberClockEdges;
    NetlistGenerator::edge_spec direction;

    //either of type ExpFunc or ExpLogical
    std::vector<AstNode const *> fullClockSpecs = {};

    // if containsClockEdge == true => at least one will be != NULL
    // if containsClockEdge == false => both will be NULL
    const ExpName *clockNameExp;
    const ExpFunc *clockFuncExp;
};

#endif /* IVL_CLOCK_EDGE */
