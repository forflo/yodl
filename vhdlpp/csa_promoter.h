// FM. MA
//
// The concurrent signal assignment promoter class
// encapsulates free standing CSAs inside of fully fledged
// processes as described by VHDL 2008 TODO: Proper reference

#ifndef IVL_CSA_PROMOTER
#define IVL_CSA_PROMOTER

#include <vector>

#include "root_class.h"
#include "architec.h"
#include "predicate_generators.h"
#include "StringHeap.h"

using namespace std;

class CsaPromoter {
public:

    int operator()(AstNode *n, const vector<AstNode*> &parents){

        if(makeTypePredicate<SignalAssignment>()(n)){
            ProcessStatement *replacement = new ProcessStatement(
                perm_string::literal(""), );
        }

        if(makeTypePredicate<CondSignalAssignment>()(n)){
            ProcessStatement *replacement = new ProcessStatement();
            //TODO: implement
        }

        return 0;
    }

private:
    Entity *currentEntity;
    Scope *architecture;
    
};

#endif /* IVL_CSA_PROMOTER */
