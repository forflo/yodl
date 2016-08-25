#ifndef IVL_SYNC_PREDICATE
#define IVL_SYNC_PREDICATE

#include <expression.h>
#include <propcalc.h>
#include <scope.h>
#include <entity.h>

class SyncCondPredicate {
public:
    SyncCondPredicate(Entity *e, ScopeBase *s)
        : currentEntity(e)
        , currentScope(s) { }

    bool operator()(const Expression *e);

public:
    PropcalcFormula *fromExpression(const Expression *,
                                    const Expression *);

private:
    Entity *currentEntity;
    ScopeBase *currentScope;
};

#endif /* IVL_SYNC_PREDICATE */
