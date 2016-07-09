#ifndef IVL_PATH_FINDER
#define IVL_PATH_FINDER

# include <vector>
# include <map>

# include "root_class.h"

class PathFinder {
public:
    PathFinder(int a)
        : arity(a) {}

    int findPath(AstNode *, std::vector<AstNode *> &);
    int findPath(const AstNode *, std::vector<const AstNode *> &);

private:
    const int arity;
};

#endif /* IVL_PATH_FINDER */
