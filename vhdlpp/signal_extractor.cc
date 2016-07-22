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
int SignalExtractor::lambda_t::operator()(const AstNode *n,
                                          set<const AstNode *> &env){
    using namespace mch;

    Match(n){
        Case(C<ExpName>()){
            // check:
            // - name is simple name
            // - name denotes a signal
            // => add longest static prefix of name to env
            const ExpName *name = dynamic_cast<const ExpName*> (n);
            Signal *s;

            switch (name->name_type_){
            case ExpName::name_type_t::SIMPLE_NAME:
                s = curScope->find_signal(name->name_);
                if (s){
                    std::cout << "Signal refered with simple name "
                              << name->name_.str()
                              << " was inserted" << endl;
                    env.insert(s);
                }

                break;
            case ExpName::name_type_t::SELECTED_NAME:
                std::cout << "Signal extraction for selected names not implemented!"
                          << endl;
                break;

            // also coveres slice names
            case ExpName::name_type_t::INDEXED_NAME:
                s = curScope->find_signal(name->name_);
                if (s) {
                    env.insert(s);
                }

                if (name->indices_ != NULL) {
                    for (auto &i : *name->indices_)
                        (*this)(i, env);

                }
                break;
            default:
                std::cout << "Signal extraction for other name type not implemented!"
                          << endl;
            }
        }
        Case(C<ExpFunc>()){
            const ExpFunc *func = dynamic_cast<const ExpFunc *>(n);
            for (auto &i : func->argv_)
                (*this)(i, env);
        }
        Otherwise(){
            return 0;
        }
    } EndMatch;

    return 0;
}
