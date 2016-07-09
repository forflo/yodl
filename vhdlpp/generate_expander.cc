#include "generate_expander.h"
#include "mach7_includes.h"

int GenerateExpander::unroll(AstNode *node){

}

int GenerateExpander::modify(AstNode *){


    return 0;
}

// We need to make sure that unroll and modify
// have access to the most recent scope.
int GenerateExpander::operator()(AstNode *n){
    using namespace mch;

    Match(n){
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity*>(n);
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
        }
        Case(C<>()){

        }
    } EndMatch;

    return 0;
}
