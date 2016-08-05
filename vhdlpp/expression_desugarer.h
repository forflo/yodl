#ifndef IVL_EXP_DESUGAR
#define IVL_EXP_DESUGAR

#include <functional>
#include <entity.h>
#include <scope.h>
#include <root_class.h>
#include <sequential.h>

/* This functor desugars every expression
 * It assumes every expression has already been
 * typechecked! If applied onto unchecked AST's,
 * the behaviour is undefined!
 * Note that the GenericTraverser in use needs
 * to be started using NONRECUR!
 */
class ExpressionDesugarer {
public:
    ExpressionDesugarer();

    function<bool (const AstNode*)> getTypePredicate(void);

    int operator()(AstNode *, const vector<AstNode *> &);

private:
    int desugarSignalAssignment(SignalSeqAssignment *s);
    int desugarVariableAssignment(VariableSeqAssignment *s);
    int desugarCaseCondition(Expression *e);
    int desugarChoiceExpression(Expression *e);

    ScopeBase *currentScope;
    Entity *currentEntity;
};

#endif /* IVL_EXP_DESUGAR */
