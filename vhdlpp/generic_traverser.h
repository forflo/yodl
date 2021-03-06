// FM. MA
#include <vector>
#include <iostream>
#include <functional>

// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "root_class.h"
#include "vtype.h"
#include "mach7_includes.h"

class GenericTraverser {
public:
    enum recur_t {
        RECUR,
        NONRECUR
    };

    // TODO: At the moment, one has to explicitly static_cast the
    // second argument of all four ctor overloads, because of the
    // misbehaving type deduction for std::function templates. Thi
    // should be simplified!
    ////
public:
    GenericTraverser(std::function<bool (const AstNode *)> p,
                     std::function<int (const AstNode *)> v,
                     recur_t r)
        : isMutating(false)
        , isNary(false)
        , predicate(p)
        , constVisitorU(v)
        , recurSpec(r) { }

    GenericTraverser(std::function<bool (const AstNode *)> p,
                     std::function<int (AstNode *)> v,
                     recur_t r)
        : isMutating(true)
        , isNary(false)
        , predicate(p)
        , mutatingVisitorU(v)
        , recurSpec(r) { }

    // Overloads for Nary Traversers
    GenericTraverser(std::function<bool (const AstNode*)> p,
                     std::function<int (const AstNode *,
                                        const std::vector<const AstNode *> &)> v,
                     recur_t r)
        : isMutating(false)
        , isNary(true)
        , predicate(p)
        , constNaryVisitorU(v)
        , recurSpec(r) { }

    GenericTraverser(std::function<bool (const AstNode*)> p,
                     std::function<int (AstNode *,
                                        const std::vector<AstNode *> &)> v,
                     recur_t r)
        : isMutating(true)
        , isNary(true)
        , predicate(p)
        , mutatingNaryVisitorU(v)
        , recurSpec(r) { }

    ~GenericTraverser() = default;

    bool wasError(){ return errorFlag; }
    bool isNaryTraverser() { return isNary; }
    bool isMutatingTraverser() { return isMutating; }

    void emitTraversalMessages(std::ostream &, const char *);
    void emitErrorMessages(std::ostream &, const char*);

    int operator()(AstNode *);
    int operator()(const AstNode *);

private:
    void traverse(AstNode *);

    void traverseMutating(AstNode *);
    void traverseMutating(ComponentBase *);
    void traverseMutating(Architecture *);
    void traverseMutating(Architecture::Statement *);
    void traverseMutating(Expression *);
    void traverseMutating(SequentialStmt *);
    void traverseMutating(VType *);
    void traverseMutating(SigVarBase *);

    // Unfortunately, I don't know a way to circumvent
    // this redundancy...
    void traverseConst(const AstNode *);
    void traverseConst(const ComponentBase *);
    void traverseConst(const Architecture *);
    void traverseConst(const Architecture::Statement *);
    void traverseConst(const Expression *);
    void traverseConst(const SequentialStmt *);
    void traverseConst(const VType *);
    void traverseConst(const SigVarBase *);
    void traverseConst(const CaseSeqStmt::CaseStmtAlternative *);

    // visitor visits all nodes and
    // mutating_visitor visits all mutable nodes
    //
    // because of the constness of certain Expression and VType
    // Subclass members, mutating_visitor only visits
    // Objects related to the AstNode Subclasses
    // ComponentBase, Architecture,
    // Architecture::Statement, SequentialStmt and SigVarBase
    void constVisitor(const AstNode *);
    void mutatingVisitor(AstNode *);

    bool noFurtherRecur(const AstNode *);
    bool noFurtherMRecur(AstNode *);

private:
    std::vector<string> traversalMessages;
    std::vector<string> traversalErrors;
    bool errorFlag = false;
    bool isMutating;
    bool isNary = false;

    std::function<bool (const AstNode *)> predicate;

    std::function<int (const AstNode *)> constVisitorU;
    std::function<int (AstNode *)> mutatingVisitorU;

    // for n-ary visitor constructor overloads
    std::function<int (const AstNode*,
                       const std::vector<const AstNode *> &)> constNaryVisitorU;
    std::function<int (AstNode *,
                       const std::vector<AstNode *> &)> mutatingNaryVisitorU;

    std::vector<AstNode *> currentPath;
    std::vector<const AstNode *> currentPathConst;

    recur_t recurSpec;
};

//TODO: Add path inserts/removals into
//      generic_{mutating,const}_traverser.cc
