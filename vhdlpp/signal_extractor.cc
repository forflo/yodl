#include <set>
#include <functional>

#include "signal_extractor.h"
#include "root_class.h"
#include "stateful_lambda.h"
#include "predicate_generators.h"

int SignalExtractor::operator()(const AstNode *node){
    if (makeTypePredicate<Entity>()(node)){
        currentEntity = dynamic_cast<const Entity *>(node);
        return 0;
    }

    if (makeTypePredicate<ScopeBase>()(node)){
        currentScope = dynamic_cast<const ScopeBase *>(node);
        return 0;
    }

    StatefulLambda<std::set<const AstNode *>> extractor(
        static_cast<std::function <int (const AstNode *, std::set<const AstNode *> &env)>>(
            [](const AstNode *n, std::set<const AstNode *> &env) -> int {



                return 0;
            }));

    GenericTraverser

    return 0;
}
