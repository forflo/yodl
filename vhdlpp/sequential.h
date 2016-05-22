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

class ScopeBase;
class Entity;
class Expression;
class SequentialStmt;

struct SeqStmtVisitor {
    virtual ~SeqStmtVisitor() {}

    virtual void operator()(SequentialStmt *s) = 0;
};

// --OK DOT
class SequentialStmt : public LineInfo {
public:
    SequentialStmt();
    virtual ~SequentialStmt() = 0;

public:
    virtual int elaborate(Entity *ent, ScopeBase *scope);
    virtual int emit(ostream& out, Entity *entity, ScopeBase *scope);
    virtual void dump(ostream& out, int indent) const;
    virtual void write_to_stream(std::ostream& fd);

    // FM. MA
    virtual SimpleTree<map<string, string>> *emit_strinfo_tree() const = 0;

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

protected:
    int elaborate_substatements(Entity *ent, ScopeBase *scope);
    int emit_substatements(std::ostream& out, Entity *ent, ScopeBase *scope);
    void write_to_stream_substatements(ostream& fd);

protected:
    perm_string                 name_;
    std::list<SequentialStmt *> stmts_;
};

// --OK DOT
class IfSequential : public SequentialStmt {
public:
    // --OK DOT
    class Elsif : public LineInfo {
    public:
        Elsif(Expression *cond, std::list<SequentialStmt *> *tr);
        ~Elsif();

        int elaborate(Entity *entity, ScopeBase *scope);
        int condition_emit(ostream& out, Entity *entity, ScopeBase *scope);
        int statement_emit(ostream& out, Entity *entity, ScopeBase *scope);

        void condition_write_to_stream(ostream& fd);
        void statement_write_to_stream(ostream& fd);

        void dump(ostream& out, int indent) const;
        void visit(SeqStmtVisitor& func);

        SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    private:
        Expression *cond_;
        std::list<SequentialStmt *> if_;
    private:           // not implemented
        Elsif(const Elsif&);
        Elsif& operator =(const Elsif&);
    };

public:
    IfSequential(Expression *cond, 
        std::list<SequentialStmt *> *tr,
        std::list<IfSequential::Elsif *> *elsif,
        std::list<SequentialStmt *> *fa);
    ~IfSequential();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;
    void visit(SeqStmtVisitor& func);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

    const Expression *peek_condition() const {
        return cond_;
    }

    size_t false_size() const {
        return else_.size();
    }

    /* These method extract (and remove) the sub-statements from
     * the true or false clause. */
    void extract_true(std::list<SequentialStmt *>& that);
    void extract_false(std::list<SequentialStmt *>& that);

public:
    Expression *cond_;
    std::list<SequentialStmt *>      if_;
    std::list<IfSequential::Elsif *> elsif_;
    std::list<SequentialStmt *>      else_;
};

// --OK DOT
class ReturnStmt : public SequentialStmt {
public:
    explicit ReturnStmt(Expression *val);

    ~ReturnStmt();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    const Expression *peek_expr() const {
        return val_;
    }

    void cast_to(const VType *type);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
    Expression *val_;
};

// --OK DOT
/* Grammar hint: signal_assignment ::= name LEQ waveform ';' */
class SignalSeqAssignment : public SequentialStmt {
public:
    SignalSeqAssignment(Expression *sig, std::list<Expression *> *wav);
    ~SignalSeqAssignment();

public:
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
    Expression              *lval_;
    std::list<Expression *> waveform_;
};

// --OK DOT
/* Grammar hint: case_statement ::= K_case expression K_is ... */
class CaseSeqStmt : public SequentialStmt {
public:
    class CaseStmtAlternative : public LineInfo {
    public:
        CaseStmtAlternative(std::list<Expression *> *exp, 
                std::list<SequentialStmt *> *stmts);
        ~CaseStmtAlternative();
        void dump(std::ostream& out, int indent) const;
        int elaborate_expr(Entity *ent, 
                ScopeBase *scope, 
                const VType *ltype);
        int elaborate(Entity *ent, ScopeBase *scope);
        int emit(ostream& out, Entity *entity, ScopeBase *scope);
        void write_to_stream(std::ostream& fd);
        void visit(SeqStmtVisitor& func);

        // FM. MA
        SimpleTree<map<string, string>> *emit_strinfo_tree() const;

    private:
        std::list<Expression *>     *exp_;
        std::list<SequentialStmt *> stmts_;
    private:         // not implemented
        CaseStmtAlternative(const CaseStmtAlternative&);
        CaseStmtAlternative& operator =(const CaseStmtAlternative&);
    };

public:
    CaseSeqStmt(Expression *cond, std::list<CaseStmtAlternative *> *sp);
    ~CaseSeqStmt();

public:
    void dump(ostream& out, int indent) const;
    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void write_to_stream(std::ostream& fd);
    void visit(SeqStmtVisitor& func);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
    Expression *cond_;
    std::list<CaseStmtAlternative *> alt_;
};

// --OK DOT
/* Grammar rule: procedure_call ::= 
 *      IDENTIFIER ';' 
 *      | IDENTIFIER '(' argument_list ')' ';' */
class ProcedureCall : public SequentialStmt {
public:
    explicit ProcedureCall(perm_string name);

    ProcedureCall(perm_string name, std::list<named_expr_t *> *param_list);
    ProcedureCall(perm_string name, std::list<Expression *> *param_list);
    ~ProcedureCall();

    int elaborate(Entity *ent, ScopeBase *scope);
    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
    perm_string               name_;
    std::list<named_expr_t *> *param_list_;
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
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
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
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
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
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
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
    void write_to_stream(std::ostream& fd);
    void dump(ostream& out, int indent) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
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
    void write_to_stream(std::ostream& fd);

    inline Expression *message() const {
        return msg_;
    }

    inline severity_t severity() const {
        return severity_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

protected:
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
    void write_to_stream(std::ostream& fd);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
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
    void write_to_stream(std::ostream& fd);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
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
    void write_to_stream(std::ostream& fd);

    inline wait_type_t type() const {
        return type_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

private:
    wait_type_t type_;
    Expression  *expr_;
    // Sensitivity list for 'wait until' statement
    std::set<ExpName *> sens_list_;
};

#endif /* IVL_sequential_H */
