#ifndef IVL_LOOP_UNROLLER
#define IVL_LOOP_UNROLLER

#include "root_class.h"
#include "mach7_includes.h"
#include "sequential.h"

class ForLoopUnroller {
public:

    int operator()(const AstNode *){
        return 0;
    }
};

#endif /*  */
