#include <sync_condition_predicate.h>
#include <clock_edge_recognizer.h>
#include <generic_traverser.h>
#include <propcalc.h>

#include <tuple>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "simple_match/include/simple_match/simple_match.hpp"

namespace simple_match {
    namespace customization {
        template<> struct tuple_adapter<ExpFunc>{
            enum { tuple_len = 2 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.name_, t.argv_)); }
        };

        template<> struct tuple_adapter<ExpRelation>{
            enum {tuple_len = 3 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.operand1_, t.operand2_, t.fun_)); }
        };

        template<> struct tuple_adapter<ExpAttribute>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.name_)); }
        };

        template<> struct tuple_adapter<ExpUNot>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.operand1_)); }
        };

        template<> struct tuple_adapter<ExpLogical>{
            enum { tuple_len = 3 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.operand1_, t.operand2_, t.fun_)); }
        };

        template<> struct tuple_adapter<ExpCharacter>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.value_)); }
        };

        template<> struct tuple_adapter<ExpName>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(
                    std::tie(t.name_)); }
        };
    }
}

PropcalcFormula *SyncCondPredicate::fromExpression(const Expression *clockEdge,
                                                   const Expression *exp){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    static int counter = 0;

    if (clockEdge == exp)
        return new PropcalcVar("CLK");

    return match(
        exp,

        some<ExpLogical>(ds(_x, _x, _x)),
        [this, clockEdge](const Expression *l,
               const Expression *r,
               ExpLogical::fun_t op){

            return static_cast<PropcalcFormula*>(
                new PropcalcTerm(
                    fromExpression(clockEdge, l),
                    PropcalcApi::fromExpLogical(op),
                    fromExpression(clockEdge, r)));
        },

        some<ExpUNot>(ds(_x)),
        [this, clockEdge](const Expression *r){
            return static_cast<PropcalcFormula*>(
                new PropcalcNot(fromExpression(clockEdge, r)));
        },

        some(), [](const Expression &){
            stringstream tmp;
            tmp << "V" << counter++;

            return static_cast<PropcalcFormula*>(
                new PropcalcVar(tmp.str()));
        },
        none(), [](){
            std::cout << "Error NULL!\n";
            return static_cast<PropcalcFormula*>(
                new PropcalcConstant(false));
        });
}


/* WARNING: Only meant to work on Expressions of type BOOL! */
bool SyncCondPredicate::operator()(const Expression *e){
    ClockEdgeRecognizer clockEdge;

    clockEdge(e);

    if (clockEdge.fullClockSpecs.size() != 1)
        return false; // NOTE: more than one clock spec currently not supported

    const AstNode *edge = clockEdge.fullClockSpecs[0];

    GenericTraverser traverser(
        [edge](const AstNode *n) -> bool {
            return (n == edge ? true : false);
        },
        static_cast<function<
        int (const AstNode *, const std::vector<const AstNode *> &)>>(
            [](const AstNode *n, const std::vector<const AstNode *> &v) -> int {

                return 0;
            }),
        GenericTraverser::RECUR);

    return false;
}
