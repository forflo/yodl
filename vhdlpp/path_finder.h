// FM. MA
#ifndef IVL_PATH_FINDER
#define IVL_PATH_FINDER

# include <vector>
# include <map>
# include <list>

# include "root_class.h"

class PathFinder {
public:
    PathFinder(size_t a)
        : arity(a) {}

    int findPath(AstNode *, std::vector<std::vector<AstNode *>> &);
    int findPath(const AstNode *, std::vector<std::vector<const AstNode *>> &);

private:
    int getNaryPaths(size_t, const std::list<AstNode *> &childs,
                     std::vector<std::vector<AstNode *>> &,
                     std::vector<AstNode*> &);

    const std::list<AstNode *> getListOfChilds(AstNode *n);

private:
    const size_t arity;
};

#endif /* IVL_PATH_FINDER */
