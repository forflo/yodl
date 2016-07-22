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

    worker(node);

    return 0;
}

// for detailed description of the rules used
// visit IEEE Std 1076-2008 page 146, p. 10.2
int SignalExtractor::worker(const AstNode *n){
    using namespace mch;

    Match(n){
        Case(C<ExpName>()){
            const ExpName *name = dynamic_cast<const ExpName*> (n);
            Signal *s;

            switch (name->name_type_){
            case ExpName::name_type_t::SIMPLE_NAME:
                s = currentScope->find_signal(name->name_);
                if (s){
                    std::cout << "Signal refered with simple name "
                              << name->name_.str()
                              << " was inserted" << endl;
                    signals.insert(s);
                }

                break;
            case ExpName::name_type_t::SELECTED_NAME:
                std::cout << "Signal extraction for selected names not implemented!"
                          << endl;
                break;

            // also coveres slice names
            case ExpName::name_type_t::INDEXED_NAME:
                s = currentScope->find_signal(name->name_);
                if (s) {
                    signals.insert(s);
                }

                if (name->indices_ != NULL) {
                    for (auto &i : *name->indices_)
                        worker(i);

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
                worker(i);
        }
        Otherwise(){
            return 0;
        }
    } EndMatch;

    return 0;
}
