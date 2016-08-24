#include <clock_edge_recognizer.h>
#include <tuple>
#include <utility>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>

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

void ClockEdgeRecognizer::reset(void){
    containsClockEdge = false;
    numberClockEdges = 0;
    direction = NetlistGenerator::edge_spec::UNDEF;

    clockNameExp = NULL;
    clockFuncExp = NULL;
}

int ClockEdgeRecognizer::operator()(const Expression *n){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    auto checkHelper = [this](perm_string attrName,
                              char charVal,
                              const ExpName *tmp,
                              const char *checkAttrName)
        -> void {
        if (tmp == NULL)
            std::cout << "ClockEdgeRecognizer checkHelper. Impossible!"
                      << std::endl;

        if ((charVal == '0' || charVal == '1') &&
            (!strcmp(attrName, checkAttrName))){

            clockNameExp = tmp;
            containsClockEdge = true;
            numberClockEdges++;
            if (charVal == '0')
                direction = NetlistGenerator::edge_spec::FALLING;
            else
                direction = NetlistGenerator::edge_spec::RISING;
        }
    };

    match(n,
          some<ExpFunc>(ds(_x, _x)),
          [this,checkHelper,n](perm_string funcName, std::vector<Expression*> v){
            clockFuncExp = dynamic_cast<const ExpFunc*>(n);

            if (v.size() == 1 &&
                (!strcmp(funcName, "falling_edge") ||
                 !strcmp(funcName, "rising_edge" ))) {
                containsClockEdge = true;
                numberClockEdges++;
            }

            if (!strcmp(funcName, "falling_edge"))
                direction = NetlistGenerator::edge_spec::FALLING;
            if (!strcmp(funcName, "rising_edge"))
                direction = NetlistGenerator::edge_spec::RISING;

          },
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
                          "event");
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
                              ->operand1_),
                          "event");
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
                          "stable");
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
                              ->operand1_),
                          "stable");
          },
          some(), [](const Expression &){std::cout << "dummy match!\n";},
          none(), [](){std::cout << "dummy match!\n";}
        );

//        // mirror cases for case 1
//        Case(C<ExpLogical>(
//                 C<ExpAttribute>(attrName),
//                 C<ExpRelation>(
//                     C<ExpName>(name),
//                     C<ExpCharacter>(charVal),
//                     relOp),
//                 logOp)){
//
//            checkHelper(attrName, charVal,
//                        dynamic_cast<const ExpName *>(
//                            dynamic_cast<const ExpRelation *>(
//                                dynamic_cast<const ExpLogical *>(n)
//                                ->operand2_)
//                            ->operand1_),
//                        "event", logOp, relOp);
//            break;
//        }
//        Case(C<ExpLogical>(
//                 C<ExpAttribute>(attrName),
//                 C<ExpRelation>(
//                     C<ExpCharacter>(charVal),
//                     C<ExpName>(name),
//                     relOp),
//                 logOp)){
//
//            checkHelper(attrName, charVal,
//                        dynamic_cast<const ExpName *>(
//                            dynamic_cast<const ExpRelation *>(
//                                dynamic_cast<const ExpLogical *>(n)
//                                ->operand2_)
//                            ->operand2_),
//                        "event", logOp, relOp);
//            break;
//        }
//
//        // mirror cases for case 2
//        Case(C<ExpLogical>(
//                 C<ExpUNot>(
//                     C<ExpAttribute>(attrName)),
//                 C<ExpRelation>(
//                     C<ExpName>(name),
//                     C<ExpCharacter>(charVal),
//                     relOp),
//                 logOp)){
//
//            checkHelper(attrName, charVal,
//                        dynamic_cast<const ExpName *>(
//                            dynamic_cast<const ExpRelation *>(
//                                dynamic_cast<const ExpLogical *>(n)
//                                ->operand2_)
//                            ->operand1_),
//                        "stable", logOp, relOp);
//            break;
//        }
//        Case(C<ExpLogical>(
//                 C<ExpUNot>(
//                     C<ExpAttribute>(attrName)),
//                 C<ExpRelation>(
//                     C<ExpCharacter>(charVal),
//                     C<ExpName>(name),
//                     relOp),
//                 logOp)){
//
//            checkHelper(attrName, charVal,
//                        dynamic_cast<const ExpName *>(
//                            dynamic_cast<const ExpRelation *>(
//                                dynamic_cast<const ExpLogical *>(n)
//                                ->operand2_)
//                            ->operand2_),
//                        "stable", logOp, relOp);
//            break;
//        }
//        Otherwise(){ break; }
//    } EndMatch;
    return 0;
}
