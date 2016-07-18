#ifndef IVL_SIGNAL_EXTRACTOR
#define IVL_SIGNAL_EXTRACTOR

#include "root_class.h"
#include "entity.h"
#include "scope.h"

class SignalExtractor {
public:
    SignalExtractor() {}

    int operator()(const AstNode *);

private:
    const Entity *currentEntity;
    const ScopeBase *currentScope;
};

#endif /* IVL_SIGNAL_EXTRACTOR */
