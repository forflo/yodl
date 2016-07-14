const char COPYRIGHT[] =
    "Copyright (c) 2011-2015 Stephen Williams (steve@icarus.com)\n"
    "Copyright CERN 2012 / Stephen Williams (steve@icarus.com)";

/*
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

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

// code base specific includes
#include "vhdlpp_config.h"
#include "generate_graph.h"
#include "version_base.h"
#include "simple_tree.h"
#include "StringHeap.h"
#include "entity.h"
#include "compiler.h"
#include "sequential.h"
#include "library.h"
#include "std_funcs.h"
#include "std_types.h"
#include "architec.h"
#include "parse_api.h"
#include "generic_traverser.h"
#include "vtype.h"
#include "parse_context.h"
#include "mach7_includes.h"
#include "loop_unroller.h"
#include "name_replacer.h"
#include "generate_expander.h"
#include "stateful_lambda.h"

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

#pragma clang diagnostic ignored "-Wshadow"

using namespace std;
using namespace mch;


bool verbose_flag = false;
// Where to dump design entities
const char *work_path = "ivl_vhdl_work";
const char *dump_libraries_path       = 0;
const char *debug_log_path            = 0;

bool     debug_elaboration = false;
ofstream debug_log_file;

extern void dump_libraries(ostream& file);
extern void parser_cleanup();

void main_parse_arguments(int argc, char *argv[]){
#   define VERSION_TAG    "VERSION_TAG"
#   define NOTICE         "NOTICE\n"

    int opt;
    while ((opt = getopt(argc, argv, "L:vVw:")) != EOF) {
        switch (opt) {
        case 'L':
            library_add_directory(optarg);
            break;

        case 'v':
            fprintf(stderr, "Icarus Verilog VHDL Parse version "
                    VERSION " (" VERSION_TAG ")\n\n");
            fprintf(stderr, "%s\n\n", COPYRIGHT);
            fputs(NOTICE, stderr);
            verbose_flag = true;
            break;

        case 'V':
            fprintf(stdout, "Icarus Verilog VHDL Parse version "
                    VERSION " (" VERSION_TAG ")\n\n");
            fprintf(stdout, "%s\n\n", COPYRIGHT);
            fputs(NOTICE, stdout);
            break;

        case 'w':
            work_path = optarg;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    int errors = 0;
    int rc;

    main_parse_arguments(argc, argv);

    if (debug_log_path) {
        debug_log_file.open(debug_log_path);
    }

    if ((rc = mkdir(work_path, 0777)) < 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Icarus Verilog VHDL unable to create work "
                    "directory %s, errno=%d\n", work_path, errno);
            return -1;
        }
        struct stat stat_buf;
        rc = stat(work_path, &stat_buf);

        if (!S_ISDIR(stat_buf.st_mode)) {
            fprintf(stderr, "Icarus Verilog VHDL work path `%s' "
                    "is not a directory.\n", work_path);
            return -1;
        }
    }

    std::cout.precision(std::numeric_limits<double>::digits10);
    library_set_work_path(work_path);

    StandardTypes *std_types = (new StandardTypes())->init();
    StandardFunctions *std_funcs = (new StandardFunctions())->init();
    ParserContext *cont = (new ParserContext(std_types, std_funcs))->init();

    for (int i = optind; i < argc; i += 1) {
        rc = ParserUtil::parse_source_file(argv[i], perm_string(), cont);
        if (rc < 0) {
            return 1;
        }

        if (verbose_flag) {
            fprintf(stderr, "parse_source_file() returns %d"
                    ", parse_errors=%d, parse_sorrys=%d\n",
                    rc, cont->parse_errors, cont->parse_sorrys);
        }

        if (cont->parse_errors > 0) {
            fprintf(stderr, "Encountered %d errors parsing %s\n",
                    cont->parse_errors, argv[i]);
        }

        if (cont->parse_sorrys > 0) {
            fprintf(stderr, "Encountered %d unsupported constructs parsing %s\n",
                    cont->parse_sorrys, argv[i]);
        }

        if (cont->parse_errors || cont->parse_sorrys) {
            errors += cont->parse_errors;
            errors += cont->parse_sorrys;
            break;
        }
    }

    /* Playground */
    cout << "There are " <<  cont->design_entities.size() << " entities\n";

    auto iter = cont->design_entities.begin();
    cout << "Entity found!\n";

    ForLoopUnroller forUnroller{};

    GenericTraverser loopUnroller(
        [](const AstNode *n) -> bool {
            Match(n){
                Case(C<ProcessStatement>()){
                    return true;
                }
                Otherwise(){
                    return false;
                }
            } EndMatch;
            return false;
        },
        static_cast<function<int (AstNode *)>>(forUnroller),
        GenericTraverser::NONRECUR);

    loopUnroller(iter->second);

    GenerateExpander genE;
    GenericTraverser genExpander(
        [](const AstNode *n) -> bool {
            Match(n){
                Case(C<ExpName>()){return true;}
                Case(C<Architecture>()){return true;}
                Case(C<ScopeBase>()){return true;}
                Case(C<BlockStatement>()){return true;}
            } EndMatch;
            return false;
        },
        genE,
        GenericTraverser::NONRECUR);

    genExpander(iter->second);

    DotGraphGenerator()
        .setBlacklist({"node-pointer"})(
            std::cout, "foobar",
            dynamic_cast<Entity*>(iter->second)->emit_strinfo_tree());

    DotGraphGenerator()(
        std::cout,
        dynamic_cast<Entity*>(iter->second)->emit_strinfo_tree());

    cout << "\n\n";

    /* End Playground */

    if (dump_libraries_path) {
        ofstream file(dump_libraries_path);

        dump_libraries(file);
    }


    cout << "VHDL-AST-Dump" << endl;
    cont->dump_design_entities(std::cout);

    cout << endl;
    cout << "Verilog" << endl;

    if (errors > 0) {
        delete cont;
        return 2;
    }

    errors = ParserUtil::elaborate_entities(cont);
    if (errors > 0) {
        fprintf(stderr, "%d errors elaborating design.\n", errors);
        delete cont;
        return 3;
    }

    errors = elaborate_libraries();
    if (errors > 0) {
        fprintf(stderr, "%d errors elaborating libraries.\n", errors);
        delete cont;
        return 4;
    }

    std_types->emit_std_types(cout);

    errors = emit_packages();
    if (errors > 0) {
        fprintf(stderr, "%d errors emitting packages.\n", errors);
        delete cont;
        return 5;
    }

    errors = cont->emit_entities();
    if (errors > 0) {
        fprintf(stderr, "%d errors emitting design.\n", errors);
        delete cont;
        return 6;
    }

    delete cont;
    return 0;
}
