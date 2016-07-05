#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>

////
// mach7 setup
// Support for N-ary Match statement on patterns
#include <type_switchN-patterns.hpp>
#include <address.hpp>      // Address and dereference combinators
#include <bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <constructor.hpp>  // Support for constructor patterns
#include <equivalence.hpp>  // Equivalence combinator +

////
// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "scope.h"
#include "StringHeap.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "vtype.h"

// tag: [DESIGN HIERARCHY]
// template specializiation for desing hierarchies
namespace mch {
    template <> struct bindings<Architecture> {
        Members(Architecture::statements_, // list<Architecture::Statement*>
                Architecture::cur_component_, // ComponentInstanciation *
                //Architecture::cur_process_, // ProcessStatement *
                Architecture::name_); //perm_string
    };

    template <> struct bindings<Entity> {
        Members(Entity::arch_, // map<perm_string, Architecture*>
                Entity::bind_arch_, //Architecture *
                Entity::declarations_); //map <perm_string, VType::decl_t>
    };

    template <> struct bindings<ComponentBase> {
        Members(ComponentBase::name_, // perm_string
                ComponentBase::parms_, //vector<InterfacePort*>
                ComponentBase::ports_); //vector<InterfacePort*>
    };

    template <> struct bindings<ScopeBase> {
        Members(ScopeBase::old_signals_, //map<perm_string, Signal*>
                ScopeBase::new_signals_, //same
                ScopeBase::old_variables_, //map<perm_string, Variable*>
                ScopeBase::new_variables_, //same
                ScopeBase::new_components_, //map<perm_string, ComponentBase*>
                ScopeBase::new_components_, //same
                ScopeBase::use_types_, //map<perm_string, const VType*>
                ScopeBase::cur_types_, //same
                ScopeBase::use_constants_, //map<perm_string, struct const_t*>
                ScopeBase::use_subprograms_);//same
        // Due to a limitiation of Mach7 we're stuck with 10 member
        // bindings...

//        Members(//ScopeBase::use_subprograms_, //map<perm_string, SubHeaderList>
//                ScopeBase::cur_subprograms_, //same
//                ScopeBase::scopes_, //map<perm_string, ScopeBase*>
//                ScopeBase::use_enums_, //list<const VTypeEnum*>
//                ScopeBase::finalizers_, //list<SequentialStmt*>
//                ScopeBase::initializers_, //same
//                ScopeBase::package_header,//Package *
//                ScopeBase::name_); // perm_string
    };
};

// tag: [CONCURRENT STATEMENT]
// template specializations for the Architecture::Statemet class tree
namespace mch {
    //template <> struct bindings<Architecture::Statement> {};

    template <> struct bindings<GenerateStatement> {
        Members(GenerateStatement::name_, //perm_string
                GenerateStatement::statements_); //Statement*
    };

    template <> struct bindings<ForGenerate> {
        Members(ForGenerate::genvar_, //perm_string
                ForGenerate::msb_, //Expression*
                ForGenerate::lsb_); //Expression*
    };

    template <> struct bindings<IfGenerate> {
        Members(IfGenerate::cond_); //Expression*
    };

    template <> struct bindings<SignalAssignment> {
        Members(SignalAssignment::lval_, //ExpName *
                SignalAssignment::rval_); //list<Expression*>
    };

    template <> struct bindings<CondSignalAssignment> {
        Members(CondSignalAssignment::lval_, //ExpName*
                CondSignalAssignment::options_, //list<ExpConditional::case_t*>
                CondSignalAssignment::sens_list_); //list<const ExpName*>
    };

    template <> struct bindings<ComponentInstantiation> {
        Members(ComponentInstantiation::iname_, //perm_string
                ComponentInstantiation::cname_, //same
                ComponentInstantiation::generic_map_, //map<perm_string, Expression*
                ComponentInstantiation::port_map_); //same
    };

    template <> struct bindings<BlockStatement> {
        Members(BlockStatement::label_, // perm_string
                BlockStatement::header_, // BlockHeader *
                BlockStatement::concurrent_stmts_); // list<Statement*>
    };

    template <> struct bindings<StatementList> {
        Members(StatementList::statements_); //list<SequentialStmt*>
    };

    template <> struct bindings<FinalStatement> {};

    template <> struct bindings<InitialStatement> {};

    template <> struct bindings<ProcessStatement> {
        Members(ProcessStatement::iname_, //perm_string
                ProcessStatement::sensitivity_list_); //list<Expression*>
    };
};

// tag: [EXPRESSION]
// template specializations for the Expression class tree
namespace mch {
    template <> struct bindings<Expression> {
        Members(Expression::type_); //const VType *
    };

    template <> struct bindings<ExpUnary> {
        Members(ExpUnary::operand1_); //Expression *
    };

    template <> struct bindings<ExpBinary> {
        Members(ExpBinary::operand1_, //Expression *
                ExpBinary::operand2_); //same
    };

    template <> struct bindings<ExpAggregate> {
        Members(ExpAggregate::elements_, //vector<ExpAggregate::element_t *>
                ExpAggregate::aggregate_) //vector<ExpAggregate::choice_element>
    };

    template <> struct bindings<ExpAggregate::element_t> {
        Members(ExpAggregate::element_t::fields_, //vector<choice_t*>
                ExpAggregate::element_t::val_) //Expression *
    };

    template <> struct bindings<ExpAggregate::choice_element> {
        Members(ExpAggregate::choice_element::choice, //ExpAggregate::choice_t *
                ExpAggregate::choice_element::expr, // Expression *
                ExpAggregate::choice_element::alias_flag); // bool
    };

    template <> struct bindings<ExpAggregate::choice_t> {
        Members(ExpAggregate::choice_t::expr_, // auto_ptr<Expression>
                ExpAggregate::choice_t::range_); // auto_ptr<ExpRange>
    };

    template <> struct bindings<ExpConcat> {
        Members(ExpConcat::operand1_, //Expression *
                ExpConcat::operand2_); //same
    };

    template <> struct bindings<ExpDelay> {
        Members(ExpDelay::expr_,
                ExpDelay::delay_);
    };

    template <> struct bindings<ExpNew> {
        Members(ExpNew::size_);
    };

    template <> struct bindings<ExpCast> {
        Members(ExpCast::base_,
                ExpCast::type_);
    };

    //TODO: no comments!
    //template <> struct bindings<ExpUNot> {}; not acutally needed
    //template <> struct bindings<ExpUAbs> {}; not acutally needed

    template <> struct bindings<ExpRange> {
        Members(ExpRange::left_,
                ExpRange::right_,
                // ExpRange::direction_,
                 ExpRange::range_expr_,
                ExpRange::range_base_);
                //ExpRange::range_reverse_);
    };

    template <> struct bindings<ExpTime> {
        Members(ExpTime::amount_,
                ExpTime::unit_);
    };

    template <> struct bindings<ExpArithmetic> {
        Members(ExpArithmetic::fun_);
    };

    template <> struct bindings<ExpCharacter> {
        Members(ExpCharacter::value_);
    };

    template <> struct bindings<ExpBitstring> {
        Members(ExpBitstring::value_);
    };

    template <> struct bindings<ExpAttribute> {
        Members(ExpAttribute::name_,
                ExpAttribute::args_);
    };

    template <> struct bindings<ExpTypeAttribute> {
        Members(ExpTypeAttribute::base_);
    };

    template <> struct bindings<ExpObjAttribute> {
        Members(ExpObjAttribute::base_);
    };

    template <> struct bindings<ExpLogical> {
        Members(ExpLogical::fun_);
    };

    template <> struct bindings<ExpInteger> {
        Members(ExpInteger::value_);
    };

    template <> struct bindings<ExpReal> {
        Members(ExpReal::value_);
    };

    template <> struct bindings<ExpEdge> {
        Members(ExpEdge::fun_); // enum fun_t
    };

    template <> struct bindings<ExpSelected> {
        Members(ExpSelected::selector_);
    };

    template <> struct bindings<ExpConditional> {
        Members(ExpConditional::options_);
    };

    template <> struct bindings<ExpConditional::case_t> {
        Members(ExpConditional::case_t::cond_,
                ExpConditional::case_t::true_clause_);
    };

    template <> struct bindings<ExpFunc> {
        Members(ExpFunc::name_,
                ExpFunc::def_,
                ExpFunc::argv_);
    };

    template <> struct bindings<ExpName> {
        //TODO: add prefix
        Members(//ExpName::prefix_,
                ExpName::name_,
                ExpName::indices_);
    };

    template <> struct bindings<ExpString> {
        Members(ExpString::value_);
    };

    template <> struct bindings<ExpShift> {
        Members(ExpShift::shift_);
    };

    template <> struct bindings<ExpScopedName> {
        Members(ExpScopedName::scope_name_,
                ExpScopedName::scope_,
                ExpScopedName::name_);
    };

    template <> struct bindings<ExpRelation> {
        Members(ExpRelation::fun_);
    };
};

// tag: [VTYPE]
// template specializations for Vtype class tree
namespace mch {
    template <> struct bindings<VType> {};

    template <> struct bindings<VTypeERROR> {};

    template <> struct bindings<VTypePrimitive> {
        Members(VTypePrimitive::type_, //type_t (enum of VTypePrimitive)
                VTypePrimitive::packed_); //bool
    };

    template <> struct bindings<VTypeArray::range_t> {
        Members(VTypeArray::range_t::msb_, //Expression*
                VTypeArray::range_t::lsb_, //Expression*
                VTypeArray::range_t::direction_); //bool
    };

    template <> struct bindings<VTypeArray> {
        Members(VTypeArray::etype_, //VType *
                VTypeArray::ranges_, //vector<range_t>
                VTypeArray::signed_flag_, //bool
                VTypeArray::parent_); //VTypeArray *
    };

    template <> struct bindings<VTypeRange> {
        Members(VTypeRange::base_); //const VType *
    };

    template <> struct bindings<VTypeRangeConst> {
        Members(VTypeRangeConst::start_, //const int64_t
                VTypeRangeConst::end_); //const int64_t
    };

    template <> struct bindings<VTypeRangeExpr> {
        Members(VTypeRangeExpr::start_, //Expression *
                VTypeRangeExpr::end_, //same
                VTypeRangeExpr::downto_); //bool
    };

    template <> struct bindings<VTypeEnum> {
        Members(VTypeEnum::names_); //vector<perm_string>
    };

    template <> struct bindings<VTypeRecord::element_t> {
        Members(VTypeRecord::element_t::name_, //perm_string
                VTypeRecord::element_t::type_); //const VType *
    };

    template <> struct bindings<VTypeRecord> {
        Members(VTypeRecord::elements_); //vector<element_t*>
    };

    template <> struct bindings<VTypeDef> {
        Members(VTypeDef::name_, //perm_string
                VTypeDef::type_); //const VType *
    };

    template <> struct bindings<VSubTypeDef> {};
};

// tag: [VSignal]
// template specializations for the Signal class tree (and for Variable)
namespace mch {
    template <> struct bindings<SigVarBase> {
        Members(SigVarBase::name_, //perm_string
                SigVarBase::type_, //const VType *t
                SigVarBase::init_expr_, //Expression *
                SigVarBase::refcnt_sequ_); // unsigned
    };

    template <> struct bindings<Signal> {};
    template <> struct bindings<Variable> {};
};

// tag: [SEQUENTIAL STATEMENT]
// template specializations for the Sequential class tree
namespace mch {
    template <> struct bindings<SequentialStmt> {};

    template <> struct bindings<LoopStatement> {
        Members(LoopStatement::name_, //perm_string
                LoopStatement::stmts_); //list<SequentialStmt*>
    };

    template <> struct bindings<WhileLoopStatement> {
        Members(WhileLoopStatement::cond_); //Expression *
    };

    template <> struct bindings<ForLoopStatement> {
        Members(ForLoopStatement::it_, //perm_string
                ForLoopStatement::range_); //ExpRange *
    };

    template <> struct bindings<BasicLoopStatement> {};

    template <> struct bindings<IfSequential> {
        Members(IfSequential::cond_, //Expression*
                IfSequential::if_, //list<SequentialStmt*>
                IfSequential::elsif_, //list<IfSequential::Elsif *>
                IfSequential::else_); // list<SequentialStmt*>
    };

    template <> struct bindings<ReturnStmt> {
        Members(ReturnStmt::val_); //Expression *
    };

    template <> struct bindings<ReportStmt> {
        Members(ReportStmt::msg_, //Expression *
                ReportStmt::severity_); //severity_t
                                        //(an enum from class Report Stmt)
    };

    template <> struct bindings<AssertStmt> {
        Members(AssertStmt::cond_);//Expression *
                //AssertStmt::default_msg_); //const char *
                                             // somehow does not compile
    };

    template <> struct bindings<WaitStmt> {
        Members(WaitStmt::type_, //typedef enum {...} wait_type_t
                WaitStmt::expr_, //Expression*
                WaitStmt::sens_list_); //set<ExpName*>
    };

    template <> struct bindings<SignalSeqAssignment> {
        Members(SignalSeqAssignment::lval_, //Expression *
                SignalSeqAssignment::waveform_); //list<Expression *>
    };

    template <> struct bindings<CaseSeqStmt::CaseStmtAlternative> {
        Members(CaseSeqStmt::CaseStmtAlternative::exp_, //list<Expression*>
                CaseSeqStmt::CaseStmtAlternative::stmts_); //list<sequentialStmt*>
    };

    template <> struct bindings<CaseSeqStmt> {
        Members(CaseSeqStmt::cond_, //Expression*
                CaseSeqStmt::alt_); //list<CaseStmtAlternative>
    };


    template <> struct bindings<ProcedureCall> {
        Members(ProcedureCall::name_, //perm_string
                ProcedureCall::param_list_, //list<named_expr_t*> *
                ProcedureCall::def_); //SubprogramHeader *
    };

    template <> struct bindings<VariableSeqAssignment> {
        Members(VariableSeqAssignment::lval_, //Expression*
                VariableSeqAssignment::rval_); //same
    };
};
