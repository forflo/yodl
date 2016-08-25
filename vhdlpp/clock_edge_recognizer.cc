#include <clock_edge_recognizer.h>
#include <tuple>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "simple_match/include/simple_match/simple_match.hpp"

/* Binding customizations.
   Visit https://github.com/jbandela/simple_match#tutorial */
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

void ClockEdgeRecognizer::reset(void){
    containsClockEdge = false;
    numberClockEdges = 0;
    direction = NetlistGenerator::edge_spec::UNDEF;

    fullClockSpecs = {};

    clockNameExp = NULL;
    clockFuncExp = NULL;
}

int ClockEdgeRecognizer::operator()(const AstNode *n){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    auto checkHelper = [this](perm_string attrName,
                              char charVal,
                              const ExpName *tmp,
                              const AstNode *edge,
                              const char *checkAttrName) -> void {
        if (tmp == NULL)
            std::cout << "ClockEdgeRecognizer checkHelper. NULL!"
                      << std::endl;

        if ((charVal == '0' || charVal == '1') &&
            (!strcmp(attrName, checkAttrName))){

            fullClockSpecs.push_back(edge);
            clockNameExp = tmp;
            containsClockEdge = true;
            numberClockEdges++;
            if (charVal == '0')
                direction = NetlistGenerator::edge_spec::FALLING;
            else
                direction = NetlistGenerator::edge_spec::RISING;
        }
    };

    /* Since a clock edge can have varioius forms in VHDL, we need to
       check if the current Expression fit's any of them. For instance,
       a clock edge specification could be given as:

       - `clk = '1' and clk'event` or also as
       - `not clk'stable and clk = '0'` (This would model a falling clock edge)

       We need to check for all the permutations and extract the
       according semantics. Because of this, the following code is not
       very beautiful */
    match(n,
          some<ExpFunc>(ds(_x, _x)),
          [this,n](perm_string funcName, std::vector<Expression*> v){
            clockFuncExp = dynamic_cast<const ExpFunc*>(n);

            if (v.size() == 1 &&
                (!strcmp(funcName, "falling_edge") ||
                 !strcmp(funcName, "rising_edge" ))) {
                containsClockEdge = true;
                fullClockSpecs.push_back(n);
                numberClockEdges++;
            }

            if (!strcmp(funcName, "falling_edge"))
                direction = NetlistGenerator::edge_spec::FALLING;
            if (!strcmp(funcName, "rising_edge"))
                direction = NetlistGenerator::edge_spec::RISING;

          },

          /* the associated lambda only get's executed if n's
             tree structure matches the following pattern:
             (Words in parentheses are member names)

             (n) has type ExpLogical
              |
              +-> (operand1_) has type ExpRelation
              |   +-> (operand1_) has type ExpName
              |   +-> (operand2_) has type ExpCharacter
              |   |
              |   +-> (fun_) Operator is EQ
              +-> (operand2_) is type of ExpAttribute
              |
              +-> (fun_) Operator is AND */
          some<ExpLogical>(
              ds(some<ExpRelation>(
                     ds(some<ExpName>(ds(_x)),
                        some<ExpCharacter>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 some<ExpAttribute>(ds(_x)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string , char chr, perm_string atr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand1_)
                              ->operand1_),
                          dynamic_cast<const Expression *>(n), "event");
          },

          some<ExpLogical>(
              ds(some<ExpRelation>(
                     ds(some<ExpCharacter>(ds(_x)),
                        some<ExpName>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 some<ExpAttribute>(ds(_x)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](char chr, perm_string , perm_string atr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand1_)
                              ->operand2_),
                          dynamic_cast<const Expression *>(n), "event");
          },

          some<ExpLogical>(
              ds(some<ExpRelation>(
                     ds(some<ExpName>(ds(_x)),
                        some<ExpCharacter>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 some<ExpUNot>(
                     ds(some<ExpAttribute>(ds(_x)))),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string, char chr, perm_string atr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand1_)
                              ->operand1_),
                          dynamic_cast<const Expression *>(n), "stable");
          },

          some<ExpLogical>(
              ds(some<ExpRelation>(
                     ds(some<ExpCharacter>(ds(_x)),
                        some<ExpName>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 some<ExpUNot>(
                     ds(some<ExpAttribute>(ds(_x)))),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](char chr, perm_string, perm_string atr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand1_)
                              ->operand2_),
                          dynamic_cast<const Expression *>(n), "stable");
          },

          some<ExpLogical>(
              ds(some<ExpUNot>(
                     ds(some<ExpAttribute>(ds(_x)))),
                 some<ExpRelation>(
                     ds(some<ExpCharacter>(ds(_x)),
                        some<ExpName>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string atr, char chr, perm_string){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand2_)
                              ->operand2_),
                          dynamic_cast<const Expression *>(n), "stable");
          },

          some<ExpLogical>(
              ds(some<ExpUNot>(
                     ds(some<ExpAttribute>(ds(_x)))),
                 some<ExpRelation>(
                     ds(some<ExpName>(ds(_x)),
                        some<ExpCharacter>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string atr, perm_string, char chr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand2_)
                              ->operand1_),
                          dynamic_cast<const Expression *>(n), "stable");
          },

          some<ExpLogical>(
              ds(some<ExpAttribute>(ds(_x)),
                 some<ExpRelation>(
                     ds(some<ExpName>(ds(_x)),
                        some<ExpCharacter>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string atr, perm_string, char chr){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand2_)
                              ->operand1_),
                          dynamic_cast<const Expression *>(n), "stable");
          },

          some<ExpLogical>(
              ds(some<ExpAttribute>(ds(_x)),
                 some<ExpRelation>(
                     ds(some<ExpCharacter>(ds(_x)),
                        some<ExpName>(ds(_x)),
                        ExpRelation::fun_t::EQ)),
                 ExpLogical::fun_t::AND)),
          [checkHelper,n](perm_string atr, char chr, perm_string){
              checkHelper(atr, chr,
                          dynamic_cast<const ExpName *>(
                              dynamic_cast<const ExpRelation *>(
                                  dynamic_cast<const ExpLogical *>(n)
                                  ->operand2_)
                              ->operand2_),
                          dynamic_cast<const Expression *>(n), "stable");
          },
          some(), [](const AstNode &){std::cout << "Don't know?!!\n";},
          none(), [](){std::cout << "Null pointer was given!\n";});

    return 0;
}
