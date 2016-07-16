#include "csa_promoter.h"

using namespace std;


int CsaPromoter::operator()(AstNode *n, const vector<AstNode*> &parents){
    ProcessStatement *replacement;

    if(makeTypePredicate<SignalAssignment>()(n)){
        replacement = new ProcessStatement(
            perm_string::literal(""), *currentScope, 0, 0);

        SignalAssignment *temp = dynamic_cast<SignalAssignment*>(n);

        SignalSeqAssignment *assign = new SignalSeqAssignment(
            temp->lval_->clone(), temp->rval_);

        replacement->statements_.push_back(assign);
    }

    if(makeTypePredicate<CondSignalAssignment>()(n)){
        replacement = new ProcessStatement(
            perm_string::literal(""), *currentScope, 0, 0);
        //TODO: implement
    }

    

    return 0;
}
