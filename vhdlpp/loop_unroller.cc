#include "loop_unroller.h"
#include "root_class.h"
#include "mach7_includes.h"
#include "sequential.h"
#include "entity.h"
#include "expression.h"

int ForLoopUnroller::unroll(const AstNode *forNode, bool top){
    auto *f = dynamic_cast<const ForLoopStatement*>(forNode);

    perm_string iterator = f->it_;
    std::list<SequentialStmt *> statements = f->stmts_;
    std::list<SequentialStmt *> expandedStatements;
    ExpRange *range = f->range_;

    int64_t leftVal, rightVal;
    int rc = 0;

    //TODO: fix constness issue
    rc += range->left_->evaluate(currentEntity, currentScope, leftVal);
    rc += range->right_->evaluate(currentEntity, currentScope, rightVal);

    // non static range
    if (rc != 0){
        return 1;
    }

    // if there is another for statement below this one
    // recurse into it in order to expand it before
    // the expansion of this loop starts
    for (auto &i : statements){
        Match(i){
            Case(mch::C<ForLoopStatement>()){
                unroll(i, false);
            }
        } EndMatch;
    }

    ExpRange::range_dir_t dir = range->direction_;

    // expansion starts here
    switch(dir) {
    case ExpRange::range_dir_t::DOWNTO:
        if (leftVal < rightVal) { /* SEMANTIC ERROR */ return 1; }
        for (int i = leftVal; i >= rightVal; i--){
            for (auto &i : statements)
                expandedStatements.push_back(i->clone());
        }

        break;
    case ExpRange::range_dir_t::TO:
        if (leftVal > rightVal) { /* SEMANTIC ERROR */ return 1; }
        for (int i = leftVal; i <= rightVal; i++){
            for (auto &i : statements)
                expandedStatements.push_back(i->clone());
        }

        break;
    default:
        return 1; //ERROR
    }

    f->stmts_ =

    return 0;
}

int ForLoopUnroller::operator()(const AstNode *n){
    using namespace mch;

    Match(n){
        Case(C<Entity>()){
            currentEntity = dynamic_cast<const Entity*>(n);
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<const ScopeBase *>(n);
        }
        Case(C<ForLoopStatement>()){
            return unroll(n, true);
        }
        Otherwise(){ return 0; }
    } EndMatch;
    return 0;
}
