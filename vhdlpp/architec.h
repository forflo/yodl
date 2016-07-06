#ifndef IVL_architec_H
#define IVL_architec_H

/*
 * Copyright (c) 2011-2014 Stephen Williams (steve@icarus.com)
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

# include <list>
# include <map>
# include <stdio.h>

# include "StringHeap.h"
# include "LineInfo.h"
# include "scope.h"
# include "simple_tree.h"
# include "root_class.h"

class ParserContext;
class ComponentInstantiation;
class Entity;
class Expression;
class ExpName;
class GenerateStatement;
class ProcessStatement;
class SequentialStmt;
class Signal;
class named_expr_t;
class ExpRange;

using namespace std;

// --OK DOT (FM.)
/* The Architecture class carries the contents (name, statements,
 * etc.) of a parsed VHDL architecture. These objects are ultimately
 * put into entities. */
class Architecture : public Scope, public LineInfo, public AstNode {
public:
    // --OK DOT (FM.)
    /* Architectures contain concurrent statements, that are
     * derived from this nested class. */
    class Statement : public LineInfo , public AstNode {
    public:
        Statement();
        virtual ~Statement() = 0;

        virtual int elaborate(Entity *ent, Architecture *arc);
        virtual int emit(ostream& out, Entity *ent, Architecture *arc);
        virtual void dump(ostream& out, int indent = 0) const;

        // FM. MA
        virtual SimpleTree<map<string, string>> *emit_strinfo_tree() const = 0;
        virtual Statement *clone() const = 0;
    };

public:
    /* Create an architecture from its name and its statements.
     * NOTE: The statement list passed in is emptied. */
    Architecture(perm_string name,
            const ActiveScope& ref,
            list<Architecture::Statement *>& s);

    Architecture(perm_string name); //FM. MA| empty constructor for better clone()

    ~Architecture();

    perm_string get_name() const {
        return name_;
    }

    bool find_constant(perm_string by_name,
                       const VType *& typ,
                       Expression *& exp) const;
    Variable *find_variable(perm_string by_name) const;

    // Sets the currently processed component
    // (to be able to reach its parameters).
    void set_cur_component(ComponentInstantiation *component) {
        assert(!cur_component_ || !component);
        cur_component_ = component;
    }

    // Sets the currently elaborated process
    // (to use its scope for variable resolving).
    void set_cur_process(ProcessStatement *process) {
        assert(!cur_process_ || !process);
        cur_process_ = process;
    }

    // Elaborate this architecture in the context of the given entity.
    int elaborate(Entity *entity);

    // These methods are used while in the scope of a generate
    // block to mark that a name is a genvar at this point.
    const VType *probe_genvar_type(perm_string);
    void push_genvar_type(perm_string gname, const VType *gtype);
    void pop_genvar_type(void);

    int get_statement_amount() const {
        return statements_.size();
    }

    // These methods are used during EMIT to check for names that
    // are genvar names.
    const GenerateStatement *probe_genvar_emit(perm_string);
    void push_genvar_emit(perm_string gname, const GenerateStatement *);
    void pop_genvar_emit(void);

    // Emit this architecture to the given out file in the context
    // of the specified entity. This method is used by the
    // elaborate code to display generated code to the specified
    // output.
    int emit(ostream& out, Entity *entity);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    Architecture *clone() const;

    // The dump method writes a debug display to the given output.
    void dump(ostream& out, perm_string of_entity, int indent = 0) const;

public:
    perm_string name_;
    // Concurrent statements local to this architecture
    list<Architecture::Statement *> statements_;

    struct genvar_type_t {
        perm_string name;
        const VType *vtype;
    };

    // FM. TODO: Proper clone!
    list<genvar_type_t> genvar_type_stack_;

    struct genvar_emit_t {
        perm_string             name;
        const GenerateStatement *gen;
    };

    // FM. TODO: Proper clone!
    list<genvar_emit_t> genvar_emit_stack_;

    // Currently processed component (or NULL if none).
    ComponentInstantiation *cur_component_;

    // Currently elaborated process (or NULL if none).
    ProcessStatement *cur_process_;
};

/* This is a base class for various generate statement types. It holds
 * the generate statement name, and a list of substatements. */
class GenerateStatement : public Architecture::Statement {
public:
    GenerateStatement(perm_string gname,
            list<Architecture::Statement *>& s);
    ~GenerateStatement();

    inline perm_string get_name() const {
        return name_;
    }

    int elaborate_statements(Entity *ent, Architecture *arc);
    int emit_statements(ostream& out, Entity *ent, Architecture *arc);
    void dump_statements(ostream& out, int indent) const;

public:
    perm_string name_;
    list<Architecture::Statement *> statements_;
};


// OK DOT
class ForGenerate : public GenerateStatement {
public:
    ForGenerate(perm_string gname,
                perm_string genvar,
                ExpRange *rang,
                list<Architecture::Statement *>& s);
    ForGenerate(perm_string gname,
                             perm_string genvar,
                             Expression *lsb,
                             Expression *msb,
                             list<Architecture::Statement *>& s);
    ~ForGenerate();

    int elaborate(Entity *ent, Architecture *arc);
    int emit(ostream& out, Entity *entity, Architecture *arc);
    void dump(ostream& out, int ident = 0) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    ForGenerate *clone() const {
        list<Architecture::Statement*> copy;

        for (auto &i : statements_)
            copy.push_back(i->clone());

        return new ForGenerate(
            name_,
            genvar_,
            lsb_->clone(),
            msb_->clone(),
            copy);
    }

public:
    perm_string genvar_;
    Expression  *lsb_;
    Expression  *msb_;
};

// OK DOT
class IfGenerate : public GenerateStatement {
public:
    IfGenerate(perm_string gname,
            Expression *cond,
            list<Architecture::Statement *>& s);
    ~IfGenerate();

    int elaborate(Entity *ent, Architecture *arc);
    int emit(ostream& out, Entity *entity, Architecture *arc);

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    IfGenerate *clone() const {
        list<Architecture::Statement*> copy;

        for (auto &i : statements_)
            copy.push_back(i->clone());

        return new IfGenerate(
            name_,
            cond_->clone(),
            copy);
    }

public:
    Expression *cond_;
};

// OK DOT
/* The SignalAssignment class represents the
 * concurrent_signal_assignment that is placed in an architecture.  */
class SignalAssignment : public Architecture::Statement {
public:
    SignalAssignment(ExpName *target, list<Expression *>& rval);
    SignalAssignment(ExpName *target, Expression *rval);
    ~SignalAssignment();

    virtual int elaborate(Entity *ent, Architecture *arc);
    virtual int emit(ostream& out, Entity *entity, Architecture *arc);
    virtual void dump(ostream& out, int ident = 0) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    SignalAssignment *clone() const {
        list<Expression*> copy;

        for (auto &i : rval_)
            copy.push_back(i->clone());

        return new SignalAssignment(
            static_cast<ExpName*>(lval_->clone()),
            copy);
    }

public:
    ExpName                 *lval_;
    list<Expression *> rval_;
};

// OK DOT
class CondSignalAssignment : public Architecture::Statement {
public:
    CondSignalAssignment(ExpName *target,
            list<ExpConditional::case_t *>& options);
    ~CondSignalAssignment();

    int elaborate(Entity *ent, Architecture *arc);
    int emit(ostream& out, Entity *entity, Architecture *arc);
    void dump(ostream& out, int ident = 0) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    CondSignalAssignment *clone() const {
        list<ExpConditional::case_t*> copy;

        for (auto &i : options_)
            copy.push_back(i->clone());

        return new CondSignalAssignment(
            static_cast<ExpName*>(lval_->clone()),
            copy);
    }

public:
    ExpName *lval_;
    list<ExpConditional::case_t *> options_;

    /* List of signals that should be emitted in the related process
     * sensitivity list. It is filled during the elaboration step. */
    list<const ExpName *> sens_list_;
};

// OK DOT
class ComponentInstantiation : public Architecture::Statement {
public:
    ComponentInstantiation(perm_string iname, perm_string cname,
                           list<named_expr_t *> *parms,
                           list<named_expr_t *> *ports);
    ComponentInstantiation(perm_string i,
                           perm_string c,
                           const map<perm_string, Expression *> &generic_map,
                           const map<perm_string, Expression *> &port_map);
    ~ComponentInstantiation();

    virtual int elaborate(Entity *ent, Architecture *arc);
    virtual int emit(ostream& out, Entity *entity, Architecture *arc);
    virtual void dump(ostream& out, int indent = 0) const;

    // Returns the expression that initializes a generic (or NULL if not found).
    Expression *find_generic_map(perm_string by_name) const;

    inline perm_string instance_name() const {
        return iname_;
    }

    inline perm_string component_name() const {
        return cname_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    ComponentInstantiation *clone() const {
        return new ComponentInstantiation(
            iname_,
            cname_,
            generic_map_,
            port_map_);
    }

public:
    perm_string iname_;
    perm_string cname_;

    // FM TODO (include in simple tree output)
    map<perm_string, Expression *> generic_map_;
    map<perm_string, Expression *> port_map_;
};

// OK DOT
class StatementList : public Architecture::Statement {
public:
    StatementList(list<SequentialStmt *> *statement_list);
    StatementList(const list<SequentialStmt*> &statement_list); // FM. MA
    virtual ~StatementList();

    int elaborate(Entity *ent, Architecture *arc) {
        return elaborate(ent, static_cast<ScopeBase *> (arc));
    }

    int emit(ostream& out, Entity *ent, Architecture *arc) {
        return emit(out, ent, static_cast<ScopeBase *> (arc));
    }

    virtual int elaborate(Entity *ent, ScopeBase *scope);
    virtual int emit(ostream& out, Entity *entity, ScopeBase *scope);
    virtual void dump(ostream& out, int indent = 0) const;

    list<SequentialStmt *>& stmt_list() {
        return statements_;
    }

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    StatementList *clone() const {
        return new StatementList(statements_);
    }

public:
    list<SequentialStmt *> statements_;
};

// FM. MA
class BlockStatement : public Architecture::Statement, public Scope {
public:
    class BlockHeader : public AstNode {
    public:
        BlockHeader(list<InterfacePort*> *generic_clause,
                    list<named_expr_t*> *generic_map_aspect,
                    list<InterfacePort*> *port_clause,
                    list<named_expr_t*> *port_map_aspect);

        SimpleTree<map<string, string>> *emit_strinfo_tree() const;

        BlockHeader *clone() const;

    public:
        list<InterfacePort*> *generic_clause_;
        list<named_expr_t*> *generic_map_aspect_;
        list<InterfacePort*> *port_clause_;
        list<named_expr_t*> *port_map_aspect_;
    };

public:
    BlockStatement(BlockStatement::BlockHeader *header,
                   perm_string label,
                   const ActiveScope &scope,
                   list<Architecture::Statement*> *concurrent_stmts);

    int elaborate(void);

    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

    BlockStatement *clone() const;

public:
    perm_string label_;
    BlockHeader *header_;
    list<Architecture::Statement*> *concurrent_stmts_;
};

// OK DOT
/* There is no direct VHDL countepart to SV 'initial' statement,
 * but we can still use it during the translation process. */
class InitialStatement : public StatementList {
public:
    InitialStatement(list<SequentialStmt *> *statement_list)
        : StatementList(statement_list) {}

    // FM. MA
    InitialStatement(const list<SequentialStmt*> &statement_list)
        : StatementList(statement_list) {}

    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void dump(ostream& out, int indent = 0) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    InitialStatement *clone() const {
        return new InitialStatement(statements_);
    }
};

// OK DOT
/* There is no direct VHDL countepart to SV 'final' statement,
 * but we can still use it during the translation process. */
class FinalStatement : public StatementList {
public:
    FinalStatement(list<SequentialStmt *> *statement_list)
        : StatementList(statement_list) {}


    int emit(ostream& out, Entity *entity, ScopeBase *scope);
    void dump(ostream& out, int indent = 0) const;

    // FM. MA (not implemented)
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;

    FinalStatement(const list<SequentialStmt *> &statement_list)
        : StatementList(statement_list) {}

    FinalStatement *clone() const {
        return new FinalStatement(statements_);
    }
};

// OK DOT
class ProcessStatement : public StatementList, public Scope {
public:
    ProcessStatement(perm_string                 iname,
                     const ActiveScope&          ref,
                     list<Expression *>     *sensitivity_list,
                     list<SequentialStmt *> *statement_list);

    ~ProcessStatement();

    int elaborate(Entity *ent, Architecture *arc);
    int emit(ostream& out, Entity *entity, Architecture *arc);
    void dump(ostream& out, int indent = 0) const;

    // FM. MA
    SimpleTree<map<string, string>> *emit_strinfo_tree() const;
    // TODO: Implement
    ProcessStatement *clone() const {
        printf("Implement ME!\n");
        return NULL;
    }

public:
    perm_string             iname_;
    list<Expression *> sensitivity_list_;
};

#endif /* IVL_architec_H */
