// FM. MA
#include "csa_lifter.h"
#include "mach7_includes.h"
#include "architec.h"
#include "signal_extractor.h"
#include "generic_traverser.h"

using namespace std;
using namespace mch;

Architecture::Statement *CsaLifter::encapsulateCSA(const SignalAssignment *old){
    ProcessStatement *replacement = new ProcessStatement(
        perm_string::literal(""), *currentScope, 0, 0);

    SignalSeqAssignment *assign = new SignalSeqAssignment(
        old->lval_->clone(), old->rval_);

    replacement->statements_.push_back(assign);

    SignalExtractor extractor(currentEntity, currentScope);
    GenericTraverser traverser(
        makeNaryTypePredicate<ExpName, ExpFunc, ExpAggregate>(),
        static_cast<function<int (const AstNode *)>>(
            [&extractor](const AstNode *n) -> int { return extractor(n); }),
        GenericTraverser::RECUR);

    traverser(old);

    std::list<Expression*> names;
    for (auto &i : extractor.signals)
        names.push_back(new ExpName(i->name_));
    WaitStmt *finalWait = new WaitStmt(names);

    replacement->statements_.push_back(finalWait);

    replacement = new ProcessStatement(
        perm_string::literal(""), *currentScope, 0, 0);

    return replacement;
}

// TODO: Implement
Architecture::Statement *CsaLifter::encapsulateCCSA(
    const CondSignalAssignment *old){

    return 0;
}

int CsaLifter::modStmtList(std::list<Architecture::Statement*> &statements){
    std::list<Architecture::Statement*> newList;

    for (auto &i : statements){
        Match(i){
            Case(C<SignalAssignment>()){
                newList.push_back(
                    encapsulateCSA(
                        dynamic_cast<const SignalAssignment*>(i)));
                delete(i);
                break;
            }
            Case(C<CondSignalAssignment>()){
                newList.push_back(
                    encapsulateCCSA(
                        dynamic_cast<const CondSignalAssignment*>(i)));
                delete(i);
                break;
            }
            Otherwise(){
                newList.push_back(i);
                break;
            }
        } EndMatch;
    }

    // overwrite list with newly constructed list. We already
    // deleted the no longer needed (Cond)SignalAssignments.
    statements = newList;

    return 0;
}

int CsaLifter::operator()(AstNode *n, const vector<AstNode*> &){
    Match(n){
        Case(C<BlockStatement>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
            //sure this is correct?
            modStmtList(*dynamic_cast<BlockStatement*>(n)->concurrent_stmts_);
            break;
        }
        Case(C<Architecture>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
            modStmtList(dynamic_cast<Architecture*>(n)->statements_);
            break;
        }
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity *>(n);
            break;
        }

        Otherwise(){
            break;
        }
    } EndMatch;

    return 0;
}
