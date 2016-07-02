#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <cerrno>
#include <limits>
#include <vector>
#include <map>
#include <iostream>
#include <math.h>

////
// mach7 setup
// Support for N-ary Match statement on patterns
#include <type_switchN-patterns.hpp>
#include <address.hpp>      // Address and dereference combinators
#include <bindings.hpp>     // Mach7 support for bindings on arbitrary UDT
#include <constructor.hpp>  // Support for constructor patterns
#include <equivalence.hpp>  // Equivalence combinator +

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
#include "traverse_all.h"
#include "vtype.h"
#include "root_class.h"
#include "mach7_bindings.h"

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

void traverse(AstNode *root){
    cout << "ast root switch\n";

    Match(root){
        Case(C<Entity>()){
            traverse(static_cast<Entity*>(root));
            return ;
        }
        Case(C<Architecture>()) {
            traverse(static_cast<Architecture*>(root));
            return;
        }
        Otherwise() { return; }
    } EndMatch
}

void traverse(Entity *top){
    cout << "entity switch\n";

    var<map<perm_string, Architecture *>> archs;
    var<Architecture*> bound_arch;
    var<map<perm_string, VType::decl_t>> decls;
    var<perm_string> name;

    Match(top){
        Case(C<Entity>(archs, bound_arch, decls, name)){
            cout << "Entity detected: " << name << endl;
            for (auto &i : archs)
                traverse(i.second);
            return;
        }
        Otherwise() {
            cout << "No Entity!" << endl;
            return;
        }
    } EndMatch
}

void traverse(Architecture *arch){
    cout << "arch switch\n";

    var<list<Architecture::Statement *>> stmts;
    var<ComponentInstantiation *> comps;
    var<ProcessStatement *> procs;
    var<perm_string> name;

    Match(arch){
        Case(C<Architecture>(stmts, comps, procs, name)){
            cout << "Architecture detected: " << name << endl;
            for (auto &i : stmts)
                traverse(*i);
            return;
        }
        Otherwise(){
            cout << "No Architecture!" << endl;
            return;
        }
    } EndMatch
}

void traverse(const Architecture::Statement &s){
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
                Otherwise(){ return; } //TODO: Error log
            } EndMatch

            return;
        }

        Case(C<SignalAssignment>(lval, rval)){
            //TODO: traverse(lval);
            //TODO:
            //for (auto &i : rval)
            //    traverse(i);
            cout << "Found SignalAssignment" << endl;
        }

        Case(C<CondSignalAssignment>(lval, options, senslist)){
            //TODO: traverse(lval);
            //TODO:
            //for (auto &i : options)
            //    traverse(i);
            //for (auto &i : senslist)
            //    traverse(i);
            cout << "Found CondSignalAssignment" << endl;
        }

        Case(C<ComponentInstantiation>(iname, cname, genmap, portmap)){
            //TODO: traverse further
            cout << "Found Component Instantiation" << endl;
        }

        Case(C<StatementList>(seqStmts)){
            var<perm_string> name;
            var<list<Expression *>> procSensList;
            Match(s){
                Case(C<FinalStatement>()) {
                    cout << "Found final Statement" << endl;
                }
                Case(C<InitialStatement()) {
                    cout << "Found initial Statement" << endl;
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
            for (auto &i : static_cast<list<Architecture::Statement*>*>(stmts_ptr)){
                traverse(*i);
            }
        }

        Otherwise() {
            //cout << "Wildcard pattern for traverse(const Arch::Stmt &s)\n";
            return;
        }
    } EndMatch
}

void traverse(Expression *e){
    var<const VType *> type;
    var<Expression *> op1, op2;

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
        Case(C<ExpAggregate>()){}
        Case(C<ExpAttribute>()){}
        Case(C<ExpBitstring>()){}
        Case(C<ExpCharacter>()){}
        Case(C<ExpConcat>()){}
        Case(C<ExpConditional>()){}
        Case(C<ExpFunc>()){}
        Case(C<ExpInteger>()){}
        Case(C<ExpReal>()){}
        Case(C<ExpName>()){}
        Case(C<ExpScopedName>()){}
        Case(C<ExpString>()){}
        Case(C<ExpCast>()){}
        Case(C<ExpNew>()){}
        Case(C<ExpTime>()){}
        Case(C<ExpRange>()){}
        Case(C<ExpDelay>()){}

        Otherwise(){
            //TODO: error message
        }

    } EndMatch
}
