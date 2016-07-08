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

    int operator()(AstNode*);

private:
    Entity *currentEntity;
    ScopeBase *currentScope;

    std::list<SequentialStmt *> accumulator;

private:
    int unroll(AstNode *);
    int modifyProcess(AstNode *);
};

#endif /* ForLoopUnroller */
