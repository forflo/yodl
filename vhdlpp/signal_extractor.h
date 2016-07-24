#ifndef IVL_SIGNAL_EXTRACTOR
#define IVL_SIGNAL_EXTRACTOR

#include <set>

#include "root_class.h"
#include "entity.h"
#include "scope.h"

class SignalExtractor {
public:
    SignalExtractor() {}
    SignalExtractor(const Entity *ce,
                    const ScopeBase *cs)
        : currentEntity(ce)
        , currentScope(cs) {}

    int operator()(const AstNode *);

    std::set<const Signal *> signals;
private:

    int worker(const AstNode *);

    const Entity *currentEntity;
    const ScopeBase *currentScope;
};

#endif /* IVL_SIGNAL_EXTRACTOR */
