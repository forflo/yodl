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
        , constLambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(function<int (const AstNode *, T &)> l)
        : constLambda(l) {};

    StatefulLambda(T e, function<int (AstNode *, T &)> l)
        : environment(e)
        , mutatingLambda(l) {};

    // environment get's default initialized in this constructor
    StatefulLambda(function<int (AstNode *, T &)> l)
        : mutatingLambda(l) {};

    int operator()(const AstNode *node){
        return constLambda(node, environment);
    };

    int operator()(AstNode *node){
        return mutatingLambda(node, environment);
    };

    T environment;
private:
    function<int (const AstNode *, T &value)> constLambda;
    function<int (AstNode *, T &value)> mutatingLambda;
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
        : mutatingTraversal(false)
        , predicate(p)
        , constVisitor(v)
        , ast(a)
        , recurSpec(r) { }

    GenericTraverser(function<bool (const AstNode *)> p,
                     function<int (AstNode *)> v,
                     AstNode *a,
                     recur_t r)
        : mutatingTraversal(true)
        , predicate(p)
        , mutatingVisitor(v)
        , ast(a)
        , recurSpec(r) { }

    // Overloads for Nary Traversers
    GenericTraverser(function<bool (const AstNode*)> p,
                     function<int (const AstNode *, vector<const AstNode *>)> v,
                     AstNode *a, int ar, recur_t r)
        : mutatingTraversal(false)
        , isNary(true)
        , predicate(p)
        , constNaryVisitor(v)
        , ast(a)
        , arity(ar)
        , recurSpec(r) { }

    GenericTraverser(function<bool (const AstNode*)> p,
                     function<int (AstNode *, vector<AstNode *>)> v,
                     AstNode *a, int ar, recur_t r)
        : mutatingTraversal(true)
        , isNary(true)
        , predicate(p)
        , mutatingNaryVisitor(v)
        , ast(a)
        , arity(ar)
        , recurSpec(r) { }

    ~GenericTraverser() = default;

    void traverse();
    bool wasError(){ return errorFlag; }
    bool isNaryTraverser() { return isNary; }

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

private:
    vector<string> traversalMessages;
    vector<string> traversalErrors;
    bool errorFlag = false;
    bool mutatingTraversal;
    bool isNary = false;

    function<bool (const AstNode *)> predicate;
    // visitor visits all nodes and
    // mutating_visitor visits all mutable nodes
    //
    // because of the constness of certain Expression and VType
    // Subclass members, mutating_visitor only visits
    // Objects related to the AstNode Subclasses
    // ComponentBase, Architecture,
    // Architecture::Statement, SequentialStmt and SigVarBase
    function<int (const AstNode *)> constVisitor;
    function<int (AstNode *)> mutatingVisitor;

    // for n-ary visitor constructor overloads
    function<int (const AstNode*, vector<const AstNode *>)> constNaryVisitor;
    function<int (AstNode *, vector<AstNode *>)> mutatingNaryVisitor;

    AstNode *ast;
    int arity;
    recur_t recurSpec;
};
