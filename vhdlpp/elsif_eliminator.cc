#include "elsif_eliminator.h"
#include "mach7_includes.h"
#include "architec.h"
#include "sequential.h"

using namespace std;
using namespace mch;

int ElsifEliminator::eliminateElsif(IfSequential *ifs){
    list<SequentialStmt *> elsifCarry = ifs->else_;
    IfSequential *result;

    for (list<IfSequential::Elsif *>::reverse_iterator rev =
             ifs->elsif_.rbegin();
         rev != ifs->elsif_.rend();
         ++rev) {

        result = new IfSequential(
            (*rev)->cond_, (*rev)->if_, {}, elsifCarry);
        elsifCarry = {result};
    }

    ifs->elsif_ = {}; //don't free any members, because we still need them
    ifs->else_ = elsifCarry;

    return 0;
}

int ElsifEliminator::operator()(AstNode *n){
    Match(n){
        Case(C<IfSequential>()){
            return eliminateElsif(dynamic_cast<IfSequential*>(n));
            break;
        }
    } EndMatch;

    return 0;
}
