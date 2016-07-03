#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <inttypes.h>
#include <iostream>
#include <typeinfo>
#include <math.h>

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

#pragma clang diagnostic ignored "-Wshadow"

#if defined(HAVE_GETOPT_H)
# include <getopt.h>
#endif
// MinGW only supports mkdir() with a path. If this stops working because
// we need to use _mkdir() for mingw-w32 and mkdir() for mingw-w64 look
// at using the autoconf AX_FUNC_MKDIR macro to figure this all out.
#if defined(__MINGW32__)
# include <io.h>
# define mkdir(path, mode)    mkdir(path)
#endif

using namespace mch;
using namespace std;

//TODO:
// ExpAggregate::choice_t
// ExpAggregate::choice_element
// ExpAggregate::element_t
// ExpConditional::case_t
// ExpName::index_t

// IfSequential::Elsif
// CaseSeqStmt::CaseStmtAlternative
// BlockStatement::BlockHeader

// VTypeArray::range_t
// VTypeRecord::element_t

void GenericTraverser::traverse(AstNode *root){
    traversalMessages.push_back("Entering AstNode switch");

    Match(root){
        Case(C<Entity>()){
            traverse(static_cast<Entity*>(root));
        }
        Case(C<Architecture>()) {
            traverse(static_cast<Architecture*>(root));
        }
        Case(C<VType>()){
            traverse(static_cast<VType*>(root));
        }
        Case(C<SequentialStmt>()){
            traverse(static_cast<SequentialStmt*>(root));
        }
        Case(C<Architecture::Statement>()){
            traverse(static_cast<Architecture::Statement*>(root));
        }
        Case(C<Expression>()){
            traverse(static_cast<Expression*>(root));
        }
        Case(C<SigVarBase>()){
            traverse(static_cast<SigVarBase*>(root));
        }
        Otherwise() {
            errorFlag = true;
            traversalErrors.push_back("Unknown type in node switch");
        }
    } EndMatch
}

void GenericTraverser::traverse(ComponentBase *c){
    traversalMessages.push_back("Entering Entity switch");

    // For Entity
    var<map<perm_string, Architecture *>> entityArchs;
    var<Architecture*> entityBound;
    var<map<perm_string, VType::decl_t>> entityDecls;

    // For ComponentBase
    var<perm_string> compName;
    var<vector<InterfacePort*>> compParams, compPorts;

    Match(c){
        Case(C<ComponentBase>(compName, compParams, compPorts)) {
            Match(c){
                Case(C<Entity>(entityArchs, entityBound, entityDecls)){
                    traversalMessages.push_back("Entity detected");

                    if (predicate(c)) {
                        visitor(c);
                    }

                    for (auto &i : entityArchs)
                        traverse(i.second);
                }
                Otherwise(){
                    traversalMessages.push_back("ComponentBase detected");
                }
            } EndMatch
        }
        Otherwise() {
            errorFlag = true;
            traversalErrors.push_back("Impossible condition in traverse "
                                      "for Component Base");
        }
    } EndMatch
}

void GenericTraverser::traverse(Architecture *arch){
    traversalMessages.push_back("Entering Architecture switch");

    var<list<Architecture::Statement *>> statements;
    var<ComponentInstantiation *> componentInst;
    var<perm_string> name;
    // propably not needed
    //var<ProcessStatement *> currenProcess;

    Match(arch){
        Case(C<Architecture>(statements, componentInst, name)){
            traversalMessages.push_back(string("Architecture ")
                                        + name.str()
                                        + string(" detected"));

            for (auto &i : statements)
                traverse(i);

            if (componentInst)
                traverse(componentInst);
        }
        Otherwise(){
            errorFlag = true;
            traversalErrors.push_back("Impossible condition in "
                                      "traverse Architecture");
        }
    } EndMatch
}

void GenericTraverser::traverse(Architecture::Statement *s){
    traversalMessages.push_back("Entering Statement switch");

    var<perm_string> name, label;
    var<list<Architecture::Statement*>> stmts;
    var<list<Architecture::Statement*>*> stmts_ptr;
    var<BlockStatement::BlockHeader*> header;
    var<Expression&> cond;

    // for SignalAsignment and CondSignalAssignment
    var<ExpName*> lval;
    var<list<Expression*>> rval;
    var<list<ExpConditional::case_t*>> options;
    var<list<const ExpName*>> senslist;

    // for ComponentInstanciation
    var<perm_string> iname, cname;
    var<map<perm_string, Expression*>> genmap, portmap;

    // for StatementList subtree
    var<list<SequentialStmt*>> seqStmts;

    Match(s){
        Case(C<GenerateStatement>(name, stmts)){
            var<Expression *> cond, msb, lsb;
            var<perm_string> genvar;

            Match(s){
                Case(C<ForGenerate>(genvar, msb, lsb)){
                    //TODO: traverse further
                }
                Case(C<IfGenerate>(cond)){
                    //TODO: traverse further
                }
                Otherwise(){
                    //TODO: Error log
                }
            } EndMatch

            return;
        }

        Case(C<SignalAssignment>(lval, rval)){
            traverse(lval);

            for (auto &i : rval)
                traverse(i);
            traversalMessages.push_back("Found SignalAssignment");
        }

        Case(C<CondSignalAssignment>(lval, options, senslist)){
            traverse(lval);

            for (auto &i : options)
                traverse(i);

//            for (auto &i : senslist)
//                traverse(i);
            traversalMessages.push_back("Found CondSignalAssignment");
        }

        Case(C<ComponentInstantiation>(iname, cname, genmap, portmap)){
            //TODO: traverse further
            traversalMessages.push_back("Found Component Instantiation");
        }

        Case(C<StatementList>(seqStmts)){
            var<perm_string> name;
            var<list<Expression *>> procSensList;
            Match(s){
                Case(C<FinalStatement>()) {
                    traversalMessages.push_back("Found final Statement");
                }
                Case(C<InitialStatement>()) {
                    traversalMessages.push_back("Found initial Statement");
                }
                Case(C<ProcessStatement>(name, procSensList)) {
                    //TODO: traverse stmts, procSensList;
                    cout << "Found process: " << name << endl;
                }
                Otherwise() {
                    //TODO: Error msg!
                    return ;
                }
            } EndMatch
        }

        Case(C<BlockStatement>(label, header, stmts_ptr)){
            cout << "Found Block statement: " << label << "\n";
            for (auto &i : *static_cast<list<Architecture::Statement*>*>(stmts_ptr)){
                traverse(i);
            }
        }

        Otherwise() {
            //TODO: Error
            //cout << "Wildcard pattern for traverse(const Arch::Stmt &s)\n";
            return;
        }
    } EndMatch
}

void GenericTraverser::traverse(Expression *e){
    var<const VType *> type;
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

    Match(e){
        Case(C<ExpUnary>(op1)){
            var<ExpEdge::fun_t> edgeSpec;

            Match(e){
                Case(C<ExpEdge>(edgeSpec)){
                    //TODO: implement
                }
                Case(C<ExpUAbs>()){
                    //TODO: implement
                }
                Case(C<ExpUNot>()){
                    //TODO: implement
                }
            } EndMatch
        }

        Case(C<ExpBinary>(op1, op2)){
            var<ExpArithmetic::fun_t> arithOp;
            var<ExpLogical::fun_t> logOp;
            var<ExpRelation::fun_t> relOp;
            var<ExpShift::shift_t> shiftOp;

            Match(e){
                Case(C<ExpArithmetic>(arithOp)){
                    //TODO:
                }
                Case(C<ExpLogical>(logOp)){
                    //TODO:
                }
                Case(C<ExpRelation>(relOp)){
                    //TODO:
                }
                Case(C<ExpShift>(shiftOp)){
                    //TODO:
                }
                Otherwise() {/*error*/}
            } EndMatch
        }

        Case(C<ExpAggregate>(elements, aggregate)){
            //TODO: implement
            traversalMessages.push_back("ExpAggregate");
        }

        Case(C<ExpAttribute>(attribName, attribArgs)){
            var<ExpName *> attribBase;
            var<const VType *> attribTypeBase;

            Match(e){
                Case(C<ExpObjAttribute>(attribBase)){
                    //TODO: Implement
                }
                Case(C<ExpTypeAttribute>(attribTypeBase)){
                    //TODO: Implement
                }
                Otherwise(){
                    //TODO: No error. Just base class
                }
            } EndMatch
            //TODO: implement
            traversalMessages.push_back("ExpAttribute");
        }

        Case(C<ExpBitstring>(bitString)){
            //TODO: implement
            traversalMessages.push_back("ExpBitstring");
        }

        Case(C<ExpCharacter>(charValue)){
            //TODO: implement
            traversalMessages.push_back("ExpCharacter");
        }

        Case(C<ExpConcat>(concLeft, concRight)){
            //TODO: implement
            traversalMessages.push_back("ExpConcat");
        }

        Case(C<ExpConditional>(condOptions)){
            Match(e){
                Case(C<ExpSelected>(selector)){
                    //TODO: implement
                }
                Otherwise(){
                    //TODO: Just base class
                }
            } EndMatch
            //TODO: implement
            traversalMessages.push_back("ExpConditional");
        }

        Case(C<ExpFunc>(funcName, definition, argVector)){
            //TODO: implement
            traversalMessages.push_back("ExpFunc");
        }

        Case(C<ExpInteger>(intValue)){
            //TODO: implement
            traversalMessages.push_back("ExpInteger");
        }

        Case(C<ExpReal>(dblValue)){
            //TODO: implement
            traversalMessages.push_back("ExpReal");
        }

        Case(C<ExpName>(nameName, indices)){
            Match(e){
                Case(C<ExpNameALL>()){
                    //TODO: Implement
                }
                Otherwise(){
                    //TODO: Here just the base class
                }
            } EndMatch
            //TODO: implement
            traversalMessages.push_back("ExpName");
        }

        Case(C<ExpScopedName>(scopeName, scope, scopeNameName)){
            //TODO: implement
            traversalMessages.push_back("ExpScopedName");
        }

        Case(C<ExpString>(strValue)){
            //TODO: implement
            traversalMessages.push_back("ExpString");
        }

        Case(C<ExpCast>(castExp, castType)){
            //TODO: implement
            traversalMessages.push_back("ExpCast");
        }

        Case(C<ExpNew>(newSize)){
            //TODO: implement
            traversalMessages.push_back("ExpNew");
        }

        Case(C<ExpTime>(timeAmount, timeUnit)){
            //TODO: implement
            traversalMessages.push_back("ExpTime");
        }

        Case(C<ExpRange>(rangeLeft, rangeRight,
                         //direction, rangeExpr,
                         rangeBase, rangeReverse)){
            //TODO: implement
         //     traversalMessages.push_back("ExpRange");
        }

        Case(C<ExpDelay>(delayExpr, delayDelay)){
            //TODO: implement
            traversalMessages.push_back("ExpDelay");
        }

        Otherwise(){
            //TODO: error message
        }
    } EndMatch
}

void GenericTraverser::traverse(SequentialStmt *seq){
    var<perm_string> loopName;
    var<list<SequentialStmt*>> loopStmts;

    // For IfSequential
    var<Expression *> ifCond;
    var<list<SequentialStmt*>> ifPath, elsePath;
    var<list<IfSequential::Elsif*>> elsifPaths;
    // For ReturnStmt
    var<Expression*> retValue;

    // For SignalSeqAssignment
    var<Expression*> assignLval;
    var<list<Expression *>> waveform;

    // For CaseStmtAlternative
    var<Expression *> caseCond;
    var<list<CaseSeqStmt::CaseStmtAlternative>> caseAlternatives;
    // For ProcedureCall
    var<perm_string> procName;
    var<list<named_expr_t*> *> procParams;
    var<SubprogramHeader *> procDef;

    // For VariableSeqAssignment
    var<Expression *> varLval, varRval;

    // For ReportStmt
    var<Expression *> reportMsg, assertCond;
    var<ReportStmt::severity_t> reportSeverity;

    // For WaitStmt
    var<WaitStmt::wait_type_t> waitType;
    var<Expression *> waitExpr;
    var<set<ExpName*>> waitSens;

    Match(seq){
        Case(C<LoopStatement>(loopName, loopStmts)){
            var<Expression *> whileCond;
            var<perm_string> iterVar;
            var<ExpRange*> iterRange;

            Match(seq){
                Case(C<WhileLoopStatement>(whileCond)){
                    //TODO:
                }

                Case(C<ForLoopStatement>(iterVar, iterRange)){
                    //TODO:
                }

                Case(C<BasicLoopStatement>()){
                    //TODO:
                }

                Otherwise(){
                    //TODO: Error!
                }
            } EndMatch
        }

        Case(C<IfSequential>(ifCond, ifPath,
                             elsifPaths, elsePath)){
            //TODO:
        }

        Case(C<ReturnStmt>(retValue)){
            //TODO:
        }

        Case(C<SignalSeqAssignment>(assignLval, waveform)){
            //TODO:
        }

        //FIXME: Runies build. Don't know why
//        Case(C<CaseSeqStmt>(caseCond, caseAlternatives)){
//            //TODO:
//        }

        Case(C<ProcedureCall>(procName, procParams, procDef)){
            //TODO:
        }

        Case(C<VariableSeqAssignment>(varLval, varRval)){
            //TODO:
        }

        Case(C<ReportStmt>(reportMsg, reportSeverity)){
            Match(seq){
                Case(C<AssertStmt>(assertCond)){
                    //TODO:
                }
                Otherwise(){
                    //TODO: Just ReportStmt
                }
            } EndMatch
        }

        Case(C<WaitForStmt>()){
            //TODO:
        }

        Case(C<WaitStmt>(waitType, waitExpr, waitSens)){
            //TODO:
        }

        Otherwise(){
            //TODO: error message
        }
    } EndMatch
}

void GenericTraverser::traverse(VType *type){
    // For VTypePrimitive
    var<VTypePrimitive::type_t> primType;
    var<bool> primPacked;

    // For VTypeArray
    var<const VType *> arrEtype;
    var<vector<VTypeArray::range_t>> arrRanges;
    var<bool> arrSigFlag;
    var<const VTypeArray *> arrParent;

    // For VTypeRange
    var<const VType *> rangeBase;

    // For VTypeRangeConst
    var<int64_t> cRangeStart, cRangeEnd;

    // For VTypeRangeExpr
    var<Expression *> eRangeStart, eRangeEnd;
    var<bool> eDownto;

    // For VTypeRecord
    var<vector<VTypeRecord::element_t *>> recordElements;

    // For VTypeEnum
    var<vector<perm_string>> enumNames;

    // For VTypeDef
    var<perm_string> typeName;
    var<const VType *> typeType;

    Match(type){
        Case(C<VTypeERROR>()){
            //TODO:
        }

        Case(C<VTypePrimitive>(primType, primPacked)){
            //TODO:
        }

        Case(C<VTypeArray>(arrEtype, arrRanges, arrSigFlag, arrParent)){
            //TODO:
        }

        Case(C<VTypeRange>(rangeBase)){
            //TODO:
        }

        Case(C<VTypeRangeConst>(cRangeStart, cRangeEnd)){
            //TODO:
        }

        Case(C<VTypeRangeExpr>(eRangeStart, eRangeEnd, eDownto)){
            //TODO:
        }

        Case(C<VTypeEnum>(enumNames)){
            //TODO:
        }

        Case(C<VTypeRecord>(recordElements)){
            //TODO:
        }

        Case(C<VTypeDef>(typeName, typeType)){
            //TODO:
        }

        Case(C<VSubTypeDef>()){
            //TODO:
        }
    } EndMatch
}

void GenericTraverser::traverse(SigVarBase *signal){
    Match(signal) {
        Case(C<Signal>()){
            //TODO:
        }
        Case(C<Variable>()){
            //TODO:
        }
        Otherwise(){
            // TODO: Error?
        }
    } EndMatch
}

void GenericTraverser::traverse(){
    traverse(ast);
}

void GenericTraverser::emitTraversalMessages(ostream &out, const char* delimit){
    for (auto &i : traversalMessages){
        out << i << delimit;
    }
}

void GenericTraverser::emitErrorMessages(ostream &out, const char* delimit){
    for (auto &i : traversalErrors){
        out << i << delimit;
    }
}
