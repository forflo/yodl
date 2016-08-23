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

    int operator()(const AstNode *);
    void reset(void);

    bool containsClockEdge;
    int numberClockEdges;
    NetlistGenerator::edge_spec direction;
};

#endif /* IVL_CLOCK_EDGE */
