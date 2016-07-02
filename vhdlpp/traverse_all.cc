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

void traverse(AstRoot *root){
    Match(root){
        Case(C<Entity>()){ traverse(static_cast<Entity*>(root)); return ;}
        Case(C<Architecture>()) { traverse( (Architecture *) root); return; }
        Otherwise() { return; }
    } EndMatch
}

void traverse(Entity *top){
    var<map<perm_string, Architecture *>> archs;
    var<Architecture*> bound_arch;
    var<map<perm_string, VType::decl_t>> decls;
    var<perm_string> name;

    Match(top){
        Case(C<Entity>(archs, bound_arch, decls, name)){
            cout << "Entity detected: " << name << endl;
//            for (auto &i : archs)
//                traverse(*(i.second));
            return;
        }
        Otherwise() {
            cout << "No Entity!" << endl;
            return;
        }
    } EndMatch
}

void traverse(Architecture *arch){
    var<list<Architecture::Statement *>> stmts;
    var<ComponentInstantiation *> comps;
    var<ProcessStatement *> procs;
    var<perm_string> name;

    Match(arch){
        Case(C<Architecture>(stmts, comps, procs, name)){
            cout << "Architecture detected: " << name << endl;
//            for (auto &i : stmts)
//                traverse(*i);
            return;
        }
        Otherwise(){
            cout << "No Architecture!" << endl;
            return;
        }
    } EndMatch
}
//
//void traverse(const Architecture::Statement &s){
//    var<perm_string> name, label;
//    var<list<Architecture::Statement*>> stmts;
//    var<list<Architecture::Statement*>&> stmts_ptr;
//    var<BlockStatement::BlockHeader&> header;
//    var<Expression&> cond;
//    var<list<Expression*>> rval;
//    var<ExpName&> lval;
//
//    Match(s){
//        Case(C<BlockStatement>(label, &header, &stmts_ptr)){
//            cout << "Found Block statement: " << label << "\n";
//            for (auto &i : (list<Architecture::Statement*>&) stmts_ptr){
//                traverse(*i);
//            }
//        }
//        Case(C<ForGenerate>(label, stmts)){
//            cout << "Found for generatate statement: " << label << "\n";
//            for (auto &i : (list<Architecture::Statement*>) stmts)
//                traverse(*i);
//        }
//        Case(C<IfGenerate>(label, stmts, &cond)){
//            cout << "found if generate statement: " << name << "\n";
//            for (auto &i : (list<Architecture::Statement*>) stmts)
//                traverse(*i);
//        }
//        Case(C<SignalAssignment>(&lval, rval)){
//            cout << "Found SignalAssignment" << endl;
//        }
//        Otherwise() {
//            //cout << "Wildcard pattern for traverse(const Arch::Stmt &s)\n";
//            return;
//        }
//    } EndMatch
//}

