// FM. MA
// This file contains some useful metafunctions for
// the quick-and-dirty usage of GenericTraverser
////
#ifndef IVL_PREDICATE_GENERATOR
#define IVL_PREDICATE_GENERATOR

#include "root_class.h"
#include "mach7_includes.h"

template<typename T> struct makeTypePredicate {
    bool operator()(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){ return true; }
        } EndMatch;

        return false;
    }
};

template<typename T, typename ...J> struct makeNaryTypePredicate;
template<typename T> struct makeNaryTypePredicate<T>;

template<typename T> struct makeNaryTypePredicate<T>{
    bool operator()(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){ return true; }
        } EndMatch;
        return false;
    }
};

// simple recursive template that can be used to generate
// type predicates that check for multiple types.
// Usage: makeNaryTypePredicate<typeFoo, typeBar, typeBaz>()(AstNodePointerFoo);
// WARNING: This should not be used for huge type parameter lists as the
//          created type switching algorithm is slow (O(n), with n being the
//          number of type given to this template)
template<typename T, typename ...J> struct makeNaryTypePredicate {
    bool operator()(const AstNode *n){
        if (helper(n)) {
            return true;
        } else {
            return makeNaryTypePredicate<J...>()(n);
            return false;
        }
    }
private:
    bool helper(const AstNode *n){
        Match(n){
            CaseInT(mch::C<T>()){
                return true;
            }
        } EndMatch;
        return false;
    }
};

#endif /* IVL_PREDICATE_GENERATOR */
