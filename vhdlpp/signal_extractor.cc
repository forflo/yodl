#include <set>
#include <functional>

#include "signal_extractor.h"
#include "root_class.h"
#include "stateful_lambda.h"
#include "predicate_generators.h"
#include "mach7_includes.h"
#include "vsignal.h"

using namespace std;

int SignalExtractor::operator()(const AstNode *node){
    if (makeTypePredicate<Entity>()(node)){
        currentEntity = dynamic_cast<const Entity *>(node);
        return 0;
    }

    if (makeTypePredicate<ScopeBase>()(node)){
        currentScope = dynamic_cast<const ScopeBase *>(node);
        return 0;
    }

    StatefulLambda<set<const AstNode *>> extractor(
        static_cast<function <int (const AstNode *, set<const AstNode *> &env)>>(
            lambda_t(currentEntity, currentScope)));

//    GenericTraverser

    return 0;
}

// for detailed description of the rules used
// visit IEEE Std 1076-2008 page 146, p. 10.2
int SignalExtractor::lambda_t::operator()(const AstNode *n, set<const AstNode *> &env){
    using namespace mch;
    Match(n){
        Case(C<ExpName>()){
            // check:
            // - name is simple name
            // - name denotes a signal
            // => add longest static prefix of name to env
            const ExpName *n = dynamic_cast<const ExpName*> (n);

            Signal *s = curScope->find_signal(n->name_);
            if (s != 0){
                std::cout << "Signal refered with simple name "
                          << n->name_.str()
                          << " was inserted" << endl;
                env.insert(s);
            }
        }
        Case(C<ExpFunc>()){

        }
        Otherwise(){ return 0; }
    } EndMatch;

    return 0;
}
