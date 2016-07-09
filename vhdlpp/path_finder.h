// FM. MA
#ifndef IVL_PATH_FINDER
#define IVL_PATH_FINDER

# include <vector>
# include <iostream>
# include <map>
# include <list>

# include "architec.h"
# include "sequential.h"
# include "expression.h"
# include "vtype.h"
# include "root_class.h"

class PathFinder {
public:
    PathFinder(size_t a)
        : arity(a) {}

    int findPath(AstNode *);
    int findPath(const AstNode *);

    const std::vector<std::vector<AstNode *>> getPaths();
    size_t getArity(){ return arity; };

    static const std::list<AstNode *> getListOfChilds(AstNode *n);

private:
    int getNaryPaths(size_t, const std::list<AstNode *> &childs,
                     std::vector<AstNode*> &);

    static const std::list<AstNode *> getListOfChilds(Architecture *);
    static const std::list<AstNode *> getListOfChilds(SequentialStmt *);
    static const std::list<AstNode *> getListOfChilds(Expression *);
    static const std::list<AstNode *> getListOfChilds(VType *);
    static const std::list<AstNode *> getListOfChilds(Architecture::Statement *);
    static const std::list<AstNode *> getListOfChilds(SigVarBase *);

private:
    const size_t arity;
    std::vector<std::vector<AstNode *>> paths;
};

std::ostream &operator<<(std::ostream &out, PathFinder &us);

#endif /* IVL_PATH_FINDER */
