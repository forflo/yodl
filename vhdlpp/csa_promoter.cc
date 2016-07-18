#include "csa_promoter.h"
#include "mach7_includes.h"

using namespace std;
using namespace mch;


int CsaPromoter::operator()(AstNode *n, const vector<AstNode*> &parents){
    ProcessStatement *replacement = 0;

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

    if (replacement) {
        Match(parents[0]){
            Case(C<BlockStatement>()){

            }
            Case(C<Architecture>()){

            }
            Otherwise(){
                std::cout << "stange error in CasPromoter" << endl;
                return 1;
            }
        } EndMatch;
    }

    return 0;
}
