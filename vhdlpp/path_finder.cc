// FM. MA
#include <vector>
#include <map>

////
// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "scope.h"
#include "StringHeap.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "generic_traverser.h"
#include "vtype.h"
#include "root_class.h"
#include "mach7_bindings.h"
#include "path_finder.h"

#pragma clang diagnostic ignored "-Wshadow"

int PathFinder::findPath(
    AstNode *startNode, std::vector<std::vector<AstNode *>> &paths){

}

int PathFinder::findPath(
    const AstNode *startNode, std::vector<std::vector<const AstNode *>> &paths){

}


int PathFinder::getNaryPaths(size_t depth, const std::list<AstNode *> &childs,
                             std::vector<std::vector<AstNode*>> &paths,
                             std::vector<AstNode *> &accumulator){

    if (accumulator.size() != arity){
        return 1;
    }

    if (depth < arity){
        for (auto &i : childs){
            accumulator[depth] = i;
            getNaryPaths(depth++, getListOfChilds(i), paths, accumulator);
        }
    } else {
        paths.push_back(accumulator);
    }

    return 0;
}


std::list<AstNode *> PathFinder::getListOfChilds(AstNode *e){
    using namespace mch;

    var<VType *> type;
    var<Expression *> op1, op2;

    var<vector<ExpAggregate::element_t*>> elements;
    var<vector<ExpAggregate::choice_element>> aggregate;

    var<perm_string> attribName;
    var<list<Expression *> *> attribArgs;

    // for ExpBitstring
    var<vector<char>> bitString;

    // for ExpCharacter
    var<char> charValue;

    // For ExpConcat
    var<Expression*> concLeft, concRight;

    // For ExpConditional
    var<list<ExpConditional::case_t*>> condOptions;
    var<Expression*> selector;

    // For ExpFunc
    var<perm_string> funcName;
    var<SubprogramHeader *> definition;
    var<vector<Expression*>> argVector;

    // For ExpInteger
    var<int64_t> intValue;

    // For ExpReal
    var<double> dblValue;

    // For ExpName
    var<perm_string> nameName;
    var<list<Expression*>*> indices;

    //For ExpScopedName
    var<perm_string> scopeName;
    var<ScopeBase *> scope;
    var<ExpName*> scopeNameName;

    // For ExpCast
    var<Expression*> castExp;
    var<const VType *> castType;

    // For ExpRange
    var<Expression*> rangeLeft, rangeRight;
    var<ExpRange::range_dir_t> direction;
    var<bool> rangeExpr, rangeReverse;
    var<ExpName *> rangeBase;

    // For ExpNew
    var<Expression*> newSize;

    // For ExpTime
    var<uint64_t> timeAmount;
    var<ExpTime::timeunit_t> timeUnit;

    // For ExpString
    var<string> strValue;

    // For ExpDelay
    var<Expression *> delayExpr, delayDelay;

    std::list<AstNode *> result;

    if (e == NULL) { result; }
    Match(e){
        Case(C<ExpUnary>(op1)){

            var<ExpEdge::fun_t> edgeSpec;

            Match(e){
                Case(C<ExpEdge>(edgeSpec)){

                }
                Case(C<ExpUAbs>()){




                }
                Case(C<ExpUNot>()){


                }
            } EndMatch;
        }

        Case(C<ExpBinary>(op1, op2)){

            var<ExpArithmetic::fun_t> arithOp;
            var<ExpLogical::fun_t> logOp;
            var<ExpRelation::fun_t> relOp;
            var<ExpShift::shift_t> shiftOp;

            Match(e){
                Case(C<ExpArithmetic>(arithOp)){

                }
                Case(C<ExpLogical>(logOp)){


                }
                Case(C<ExpRelation>(relOp)){

                }
                Case(C<ExpShift>(shiftOp)){

                }
                Otherwise() {
                    errorFlag = true;

                    /*error*/
                }
            } EndMatch;
        }

        Case(C<ExpAggregate>(elements, aggregate)){


            // TODO: implement descents
        }

        Case(C<ExpAttribute>(attribName, attribArgs)){

            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;

            Match(e){
                Case(C<ExpObjAttribute>(attribBase)){


                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){


                }
                Otherwise(){
                    errorFlag = true;

                }
            } EndMatch;
        }

        Case(C<ExpBitstring>(bitString)){

        }

        Case(C<ExpCharacter>(charValue)){
        }

        Case(C<ExpConcat>(concLeft, concRight)){
        }

        Case(C<ExpConditional>(condOptions)){

            Match(e){
                Case(C<ExpSelected>(selector)){

                }

                Otherwise(){

                }
            } EndMatch;

        }

        Case(C<ExpFunc>(funcName, definition, argVector)){

        }

        Case(C<ExpInteger>(intValue)){


        }

        Case(C<ExpReal>(dblValue)){
        }

        Case(C<ExpName>(nameName, indices)){


            Match(e){
                Case(C<ExpNameALL>()){
                }
                Otherwise(){

                }
            } EndMatch;
        }

        Case(C<ExpScopedName>(scopeName, scope, scopeNameName)){
        }

        Case(C<ExpString>(strValue)){
        }

        Case(C<ExpCast>(castExp, castType)){
        }

        Case(C<ExpNew>(newSize)){
        }

        Case(C<ExpTime>(timeAmount, timeUnit)){
        }

        Case(C<ExpRange>(rangeLeft, rangeRight,
                         /*direction,*/ rangeExpr,
                         rangeBase/*, rangeReverse*/)){




        }

        Case(C<ExpDelay>(delayExpr, delayDelay)){




        }

        Otherwise(){
            errorFlag = true;

        }
    } EndMatch;
}
