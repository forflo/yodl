#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <expression.h>
#include <vector>
#include <iostream>

#include <propcalc.h>

#include "simple_match/include/simple_match/simple_match.hpp"

/* A simple implementation of the propositional calculus */

namespace simple_match {
    namespace customization {
        template<> struct tuple_adapter<PropcalcVar>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.name_)); }};
        template<> struct tuple_adapter<PropcalcConstant>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.value_)); }};
        template<> struct tuple_adapter<PropcalcTerm>{
            enum { tuple_len = 3 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.l_, t.r_, t.op_)); }};
        template<> struct tuple_adapter<PropcalcNot>{
            enum { tuple_len = 1 };
            template<size_t I, class T> static decltype(auto)
                get(T&& t) { return std::get<I>(std::tie(t.r_)); }};
    }
}

bool PropcalcApi::evaluate(PropcalcFormula *form,
                           std::map<std::string, bool> &bindings){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    return match(
        form,
        some<PropcalcVar>(ds(_x)), [&bindings](std::string name){
            return bindings[name];
        },
        some<PropcalcConstant>(ds(_x)), [](bool value){
            return value;
        },
        some<PropcalcNot>(ds(_x)), [&bindings](PropcalcFormula *r){
            return evaluate(r, bindings);
        },
        some<PropcalcTerm>(ds(_x, _x, _x)),
        [form,&bindings](PropcalcFormula *l, PropcalcFormula *r,
                         PropcalcTerm::operator_t op){
            switch(op){
            case PropcalcTerm::operator_t::AND:
                return evaluate(l, bindings) && evaluate(r, bindings);
                break;
            case PropcalcTerm::operator_t::OR:
                return evaluate(l, bindings) || evaluate(r, bindings);
                break;
            case PropcalcTerm::operator_t::NOR:
                return !(evaluate(l, bindings) || evaluate(r, bindings));
                break;
            case PropcalcTerm::operator_t::XNOR:
                return evaluate(l, bindings) == evaluate(r, bindings);
                break;
            case PropcalcTerm::operator_t::NAND:
                return !(evaluate(l, bindings) && evaluate(r, bindings));
                break;
            case PropcalcTerm::operator_t::XOR:
                return (evaluate(l, bindings) != evaluate(r, bindings));
                break;
            case PropcalcTerm::operator_t::IFTHEN:
                return (!evaluate(l, bindings)) || evaluate(r, bindings);
                break;
            default:
                std::cout << "ERROR! Impossible condition\n"; return false;
                break;
            }
        },
        some(), [](PropcalcFormula &){ return false; },
        none(), [](){ return false; });
}

void PropcalcApi::extractNames(PropcalcFormula *form,
                               std::set<std::string> &names){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    match(form,
          some<PropcalcVar>(ds(_x)), [&names](std::string name){
              names.insert(name);
          },
          some<PropcalcNot>(ds(_x)), [&names](PropcalcFormula *r){
              extractNames(r, names);
          },
          some<PropcalcConstant>(ds(_x)), [](bool){}, // not needed
          some<PropcalcTerm>(ds(_x, _x, _x)),
          [&names](PropcalcFormula *l, PropcalcFormula *r, PropcalcTerm::operator_t){
              extractNames(l, names);
              extractNames(r, names);
          },
          some(), [](PropcalcFormula &){},
          none(), [](){});
    return;
}

std::ostream &operator<<(std::ostream &out, PropcalcFormula *form){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    match(form,
          some<PropcalcVar>(ds(_x)), [&out](std::string name){
              out << name ;
          },
          some<PropcalcConstant>(ds(_x)), [&out](bool value){
              out << (value ? "true" : "false");
          },
          some<PropcalcNot>(ds(_x)), [&out](PropcalcFormula *r){
              out << "~ " << r;
          },
          some<PropcalcTerm>(ds(_x, _x, _x)),
          [form,&out](PropcalcFormula *l, PropcalcFormula *r,
                      PropcalcTerm::operator_t op){
              switch(op){
              case PropcalcTerm::operator_t::AND:
                  out << "(" << l  << " & " << r << ")";
                  break;
              case PropcalcTerm::operator_t::OR:
                  out << "(" << l  << " | " << r << ")";
                  break;
              case PropcalcTerm::operator_t::NOR:
                  out << "(" << l  << " nor " << r << ")";
                  break;
              case PropcalcTerm::operator_t::NAND:
                  out << "(" << l  << " nand " << r << ")";
                  break;
              case PropcalcTerm::operator_t::XNOR:
                  out << "(" << l  << " <-> " << r << ")";
                  break;
              case PropcalcTerm::operator_t::XOR:
                  out << "(" << l  << " ^ " << r << ")";
                  break;
              case PropcalcTerm::operator_t::IFTHEN:
                  out << "(" << l  << " -> " << r << ")";
                  break;
              default:
                  std::cout << "ERROR! Impossible condition\n";
                  break;
              }
          },
          some(), [](PropcalcFormula &){ return;  },
          none(), [](){ return; });

    return out;
}

PropcalcTerm::operator_t PropcalcApi::fromExpLogical(ExpLogical::fun_t op){
    switch(op){
    case ExpLogical::fun_t::AND: return PropcalcTerm::operator_t::AND;
    case ExpLogical::fun_t::OR: return PropcalcTerm::operator_t::OR;
    case ExpLogical::fun_t::NOR: return PropcalcTerm::operator_t::NOR;
    case ExpLogical::fun_t::XOR: return PropcalcTerm::operator_t::XOR;
    case ExpLogical::fun_t::NAND: return PropcalcTerm::operator_t::NAND;
    case ExpLogical::fun_t::XNOR: return PropcalcTerm::operator_t::XNOR;
    case ExpLogical::fun_t::IFTHEN: return PropcalcTerm::operator_t::IFTHEN;
    }
}

bool PropcalcApi::proveH(PropcalcFormula *form,
                          std::vector<std::string> todo,
                          std::map<std::string, bool> m){
    if (todo.size() > 0){
        std::string tmp = *todo.begin();
        bool result = true;
        todo.erase(todo.begin());

        m[tmp] = true;
        result = result && proveH(form, todo, m);

        m[tmp] = false;
        result = result && proveH(form, todo, m);
        return result;
    } else if (todo.size() == 0) {
        return evaluate(form, m);
    } else {
        return false;
    }
}

bool PropcalcApi::prove(PropcalcFormula *form){
    std::set<std::string> names;
    extractNames(form, names);
    std::vector<std::string> v;

    for (auto &i: names)
        v.push_back(i);

    return proveH(form, v, {});
}
