#ifndef IVL_IFELSE_CASE
#define IVL_IFELSE_CASE

#include "root_class.h"
#include "sequential.h"

/* This visitor needs to be used along with the following
   type predicate: makeTypePredicate<ProcessStatement>() */
class BranchesToCases {
public:
    BranchesToCases() = default;

    int operator()(AstNode *);

private:
};

#endif /* IVL_IFELSE_CASE */
