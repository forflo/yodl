#ifndef IVL_CLOCK_EDGE
#define IVL_CLOCK_EDGE

#include <netlist_generator.h>

class ClockEdgeRecognizer {
public:
    ClockEdgeRecognizer()
        : containsClockEdge(false)
        , numberClockEdges(0)
        , direction(NetlistGenerator::edge_spec::UNDEF)
        { }

    int operator()(AstNode *);
    void reset(void);

    bool containsClockEdge;
    int numberClockEdges;
    NetlistGenerator::edge_spec direction;

    // if containsClockEdge == true => at least one will be != NULL
    // if containsClockEdge == false => both will be NULL
    ExpName *clockNameExp;
    ExpFunc *clockFuncExp;
};

#endif /* IVL_CLOCK_EDGE */
