/* FM. MA */

#include "expression.h"
#include "subprogram.h"
#include "parse_types.h"
#include "std_funcs.h"
#include "std_types.h"
#include "sequential.h"
#include "entity.h"
#include "scope.h"
#include "simple_tree/simple_tree.h"
#include "enum_overloads.h"

#include <sstream>
#include <string>
#include <iostream>

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
        case ExpEdge::fun_t::ANYEDGE  : os << "anyedge"; break;
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

ostream &operator<<(ostream &os, ScopeBase::transfer_type_t fun){
    switch(fun){
        case ScopeBase::transfer_type_t::SIGNALS: os << "SIGNALS"; break;
        case ScopeBase::transfer_type_t::VARIABLES: os << "VARIABLES"; break;
        case ScopeBase::transfer_type_t::COMPONENTS: os << "COMPONENTS"; break;
        case ScopeBase::transfer_type_t::ALL: os << "ALL"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, ReportStmt::severity_t fun){
    switch(fun){
        case ReportStmt::severity_t::UNSPECIFIED: os << "UNSPECIFIED"; break;
        case ReportStmt::severity_t::NOTE: os << "NOTE"; break;
        case ReportStmt::severity_t::WARNING: os << "WARNING"; break;
        case ReportStmt::severity_t::ERROR: os << "ERROR"; break;
        case ReportStmt::severity_t::FAILURE: os << "FAILURE"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, WaitStmt::wait_type_t fun){
    switch(fun){
        case WaitStmt::wait_type_t::ON: os << "ON"; break;
        case WaitStmt::wait_type_t::UNTIL: os << "UNTIL"; break;
        case WaitStmt::wait_type_t::FINAL: os << "FINAL"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, typedef_topo_t fun){
    switch(fun){
        case NONE: os << "NONE"; break;
        case PENDING: os << "PENDING"; break;
        case MARKED: os << "MARKED"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, VTypePrimitive::type_t fun){
    switch(fun){
        case VTypePrimitive::type_t::BIT: os << "BIT"; break;
        case VTypePrimitive::type_t::INTEGER: os << "INTEGER"; break;
        case VTypePrimitive::type_t::NATURAL: os << "NATURAL"; break;
        case VTypePrimitive::type_t::REAL: os << "REAL"; break;
        case VTypePrimitive::type_t::STDLOGIC: os << "STDLOGIC"; break;
        case VTypePrimitive::type_t::TIME: os << "TIME"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, entity_aspect_t::entity_aspect_type_t fun){
    switch(fun){
        case entity_aspect_t::ENTITY: os << "ENTITY"; break;
        case entity_aspect_t::CONFIGURATION: os << "CONFIGURATION"; break;
        case entity_aspect_t::OPEN: os << "OPEN"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, instant_list_t::application_domain_t fun){
    switch(fun){
        case instant_list_t::application_domain_t::ALL: os << "ALL"; break;
        case instant_list_t::application_domain_t::OTHERS: os << "OTHERS"; break;
        case instant_list_t::application_domain_t::NONE: os << "NONE"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

ostream &operator<<(ostream &os, port_mode_t fun){
    switch(fun){
        case PORT_NONE: os << "PORT_NONE"; break;
        case PORT_IN: os << "PORT_IN"; break;
        case PORT_OUT: os << "PORT_OUT"; break;
        case PORT_INOUT: os << "PORT_INOUT"; break;
        default: os.setstate(std::ios_base::failbit); break;
    }

    return os;
}

