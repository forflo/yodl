#include "ifelse_case_converter.h"
#include "mach7_includes.h"
#include "sequential.h"
#include "architec.h"
#include "expression.h"

using namespace std;
using namespace mch;

bool containsIfSequential(const list<SequentialStmt*> &slist){
    for (auto &i : slist){
        Match(i){
            Case(C<IfSequential>()){
                return true;
            }
        } EndMatch;
    }

    return false;
}

CaseSeqStmt *makeCaseSeq(const Expression *cond,
                         const list<SequentialStmt *> &if_,
                         const list<SequentialStmt *> &else_){
    ExpName *tr = new ExpName(perm_string::literal("TRUE"));
    ExpName *fa = new ExpName(perm_string::literal("FALSE"));

    list<Expression *> tmpTrue, tmpFalse;
    tmpTrue = {tr};
    tmpFalse = {fa};

    list<SequentialStmt *> tmpIf, tmpElse;
    for (auto &i : if_)
        tmpIf.push_back(i->clone());
    for (auto &i : else_)
        tmpElse.push_back(i->clone());

    CaseSeqStmt::CaseStmtAlternative *altTrue, *altFalse;
    altTrue = new CaseSeqStmt::CaseStmtAlternative(&tmpTrue, &tmpIf);
    altFalse = new CaseSeqStmt::CaseStmtAlternative(&tmpFalse, &tmpElse);
    list<CaseSeqStmt::CaseStmtAlternative *> alternatives = {altTrue, altFalse};

    return new CaseSeqStmt(cond->clone(), &alternatives);
}

CaseSeqStmt *transformIfElse(const IfSequential *ifs){
    if (containsIfSequential(ifs->if_) &&
        containsIfSequential(ifs->else_)) {

    } else if (!containsIfSequential(ifs->if_) &&
               containsIfSequential(ifs->else_)) {

    } else if (containsIfSequential(ifs->if_) &&
               !containsIfSequential(ifs->else_)) {

    } else {
        return makeCaseSeq(ifs->if_, ifs->else_);
    }

    return 0;
}

int BranchesToCases::operator()(AstNode *n){
    ProcessStatement *proc = dynamic_cast<ProcessStatement*>(n);
    list<SequentialStmt *> newStmtList;

    for (auto &i : proc->statements_){
        Match(i){
            Case(C<IfSequential>()){
                newStmtList.push_back(transformIfElse(
                        dynamic_cast<IfSequential*>(i)));

                delete i;
            }
            Otherwise(){
                newStmtList.push_back(i);
            }
        } EndMatch;
    }


    return 0;
}
