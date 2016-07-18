#ifndef IVL_SIGNAL_EXTRACTOR
#define IVL_SIGNAL_EXTRACTOR

#include <set>

#include "root_class.h"
#include "entity.h"
#include "scope.h"

class SignalExtractor {
public:
    SignalExtractor() {}

    int operator()(const AstNode *);



private:
    class lambda_t {
    public:
        lambda_t(const Entity *e, const ScopeBase *s)
            : curEnt(e) , curScope(s) {}

        int operator()(const AstNode *, std::set<const AstNode *> &env);

    private:
        const Entity *curEnt;
        const ScopeBase *curScope;
    };

    const Entity *currentEntity;
    const ScopeBase *currentScope;
};

#endif /* IVL_SIGNAL_EXTRACTOR */
