/* FM. MA */

#include "expression.h"
#include "subprogram.h"
#include "parse_types.h"
#include "scope.h"
#include "simple_tree/simple_tree.h"
#include "enum_overloads.h"

#include <sstream>
#include <string>
#include <iostream>
#include <typeinfo>
#include <cstring>
#include <ivl_assert.h>
#include <cassert>
#include <map>
#include <vector>

using namespace std;

/* Literal expressions */
SimpleTree<map<string, string>> *ExpInteger::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpInteger"},
            {"value", (dynamic_cast<stringstream&>(
                stringstream{} << value_)).str()}});
}

SimpleTree<map<string, string>> *ExpReal::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpReal"},
            {"value", (dynamic_cast<stringstream&>(
                stringstream{} << value_)).str()}});
}

SimpleTree<map<string, string>> *ExpString::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpReal"},
            {"value", value_}});
}

SimpleTree<map<string, string>> *ExpName::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpName"},
            {"value", name_.str()}});

    for (auto &i : *indices_)
        result->forest.push_back(i->emit_strinfo_tree());

    // prefix_ needs no attention, because currently it's completely unsused
    //if (prefix_)
    //    result->forest.push_back(prefix_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpTime::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpTime"},
            {"unit", (dynamic_cast<stringstream&>(
                stringstream{} << unit_)).str()},
            {"time value", (dynamic_cast<stringstream&>(
                stringstream{} << amount_)).str()}});
}

SimpleTree<map<string, string>> *ExpBitstring::emit_strinfo_tree() const {
    stringstream conv;
    for (vector<char>::const_iterator i = value_.begin();
            i != value_.end();
            ++i)
        conv << *i;

    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpBitstring"},
            {"value", conv.str()}});
}

SimpleTree<map<string, string>> *ExpFunc::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpFunc"},
            {"value", name_.str()}});

    for (auto &i : argv_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpCharacter::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpCharacter"},
            {"value", string{value_}}});
}

SimpleTree<map<string, string>> *ExpConditional::case_t::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Case"}});

    /* In a statement like
     * fnord <= '1' when (start = '1') else '0';
     * a case_t object for the else alternative does
     * only contain NULL for the condition, which is
     * represented as pointer to Expression */
    if (cond_)
        result->forest.push_back(cond_->emit_strinfo_tree());

    for (auto &i : true_clause_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpConditional::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpConditional"}});

    for (auto &i : options_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpAggregate::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpAggregate"}});
}

SimpleTree<map<string, string>> *ExpAttribute::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
             {"node-type", "ExpAttribute"},
             {"attribute", name_.str()}});

    for (auto &i : *args_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpNew::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpNew"}});

    result->forest.push_back(size_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpDelay::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpDelay"}});

    result->forest.push_back(expr_->emit_strinfo_tree());
    result->forest.push_back(delay_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpCast::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpCast"}});

    result->forest.push_back(base_->emit_strinfo_tree());
    result->forest.push_back(type_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpObjAttribute::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
             {"node-type", "ExpObjAttribute"},
             {"attribute", name_.str()}});

    /* The attribute might not have arguments, in which
     * case args_ will be NULL */
    if (args_)
        for (auto &i : *args_)
            result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(base_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *ExpTypeAttribute::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
             {"node-type", "ExpTypeAttribute"},
             {"attribute", name_.str()}});

    for (auto &i : *args_)
        result->forest.push_back(i->emit_strinfo_tree());

    result->forest.push_back(base_->emit_strinfo_tree());

    return result;
}
/* All ExpBinary implementors */
SimpleTree<map<string, string>> *ExpArithmetic::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpArithmetic"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ExpLogical::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpLogical"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ExpRelation::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpRelation"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ExpShift::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpShift"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << shift_)).str()}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

/* All ExpUnary implementors */
SimpleTree<map<string, string>> *ExpEdge::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpEdge"},
            {"edgespec", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});

    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

SimpleTree<map<string, string>> *ExpUAbs::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpUAbs"},
            {"operator", "abs"}});

    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

SimpleTree<map<string, string>> *ExpUNot::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpUNot"},
            {"operator", "not"}});

    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

/* Not implementing ExpBinary or ExpUnary but also has two operands */
SimpleTree<map<string, string>> *ExpConcat::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpConcat"},
            {"operator", "&"}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

SimpleTree<map<string, string>> *ExpRange::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpRange"},
            {"rangespec", (dynamic_cast<stringstream&>(
                stringstream{} << direction_)).str()}});

    result->forest = {
        left_->emit_strinfo_tree(),
        right_->emit_strinfo_tree()};

    return result;
}
