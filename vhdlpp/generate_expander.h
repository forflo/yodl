// FM. MA
#ifndef IVL_GENERATE_EXPANDER
#define IVL_GENERATE_EXPANDER

#include <vector>
#include <map>

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
#include "generic_traverser.h"
#include "vtype.h"
#include "root_class.h"
#include "mach7_bindings.h"
#include "path_finder.h"

class GenerateExpander {
public:
    GenerateExpander() = default;

    int operator()(AstNode*);

private:
    Entity *currentEntity;
    ScopeBase *currentScope;

    std::list<Architecture::Statement *> accumulator;

    int unroll(AstNode *);
    int modify(AstNode *);
};


#endif /* IVL_GENERATE_EXPANDER */
