#ifndef IVL_scope_H
#define IVL_scope_H

/*
 * Copyright (c) 2011-2016 Stephen Williams (steve@icarus.com)
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

# include <algorithm>
# include <list>
# include <vector>
# include <map>

# include "StringHeap.h"
# include "entity.h"
# include "expression.h"
# include "vsignal.h"
# include "std_funcs.h"
# include "std_types.h"

class ParserContext;
class ActiveScope;
class Architecture;
class ComponentBase;
class Package;
class SubprogramHeader;
class VType;
class SequentialStmt;

using namespace std;

typedef list<SubprogramHeader *> SubHeaderList;

template<typename T>
struct delete_object {
    void operator()(T *item) {
        delete item;
    }
};

template<typename T>
struct delete_pair_second {
    void operator()(pair<perm_string, T *> item) {
        delete item.second;
    }
};

//FM. MA | Clone implementation NOTE:
//
//A correct clone() Method - like the one
//of the Expression base class - of classes inheriting
//ScopeBase, is especially tricky to implement.
//I'll assume that once an Object of Singal, Variable,
//ComponentBase ... is created. It won't be modified or
//feed. Based on this assumption it's feasible to
//completely omit deep copying of all Object pointed
//to by the maps (members of this class). Thus, cloned AST's
//will share nothing but those objects...
//
//This make-shift solution will (hopefully) be rewritten
//once I understand every aspect of the implemented scoping.
//Maybe the whole scope management needs complete rewrite...
////
class ScopeBase : public AstNode {
public:
    ScopeBase() : package_header_(0) {}

    // FM. MA
    ScopeBase(ParserContext *context)
        : context_(context)
        , package_header_(0) {}

    ScopeBase(const ScopeBase &);

    explicit ScopeBase(const ActiveScope& ref);

    virtual ~ScopeBase() = 0;

    ScopeBase *find_scope(perm_string name) const;
    const VType *find_type(perm_string by_name);
    virtual bool find_constant(perm_string by_name,
                               const VType *& typ,
                               Expression *& exp) const;
    Signal *find_signal(perm_string by_name) const;
    virtual Variable *find_variable(perm_string by_name) const;
    virtual const InterfacePort *find_param(perm_string by_name) const;
    const InterfacePort *find_param_all(perm_string by_name) const;
    SubHeaderList find_subprogram(perm_string by_name) const;

    // Checks if a string is one of possible enum values. If so, the enum
    // type is returned, otherwise NULL.
    const VTypeEnum *is_enum_name(perm_string name) const;

    // Moves signals, variables and components from another scope to
    // this one. After the transfer new_* maps are cleared in the source scope.
    enum transfer_type_t {
        SIGNALS = 1,
        VARIABLES = 2,
        COMPONENTS = 4,
        ALL = 0xffff
    };

    void transfer_from(ScopeBase& ref, transfer_type_t what = ALL);

    inline void bind_subprogram(perm_string name, SubprogramHeader *obj) {
        map<perm_string, SubHeaderList>::iterator it;
        if ((it = use_subprograms_.find(name)) != use_subprograms_.end()) {
            it->second.remove(obj);
        }
        cur_subprograms_[name].push_back(obj);
    }

    // Adds a statement to implicit initializers list
    // (emitted in a 'initial block).
    void add_initializer(SequentialStmt *s) {
        initializers_.push_back(s);
    }

    // Adds a statement to implicit finalizers list
    // (emitted in a 'final' block).
    void add_finalizer(SequentialStmt *s) {
        finalizers_.push_back(s);
    }

    void dump_scope(ostream& out) const;

    // Looks for a subprogram with specified name and parameter types.
    SubprogramHeader *match_subprogram(perm_string name,
                                       const list<const VType *> *params) const;

    perm_string peek_name() const {
        return name_;
    }

    void set_package_header(Package *pkg) {
        assert(package_header_ == 0);
        package_header_ = pkg;
    }

public:
    void cleanup();

    //containers' cleaning helper functions
    template<typename T> void delete_all(list<T *>& c) {
        for_each(c.begin(), c.end(), ::delete_object<T> ());
    }

    template<typename T> void delete_all(map<perm_string, T *>& c) {
        for_each(c.begin(), c.end(), ::delete_pair_second<T> ());
    }

    /* The new_*_ maps below are managed only by the ActiveScope
     * derived class. When any scope is constructed from the
     * ActiveScope, the new_*_ and old_*_ maps are merged and
     * installed into the old_*_ maps. Thus, all other derived
     * classes should only use the old_*_ maps. */

    //TODO: Replace context with pointers to
    //      a StandardTypes and StandardFunctions object
    //FM. MA Added ParserContext to members
    ParserContext *context_;

    //TODO: In order to implement clone without duplication
    //      of Singal, Variable ... Objects. The Pointers to
    //      these Objects will have to be smart pointers in
    //      in the future. Implement this! The need for the
    //      ScopeBase::cleanup() Method of course would vanish.
    // Signal declarations...
    map<perm_string, Signal *> old_signals_;              //previous scopes
    map<perm_string, Signal *> new_signals_;              //current scope
    // Variable declarations...
    map<perm_string, Variable *> old_variables_;          //previous scopes
    map<perm_string, Variable *> new_variables_;          //current scope
    // Component declarations...
    map<perm_string, ComponentBase *> old_components_;    //previous scopes
    map<perm_string, ComponentBase *> new_components_;    //current scope
    // Type declarations...
    map<perm_string, const VType *> use_types_;           //imported types
    map<perm_string, const VType *> cur_types_;           //current types

    //TODO: Refactor me! Maybe put me inside vsignal.{h,cc}?
    // Constant declarations...
    struct const_t {
        const_t(const VType *t, Expression *v)
            : typ(t)
            , val(v) {}

        ~const_t() {
            delete val;
        }

        // FM. MA
        SimpleTree<map<string, string>> *emit_strinfo_tree() const {
            auto result =  new SimpleTree<map<string, string>>(
                map<string, string>{
                    {"node-type", "Constant"}});

            result->forest.push_back(typ->emit_strinfo_tree());
            result->forest.push_back(val->emit_strinfo_tree());

            return result;
        }
        const_t *clone() const {
            return new const_t(typ->clone(), val->clone());
        }

        const VType *typ;
        Expression  *val;
    };

    map<perm_string, struct const_t *> use_constants_;    //imported constants
    map<perm_string, struct const_t *> cur_constants_;    //current constants

    map<perm_string, SubHeaderList> use_subprograms_;     //imported
    map<perm_string, SubHeaderList> cur_subprograms_;     //current

    // for what? Maybe list of scopes less nested than this one
    map<perm_string, ScopeBase *> scopes_;

    list<const VTypeEnum *> use_enums_;

    // List of statements that should be emitted in a 'initial' block
    list<SequentialStmt *> initializers_;

    // List of statements that should be emitted in a 'final' block
    list<SequentialStmt *> finalizers_;

    void do_use_from(const ScopeBase *that);

    // If this is a package body, then there is a Package header
    // already declared.
    Package *package_header_;

    // Generates an unique name for the scope
    void generate_name();

private:
    perm_string name_;
};



class Scope : public ScopeBase {
public:
    explicit Scope(const ActiveScope& ref)
        : ScopeBase(ref) {}

    Scope(int i){ i++; }

    virtual ~Scope() {}

    ComponentBase *find_component(perm_string by_name);

protected:
    // Helper method for emitting signals in the scope.
    int emit_signals(ostream& out, Entity *ent, ScopeBase *scope);
    int emit_variables(ostream& out, Entity *ent, ScopeBase *scope);
};


/* The active_scope object accumulates declarations for the scope that
 * is in the process of being parsed. When the declarations are over,
 * they are transferred over to the specific scope. The ActiveScope is
 * used by the parser to build up scopes. */
class ActiveScope : public ScopeBase {
public:
    //FM. MA | Needed for ProcessStatement::clone()
    ActiveScope(const Scope &s)
        : ScopeBase(s)
        , context_entity_(0) {}

    ActiveScope(ParserContext *context)
        : ScopeBase(context)
        , context_entity_(0) {}

    explicit ActiveScope(const ActiveScope *par);

    ~ActiveScope() {}

    /* Pull items from "that" scope into "this" scope as is
     * defined by a "use" directive. The parser uses this method
     * to implement the "use <pkg>::*" directive. */
    void use_from(const Scope *that) {
        do_use_from(that);
    }

    /* This function returns true if the name is a vectorable
     * name. The parser uses this to distinguish between function
     * calls and array index operations. */
    bool is_vector_name(perm_string name) const;

    /* Locate the subprogram by name. The subprogram body uses
     * this to locate the subprogram declaration. Note that the
     * subprogram may be in a package header. */
    SubprogramHeader *recall_subprogram(const SubprogramHeader *subp) const;

    /* All bind_name function check if the given name was present
     * in previous scopes. If it is found, it is erased (but the pointer
     * is not freed), in order to implement name shadowing. The pointer
     * be freed only in the scope where the object was defined. This is
     * done in ScopeBase::cleanup() function .*/
    void bind_name(perm_string name, Signal *obj) {
        map<perm_string, Signal *>::iterator it;
        if ((it = old_signals_.find(name)) != old_signals_.end()) {
            old_signals_.erase(it);
        }
        new_signals_[name] = obj;
    }

    void bind_name(perm_string name, Variable *obj) {
        map<perm_string, Variable *>::iterator it;
        if ((it = old_variables_.find(name)) != old_variables_.end()) {
            old_variables_.erase(it);
        }
        new_variables_[name] = obj;
    }

    void bind_name(perm_string name, ComponentBase *obj) {
        map<perm_string, ComponentBase *>::iterator it;
        if ((it = old_components_.find(name)) != old_components_.end()) {
            old_components_.erase(it);
        }
        new_components_[name] = obj;
    }

    void bind_name(perm_string name, const VType *t) {
        map<perm_string, const VType *>::iterator it;
        if ((it = use_types_.find(name)) != use_types_.end()) {
            use_types_.erase(it);
        }
        cur_types_[name] = t;
    }

    void bind_scope(perm_string name, ScopeBase *scope) {
        assert(scopes_.find(name) == scopes_.end());
        scopes_[name] = scope;
    }

    inline void use_enum(const VTypeEnum *t) {
        use_enums_.push_back(t);
    }

    inline void use_name(perm_string name, const VType *t) {
        use_types_[name] = t;
    }

    void bind_name(perm_string name, const VType *obj, Expression *val) {
        map<perm_string, const_t *>::iterator it;
        if ((it = use_constants_.find(name)) != use_constants_.end()) {
            use_constants_.erase(it);
        }
        cur_constants_[name] = new const_t(obj, val);
    }

    void bind(Entity *ent) {
        context_entity_ = ent;
    }

    void destroy_global_scope() {
        cleanup();
    }

    // Keep track of incomplete types until their proper
    // definition shows up.
    map<perm_string, VTypeDef *> incomplete_types;

private:
    Entity *context_entity_;
};

#endif /* IVL_scope_H */
