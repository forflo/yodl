// FM. MA | This class provides a common base class
//          for all other class hierarchies in this
//          project. This is done in order to simplify
//          working with the mach7's pattern matching
//          facilities
////

#ifndef IVL_ROOT_CLASS
#define IVL_ROOT_CLASS

class AllRoot {
    // needed to make complete class tree
    // polymorphic
    virtual ~AllRoot() {};
};

#endif //IVL_ROOT_CLASS
