// FM. MA | This class provides a common base class
//          for all other class hierarchies in this
//          project. This is done in order to simplify
//          working with the mach7's pattern matching
//          facilities
////

#ifndef IVL_ROOT_CLASS
#define IVL_ROOT_CLASS

class AstNode {
    // needed to make complete class tree
    // polymorphic (refer to:
    // https://github.com/solodon4/Mach7/issues/62)
public:
    virtual ~AstNode() {};
};

#endif //IVL_ROOT_CLASS
