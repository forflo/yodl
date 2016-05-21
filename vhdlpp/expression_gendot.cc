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
simple_tree<map<string, string>> *ExpInteger::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpInteger"},
            {"value", (dynamic_cast<stringstream&>(
                stringstream{} << value_)).str()}});
}

simple_tree<map<string, string>> *ExpReal::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpReal"},
            {"value", (dynamic_cast<stringstream&>(
                stringstream{} << value_)).str()}}); 
}

simple_tree<map<string, string>> *ExpString::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpReal"},
            {"value", value_}});
}

simple_tree<map<string, string>> *ExpName::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpName"},
            {"value", name_.str()}});
}

simple_tree<map<string, string>> *ExpTime::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpTime"},
            {"unit", (dynamic_cast<stringstream&>(
                stringstream{} << unit_)).str()},
            {"time value", (dynamic_cast<stringstream&>(
                stringstream{} << amount_)).str()}}); 
}

simple_tree<map<string, string>> *ExpBitstring::emit_strinfo_tree() const {
    stringstream conv;
    for (vector<char>::const_iterator i = value_.begin();
            i != value_.end();
            ++i)
        conv << *i;

    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpBitstring"},
            {"value", conv.str()}});
}

simple_tree<map<string, string>> *ExpFunc::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpFunc"},
            {"value", name_.str()}});

    for (vector<Expression *>::const_iterator i = argv_.begin();
            i != argv_.end();
            ++i){
        result->forest.push_back((*i)->emit_strinfo_tree());
    }

    return result;
}

simple_tree<map<string, string>> *ExpCharacter::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpCharacter"},
            {"value", string{value_}}});
}


/* All ExpBinary implementors */
simple_tree<map<string, string>> *ExpArithmetic::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpArithmetic"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpLogical::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpLogical"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});

    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpRelation::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpRelation"},
            {"operator", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpShift::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
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
simple_tree<map<string, string>> *ExpEdge::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpEdge"},
            {"edgespec", (dynamic_cast<stringstream&>(
                stringstream{} << fun_)).str()}});
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

simple_tree<map<string, string>> *ExpUAbs::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpUAbs"},
            {"operator", "abs"}});
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

simple_tree<map<string, string>> *ExpUNot::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpUNot"},
            {"operator", "not"}});
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

/* Not implementing ExpBinary or ExpUnary but also has two operands */
simple_tree<map<string, string>> *ExpConcat::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpConcat"},
            {"operator", "&"}});
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        operand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpRange::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>(
        map<string, string>{
            {"node-type", "ExpRange"},
            {"rangespec", (dynamic_cast<stringstream&>(
                stringstream{} << direction_)).str()}});
    
    result->forest = {
        left_->emit_strinfo_tree(),
        right_->emit_strinfo_tree()};

    return result;
}
