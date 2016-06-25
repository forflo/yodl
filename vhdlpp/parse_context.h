// FM. MA

#ifndef IVL_IG_PARSE_CONTEXT
#define IVL_IG_PARSE_CONTEXT

// TODO incorporate parser_cleanup
#include <cstdarg>
#include <iostream>
#include <cstring>
#include <list>
#include <stack>
#include <map>
#include <vector>
#include <ivl_assert.h>
#include <assert.h>

#include "vhdlpp_config.h"
#include "vhdlint.h"
#include "vhdlreal.h"
#include "compiler.h"
#include "parse_api.h"
#include "architec.h"
#include "expression.h"
#include "sequential.h"
#include "subprogram.h"
#include "package.h"
#include "vsignal.h"
#include "vtype.h"
#include "std_funcs.h"
#include "std_types.h"
#include "parse_types.h"

#include <stdio.h>

class ParserContext {
public:
    ParserContext(StandardTypes *types, StandardFunctions *funcs)
        : global_types(types)
        , global_functions(funcs) {}

    ~ParserContext(){
        parser_cleanup();
    }

    // holds all entities of a design
    map<perm_string, Entity *> design_entities;

    /* Create an initial scope that collects all the global
     * declarations. Also save a stack of previous scopes, as a way to
     * manage lexical scopes. */
    ActiveScope *active_scope = new ActiveScope();
    stack<ActiveScope*> scope_stack;
    SubprogramHeader *active_sub = NULL;
    ActiveScope *arc_scope = NULL;

    StandardTypes *global_types;
    StandardFunctions *global_functions;

    unsigned int parse_errors = 0;
    unsigned int parse_sorrys = 0;

    /* When a scope boundary starts, call the push_scope function to push
     * a scope context. Preload this scope context with the contents of
     * the parent scope, then make this the current scope. When the scope
     * is done, the pop_scope function pops the scope off the stack and
     * resumes the scope that was the parent. */
    void push_scope(void) {
        assert(active_scope);
        scope_stack.push(active_scope);
        active_scope = new ActiveScope (active_scope);
    }

    int emit_entities(void) {
        int errors = 0;

        for (map<perm_string, Entity *>::iterator cur = design_entities.begin()
                 ; cur != design_entities.end(); ++cur) {
            errors += cur->second->emit(cout);
        }

        return errors;
    }

    void output_fnord(void) {
        printf("fnord\n");
    }

    void pop_scope(void) {
        delete active_scope;
        assert(! scope_stack.empty());
        active_scope = scope_stack.top();
        scope_stack.pop();
    }

    void delete_global_scope(void) {
        active_scope->destroy_global_scope();
        delete active_scope;
    }

    //delete global entities that were gathered over the parsing process
    void delete_design_entities(void) {
        for(map<perm_string,Entity*>::iterator cur = design_entities.begin()
                ; cur != design_entities.end();
            ++cur)
            delete cur->second;
    }

    bool is_subprogram_param(perm_string name) {
        if(!active_sub)
            return false;

        return (active_sub->find_param(name) != NULL);
    }

    const VType *parse_type_by_name(perm_string name) {
        return active_scope->find_type(name);
    }

    void init(void){
        preload_global_types();
        global_functions->preload_std_funcs();
    }

    //Remove the scope created at the beginning of parser's work.
    //After the parsing active_scope should keep it's address
    void preload_global_types(void) {
        global_types->generate_types();
        global_types->add_global_types_to(active_scope);
    }

    //delete global entities that were gathered over the parsing process
    void delete_design_entities(void) {
        for(auto &i : design_entities)
            delete i.second;
    }

    void dump_design_entities(ostream& file) {
        for (auto &i : design_entities)
            i.second->dump(file);
    }

    //used to delete this object
    void parser_cleanup(void) {
        delete_design_entities();
        delete_global_scope();
        global_functions->delete_std_funcs();
        lex_strings.cleanup();
    }
};

class ParserUtil {
public:
    static void bind_entity_to_active_scope(ParserContext *c,
                                     const char *ename,
                                     ActiveScope *scope) {
        perm_string ekey = lex_strings.make(ename);
        std::map<perm_string, Entity *>::const_iterator idx =
            design_entities.find(ekey);

        if (idx == design_entities.end()) {
            return;
        }

        //TODO:?
        scope->bind(idx->second);
    }

    static inline void add_location(LineInfo *tmp, const struct yyltype &where) {
        tmp->set_lineno(where.first_line);
        tmp->set_file(filename_strings.make(where.text));
    }

    static void sorrymsg(ParserContext *c, const YYLTYPE&loc, const char*fmt, ...) {
        va_list ap;
        va_start(ap, fmt);

        fprintf(stderr, "%s:%u: sorry: ", loc.text, loc.first_line);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        c->parse_sorrys += 1;
    }

    static void errormsg(ParserContext *c,
                  const YYLTYPE&loc,
                  const char*fmt, ...) {
        va_list ap;
        va_start(ap, fmt);

        fprintf(stderr, "%s:%u: error: ", loc.text, loc.first_line);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        c->parse_errors += 1;
    }

    static void bind_architecture_to_entity(ParserContext *c,
                                            const char *ename,
                                            Architecture *arch) {
        perm_string ekey = lex_strings.make(ename);
        std::map<perm_string, Entity *>::const_iterator idx =
            design_entities.find(ekey);

        if (idx == design_entities.end()) {
            cerr << arch->get_fileline() << ": error: No entity " << ekey
                 << " for architecture " << arch->get_name()
                 << "." << endl;
            c->parse_errors += 1;
            return;
        }

        /* FIXME: entities can have multiple architectures attached to them
         * This is to be configured by VHDL's configurations (not yet implemented) */
        Architecture *old_arch = idx->second->add_architecture(arch);
        if (old_arch != arch) {
            cerr << arch->get_fileline() << ": warning: "
                 << "Architecture " << arch->get_name()
                 << " for entity " << idx->first
                 << " is already defined here: "
                 << old_arch->get_fileline() << endl;
            c->parse_errors += 1;
        }
    }

    static int parse_source_file(const char*file_path,
                                 perm_string parse_library_name,
                                 ParserContext *c) {
        FILE *fd = fopen(file_path, "r");
        if (fd == 0) {
            perror(file_path);
            return -1;
        }

        yyscan_t scanner = prepare_lexor(fd);

        int rc = yyparse(scanner, file_path, parse_library_name, c);
        fclose(fd);
        destroy_lexor(scanner);

        return rc;
    }
};

#endif /* IVL_IG_PARSE_CONTEXT */
