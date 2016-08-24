#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <cstdint>
#include <string>
#include <stdexcept>
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

void PropcalcApi::extractNames(PropcalcFormula *form, std::set<std::string> &names){
    using namespace simple_match;
    using namespace simple_match::placeholders;

    match(form,
          some<PropcalcVar>(ds(_x)), [&names](std::string name){
              names.insert(name);
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

bool PropcalcApi::prooveH(PropcalcFormula *form,
             std::vector<std::string> todo,
             std::map<std::string, bool> m){
    if (todo.size() > 0){
        std::string tmp = *todo.begin();
        bool result = true;
        todo.erase(todo.begin());

        m[tmp] = true;
        result = result && prooveH(form, todo, m);

        m[tmp] = false;
        result = result && prooveH(form, todo, m);
        return result;
    } else if (todo.size() == 0) {
        return evaluate(form, m);
    } else {
        return false;
    }
}

bool PropcalcApi::proove(PropcalcFormula *form){
    std::set<std::string> names;
    extractNames(form, names);
    std::vector<std::string> v;
    for (auto &i: names)
        v.push_back(i);

    return prooveH(form, v, {});
}
