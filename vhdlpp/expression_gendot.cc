/* FM. MA */

#define XTL_DEFAULT_SYNTAX 'S'

/* FM. Pattern matching */
#include "mach7/match.hpp"                 // Support for Match statement
#include "mach7/patterns/constructor.hpp"  // Support for constructor patterns
#include "mach7/patterns/n+k.hpp"          // Support for n+k patterns

#include "expression.h"
#include "subprogram.h"
#include "parse_types.h"
#include "scope.h"
#include "simple_tree/simple_tree.h"

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

/* Overloaded for enum to string conversion */
ostream &operator<<(ostream &os, ExpLogical::fun_t fun){
    switch(fun){
        case ExpLogical::fun_t::AND  : os << "and"; break;
        case ExpLogical::fun_t::OR   : os << "or"; break;
        case ExpLogical::fun_t::NOR  : os << "nor"; break;
        case ExpLogical::fun_t::NAND : os << "nand"; break;
        case ExpLogical::fun_t::XOR  : os << "xor"; break;
        case ExpLogical::fun_t::XNOR : os << "xnor"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpEdge::fun_t fun){
    switch(fun){
        case ExpEdge::fun_t::NEGEDGE  : os << "negedge"; break;
        case ExpEdge::fun_t::ANYEDGE   : os << "anyedge"; break;
        case ExpEdge::fun_t::POSEDGE  : os << "posedge"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpTime::timeunit_t fun){
    switch(fun){
        case ExpTime::FS: os << "fs"; break;
        case ExpTime::PS: os << "ps"; break;
        case ExpTime::NS: os << "ns"; break;
        case ExpTime::US: os << "us"; break;
        case ExpTime::MS: os << "ms"; break;
        case ExpTime::S : os << "s"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpRange::range_dir_t fun){
    switch(fun){
        case ExpRange::TO     : os << "to"; break;
        case ExpRange::DOWNTO : os << "downto"; break;
        case ExpRange::AUTO   : os << "auto"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpShift::shift_t fun){
    switch(fun){
        case ExpShift::shift_t::SRL: os << "srl"; break;
        case ExpShift::shift_t::SLL: os << "sll"; break;
        case ExpShift::shift_t::SRA: os << "sra"; break;
        case ExpShift::shift_t::SLA: os << "sla"; break;
        case ExpShift::shift_t::ROL: os << "rol"; break;
        case ExpShift::shift_t::ROR: os << "ror"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpRelation::fun_t fun){
    switch(fun){
        case ExpRelation::fun_t::EQ : os << "="; break;
        case ExpRelation::fun_t::LT : os << "<"; break;
        case ExpRelation::fun_t::GT : os << ">"; break;
        case ExpRelation::fun_t::NEQ: os << "/="; break;
        case ExpRelation::fun_t::LE : os << "<="; break;
        case ExpRelation::fun_t::GE : os << ">="; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ExpArithmetic::fun_t fun){
    switch(fun){
        case ExpArithmetic::fun_t::PLUS: os << "+"; break;
        case ExpArithmetic::fun_t::MINUS: os << "-"; break;
        case ExpArithmetic::fun_t::MULT: os << "*"; break;
        case ExpArithmetic::fun_t::DIV: os << "/"; break;
        case ExpArithmetic::fun_t::MOD: os << "mod"; break;
        case ExpArithmetic::fun_t::REM: os << "rem"; break;
        case ExpArithmetic::fun_t::POW: os << "**"; break;
        case ExpArithmetic::fun_t::xCONCAT: os << "&"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

/* Literal expressions */
simple_tree<map<string, string>> *ExpInteger::emit_string_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpInteger"},
        {"value", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(value_))).str()}}; 
}

simple_tree<map<string, string>> *ExpReal::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpReal"},
        {"value", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(value_))).str()}}; 
}

simple_tree<map<string, string>> *ExpString::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpReal"},
        {"value", value_}};
}

simple_tree<map<string, string>> *ExpName::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpName"},
        {"value", name_.str()}};
}

simple_tree<map<string, string>> *ExpTime::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpTime"},
        {"unit", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(unit_))).str()},
        {"time value", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(amount_))).str()}}; 
}

simple_tree<map<string, string>> *ExpBitstring::emit_strinfo_tree() const {
    stringstream conv;
    for (vector<char>::iterator i = value_.begin();
            i != value_.end();
            ++i)
        conv << *i;

    return new simple_tree<map<string, string>>{
        {"node-type", "ExpBitstring"},
        {"value", conv.str()}};
}

simple_tree<map<string, string>> *ExpFunc::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpFunc"},
        {"value", name_.str()} };

    for (vector<Expression *>::iterator i = argv_.begin();
            i != argv_.end();
            ++i){
        result->forest.push_back((*i)->emit_strinfo_tree());
    }

    return result;
}

simple_tree<map<string, string>> *ExpCharacter::emit_strinfo_tree() const {
    return new simple_tree<map<string, string>>{
        {"node-type", "ExpCharacter"},
        {"value", string{value_}} };
}


/* All ExpBinary implementors */
simple_tree<map<string, string>> *ExpArithmetic::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpArithmetic"},
        {"operator", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(fun_))).str()} };
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        opreand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpLogical::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpLogical"},
        {"operator", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(fun_))).str()}};

    (*result).forest = {
        (*operand1_).emit_strinfo_tree(),
        (*operand2_).emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpRelation::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpRelation"},
        {"operator", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(fun_))).str()} };
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        opreand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpShift::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpShift"},
        {"operator", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(shift_))).str()} };
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        opreand2_->emit_strinfo_tree()};

    return result;
}

/* All ExpUnary implementors */
simple_tree<map<string, string>> *ExpEdge::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpEdge"},
        {"edgespec", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(fun_))).str()} };
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

simple_tree<map<string, string>> *ExpUAbs::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpUAbs"},
        {"operator", "abs"}};
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

simple_tree<map<string, string>> *ExpUNot::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpUNot"},
        {"operator", "not"}};
    
    result->forest = { operand1_->emit_strinfo_tree() };

    return result;
}

/* Not implementing ExpBinary or ExpUnary but also has two operands */
simple_tree<map<string, string>> *ExpConcat::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpConcat"},
        {"operator", "&"}};
    
    result->forest = {
        operand1_->emit_strinfo_tree(),
        opreand2_->emit_strinfo_tree()};

    return result;
}

simple_tree<map<string, string>> *ExpRange::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpRange"},
        {"rangespec", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(direction_))).str()} };
    
    result->forest = {
        left_->emit_strinfo_tree(),
        right_->emit_strinfo_tree()};

    return result;
}
