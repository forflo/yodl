#ifndef IVL_STATEFUL_LAMBDA
#define IVL_STATEFUL_LAMBDA

#include <functional>

#include "root_class.h"

// encapsulates a lambda and an appropriate state
template<typename T> class StatefulLambda {
public:
    StatefulLambda(T e, std::function<int (const AstNode *, T &)> l)
        : environment(e)
        , constLambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(std::function<int (const AstNode *, T &)> l)
        : constLambda(l) {};

    StatefulLambda(T e, std::function<int (AstNode *, T &)> l)
        : environment(e)
        , mutatingLambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(std::function<int (AstNode *, T &)> l)
        : mutatingLambda(l) {};

    int operator()(const AstNode *node){
        return constLambda(node, environment);
    };

    int operator()(AstNode *node){
        return mutatingLambda(node, environment);
    };

    T environment;
private:
    std::function<int (const AstNode *, T &value)> constLambda;
    std::function<int (AstNode *, T &value)> mutatingLambda;
};

#endif /* IVL_STATEFUL_LAMBDA */
