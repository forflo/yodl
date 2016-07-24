#ifndef IVL_ELSIF_ELIMINATOR
#define IVL_ELSIF_ELIMINATOR

#include "root_class.h"
#include "sequential.h"

/* This visitor works without cloning */
class ElsifEliminator {
public:
    ElsifEliminator() = default;

    int operator()(AstNode *);

private:
    int eliminateElsif(IfSequential *);
};

#endif /* IVL_ELSIF_ELIMINATOR */
