// FM. MA

#include <cstdarg>

#include "parse_context.h"

/* Implementation for class ParserContext */

/* When a scope boundary starts, call the push_scope function to push
 * a scope context. Preload this scope context with the contents of
 * the parent scope, then make this the current scope. When the scope
 * is done, the pop_scope function pops the scope off the stack and
 * resumes the scope that was the parent. */
void ParserContext::push_scope(void) {
    assert(active_scope);
    scope_stack.push(active_scope);
    active_scope = new ActiveScope(active_scope);
}

int ParserContext::emit_entities(void) {
    int errors = 0;

    for (map<perm_string, Entity *>::iterator cur = design_entities.begin()
             ; cur != design_entities.end(); ++cur) {
        errors += cur->second->emit(cout);
    }

    return errors;
}

void ParserContext::output_fnord(void) {
    printf("fnord\n");
}

void ParserContext::pop_scope(void) {
    delete active_scope;
    assert(!scope_stack.empty());
    active_scope = scope_stack.top();
    scope_stack.pop();
}

//delete global entities that were gathered over the parsing process
void ParserContext::delete_design_entities(void) {
    for(map<perm_string,Entity*>::iterator cur = design_entities.begin()
            ; cur != design_entities.end();
        ++cur)
        delete cur->second;
}

bool ParserContext::is_subprogram_param(perm_string name) {
    if(!active_sub)
        return false;

    return (active_sub->find_param(name) != NULL);
}

const VType *ParserContext::parse_type_by_name(perm_string name) {
    return active_scope->find_type(name);
}

ParserContext *ParserContext::init(void){
    //Remove the scope created at the beginning of parser's work.
    //After the parsing active_scope should keep it's address
    active_scope = new ActiveScope(this);

    global_types->generate_types();
    global_types->add_global_types_to(active_scope);
    global_functions->preload_std_funcs();

    return this;
}

void ParserContext::dump_design_entities(ostream& file) {
    for (auto &i : design_entities)
        i.second->dump(file);
}

//used to delete this object
void ParserContext::parser_cleanup(void) {
    delete_design_entities();

    active_scope->destroy_global_scope();
    delete active_scope;

    global_functions->delete_std_funcs();
    lex_strings.cleanup();
}

/* Implementation for PraserUtil */
void ParserUtil::bind_entity_to_active_scope(ParserContext *c,
                                             const char *ename,
                                             ActiveScope *scope) {
    perm_string ekey = lex_strings.make(ename);
    std::map<perm_string, Entity *>::const_iterator idx =
        c->design_entities.find(ekey);

    if (idx == c->design_entities.end()) {
        return;
    }

    //TODO:?
    scope->bind(idx->second);
}

void ParserUtil::add_location(LineInfo *tmp, const struct yyltype &where) {
    tmp->set_lineno(where.first_line);
    tmp->set_file(filename_strings.make(where.text));
}

void ParserUtil::sorrymsg(ParserContext *c, const YYLTYPE&loc, const char*fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    fprintf(stderr, "%s:%u: sorry: ", loc.text, loc.first_line);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    c->parse_sorrys += 1;
}

void ParserUtil::errormsg(ParserContext *c,
                          const YYLTYPE&loc,
                          const char*fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    fprintf(stderr, "%s:%u: error: ", loc.text, loc.first_line);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    c->parse_errors += 1;
}

void ParserUtil::bind_architecture_to_entity(ParserContext *c,
                                             const char *ename,
                                             Architecture *arch) {
    perm_string ekey = lex_strings.make(ename);
    std::map<perm_string, Entity *>::const_iterator idx =
        c->design_entities.find(ekey);

    if (idx == c->design_entities.end()) {
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

int ParserUtil::parse_source_file(const char *file_path,
                                  perm_string parse_library_name,
                                  ParserContext *c) {
    FILE *fd = fopen(file_path, "r");
    if (fd == 0) {
        perror(file_path);
        return -1;
    }

    yyscan_t scanner = LexerUtil::prepare_lexor(fd);

    int rc = yyparse(scanner, file_path, parse_library_name, c);
    fclose(fd);
    LexerUtil::destroy_lexor(scanner);

    return rc;
}

int ParserUtil::elaborate_entities(ParserContext *context){
    int errors = 0;

    // FM. MA switched to c++ syntax
    for (auto &i : context->design_entities)
        errors += i.second->elaborate();

    return errors;
}
