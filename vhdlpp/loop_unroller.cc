#include "loop_unroller.h"
#include "root_class.h"
#include "mach7_includes.h"
#include "sequential.h"
#include "entity.h"
#include "expression.h"
#include "architec.h"
#include <iostream>

int ForLoopUnroller::unroll(AstNode *forNode){
    auto *f = dynamic_cast<ForLoopStatement*>(forNode);

    perm_string iterator = f->it_;
    std::list<SequentialStmt *> statements = f->stmts_;
    std::list<SequentialStmt *> localAcc;
    ExpRange *range = f->range_;
    ExpRange::range_dir_t dir = range->direction_;

    int64_t leftVal, rightVal;
    bool containsNoForLoop = true;
    bool rc = true;

    //TODO: fix constness issue
    rc = rc && range->left_->evaluate(
        currentEntity, currentScope, leftVal);
    rc = rc && range->right_->evaluate(
        currentEntity, currentScope, rightVal);

    // non static range
    if (rc == false){
        return 1;
    }

    // if there is another for statement below this one
    // recurse into it in order to expand it before
    // the expansion of this loop starts
    for (auto &i : statements){
        Match(i){
            Case(mch::C<ForLoopStatement>()){
                if (unroll(i) != 0){
                    return 1;
                }
            }
        } EndMatch;
    }

    for (auto &i : statements){
        Match(i){
            Case(mch::C<ForLoopStatement>()) {
                containsNoForLoop = false;
            }
        } EndMatch;
    }

    if (containsNoForLoop){
        // only shallow copy, because that gets cloned further below
        for (auto &i : statements){
            localAcc.push_back(i);
        }
    } else {
        for (auto &i : accumulator){
            localAcc.push_back(i);
        }

        accumulator.clear();
    }

    // expansion starts here
    switch(dir) {
    case ExpRange::range_dir_t::DOWNTO:
        if (leftVal < rightVal) { /* SEMANTIC ERROR */ return 1; }
        for (int i = leftVal; i >= rightVal; i--){
            for (auto &j : localAcc)
                accumulator.push_back(j->clone());
        }

        break;
    case ExpRange::range_dir_t::TO:
        if (leftVal > rightVal) { /* SEMANTIC ERROR */ return 1; }
        for (int i = leftVal; i <= rightVal; i++){
            for (auto &j : localAcc)
                accumulator.push_back(j->clone());
        }

        break;
    default:
        return 1; //ERROR
    }

    return 0;
}

int ForLoopUnroller::modifyProcess(AstNode *process){
    ProcessStatement *proc = dynamic_cast<ProcessStatement*>(process);

    using namespace mch;

    for (list<SequentialStmt *>::iterator b = proc->statements_.begin();
         b != proc->statements_.end();
         ++b){

        Match(*b){
            Case(C<ForLoopStatement>()){
                unroll(*b);

                proc->statements_.splice(b, accumulator);
                delete *b; //free old unexpanded subtree
                proc->statements_.erase(b);
                //TODO: Bind iteration variables to appropriate values
                std::advance(b, accumulator.size() - 1);
            }
        } EndMatch;
    }

    return 0;
}

int ForLoopUnroller::operator()(AstNode *n){
    using namespace mch;

    Match(n){
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity*>(n);
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
        }
        Case(C<ProcessStatement>()){
            return modifyProcess(n);
        }
        Otherwise(){ return 0; }
    } EndMatch;
    return 0;
}
