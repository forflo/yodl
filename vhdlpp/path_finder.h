// FM. MA
#ifndef IVL_PATH_FINDER
#define IVL_PATH_FINDER

# include <vector>
# include <iostream>
# include <map>
# include <list>

# include "root_class.h"

class PathFinder {
public:
    PathFinder(size_t a)
        : arity(a) {}

    int findPath(AstNode *);
    int findPath(const AstNode *);

    const std::vector<std::vector<AstNode *>> getPaths();
    size_t getArity(){ return arity; };

private:
    int getNaryPaths(size_t, const std::list<AstNode *> &childs,
                     std::vector<AstNode*> &);

    const std::list<AstNode *> getListOfChilds(AstNode *n);

private:
    const size_t arity;
    std::vector<std::vector<AstNode *>> paths;
};

std::ostream &operator<<(std::ostream &out, PathFinder &us);

#endif /* IVL_PATH_FINDER */
