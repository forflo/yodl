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

#if defined(HAVE_GETOPT_H)
# include <getopt.h>
#endif
// MinGW only supports mkdir() with a path. If this stops working because
// we need to use _mkdir() for mingw-w32 and mkdir() for mingw-w64 look
// at using the autoconf AX_FUNC_MKDIR macro to figure this all out.
#if defined(__MINGW32__)
# include <io.h>
# define mkdir(path, mode)    mkdir(path)
#endif

using namespace std;

// encapsulates a lambda and an appropriate state
template<typename T>
class StatefulLambda {
public:
    StatefulLambda(T e, function<int (AstNode *, T &)> l)
        : environment(e)
        , lambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(function<int (AstNode *, T &)> l)
        : lambda(l) {};

    int operator()(AstNode *node){
        return lambda(node, environment);
    };

private:
    T environment{};
    function<int (AstNode *, T &value)> lambda;
};


class GenericTraverser {
public:
    enum recur_t {
        RECUR,
        NONRECUR
    };

public:
    GenericTraverser(function<bool (AstNode *)> p,
                     function<int (AstNode *)> v,
                     AstNode *a,
                     recur_t r)
        : predicate(p)
        , visitor(v)
        , ast(a)
        , recurSpec(r) { }

    ~GenericTraverser() = default;

public:
    void traverse();
    bool wasError(){ return errorFlag; };

    void emitTraversalMessages(ostream &, const char *);
    void emitErrorMessages(ostream &, const char*);

private:
    void traverse(AstNode *);
    void traverse(ComponentBase *);
    void traverse(Architecture *);
    void traverse(Architecture::Statement *);
    void traverse(Expression *);
    void traverse(SequentialStmt *);
    void traverse(VType *);
    void traverse(SigVarBase *);

private:
    //template <typename T> TraverserEnvironment<T> environment;
    vector<string> traversalMessages;
    vector<string> traversalErrors;
    bool errorFlag = false;

private:
    function<bool (AstNode *)> predicate;
    function<int (AstNode *)> visitor;
    AstNode *ast;
    recur_t recurSpec;
};
