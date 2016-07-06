#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>
#include <functional>

// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "root_class.h"
#include "vtype.h"
#include "mach7_includes.h"

using namespace std;

// encapsulates a lambda and an appropriate state
template<typename T> class StatefulLambda {
public:
    StatefulLambda(T e, function<int (const AstNode *, T &)> l)
        : environment(e)
        , lambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(function<int (const AstNode *, T &)> l)
        : lambda(l) {};

    int operator()(const AstNode *node){
        return lambda(node, environment);
    };

private:
    T environment{};
    function<int (const AstNode *, T &value)> lambda;
};


class GenericTraverser {
public:
    enum recur_t {
        RECUR,
        NONRECUR
    };

public:
    GenericTraverser(function<bool (const AstNode *)> p,
                     function<int (const AstNode *)> v,
                     AstNode *a,
                     recur_t r)
        : predicate(p)
        , visitor(v)
        , ast(a)
        , recurSpec(r) { }

    ~GenericTraverser() = default;

    void traverse();
    bool wasError(){ return errorFlag; };

    void emitTraversalMessages(ostream &, const char *);
    void emitErrorMessages(ostream &, const char*);

private:
    void traverse(AstNode *);
    void traverse(ComponentBase *);
    void traverse(Architecture *);
    void traverse(Architecture::Statement *);
    void traverse(const Expression *);
    void traverse(SequentialStmt *);
    void traverse(const VType *);
    void traverse(SigVarBase *);

    vector<string> traversalMessages;
    vector<string> traversalErrors;
    bool errorFlag = false;

    function<bool (const AstNode *)> predicate;
    function<int (const AstNode *)> visitor;
    AstNode *ast;
    recur_t recurSpec;
};
