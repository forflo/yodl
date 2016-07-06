// FM. MA

#ifndef IVL_IG_PARSE_CONTEXT
#define IVL_IG_PARSE_CONTEXT

#include <list>
#include <stack>
#include <map>

#include "vhdlpp_config.h"
#include "lexor_util.h"
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
#include "StringHeap.h"

//TODO: This is ugly, find another way
class ParserContext;
extern int yyparse(yyscan_t, const char *,
                   perm_string, ParserContext *);


class ParserContext {
public:
    ParserContext(StandardTypes *types, StandardFunctions *funcs)
        : global_types(types)
        , global_functions(funcs) {}

    ~ParserContext(){
        parser_cleanup();
    }

public:
    /* As the parser parses entities, it puts them into this map. It uses
     * a map because sometimes it needs to look back at an entity by name.  */
    map<perm_string, Entity *> design_entities;

    /* Create an initial scope that collects all the global
     * declarations. Also save a stack of previous scopes, as a way to
     * manage lexical scopes. */
    stack<ActiveScope*> scope_stack;
    ActiveScope *active_scope = NULL;
    SubprogramHeader *active_sub = NULL;
    ActiveScope *arc_scope = NULL;

    //TODO: These classes don't really belong here.
    //      They belong into the scope object
    StandardTypes *global_types;
    StandardFunctions *global_functions;

    unsigned int parse_errors = 0;
    unsigned int parse_sorrys = 0;

public:
    /* When a scope boundary starts, call the push_scope function to push
     * a scope context. Preload this scope context with the contents of
     * the parent scope, then make this the current scope. When the scope
     * is done, the pop_scope function pops the scope off the stack and
     * resumes the scope that was the parent. */
    void push_scope(void);

    int emit_entities(void);

    void output_fnord(void);

    void pop_scope(void);

    //delete global entities that were gathered over the parsing process
    void delete_design_entities(void);

    bool is_subprogram_param(perm_string name);

    const VType *parse_type_by_name(perm_string name);

    ParserContext *init(void);

    void dump_design_entities(ostream& file);

    //used to delete this object
    void parser_cleanup(void);
};

class ParserUtil {
public:
    static void bind_entity_to_active_scope(ParserContext *c,
                                            const char *ename,
                                            ActiveScope *scope);

    static void add_location(LineInfo *tmp,
                             const struct yyltype &where);

    static void sorrymsg(ParserContext *c,
                         const YYLTYPE&loc,
                         const char*fmt, ...);

    static void errormsg(ParserContext *c,
                         const YYLTYPE&loc,
                         const char*fmt, ...);

    static void bind_architecture_to_entity(ParserContext *c,
                                            const char *ename,
                                            Architecture *arch);

    static int parse_source_file(const char *file_path,
                                 perm_string parse_library_name,
                                 ParserContext *c);

    static int elaborate_entities(ParserContext *context);
};

#endif /* IVL_IG_PARSE_CONTEXT */
