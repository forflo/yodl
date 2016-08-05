#include <expression_desugarer.h>
#include <predicate_generators.h>
#include <sequential.h>
#include <mach7_includes.h>

function<bool (const AstNode*)> ExpressionDesugarer::getTypePredicate(void){
    return makeNaryTypePredicate<Entity, ScopeBase, Expression>();
}

int ExpressionDesugarer::desugarSignalAssignment(SignalSeqAssignment *s){
    Expression *lval = s->lval_;
    Expression *rval
    
    Match()

    return 0;
}

int ExpressionDesugarer::desugarVariableAssignment(VariableSeqAssignment *s){

    return 0;
}

int ExpressionDesugarer::desugarCaseCondition(Expression *e){

    return 0;
}

int ExpressionDesugarer::desugarChoiceExpression(Expression *e){

    return 0;
}

int ExpressionDesugarer::operator()(AstNode *n,
                                    const vector<AstNode *> &parents){
    using namespace mch;

    Match(n){
        Case(C<Entity>()){
            currentEntity = dynamic_cast<Entity *>(n);
            break;
        }
        Case(C<ScopeBase>()){
            currentScope = dynamic_cast<ScopeBase *>(n);
            break;
        }
        Case(C<Expression>()){
            if (!(parents.size() > 0)) {
                std::cout << "[ExpressionDesugarer]" << std::endl;
                std::cout << "Something is wrong with the AST" << std::endl;
                return 1;
            }
            Match(parents[0]){
                Case(C<CaseSeqStmt>()){
                    desugarCaseCondition(dynamic_cast<Expression *>(n));
                    break;
                }
                Case(C<CaseSeqStmt::CaseStmtAlternative>()){
                    desugarCaseCondition(dynamic_cast<Expression *>(n));
                    break;
                }
                Case(C<SignalSeqAssignment>()){
                    desugarSignalAssignment(dynamic_cast<SignalSeqAssignment*>(n));
                    break;
                }
                Case(C<VariableSeqAssignment>()){
                    desugarVariableAssignment(dynamic_cast<VariableSeqAssignment*>(n));
                    break;
                }
                Otherwise(){
                    std::cout << "unknown parent case occured!" << std::endl;
                    break;
                }
            } EndMatch;

            break;
        }
        Otherwise(){

            break;
        }
    } EndMatch;
}
