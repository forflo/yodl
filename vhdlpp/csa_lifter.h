// FM. MA
//
// The concurrent signal assignment promoter class
// encapsulates free standing CSAs inside of fully fledged
// processes as described by VHDL 2008 TODO: Proper reference

#ifndef IVL_CSA_PROMOTER
#define IVL_CSA_PROMOTER

#include <vector>
#include <list>

#include "root_class.h"
#include "architec.h"
#include "sequential.h"
#include "scope.h"
#include "predicate_generators.h"
#include "StringHeap.h"

using namespace std;

/* CsaLifters must be put into an GenericTraverser which
   is equipped with the following type predicate:
   makeNaryTypePredicate<Entity, BlockStatement,
   Architecture> */
class CsaLifter {
public:

    int operator()(AstNode *n, const vector<AstNode*> &parents);

private:
    int modStmtList(std::list<Architecture::Statement*> &);
    Architecture::Statement *encapsulateCSA(const SignalAssignment*);
    Architecture::Statement *encapsulateCCSA(const CondSignalAssignment*);

    Entity *currentEntity;
    ScopeBase *currentScope;
};

#endif /* IVL_CSA_PROMOTER */
