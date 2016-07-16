#ifndef IVL_sequential_H
#define IVL_sequential_H

/*
 * Copyright (c) 2011-2014 Stephen Williams (steve@icarus.com)
 * Copyright CERN 2013 / Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include <set>

# include "LineInfo.h"
# include "parse_types.h"
# include "root_class.h"

class ScopeBase;
class Entity;
class Expression;
class SequentialStmt;

using namespace std;

struct SeqStmtVisitor {
    virtual ~SeqStmtVisitor() {}

    virtual void operator()(SequentialStmt *s) = 0;
};

// --OK DOT
class SequentialStmt : public LineInfo, public AstNode {
public:
    SequentialStmt();
    virtual ~SequentialStmt() = 0;

public:
    virtual int elaborate(Entity *ent, ScopeBase *scope);
    virtual int emit(ostream& out, Entity *entity, ScopeBase *scope);
    virtual void dump(ostream& out, int indent) const;
    virtual void write_to_stream(ostream& fd);

    // FM. MA
    virtual SimpleTree<map<string, string>> *emit_strinfo_tree() const = 0;
    virtual SequentialStmt *clone() const = 0;

    // Recursively visits a tree of sequential statements.
    virtual void visit(SeqStmtVisitor& func) {
        func(this);
    }
};

// --OK DOT
/* The LoopStatement is an abstract base class for the various loop
 * statements. */
class LoopStatement : public SequentialStmt {
public:
    LoopStatement(perm_string block_name, list<SequentialStmt *> *);
    virtual ~LoopStatement();

    inline perm_string loop_name() const {
        return name_;
    }

    void dump(ostream& out, int indent)  const;
    void visit(SeqStmtVisitor& func);

public:
    int elaborate_substatements(Entity *ent, ScopeBase *scope);
    int emit_substatements(ostream& out, Entity *ent, ScopeBase *scope);
    void write_to_stream_substatements(ostream& fd);

public:
    perm_string                 name_;
    list<SequentialStmt *> stmts_;
};

// --OK DOT
class IfSequential : public SequentialStmt {
public:
    // --OK DOT
    class Elsif : public LineInfo, public AstNode {
    public:
        Elsif(Expression *cond, list<SequentialStmt *> *tr);
        ~Elsif();

        int elaborate(Entity *entity, ScopeBase *scope);
        int condition_emit(ostream& out, Entity *entity, ScopeBase *scope);
        int statement_emit(ostream& out, Entity *entity, ScopeBase *scope);

        void condition_write_to_stream(ostream& fd);
        void statement_write_to_stream(ostream& fd);

        void dump(ostream& out, int indent) const;
        void visit(SeqStmtVisitor& func);

        // FM. MA
        SimpleTree<map<string, string>> *emit_strinfo_tree() const;
        Elsif *clone() const;
    public:
        Expression *cond_;
        list<SequentialStmt *> if_;
    public:           // not implemented
        Elsif(const Elsif&);
        Elsif& operator =(const Elsif&);
    };

public:
    IfSequential(Expression *cond,
        list<SequentialStmt *> *tr,
        list<IfSequential::Elsif *> *elsif,
        list<SequentialStmt *> *fa);
    ~IfSequential();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;
    void visit(SeqStmtVisitor& func);

    const Expression *peek_condition() const {
        return cond_;
    }

    size_t false_size() const {
        return else_.size();
    }

    /* These method extract (and remove) the sub-statements from
     * the true or false clause. */
    void extract_true(list<SequentialStmt *>& that);
    void extract_false(list<SequentialStmt *>& that);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    IfSequential *clone() const { // TODO: Auslagern
        list<SequentialStmt*> *copy_if = new list<SequentialStmt*>();
        list<IfSequential::Elsif*> *copy_elseif = new list<IfSequential::Elsif*>();
        list<SequentialStmt*> *copy_else = new list<SequentialStmt*>();

        for (auto &i : if_)
            copy_if->push_back(i->clone());


        for (auto &i : elsif_)
            copy_elseif->push_back(i->clone());


        for (auto &i : else_)
            copy_else->push_back(i->clone());

        return new IfSequential(
            cond_->clone(),
            copy_if,
            copy_elseif,
            copy_else);
    };

public:
    Expression *cond_;
    list<SequentialStmt *>      if_;
    list<IfSequential::Elsif *> elsif_;
    list<SequentialStmt *>      else_;
};

// --OK DOT
class ReturnStmt : public SequentialStmt {
public:
    explicit ReturnStmt(Expression *val);

    ~ReturnStmt();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    const Expression *peek_expr() const {
        return val_;
    }

    void cast_to(const VType *type);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    ReturnStmt *clone() const {
        return new ReturnStmt(val_->clone());
    };

public:
    Expression *val_;
};

// --OK DOT
/* Grammar hint: signal_assignment ::= name LEQ waveform ';' */
class SignalSeqAssignment : public SequentialStmt {
public:
    SignalSeqAssignment(Expression *sig, list<Expression *> *wav);
    SignalSeqAssignment(Expression *sig, const list<Expression *> &wav);
    ~SignalSeqAssignment();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    SignalSeqAssignment *clone() const { //TODO: Auslagern
        list<Expression*> *copy = new list<Expression*>();

        for (auto &i : waveform_)
            copy->push_back(i->clone());

        return new SignalSeqAssignment(
            lval_->clone(),
            copy);
    };

public:
    Expression *lval_;
    list<Expression *> waveform_;
};

// --OK DOT
/* Grammar hint: case_statement ::= K_case expression K_is ... */
class CaseSeqStmt : public SequentialStmt {
public:
    class CaseStmtAlternative : public LineInfo, public AstNode {
    public:
        CaseStmtAlternative(list<Expression *> *exp,
                list<SequentialStmt *> *stmts);
        ~CaseStmtAlternative();
        void dump(ostream& out, int indent) const;
        int elaborate_expr(Entity *ent,
                ScopeBase *scope,
                const VType *ltype);
        int elaborate(Entity *ent, ScopeBase *scope);
        int emit(ostream& out, Entity *entity, ScopeBase *scope);
        void write_to_stream(ostream& fd);
        void visit(SeqStmtVisitor& func);

        // FM. MA
        SimpleTree<map<string, string>> *emit_strinfo_tree() const;
        // TODO: Auslagern
        CaseStmtAlternative *clone() const {
            list<Expression*> *copy_exp = NULL;
            list<SequentialStmt*> *copy_stmts = new list<SequentialStmt*>();

            if (exp_) {
                copy_exp = new list<Expression*>();

                for (auto &i : *exp_)
                    copy_exp->push_back(i->clone());
            }

            for (auto &i : stmts_)
                copy_stmts->push_back(i->clone());

            return new CaseStmtAlternative(copy_exp, copy_stmts);
        };

    public:         // not implemented
        CaseStmtAlternative(const CaseStmtAlternative&);
        CaseStmtAlternative& operator =(const CaseStmtAlternative&);

    public:
        list<Expression *>     *exp_;
        list<SequentialStmt *> stmts_;
    };

public:
    CaseSeqStmt(Expression *cond, list<CaseStmtAlternative *> *sp);
    ~CaseSeqStmt();

public:
    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void visit(SeqStmtVisitor& func);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    // TODO: Auslagern
    CaseSeqStmt *clone() const {
        list<CaseStmtAlternative*> *copy = new list<CaseStmtAlternative*>();

        for (auto &i : alt_)
            copy->push_back(i->clone());

        return new CaseSeqStmt(cond_->clone(), copy);
    };

public:
    Expression *cond_;
    list<CaseStmtAlternative *> alt_;
};

// --OK DOT
/* Grammar rule: procedure_call ::=
 *      IDENTIFIER ';'
 *      | IDENTIFIER '(' argument_list ')' ';' */
class ProcedureCall : public SequentialStmt {
public:
    explicit ProcedureCall(perm_string name);

    ProcedureCall(perm_string name, list<named_expr_t *> *param_list);
    ProcedureCall(perm_string name, list<Expression *> *param_list);
    ~ProcedureCall();

    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    ProcedureCall *clone() const {
        list<named_expr_t*> *copy = NULL;

        if (param_list_){
            copy = new list<named_expr_t*>();

            for (auto &i : *param_list_)
                copy->push_back(i->clone());
        }

        return new ProcedureCall(
            name_,
            copy);
    };

public:
    perm_string               name_;
    list<named_expr_t *> *param_list_;
    SubprogramHeader          *def_;
};

// --OK DOT
/* Grammar rule: variable_assignment ::= name VASSIGN expression ';' */
class VariableSeqAssignment : public SequentialStmt {
public:
    VariableSeqAssignment(Expression *sig, Expression *rval);
    ~VariableSeqAssignment();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    VariableSeqAssignment *clone() const {
        return new VariableSeqAssignment(
            lval_->clone(),
            rval_->clone());
    }

public:
    Expression *lval_;
    Expression *rval_;
};

// --OK DOT
/* Grammar rule: loop_statement ::=
 *   identifier_colon_opt K_while expression ...*/
class WhileLoopStatement : public LoopStatement {
public:
    WhileLoopStatement(perm_string loop_name,
                       Expression *, list<SequentialStmt *> *);
    ~WhileLoopStatement();

    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *ent, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    // TODO: Auslagern
    WhileLoopStatement *clone() const {
        list<SequentialStmt *> *copy = new list<SequentialStmt*>();

        for (auto &i : stmts_)
            copy->push_back(i->clone());

        return new WhileLoopStatement(
            name_,
            cond_->clone(),
            copy);
    }

public:
    Expression *cond_;
};


// --OK DOT
/* Grammar rule: loop_statement ::=
 *   identifier_colon_opt K_for IDENTIFIER K_in range
 *   K_loop sequence_of_statements ...*/
class ForLoopStatement : public LoopStatement {
public:
    ForLoopStatement(perm_string loop_name,
            perm_string index,
            ExpRange *,
            list<SequentialStmt *> *);
    ~ForLoopStatement();

    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *ent, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    // TODO: Auslagern
    ForLoopStatement *clone() const {
        list<SequentialStmt *> *copy_statements = new list<SequentialStmt*>();

        for (auto &i : stmts_)
            copy_statements->push_back(i->clone());

        return new ForLoopStatement(
            name_,
            it_,
            static_cast<ExpRange*>(range_->clone()),
            copy_statements);
    };

public:
    // Emits for-loop which direction is determined at run-time.
    // It is used for 'range & 'reverse_range attributes.
    int emit_runtime_(ostream& out, Entity *ent, ScopeBase *scope);

    perm_string it_; // FM. for it_ in ....
    ExpRange    *range_;
};

// --OK DOT
/* Grammar rule: loop_statement ::=
 *   identifier_colon_opt K_loop
 *   sequence_of_statements K_end ... */
class BasicLoopStatement : public LoopStatement {
public:
    BasicLoopStatement(perm_string lname, list<SequentialStmt *> *);
    ~BasicLoopStatement();

    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *ent, ScopeBase *scope);
    void write_to_stream(ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    // TODO: Auslagern
    BasicLoopStatement *clone() const {
        list<SequentialStmt *> *copy = new list<SequentialStmt *>();

        for (auto &i : stmts_)
            copy->push_back(i->clone());

        return new BasicLoopStatement(name_, copy);
    }
};

// --OK DOT
/* Grammar rule: report_statement ::=
 *   K_report expression severity_opt ';' */
class ReportStmt : public SequentialStmt {
public:
    typedef enum {
        UNSPECIFIED, NOTE,
        WARNING, ERROR,
        FAILURE
    } severity_t;

    ReportStmt(Expression *message, severity_t severity);
    virtual ~ReportStmt() {}

    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);

    inline Expression *message() const {
        return msg_;
    }

    inline severity_t severity() const {
        return severity_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    ReportStmt *clone() const {
        return new ReportStmt(
            msg_->clone(),
            severity_);
    };

public:
    void dump_sev_msg(ostream& out, int indent) const;

    Expression *msg_;
    severity_t severity_;
};

// --OK DOT
/* Grammar rule: assertion_statement ::=
 *   K_assert expression report_statement */
class AssertStmt : public ReportStmt {
public:
    AssertStmt(Expression *condition, Expression *message,
               ReportStmt::severity_t severity = ReportStmt::ERROR);

    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    AssertStmt *clone() const {
        return new AssertStmt(
            cond_->clone(),
            msg_->clone(),
            severity_);
    }

public:
    Expression *cond_;

    // Message displayed when there is no report assigned.
    static const char *default_msg_;
};

// --OK DOT
/* Grammar rule: wait_statement ::=
 *   K_wait K_for expression ';' */
class WaitForStmt : public SequentialStmt {
public:
    explicit WaitForStmt(Expression *delay);

    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    WaitForStmt *clone() const {
        return new WaitForStmt(delay_->clone());
    }

public:
    Expression *delay_;
};

// --OK DOT
class WaitStmt : public SequentialStmt {
public:
    typedef enum {
        ON, UNTIL,
        FINAL
    } wait_type_t;

    WaitStmt(wait_type_t typ, Expression *expression);

    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(ostream& fd);

    inline wait_type_t type() const {
        return type_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    WaitStmt *clone() const {
        set<ExpName *> copy_senslist;

        for (auto &i : sens_list_)
            copy_senslist.insert(static_cast<ExpName*>(i->clone()));

        auto result = new WaitStmt(
            type_,
            expr_->clone());

        result->sens_list_ = copy_senslist;

        return result;
    }

public:
    wait_type_t type_;
    Expression  *expr_;
    // Sensitivity list for 'wait until' statement
    set<ExpName *> sens_list_;
};

#endif /* IVL_sequential_H */
