#ifndef IVL_LOOP_UNROLLER
#define IVL_LOOP_UNROLLER

#include "root_class.h"
#include "mach7_includes.h"
#include "sequential.h"
#include "entity.h"
#include "scope.h"

class ForLoopUnroller {
public:
    ForLoopUnroller() = default;

    int operator()(const AstNode*);

private:
    const Entity *currentEntity;
    const ScopeBase *currentScope;

    int unroll(const AstNode *, bool top);
};

#endif /*  */
