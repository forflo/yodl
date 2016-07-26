#ifndef IVL_IFELSE_CASE
#define IVL_IFELSE_CASE

#include <list>

#include "root_class.h"
#include "sequential.h"
#include "expression.h"

/* This visitor needs to be used along with the following
   type predicate: makeTypePredicate<ProcessStatement>() */
class BranchesToCases {
public:
    BranchesToCases() = default;

    int operator()(AstNode *);

private:

    CaseSeqStmt *transformIfElse(const IfSequential *);
    CaseSeqStmt *makeCaseSeq(const Expression *,
                             const std::list<SequentialStmt *> &,
                             const std::list<SequentialStmt *> &);
};

#endif /* IVL_IFELSE_CASE */
