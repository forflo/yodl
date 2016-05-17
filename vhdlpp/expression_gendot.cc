/* FM. MA */

# include "expression.h"
# include "subprogram.h"
# include "parse_types.h"
# include "scope.h"
# include "simple_tree/simple_tree.h"

# include <sstream>
# include <string>
# include <iostream>
# include <typeinfo>
# include <cstring>
# include <ivl_assert.h>
# include <cassert>
# include <map>

using namespace std;

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

/* Overloaded for enum to string conversion */
ostream &operator<<(ostream &os, ExpLogical::fun_t fun){
    switch(fun){
        case ExpLogical::fun_t::AND: os << "and"; break;
        case ExpLogical::fun_t::OR: os << "or"; break;
        case ExpLogical::fun_t::NOR: os << "nor"; break;
        case ExpLogical::fun_t::NAND: os << "nand"; break;
        case ExpLogical::fun_t::XOR: os << "xor"; break;
        case ExpLogical::fun_t::XNOR: os << "xnor"; break;
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

simple_tree<map<string, string>> *ExpLogical::emit_strinfo_tree() const {
    auto result = new simple_tree<map<string, string>>{
        {"node-type", "ExpLogical"},
        {"Operator", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(fun_))).str()},
        {"value", (dynamic_cast<stringstream&>(
            stringstream{}.operator<<(value_))).str()}}; 

    (*result).forest = {
        
    };

    return result;
}

simple_tree<map<string, string>> *
