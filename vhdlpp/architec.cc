/*
 * Copyright (c) 2011 Stephen Williams (steve@icarus.com)
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

# include <cassert>

# include "architec.h"
# include "expression.h"
# include "parse_types.h"
# include "sequential.h"
// Need this for parse_errors?
# include "parse_api.h"

using namespace std;

Architecture::Architecture(perm_string name,
                           const ActiveScope& ref,
                           list<Architecture::Statement *>& s)
    : Scope(ref)
    , name_(name)
    , cur_component_(NULL)
    , cur_process_(NULL) {

    statements_.splice(statements_.end(), s);
}

//FM. MA
//TODO: If an Architecture is constructed with this ctor
//      the member of ScopeBase, being context_, is still NULL!
Architecture::Architecture(perm_string name)
    : Scope(4711)
    , name_(name) {}

Architecture::~Architecture() {
    delete_all(statements_);
    ScopeBase::cleanup();
}

Architecture *Architecture::clone() const {
    list<Architecture::Statement*> copy_stmts;

    auto result = new Architecture(name_);

    for (auto &i : statements_)
        copy_stmts.push_back(i->clone());

    // FM. MA DEBUG
    for (auto &i : copy_stmts)
        cout << "Statement copied: " << i << endl;

    result->statements_ = copy_stmts;

    result->cur_process_ = (cur_process_ ? cur_process_->clone() : NULL);
    result->cur_component_ = (cur_component_ ? cur_component_->clone() : NULL);

    return result;
}

bool Architecture::find_constant(perm_string by_name,
        const VType *& typ,
        Expression *& exp) const {
    if (Scope::find_constant(by_name, typ, exp)) {
        return true;
    }

    // Check generics in components
    if (cur_component_) {
        map<perm_string, ComponentBase *>::const_iterator c =
            new_components_.find(cur_component_->component_name());

        if (c == new_components_.end()) {
            c = old_components_.find(cur_component_->component_name());
        }

        assert(c != old_components_.end());
        ComponentBase *base = c->second;

        const InterfacePort *generic = base->find_generic(by_name);
        if (!generic) {
            return false;   // apparently there is no such generic in the component
        }
        Expression *e = cur_component_->find_generic_map(by_name);

        typ = generic->type;
        exp = e ? e : generic->expr;
        return true;
    }

    return false;
}

Variable *Architecture::find_variable(perm_string by_name) const {
    if (cur_process_) {
        return cur_process_->find_variable(by_name);
    }

    return ScopeBase::find_variable(by_name);
}


void Architecture::push_genvar_type(perm_string gname, const VType *gtype) {
    genvar_type_t tmp;

    tmp.name  = gname;
    tmp.vtype = gtype;
    genvar_type_stack_.push_back(tmp);
}


void Architecture::pop_genvar_type(void) {
    assert(!genvar_type_stack_.empty());
    genvar_type_stack_.pop_back();
}


const VType *Architecture::probe_genvar_type(perm_string gname) {
    for (list<genvar_type_t>::reverse_iterator cur = genvar_type_stack_.rbegin()
         ; cur != genvar_type_stack_.rend(); ++cur) {
        if (cur->name == gname) {
            return cur->vtype;
        }
    }
    return 0;
}


void Architecture::push_genvar_emit(perm_string gname, const GenerateStatement *gen) {
    genvar_emit_t tmp;

    tmp.name = gname;
    tmp.gen  = gen;
    genvar_emit_stack_.push_back(tmp);
}


void Architecture::pop_genvar_emit(void) {
    assert(!genvar_emit_stack_.empty());
    genvar_emit_stack_.pop_back();
}


const GenerateStatement *Architecture::probe_genvar_emit(perm_string gname) {
    for (list<genvar_emit_t>::reverse_iterator cur = genvar_emit_stack_.rbegin()
         ; cur != genvar_emit_stack_.rend(); ++cur) {
        if (cur->name == gname) {
            return cur->gen;
        }
    }
    return 0;
}


Architecture::Statement::Statement() {}


Architecture::Statement::~Statement() {}


GenerateStatement::GenerateStatement(perm_string gname,
                                     list<Architecture::Statement *>& s)
    : name_(gname) {
    statements_.splice(statements_.end(), s);
}


GenerateStatement::~GenerateStatement() {
    for_each(statements_.begin(), statements_.end(),
        ::delete_object<Architecture::Statement> ());
}


ForGenerate::ForGenerate(perm_string gname,
        perm_string genvar,
        ExpRange *rang,
        list<Architecture::Statement *>& s)
    : GenerateStatement(gname, s)
    , genvar_(genvar)
    , lsb_(rang->lsb())
    , msb_(rang->msb()) {}

// FM. MA purpose: overload for clone
ForGenerate::ForGenerate(perm_string gname,
                         perm_string genvar,
                         Expression *lsb,
                         Expression *msb,
        list<Architecture::Statement *>& s)
    : GenerateStatement(gname, s)
    , genvar_(genvar)
    , lsb_(lsb)
    , msb_(msb) {}

ForGenerate::~ForGenerate() {}


IfGenerate::IfGenerate(perm_string gname,
        Expression *cond,
        list<Architecture::Statement *>& s)
    : GenerateStatement(gname, s)
    , cond_(cond) {}


IfGenerate::~IfGenerate() {}


SignalAssignment::SignalAssignment(ExpName *name, list<Expression *>& rv)
    : lval_(name) {
    rval_.splice(rval_.end(), rv);
}


SignalAssignment::SignalAssignment(ExpName *name, Expression *rv)
    : lval_(name) {
    rval_.push_back(rv);
}


SignalAssignment::~SignalAssignment() {
    for (list<Expression *>::iterator cur = rval_.begin()
         ; cur != rval_.end(); ++cur) {
        delete *cur;
    }
    delete lval_;
}

CondSignalAssignment::CondSignalAssignment(ExpName *target,
        list<ExpConditional::case_t *>& options)
    : lval_(target) {
    options_.splice(options_.end(), options);
}


CondSignalAssignment::~CondSignalAssignment() {
    delete lval_;

    for (list<ExpConditional::case_t *>::iterator it = options_.begin();
         it != options_.end(); ++it) {
        delete *it;
    }
}

// FM. Constructor overload for easier clone implementation
ComponentInstantiation::ComponentInstantiation(perm_string i,
                                               perm_string c,
                                               const map<perm_string, Expression *> &generic_map,
                                               const map<perm_string, Expression *> &port_map)
    : iname_(i)
    , cname_(c) {

    for (auto &j : generic_map)
        generic_map_[j.first] = j.second->clone();

    for (auto &j : port_map)
        port_map_[j.first] = j.second->clone();
}

ComponentInstantiation::ComponentInstantiation(perm_string i,
        perm_string c,
        list<named_expr_t *> *parms,
        list<named_expr_t *> *ports)
    : iname_(i)
    , cname_(c) {
    typedef pair<map<perm_string, Expression *>::iterator, bool>   insert_rc;

    while (parms && !parms->empty()) {
        named_expr_t *cur = parms->front();
        parms->pop_front();
        insert_rc rc = generic_map_.insert(make_pair(cur->name(), cur->expr()));
        if (!rc.second) {
            cerr << "?:?: error: Duplicate map of generic " << cur->name()
                 << " ignored." << endl;
            // TODO: encapsulate
            //parse_errors += 1;
        }
    }

    while (ports && !ports->empty()) {
        named_expr_t *cur = ports->front();
        ports->pop_front();
        insert_rc rc = port_map_.insert(make_pair(cur->name(), cur->expr()));
        if (!rc.second) {
            cerr << "?:?: error: Duplicate map of port " << cur->name()
                 << " ignored." << endl;
            // TODO: encapsulate
            //parse_errors += 1;
        }
    }
}


ComponentInstantiation::~ComponentInstantiation() {
    for (map<perm_string, Expression *>::iterator it = generic_map_.begin()
         ; it != generic_map_.end(); ++it) {
        delete it->second;
    }
    for (map<perm_string, Expression *>::iterator it = port_map_.begin()
         ; it != port_map_.end(); ++it) {
        delete it->second;
    }
}


Expression *ComponentInstantiation::find_generic_map(perm_string by_name) const {
    map<perm_string, Expression *>::const_iterator p = generic_map_.find(by_name);

    if (p == generic_map_.end()) {
        return NULL;
    }

    return p->second;
}

// FM. More performance for clone() operation
StatementList::StatementList(const list<SequentialStmt*> &statement_list) {
    for (auto &i : statement_list)
        statements_.push_back(i->clone());
}

StatementList::StatementList(list<SequentialStmt *> *statement_list) {
    if (statement_list) {
        statements_.splice(statements_.end(), *statement_list);
    }
}


StatementList::~StatementList() {
    for (list<SequentialStmt *>::iterator it = statements_.begin();
         it != statements_.end(); ++it) {
        delete *it;
    }
}


ProcessStatement::ProcessStatement(perm_string iname,
                                   const ActiveScope& ref,
                                   list<Expression *> *sensitivity_list,
                                   list<SequentialStmt *> *statements_list)
    : StatementList(statements_list)
    , Scope(ref)
    , iname_(iname) {
    if (sensitivity_list) {
        sensitivity_list_.splice(sensitivity_list_.end(), *sensitivity_list);
    }
}

ProcessStatement *ProcessStatement::clone() const {
    std::list<Expression *> sens_list_copy;
    std::list<SequentialStmt *> *stmt_list_copy =
        new std::list<SequentialStmt *>;

    for (auto &i : sensitivity_list_)
        sens_list_copy.push_back(i->clone());

    for (auto &i : statements_)
        stmt_list_copy->push_back(i->clone());

    return new ProcessStatement(
        iname_, ActiveScope(dynamic_cast<const Scope &>(*this)),
        &sens_list_copy, stmt_list_copy);

}

ProcessStatement::~ProcessStatement() {
    for (list<Expression *>::iterator it = sensitivity_list_.begin();
         it != sensitivity_list_.end(); ++it) {
        delete *it;
    }
}


SimpleTree<map<string, string>> *BlockStatement::BlockHeader::emit_strinfo_tree()
    const {
    return empty_simple_tree();
}

BlockStatement::BlockHeader::BlockHeader(list<InterfacePort*> *generic_clause,
                         list<named_expr_t*> *generic_map_aspect,
                         list<InterfacePort*> *port_clause,
                         list<named_expr_t*> *port_map_aspect)
    : generic_clause_(generic_clause)
    , generic_map_aspect_(generic_map_aspect)
    , port_clause_(port_clause)
    , port_map_aspect_(port_map_aspect)
{ }

//TODO: use template function
BlockStatement::BlockHeader *BlockStatement::BlockHeader::clone() const {
    std::list<InterfacePort*> *generic_clause_copy =
        new std::list<InterfacePort*>;
    std::list<named_expr_t*> *generic_map_aspect_copy =
        new std::list<named_expr_t*>;;
    std::list<InterfacePort*> *port_clause_copy =
        new std::list<InterfacePort*>;
    std::list<named_expr_t*> *port_map_aspect_copy =
        new std::list<named_expr_t*>;

    if (generic_clause_)
        for (auto &i : *generic_clause_)
            generic_clause_copy->push_back(i->clone());

    if (generic_map_aspect_)
        for (auto &i : *generic_map_aspect_)
            generic_map_aspect_copy->push_back(i->clone());

    if (port_clause_)
        for (auto &i : *port_clause_)
            port_clause_copy->push_back(i->clone());

    if (port_map_aspect_)
        for (auto &i : *port_map_aspect_)
            port_map_aspect_copy->push_back(i->clone());

    return new BlockStatement::BlockHeader(
        generic_clause_copy, generic_map_aspect_copy,
        port_clause_copy, port_map_aspect_copy);
}

int BlockStatement::elaborate(Entity *, Architecture *) {
    return 0; // TODO: Implement
}

BlockStatement *BlockStatement::clone() const {
    auto *concurrent_stmts_copy = new std::list<Architecture::Statement*>;
    for (auto &i : *concurrent_stmts_)
        concurrent_stmts_copy->push_back(i->clone());

    return new BlockStatement(
        header_->clone(), label_,
        ActiveScope(dynamic_cast<const Scope &>(*this)),
        concurrent_stmts_copy);
}

BlockStatement::BlockStatement(BlockStatement::BlockHeader *header,
                               perm_string label,
                               const ActiveScope &scope,
                               list<Architecture::Statement*>
                               *concurrent_stmts)
    : Scope(scope)
    , label_(label)
    , header_(header)
    , concurrent_stmts_(concurrent_stmts) { }

SimpleTree<map<string, string>> *BlockStatement::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "BlockStatement"},
            {"label", label_.str()}});

    if (header_)
        result->forest.push_back(header_->emit_strinfo_tree());

    if (concurrent_stmts_)
        for (auto &i : *concurrent_stmts_)
            result->forest.push_back(i->emit_strinfo_tree());

    // from base scope
    for (auto &i : new_signals_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_variables_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : new_components_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : cur_types_)
        result->forest.push_back(i.second->emit_strinfo_tree());
    for (auto &i : cur_constants_)
        result->forest.push_back(i.second->emit_strinfo_tree());

    return result;
};
