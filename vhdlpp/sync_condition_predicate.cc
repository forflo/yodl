#include <sync_condition_predicate.h>
#include <clock_edge_recognizer.h>
#include <predicate_generators.h>
#include <generic_traverser.h>
#include <parse_context.h>
#include <propcalc.h>

#define DEBUG

#include <tuple>
#include <sstream>
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

        some(), [this](const Expression &){
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


// In order to be a sync condition, an expression has to fulfil
// the following formal predicate:
//
// isSyncCondition(e) =
//     isTypeOfBoolean(e) and
//     containsClockEdge(e) and
//     ((eval(e) = 1) -> (clockEdge(e) = 1))
bool SyncCondPredicate::operator()(const Expression *exp){
    ClockEdgeRecognizer clockEdges;
    bool isBoolType = true;

    if (currentEntity && currentScope){
        isBoolType = exp->probe_type(currentEntity, currentScope)->type_match(
            &currentEntity->context_->global_types->primitive_STDLOGIC);
    } else {
#ifdef DEBUG
        isBoolType = true;
#else
        isBoolType = false;
#endif
    }

    GenericTraverser traverser(
        makeTypePredicate<Expression>(),
        static_cast<function<int (const AstNode *)>>(
            [&clockEdges](const AstNode *n) -> int {
                return clockEdges(n); }),
        GenericTraverser::RECUR);

    traverser(exp);

    if (clockEdges.fullClockSpecs.size() != 1)
        return false; // NOTE: more than one clock spec currently not supported

    PropcalcFormula *formula = fromExpression(
        dynamic_cast<const Expression *>(
            clockEdges.fullClockSpecs[0]), exp);

    formula =
        new PropcalcTerm(
            new PropcalcTerm(
                formula,
                PropcalcTerm::XNOR,
                new PropcalcConstant(true)),
            PropcalcTerm::IFTHEN,
            new PropcalcTerm(
                new PropcalcVar("CLK"), //TODO: Remove hardcodedness!
                PropcalcTerm::XNOR,
                new PropcalcConstant(true)));

    std::cout << "formula: " << formula << "\n";
    std::cout << "proof of formula\n";

    bool successfullyProven = PropcalcApi::prove(formula);
    std::cout << (successfullyProven ? "true" : "false") << "\n";

    delete formula;

    return successfullyProven && isBoolType;
}
