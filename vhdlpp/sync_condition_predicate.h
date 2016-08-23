#ifndef IVL_SYNC_PREDICATE
#define IVL_SYNC_PREDICATE

#include <expression.h>

class SyncCondPredicate {
public:
    SyncCondPredicate();

    bool operator()(const Expression *e);

};

#endif /* IVL_SYNC_PREDICATE */
