/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 6 "parse.y" /* yacc.c:339  */

/*
 * Copyright (c) 2011-2013 Stephen Williams (steve@icarus.com)
 * Copyright CERN 2012-2016 / Stephen Williams (steve@icarus.com),
 * @author Maciej Suminski (maciej.suminski@cern.ch)
 *
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

# include "vhdlpp_config.h"
# include "vhdlint.h"
# include "vhdlreal.h"
# include "compiler.h"
# include "parse_api.h"
# include "parse_misc.h"
# include "architec.h"
# include "expression.h"
# include "sequential.h"
# include "subprogram.h"
# include "package.h"
# include "vsignal.h"
# include "vtype.h"
# include "std_funcs.h"
# include "std_types.h"
# include  <cstdarg>
# include  <cstring>
# include  <list>
# include  <stack>
# include  <map>
# include  <vector>
# include  "parse_types.h"
# include  <ivl_assert.h>
# include  <assert.h>

inline void FILE_NAME(LineInfo*tmp, const struct yyltype&where)
{
      tmp->set_lineno(where.first_line);
      tmp->set_file(filename_strings.make(where.text));
}


/* Recent version of bison expect that the user supply a
   YYLLOC_DEFAULT macro that makes up a yylloc value from existing
   values. I need to supply an explicit version to account for the
   text field, that otherwise won't be copied. */
# define YYLLOC_DEFAULT(Current, Rhs, N)  do {       \
  (Current).first_line   = (Rhs)[1].first_line;      \
  (Current).text         = file_path; /*(Rhs)[1].text;*/   } while (0)

static void yyerror(YYLTYPE*yyllocp,yyscan_t yyscanner,const char*file_path,bool, const char*msg);

int parse_errors = 0;
int parse_sorrys = 0;

/*
 * The parser calls yylex to get the next lexical token. It is only
 * called by the bison-generated parser.
 */
extern int yylex(union YYSTYPE*yylvalp,YYLTYPE*yyllocp,yyscan_t yyscanner);


/*
 * Create an initial scope that collects all the global
 * declarations. Also save a stack of previous scopes, as a way to
 * manage lexical scopes.
 */
static ActiveScope*active_scope = new ActiveScope;
static stack<ActiveScope*> scope_stack;
static SubprogramHeader*active_sub = NULL;
static ActiveScope*arc_scope = NULL;

/*
 * When a scope boundary starts, call the push_scope function to push
 * a scope context. Preload this scope context with the contents of
 * the parent scope, then make this the current scope. When the scope
 * is done, the pop_scope function pops the scope off the stack and
 * resumes the scope that was the parent.
 */
static void push_scope(void)
{
      assert(active_scope);
      scope_stack.push(active_scope);
      active_scope = new ActiveScope (active_scope);
}

static void pop_scope(void)
{
      delete active_scope;
      assert(! scope_stack.empty());
      active_scope = scope_stack.top();
      scope_stack.pop();
}

static bool is_subprogram_param(perm_string name)
{
    if(!active_sub)
        return false;

    return (active_sub->find_param(name) != NULL);
}

void preload_global_types(void)
{
      generate_global_types(active_scope);
}

//Remove the scope created at the beginning of parser's work.
//After the parsing active_scope should keep it's address

static void delete_global_scope(void)
{
    active_scope->destroy_global_scope();
    delete active_scope;
}

//delete global entities that were gathered over the parsing process
static void delete_design_entities(void)
{
      for(map<perm_string,Entity*>::iterator cur = design_entities.begin()
      ; cur != design_entities.end(); ++cur)
        delete cur->second;
}

//clean the mess caused by the parser
void parser_cleanup(void)
{
    delete_design_entities();
    delete_global_scope();
    delete_std_funcs();
    lex_strings.cleanup();
}

const VType*parse_type_by_name(perm_string name)
{
      return active_scope->find_type(name);
}

// This function is called when an aggregate expression is detected by
// the parser. It makes the ExpAggregate. It also tries to detect the
// special case that the aggregate is really a primary. The problem is
// that this:
//   ( <expression> )
// also matches the pattern:
//   ( [ choices => ] <expression> ... )
// so try to assume that a single expression in parentheses is a
// primary and fix the parse by returning an Expression instead of an
// ExpAggregate.
static Expression*aggregate_or_primary(const YYLTYPE&loc, std::list<ExpAggregate::element_t*>*el)
{
      if (el->size() != 1) {
	    ExpAggregate*tmp = new ExpAggregate(el);
	    FILE_NAME(tmp,loc);
	    return tmp;
      }

      ExpAggregate::element_t*el1 = el->front();
      if (el1->count_choices() > 0) {
	    ExpAggregate*tmp = new ExpAggregate(el);
	    FILE_NAME(tmp,loc);
	    return tmp;
      }

      return el1->extract_expression();
}

static list<VTypeRecord::element_t*>* record_elements(list<perm_string>*names,
						      const VType*type)
{
      list<VTypeRecord::element_t*>*res = new list<VTypeRecord::element_t*>;

      for (list<perm_string>::iterator cur = names->begin()
		 ; cur != names->end() ; ++cur) {
	    res->push_back(new VTypeRecord::element_t(*cur, type));
      }

      return res;
}

static void touchup_interface_for_functions(std::list<InterfacePort*>*ports)
{
      for (list<InterfacePort*>::iterator cur = ports->begin()
		 ; cur != ports->end() ; ++cur) {
	    InterfacePort*curp = *cur;
	    if (curp->mode == PORT_NONE)
		  curp->mode = PORT_IN;
      }
}


#line 269 "parse.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parse.hh".  */
#ifndef YY_YY_PARSE_HH_INCLUDED
# define YY_YY_PARSE_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    K_abs = 258,
    K_access = 259,
    K_after = 260,
    K_alias = 261,
    K_all = 262,
    K_and = 263,
    K_architecture = 264,
    K_array = 265,
    K_assert = 266,
    K_assume = 267,
    K_assume_guarantee = 268,
    K_attribute = 269,
    K_begin = 270,
    K_block = 271,
    K_body = 272,
    K_buffer = 273,
    K_bus = 274,
    K_case = 275,
    K_component = 276,
    K_configuration = 277,
    K_constant = 278,
    K_context = 279,
    K_cover = 280,
    K_default = 281,
    K_disconnect = 282,
    K_downto = 283,
    K_else = 284,
    K_elsif = 285,
    K_end = 286,
    K_entity = 287,
    K_exit = 288,
    K_fairness = 289,
    K_file = 290,
    K_for = 291,
    K_force = 292,
    K_function = 293,
    K_generate = 294,
    K_generic = 295,
    K_group = 296,
    K_guarded = 297,
    K_if = 298,
    K_impure = 299,
    K_in = 300,
    K_inertial = 301,
    K_inout = 302,
    K_is = 303,
    K_label = 304,
    K_library = 305,
    K_linkage = 306,
    K_literal = 307,
    K_loop = 308,
    K_map = 309,
    K_mod = 310,
    K_nand = 311,
    K_new = 312,
    K_next = 313,
    K_nor = 314,
    K_not = 315,
    K_null = 316,
    K_of = 317,
    K_on = 318,
    K_open = 319,
    K_or = 320,
    K_others = 321,
    K_out = 322,
    K_package = 323,
    K_parameter = 324,
    K_port = 325,
    K_postponed = 326,
    K_procedure = 327,
    K_process = 328,
    K_property = 329,
    K_protected = 330,
    K_pure = 331,
    K_range = 332,
    K_record = 333,
    K_register = 334,
    K_reject = 335,
    K_release = 336,
    K_rem = 337,
    K_report = 338,
    K_restrict = 339,
    K_restrict_guarantee = 340,
    K_return = 341,
    K_reverse_range = 342,
    K_rol = 343,
    K_ror = 344,
    K_select = 345,
    K_sequence = 346,
    K_severity = 347,
    K_signal = 348,
    K_shared = 349,
    K_sla = 350,
    K_sll = 351,
    K_sra = 352,
    K_srl = 353,
    K_strong = 354,
    K_subtype = 355,
    K_then = 356,
    K_to = 357,
    K_transport = 358,
    K_type = 359,
    K_unaffected = 360,
    K_units = 361,
    K_until = 362,
    K_use = 363,
    K_variable = 364,
    K_vmode = 365,
    K_vprop = 366,
    K_vunit = 367,
    K_wait = 368,
    K_when = 369,
    K_while = 370,
    K_with = 371,
    K_xnor = 372,
    K_xor = 373,
    IDENTIFIER = 374,
    INT_LITERAL = 375,
    REAL_LITERAL = 376,
    STRING_LITERAL = 377,
    CHARACTER_LITERAL = 378,
    BITSTRING_LITERAL = 379,
    LEQ = 380,
    GEQ = 381,
    VASSIGN = 382,
    NE = 383,
    BOX = 384,
    EXP = 385,
    ARROW = 386,
    DLT = 387,
    DGT = 388,
    CC = 389,
    M_EQ = 390,
    M_NE = 391,
    M_LT = 392,
    M_LEQ = 393,
    M_GT = 394,
    M_GEQ = 395
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 210 "parse.y" /* yacc.c:355  */

      port_mode_t port_mode;

      char*text;

      std::list<perm_string>* name_list;

      bool flag;
      int64_t uni_integer;
      double  uni_real;

      Expression*expr;
      std::list<Expression*>* expr_list;

      SequentialStmt* sequ;
      std::list<SequentialStmt*>*sequ_list;

      IfSequential::Elsif*elsif;
      std::list<IfSequential::Elsif*>*elsif_list;

      ExpConditional::case_t*exp_options;
      std::list<ExpConditional::case_t*>*exp_options_list;

      CaseSeqStmt::CaseStmtAlternative* case_alt;
      std::list<CaseSeqStmt::CaseStmtAlternative*>* case_alt_list;

      named_expr_t*named_expr;
      std::list<named_expr_t*>*named_expr_list;
      entity_aspect_t* entity_aspect;
      instant_list_t* instantiation_list;
      std::pair<instant_list_t*, ExpName*>* component_specification;

      const VType* vtype;

      ExpRange*range;
      std::list<ExpRange*>*range_list;
      ExpRange::range_dir_t range_dir;

      ExpArithmetic::fun_t arithmetic_op;
      std::list<struct adding_term>*adding_terms;

      ExpAggregate::choice_t*choice;
      std::list<ExpAggregate::choice_t*>*choice_list;
      ExpAggregate::element_t*element;
      std::list<ExpAggregate::element_t*>*element_list;

      std::list<VTypeRecord::element_t*>*record_elements;

      std::list<InterfacePort*>* interface_list;

      Architecture::Statement* arch_statement;
      std::list<Architecture::Statement*>* arch_statement_list;

      ReportStmt::severity_t severity;

      SubprogramHeader*subprogram;

      file_open_info_t*file_info;

#line 510 "parse.cc" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (yyscan_t yyscanner, const char*file_path, perm_string parse_library_name);

#endif /* !YY_YY_PARSE_HH_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 540 "parse.cc" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1283

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  157
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  198
/* YYNRULES -- Number of rules.  */
#define YYNRULES  450
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  884

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   395

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   143,   150,
     145,   146,   155,   141,   147,   142,   154,   156,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   148,   144,
     152,   151,   153,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   149,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   391,   391,   391,   394,   395,   396,   405,   403,   426,
     438,   443,   449,   456,   459,   460,   464,   472,   480,   485,
     490,   498,   503,   511,   521,   522,   526,   534,   535,   539,
     551,   553,   555,   557,   559,   561,   563,   567,   569,   580,
     581,   585,   586,   590,   601,   606,   621,   659,   661,   663,
     668,   673,   681,   690,   698,   717,   726,   727,   731,   742,
     751,   761,   768,   778,   783,   803,   819,   835,   846,   851,
     859,   860,   865,   870,   878,   880,   885,   887,   892,   894,
     896,   902,   911,   912,   913,   914,   915,   919,   929,   937,
     941,   942,   946,   947,   951,   952,   956,   957,   961,   962,
     966,   974,   981,   991,   996,  1003,  1003,  1006,  1007,  1011,
    1012,  1017,  1018,  1022,  1023,  1027,  1028,  1032,  1037,  1044,
    1049,  1057,  1064,  1069,  1075,  1081,  1087,  1095,  1096,  1100,
    1117,  1125,  1134,  1143,  1150,  1155,  1163,  1165,  1193,  1194,
    1199,  1204,  1209,  1214,  1219,  1227,  1229,  1237,  1239,  1247,
    1249,  1257,  1259,  1267,  1269,  1274,  1279,  1287,  1324,  1331,
    1338,  1344,  1345,  1349,  1370,  1384,  1385,  1389,  1393,  1396,
    1400,  1402,  1410,  1411,  1415,  1417,  1425,  1431,  1439,  1439,
    1441,  1441,  1448,  1467,  1478,  1487,  1493,  1501,  1502,  1506,
    1511,  1519,  1525,  1533,  1535,  1541,  1545,  1547,  1556,  1560,
    1561,  1565,  1570,  1575,  1584,  1602,  1603,  1607,  1613,  1620,
    1624,  1631,  1632,  1635,  1638,  1644,  1653,  1671,  1690,  1708,
    1709,  1710,  1713,  1713,  1716,  1733,  1736,  1739,  1753,  1764,
    1774,  1779,  1787,  1805,  1813,  1822,  1823,  1827,  1828,  1831,
    1832,  1836,  1837,  1838,  1839,  1840,  1841,  1842,  1849,  1850,
    1854,  1855,  1859,  1870,  1882,  1896,  1900,  1901,  1905,  1907,
    1914,  1914,  1917,  1919,  1926,  1927,  1932,  1937,  1939,  1955,
    1961,  1966,  1971,  1977,  1983,  2018,  2026,  2033,  2034,  2035,
    2039,  2046,  2053,  2061,  2070,  2074,  2078,  2089,  2090,  2094,
    2095,  2099,  2100,  2104,  2111,  2136,  2152,  2154,  2160,  2164,
    2171,  2176,  2181,  2193,  2208,  2213,  2221,  2228,  2230,  2235,
    2240,  2245,  2250,  2255,  2263,  2270,  2275,  2280,  2290,  2291,
    2295,  2304,  2314,  2319,  2330,  2334,  2339,  2348,  2349,  2353,
    2369,  2374,  2382,  2387,  2395,  2401,  2410,  2411,  2415,  2416,
    2417,  2418,  2419,  2420,  2421,  2422,  2423,  2424,  2425,  2426,
    2434,  2451,  2452,  2455,  2456,  2461,  2466,  2471,  2476,  2482,
    2491,  2492,  2496,  2497,  2520,  2524,  2529,  2531,  2549,  2557,
    2568,  2574,  2583,  2584,  2590,  2600,  2619,  2632,  2637,  2638,
    2642,  2643,  2647,  2648,  2652,  2653,  2656,  2656,  2659,  2660,
    2668,  2672,  2693,  2702,  2711,  2722,  2724,  2726,  2733,  2735,
    2740,  2745,  2750,  2758,  2776,  2783,  2788,  2795,  2801,  2807,
    2809,  2814,  2815,  2820,  2821,  2825,  2826,  2830,  2831,  2837,
    2842,  2848,  2857,  2868,  2875,  2876,  2880,  2885,  2890,  2895,
    2903,  2905,  2910,  2915,  2923,  2925,  2929,  2935,  2935,  2936,
    2936,  2937,  2937,  2938,  2938,  2939,  2939,  2940,  2940,  2941,
    2941
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "K_abs", "K_access", "K_after",
  "K_alias", "K_all", "K_and", "K_architecture", "K_array", "K_assert",
  "K_assume", "K_assume_guarantee", "K_attribute", "K_begin", "K_block",
  "K_body", "K_buffer", "K_bus", "K_case", "K_component",
  "K_configuration", "K_constant", "K_context", "K_cover", "K_default",
  "K_disconnect", "K_downto", "K_else", "K_elsif", "K_end", "K_entity",
  "K_exit", "K_fairness", "K_file", "K_for", "K_force", "K_function",
  "K_generate", "K_generic", "K_group", "K_guarded", "K_if", "K_impure",
  "K_in", "K_inertial", "K_inout", "K_is", "K_label", "K_library",
  "K_linkage", "K_literal", "K_loop", "K_map", "K_mod", "K_nand", "K_new",
  "K_next", "K_nor", "K_not", "K_null", "K_of", "K_on", "K_open", "K_or",
  "K_others", "K_out", "K_package", "K_parameter", "K_port", "K_postponed",
  "K_procedure", "K_process", "K_property", "K_protected", "K_pure",
  "K_range", "K_record", "K_register", "K_reject", "K_release", "K_rem",
  "K_report", "K_restrict", "K_restrict_guarantee", "K_return",
  "K_reverse_range", "K_rol", "K_ror", "K_select", "K_sequence",
  "K_severity", "K_signal", "K_shared", "K_sla", "K_sll", "K_sra", "K_srl",
  "K_strong", "K_subtype", "K_then", "K_to", "K_transport", "K_type",
  "K_unaffected", "K_units", "K_until", "K_use", "K_variable", "K_vmode",
  "K_vprop", "K_vunit", "K_wait", "K_when", "K_while", "K_with", "K_xnor",
  "K_xor", "IDENTIFIER", "INT_LITERAL", "REAL_LITERAL", "STRING_LITERAL",
  "CHARACTER_LITERAL", "BITSTRING_LITERAL", "LEQ", "GEQ", "VASSIGN", "NE",
  "BOX", "EXP", "ARROW", "DLT", "DGT", "CC", "M_EQ", "M_NE", "M_LT",
  "M_LEQ", "M_GT", "M_GEQ", "'+'", "'-'", "'&'", "';'", "'('", "')'",
  "','", "':'", "'|'", "'\\''", "'='", "'<'", "'>'", "'.'", "'*'", "'/'",
  "$accept", "design_file", "$@1", "adding_operator", "architecture_body",
  "$@2", "architecture_body_start", "architecture_statement_part",
  "argument_list", "argument_list_opt", "assertion_statement",
  "association_element", "association_list", "binding_indication",
  "binding_indication_semicolon_opt", "block_configuration",
  "block_configuration_opt", "block_declarative_item",
  "block_declarative_items", "block_declarative_items_opt",
  "case_statement", "case_statement_alternative_list",
  "case_statement_alternative", "choice", "choices",
  "component_configuration", "component_declaration", "instantiated_unit",
  "component_instantiation_statement", "component_specification",
  "composite_type_definition", "concurrent_assertion_statement",
  "concurrent_conditional_signal_assignment",
  "concurrent_simple_signal_assignment", "else_when_waveforms",
  "else_when_waveforms_opt", "else_when_waveform",
  "concurrent_signal_assignment_statement", "concurrent_statement",
  "configuration_declaration", "configuration_declarative_item",
  "configuration_declarative_items", "configuration_declarative_part",
  "configuration_item", "configuration_items", "configuration_items_opt",
  "constant_declaration", "context_clause", "context_item",
  "context_items", "design_unit", "design_units", "direction",
  "element_association", "element_association_list", "element_declaration",
  "element_declaration_list", "entity_aspect", "entity_aspect_opt",
  "entity_declaration", "enumeration_literal", "enumeration_literal_list",
  "expression_list", "expression", "expression_logical",
  "expression_logical_and", "expression_logical_or",
  "expression_logical_xnor", "expression_logical_xor", "factor",
  "file_declaration", "file_open_information", "file_open_information_opt",
  "for_generate_statement", "function_specification", "generate_statement",
  "generate_statement_body", "generic_clause_opt", "generic_clause",
  "generic_map_aspect_opt", "generic_map_aspect", "identifier_list",
  "identifier_opt", "identifier_colon_opt", "if_generate_statement",
  "if_statement", "if_statement_elsif_list_opt", "if_statement_elsif_list",
  "if_statement_elsif", "if_statement_else", "index_constraint",
  "index_subtype_definition", "index_subtype_definition_list",
  "instantiation_list", "interface_element",
  "interface_element_expression", "interface_list", "library_clause",
  "library_unit", "logical_name", "logical_name_list", "loop_statement",
  "mode", "mode_opt", "name", "indexed_name", "name_list",
  "package_declaration", "package_declaration_start",
  "package_body_declarative_item", "package_body_declarative_items",
  "package_body_declarative_part_opt", "package_declarative_item",
  "package_declarative_items", "package_declarative_part_opt",
  "package_body", "package_body_start", "parameter_list",
  "parameter_list_opt", "port_clause", "port_clause_opt",
  "port_map_aspect", "port_map_aspect_opt", "prefix", "primary",
  "primary_unit", "procedure_call", "procedure_call_statement",
  "procedure_specification", "process_declarative_item",
  "process_declarative_part", "process_declarative_part_opt",
  "process_start", "process_statement", "process_sensitivity_list_opt",
  "process_sensitivity_list", "range", "range_list",
  "record_type_definition", "relation", "report_statement",
  "return_statement", "secondary_unit", "selected_name", "selected_names",
  "selected_name_lib", "selected_names_lib", "selected_signal_assignment",
  "selected_waveform", "selected_waveform_list", "list_of_statements",
  "sequence_of_statements", "sequential_statement", "severity",
  "severity_opt", "shift_expression", "sign",
  "signal_declaration_assign_opt", "simple_expression",
  "simple_expression_2", "simple_expression_terms", "signal_assignment",
  "signal_assignment_statement", "subprogram_body_start",
  "subprogram_body", "subprogram_declaration",
  "subprogram_declarative_item", "subprogram_declarative_item_list",
  "subprogram_declarative_part", "subprogram_kind", "subprogram_kind_opt",
  "subprogram_specification", "subprogram_statement_part",
  "subtype_declaration", "subtype_indication", "suffix", "term",
  "type_declaration", "type_definition", "use_clause", "use_clause_lib",
  "use_clauses_lib", "use_clauses_opt", "variable_assignment_statement",
  "variable_assignment", "variable_declaration",
  "variable_declaration_assign_opt", "wait_statement", "waveform",
  "waveform_elements", "waveform_element", "K_architecture_opt",
  "K_configuration_opt", "K_entity_opt", "K_is_opt", "K_package_opt",
  "K_postponed_opt", "K_shared_opt", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,    43,    45,    38,    59,    40,    41,    44,    58,   124,
      39,    61,    60,    62,    46,    42,    47
};
# endif

#define YYPACT_NINF -627

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-627)))

#define YYTABLE_NINF -451

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -627,    38,   570,  -627,  -627,    43,    70,   391,  -627,   208,
    -627,   614,  -627,  -627,   -93,  -627,  -627,   363,   -87,    54,
    -627,   398,   133,    71,    80,     2,  -627,   207,  -627,  -627,
    -627,  -627,   288,  -627,   358,  -627,  -627,  -627,  -627,  -627,
    -627,   242,  -627,    10,  -627,   330,  -627,   430,   443,   438,
     488,   367,  -627,   437,   990,   336,  -627,  -627,   369,  -627,
     573,   470,   569,   571,  -627,  -627,    49,   507,    82,   552,
     555,   562,    83,    90,  -627,  -627,  -627,  -627,  1040,   653,
    -627,  -627,   559,  -627,  -627,  -627,   668,  -627,   -11,   699,
      -9,  -627,   697,  -627,    92,  -627,   586,   702,  -627,   586,
     616,   667,  -627,   716,   705,  -627,   728,   650,  -627,   239,
     663,   663,   781,   687,   -25,   282,   703,  -627,   717,   684,
     287,   407,   732,  -627,   705,  -627,   705,  -627,   705,   122,
    -627,  -627,  -627,    -9,   726,  -627,   757,  -627,  -627,  -627,
    -627,   735,   778,   738,   125,   744,   749,  -627,   865,   809,
    -627,   586,  -627,   400,  -627,   782,   183,   784,   820,  -627,
    -627,   789,  -627,    33,  -627,  -627,   198,   521,   521,   198,
     521,  -627,   191,   586,   586,   586,   767,   333,  -627,   989,
     220,  -627,  -627,   778,   880,  -627,  -627,   772,   501,  -627,
     489,  -627,   223,   780,   569,   783,   249,  -627,   872,   910,
    -627,  -627,  -627,  -627,   -24,  -627,  -627,  -627,   787,   895,
     667,  -627,   791,   196,   -86,   817,   797,   798,   800,   784,
     824,  -627,  -627,   806,  -627,  -627,  -627,  -627,   799,   109,
     109,   812,   832,  -627,  -627,  -627,  -627,  -627,  -627,    46,
     514,  -627,  -627,   457,   313,   828,  -627,   310,   814,   640,
      95,  1072,  -627,   549,   551,  -627,   567,   287,   816,   818,
     819,  -627,   847,    56,   521,   521,   587,   826,   521,    53,
     314,   473,  -627,  -627,   225,  -627,  -627,   505,  -627,  -627,
    -627,  -627,   854,  -627,  -627,  -627,  -627,   897,  -627,  -627,
    -627,   830,   596,  -627,   858,   947,   837,   592,   784,  -627,
     841,  -627,   586,  -627,   839,   601,  -627,   503,  -627,   971,
     962,  -627,   521,   597,  -627,   664,  -627,  -627,  -627,  -627,
     676,   933,  -627,    -3,   610,  -627,  -627,   588,  -627,   475,
    -627,  -627,   692,  -627,  -627,  -627,   -76,  -627,   655,  -627,
     657,   658,  -627,   521,    95,    95,    95,    95,   466,   109,
     521,   521,   521,   521,   521,   521,   521,   521,   521,   521,
     521,   521,  -627,  -627,   521,   521,   521,   521,   521,   521,
    -627,   521,  -627,  -627,  -627,    95,   549,  -627,  -627,  -627,
    -627,  -627,   447,    -2,   521,  -627,   311,   948,    84,   896,
    -627,   906,   344,  -627,   855,   521,   521,   521,  -627,  -627,
     722,   394,   859,   883,  1023,   521,   203,   730,   404,  -627,
      -2,  -627,   789,   898,   573,  -627,  -627,  -627,  -627,  -627,
     789,  -627,   863,   864,  -627,   789,   339,   521,   424,   318,
    -627,  -627,  -627,  -627,  -627,  -627,  -627,  -627,  -627,   941,
    -627,   889,   870,  -627,  -627,   586,   998,   168,   146,  -627,
     673,   168,   405,   879,   238,  -627,   679,   789,   951,  -627,
     789,  -627,   824,   907,   395,  -627,   681,   521,    46,  -627,
      46,  -627,  -627,  -627,  -627,  -627,  -627,  -627,   885,  -627,
    1028,  -627,   674,  -627,  -627,   973,  -627,   923,  -627,   926,
    -627,  -627,  -627,  -627,  -627,  -627,  -627,  -627,  -627,  -627,
    -627,  -627,  -627,  -627,  -627,    95,   924,   928,  -627,   901,
    -627,  -627,  -627,   586,   904,   932,  -627,  -627,   908,   939,
    1012,   869,   903,   912,  -627,  -627,   920,   921,   929,   201,
     689,   693,   -65,   678,  -627,  -627,  -627,  -627,  1022,  1037,
    1017,  -627,  -627,  1074,   -85,   930,  -627,   449,   936,   586,
     955,  -627,   898,  1047,   586,   958,  -627,  -627,   959,   203,
    -627,  1002,   375,   -77,  1087,  -627,  -627,  1024,   355,    39,
     728,   957,   586,   521,  -627,  -627,   521,  -627,  -627,   978,
    1041,   991,  -627,  -627,   967,  -627,   442,  -627,   994,  -627,
    -627,  -627,   168,  -627,   521,  -627,  -627,   521,   521,   521,
     521,  -627,  -627,  1067,  -627,   972,  -627,  -627,  -627,    46,
       4,  -627,   975,  1088,   176,  1088,  -627,  -627,  -627,  -627,
     976,   979,   982,   521,  1064,  1023,   521,   521,  -627,   486,
    -627,  -627,   983,   521,   984,  -627,   286,  -627,  -627,  -627,
    1047,  1091,   985,   521,  -627,   521,   987,   988,   191,  1014,
    1015,   521,   284,    20,  -627,  -627,  1010,  -627,  -627,   586,
    -627,   480,   218,   308,  -627,   992,   996,   993,    -9,  -627,
     997,  -627,  -627,  -627,   789,  1068,  -627,  -627,   799,  -627,
    -627,  1018,  -627,   731,  -627,  -627,  -627,  -627,  1030,  -627,
     233,  1130,  -627,  -627,   521,  1125,  1088,  -627,  1113,  1125,
    -627,  -627,  1105,   586,  1128,  -627,  1133,  -627,  -627,  -627,
    -627,  -627,  -627,   302,    14,  1016,  1025,  -627,  1129,  -627,
    -627,  -627,  -627,  -627,  1039,  -627,  1121,  1132,  1031,  1118,
    1106,  -627,   396,  1034,   521,  -627,   521,  -627,  -627,   191,
    -627,  -627,  -627,    -9,  1164,  -627,  -627,  -627,  -627,  -627,
     974,  1042,  1080,  1038,  1151,  -627,  1043,  1152,  1023,  1044,
    1131,   203,  1154,   535,  1045,   880,   191,  1046,   203,   521,
     503,  -627,  1048,  1137,  -627,  1050,   111,  -627,  1163,  1163,
     992,  -627,  1094,  -627,  -627,   918,   404,  -627,  1153,  -627,
    1155,  1166,  -627,   586,  1051,  1165,   191,   191,  -627,  -627,
    1106,  -627,  -627,  1168,   992,  -627,  -627,   433,  1086,  1167,
     585,  1171,   252,  1058,  -627,   203,   184,  -627,  1163,  1060,
     216,  1174,   404,  -627,  1066,  1070,  1158,  1071,  -627,  -627,
     992,   992,  1172,  1180,  -627,   203,   531,   503,  1169,  1073,
     745,   254,  1103,  -627,  -627,  -627,   941,   941,  -627,  -627,
     586,  -627,  -627,  1076,  -627,  -627,  -627,  1187,   586,  -627,
    -627,  1075,   774,   521,  1149,  1150,  1081,  -627,  1185,  1082,
    -627,  -627,  -627,   586,   586,  -627,   586,  -627,  1083,  1084,
    1085,  -627,  -627,  -627
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       2,     0,     0,     1,   112,     0,     0,     0,   110,   105,
     114,     0,   107,   108,     0,   213,   215,     0,     0,     0,
     328,     0,     0,     0,     0,     0,   318,     0,   278,   277,
     111,   279,     0,   319,     0,   211,   212,   109,   113,   210,
     209,     0,   412,     0,   411,     0,     9,     0,     0,     0,
       0,     0,   234,     0,     0,     0,   214,   324,     0,   327,
     440,     0,   442,   169,   254,     7,     0,     0,     0,     0,
       0,     0,     0,     0,   241,   242,   388,   249,     0,     0,
     389,   243,     0,   244,   245,   246,     0,   238,   239,     0,
     383,   236,     0,   235,     0,   439,   179,     0,   441,   179,
       0,   261,   168,     0,   446,   247,   444,     0,   177,     0,
       0,   257,     0,     0,     0,     0,   224,   266,   226,     0,
     323,     0,     0,   248,   446,   377,   446,   237,   446,     0,
     449,   379,   381,   382,     0,   378,     0,   374,   325,   326,
     178,     0,    93,     0,     0,     0,     0,   260,     0,     0,
     445,   179,   443,     0,   104,     0,     0,     0,     0,   256,
     286,     0,   406,     0,   404,   410,     0,     0,     0,     0,
       0,   409,     0,   179,   179,   179,     0,     0,   380,     0,
       0,    88,    91,    92,     0,    89,   130,     0,     0,   208,
       0,   170,     0,     0,   442,     0,     0,    40,     0,     0,
      30,    31,    33,    32,     0,    34,    35,    36,     0,     0,
     261,   176,     0,   392,     0,     0,     0,     0,     0,     0,
       0,   408,    63,     0,   397,   395,   396,   321,     0,     0,
       0,   224,   270,   271,   272,   269,   273,   360,   361,     0,
       0,   135,   136,   398,   267,   153,   137,   138,   225,   307,
       0,   353,   365,   366,     0,   320,     0,   322,     0,     0,
       0,   158,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   224,   346,   341,     0,   338,   343,   266,   285,   342,
     345,   344,     0,   390,   335,   372,   339,     0,   340,   417,
     347,     0,     0,    90,     0,     0,     0,   223,     0,   255,
       0,   258,   179,    38,     0,     0,    39,     0,   233,     0,
       0,   103,     0,     0,   393,     0,   101,   164,   391,   405,
       0,     0,   123,     0,     0,   131,   133,     0,   403,   267,
     155,   156,     0,   274,    48,    51,     0,   120,     0,   118,
      49,   353,   228,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   364,   116,     0,     0,     0,     0,     0,     0,
     115,     0,     4,     5,     6,     0,   367,   229,   227,   232,
     253,   252,   162,   387,     0,   349,   352,     0,     0,     0,
     348,   352,     0,   316,     0,     0,     0,     0,   429,   280,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   334,
     387,   423,     0,   416,   440,   171,   219,   221,   220,   222,
       0,   207,     0,     0,    37,     0,     0,     0,   224,     0,
      64,    82,    84,    76,    74,    83,    11,   166,    85,   448,
     165,   266,   298,    86,    78,   179,     0,     0,     0,   304,
       0,     0,     0,     0,   224,   200,     0,     0,     0,   122,
       0,   407,     0,     0,   224,    22,     0,     0,     0,   276,
       0,   134,   401,   402,   399,   400,   302,   303,    15,   154,
     139,   145,   353,   142,   143,   140,   147,   144,   149,   141,
     151,   311,   312,   313,   308,   309,   310,   359,   358,   357,
     355,   356,   354,   301,   368,     0,     0,     0,   161,     0,
     384,   385,   386,   179,     0,     0,    16,   351,     0,     0,
       0,     0,     0,     0,   317,   315,     0,     0,     0,     0,
       0,     0,     0,   224,   284,   373,   418,   282,     0,     0,
       0,   436,   431,   434,     0,   430,   433,     0,     0,   179,
     425,   414,   415,    99,   179,   206,   259,   129,   363,     0,
      12,     0,     0,     0,   438,    10,   447,     0,     0,     0,
     444,     0,   179,     0,   197,   196,     0,   102,   100,     0,
       0,     0,    61,   306,     0,   132,     0,   275,     0,   117,
      50,    49,    47,   119,     0,    14,   268,     0,     0,     0,
       0,   369,   160,     0,   157,     0,   421,   350,    17,     0,
       0,    45,     0,   188,     0,   188,   314,   426,   427,   428,
       0,     0,   228,     0,     0,     0,     0,     0,   370,     0,
     420,   419,     0,     0,     0,   413,     0,    94,    95,    97,
      98,     0,     0,     0,   204,     0,     0,     0,     0,     0,
       0,     0,    56,     0,    77,    75,   266,    79,   437,   179,
     293,     0,     0,     0,   299,   231,   300,     0,   292,    55,
       0,   394,   305,   198,     0,     0,   199,   121,    20,    19,
      18,     0,    21,     0,   146,   148,   150,   152,     0,   376,
       0,     0,    44,   186,     0,   195,   187,   190,     0,   195,
     283,   281,     0,   179,     0,   435,     0,   432,   375,   424,
     422,   203,   202,   177,     0,   201,     0,    96,     0,    87,
     205,   362,    29,    81,   266,    57,     0,     0,     0,     0,
     265,   172,     0,     0,     0,    80,     0,    67,   297,     0,
     296,   288,   290,   291,     0,   287,    54,    62,    13,   159,
       0,     0,     0,     0,     0,   189,     0,     0,     0,     0,
       0,     0,     0,   128,     0,    28,     0,     0,     0,     0,
       0,    59,     0,     0,   264,     0,     0,     8,     0,    71,
     230,   289,     0,    46,    43,     0,   194,   193,     0,   185,
       0,     0,   218,   179,     0,     0,     0,     0,   126,   127,
     265,    24,    27,     0,    60,    26,   333,     0,     0,     0,
       0,     0,     0,     0,    58,     0,     0,    69,    70,     0,
       0,     0,   192,   191,     0,     0,     0,     0,   371,    53,
     125,   124,   173,     0,   329,     0,     0,     0,     0,     0,
       0,     0,    73,    66,    68,    65,   448,   448,   184,   183,
     179,   216,    23,     0,   332,   331,   330,     0,   179,   175,
     174,     0,     0,     0,     0,     0,     0,    52,     0,     0,
     263,   262,    72,   179,   179,   217,   179,   182,     0,     0,
       0,   295,   294,   163
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -627,  -627,  -627,   856,  -627,  -627,  -627,   927,   752,  -627,
    -302,   643,  -388,  -627,  -627,  -183,  -627,  1035,  -627,  -627,
    -627,  -627,   625,   768,   628,  -627,  -119,  -627,  -627,  -627,
    -627,  -627,   677,   680,   459,  -627,  -148,  -627,  -418,  -627,
    1057,  -627,  -627,   603,  -627,  -627,  -116,  -627,  1232,  -627,
    1233,  -627,   801,   775,  -627,   931,  -627,  -627,  -627,  -627,
     785,  -627,  -152,  -161,   844,  -627,  -627,  -627,  -627,   434,
    -626,  -627,  -627,  -627,  -627,  -627,   409,  1097,  -627,   419,
    -627,   -66,   -99,  -285,  -627,  -627,   637,  -627,   557,   556,
    1049,   675,  -627,  -627,   960,  -627,  -627,  -627,  -627,  1216,
    -627,  -627,  -627,  -627,   -70,  -627,  -627,  -627,  -627,  1173,
    -627,  -627,  1181,  -627,  -627,  -627,  -627,   116,  -627,  -627,
    1052,  -627,   460,  -627,  -198,  -627,   862,  -627,  -627,   522,
    -627,  -627,  -627,  -627,  -627,  -627,  -225,  -627,  -627,  -286,
     878,  -627,  -627,   -48,  -627,  1221,  -627,   706,   435,  -627,
    -627,  -365,   995,  -627,   881,   500,  -627,  -627,  -219,  1019,
    -627,   873,  -627,  -627,  -112,   204,  1138,  -627,  -627,  -627,
     866,    73,  -627,   214,  -153,  1104,  -362,   273,  -627,    37,
    -145,  -627,  -627,  -627,   874,  -487,  -627,  -627,  -396,  -627,
     649,  -627,   867,  1089,   709,   561,    22,  -627
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,   375,    26,   103,    27,   810,   402,   596,
     272,   465,   466,   764,   765,   637,   803,   197,   198,   199,
     273,   610,   611,   335,   336,   638,    74,   653,   431,   714,
     221,   432,   433,   434,   816,   819,   817,   435,   436,    28,
     182,   183,   184,   639,   640,   641,    75,     7,     8,     9,
      10,    11,   371,   337,   338,   322,   323,   799,   800,    29,
     326,   327,   240,   543,   242,   480,   485,   487,   489,   243,
     131,   508,   509,   437,    76,   438,   811,   101,   102,   730,
     731,   188,   141,   274,   440,   275,   695,   696,   697,   754,
     314,   455,   456,   716,   189,   644,   190,    12,    30,    16,
      17,   276,   419,   420,   244,   118,   666,    31,    32,    87,
      88,    89,    77,    78,    79,    33,    34,   145,   160,   147,
     148,   774,   775,   119,   245,    35,   278,   279,    80,   742,
     743,   744,   442,   443,   570,   667,   246,   450,   222,   247,
     280,   281,    36,   248,   121,    20,    21,   444,   806,   807,
     282,   283,   284,   517,   518,   249,   250,   646,   251,   252,
     376,   285,   286,    90,    91,    81,   132,   133,   134,   512,
     513,    82,   287,    83,   214,   227,   253,    84,   223,    85,
      13,   552,   553,   288,   289,   135,   634,   290,   662,   545,
     546,   659,    96,    99,   153,   151,   567,   136
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     143,   295,   109,   117,   207,   430,   241,   241,   216,   241,
     544,   565,   530,   504,   340,   762,   254,    57,   256,    51,
     341,   728,   439,   163,   137,   120,   129,    69,   458,   627,
     200,   330,   331,   201,   217,   691,   510,   202,     3,   539,
     663,   315,   741,   218,    14,   -25,   664,   228,   559,   229,
     -25,    39,   208,   207,   392,   467,   229,    42,   316,   628,
     729,    70,   384,   177,   481,   483,   484,   486,   488,   490,
     511,    18,    47,   468,   258,   259,   260,   166,   339,   200,
     104,    49,   201,   107,   113,   130,   202,   383,   449,   166,
    -173,   115,    93,   447,   451,   449,   228,    73,   229,   138,
    -450,   451,   117,   386,   387,   389,   230,   391,   394,   277,
     228,   219,   334,   230,   292,   520,   108,   741,   609,   164,
     125,    52,   763,   176,   257,    93,   187,   430,    92,    58,
     305,   482,   482,   482,   482,   482,   482,   482,   482,   482,
     482,   482,   482,   601,   439,   497,   498,   499,   500,   501,
     502,   479,   503,   324,   453,   230,   613,   615,   116,   329,
     329,    92,    15,   647,  -173,   231,   232,   233,   234,   235,
     236,   241,   231,   232,   233,   234,   235,   236,   220,   185,
     329,   745,   471,   384,   212,   521,   482,   237,   238,    19,
      48,   239,   228,   105,   237,   238,   363,   393,   239,    50,
     385,   108,   114,   423,   228,   224,   229,   698,    43,   116,
     166,   139,   277,   815,   231,   232,   233,   234,   235,   236,
     185,   291,   204,   514,   300,   734,   158,   159,   231,   232,
     233,   234,   235,   236,   526,   527,   528,   441,   166,   241,
     239,   108,   329,   591,   108,   340,   548,   846,   531,   592,
     304,   341,    46,   839,   239,   861,   745,   324,     5,   550,
     704,   403,   193,   230,   541,   166,   561,   555,   551,    53,
     370,   204,   558,   312,   329,   329,   329,   329,   404,   329,
     329,   329,   329,   329,   329,   329,   329,   329,   329,   329,
     329,   329,   574,   711,   329,   329,   329,   329,   329,   329,
     166,   329,   213,   384,   582,   329,   589,   584,   542,   339,
     116,   684,   685,   686,   687,   579,     6,   225,   350,   563,
     385,   226,   231,   232,   233,   234,   235,   236,   843,   264,
     166,   277,   736,  -416,   277,   329,    54,    86,  -416,   108,
     405,   313,   108,   384,   237,   238,   571,   620,   239,   564,
     395,   672,   712,   203,   671,   166,   661,   451,   229,   441,
     385,    15,   737,   205,   750,   794,   351,  -240,   108,   352,
     166,   681,   808,   681,    69,   353,   228,   396,   482,   482,
     482,   482,   468,   332,   591,   783,   155,   156,   787,  -181,
     592,  -181,   565,   791,   268,   532,   649,   776,   702,   229,
      22,   209,   203,   515,   451,   713,    55,   635,    70,  -224,
       6,   650,   205,    23,   605,   230,   541,   821,   651,   842,
     823,   397,   206,    24,   840,   680,   165,   354,   355,   167,
    -180,  -169,   170,   241,   427,   329,   166,   428,  -224,   808,
     100,  -225,   683,   678,    73,   229,  -180,  -180,  -180,    19,
     632,   277,   277,   862,   738,   642,   230,   541,   398,    25,
     542,    60,   166,   348,   559,   705,   706,  -266,   430,    62,
    -169,   206,   709,   670,   231,   232,   233,   234,   235,   236,
     155,   262,   720,   560,   721,   439,    64,   228,   524,   229,
     727,   427,   656,   166,   652,   506,   237,   238,   166,   665,
     239,   542,   230,   329,   426,    61,   679,    40,   430,  -180,
      41,   507,   344,   533,   264,   231,   232,   233,   234,   235,
     236,   747,   228,    94,   229,   439,   586,   329,   329,   329,
     329,   384,   228,   752,   229,   430,    63,   237,   238,   345,
     332,   239,    44,   476,   809,    45,   230,   541,   385,   577,
     451,   171,   439,   477,   172,   277,    65,   796,   166,   166,
     733,   231,   232,   233,   234,   235,   236,   797,   551,   167,
     715,     4,   562,   778,  -181,   779,  -181,   834,   724,  -106,
     835,   230,   802,   237,   238,   478,   563,   239,   388,    97,
     229,   230,  -106,   630,   734,    95,   264,   855,   448,   798,
     229,    98,  -106,   166,   759,   231,   232,   233,   234,   235,
     236,   100,   346,   347,    -3,     4,  -167,   399,   400,   427,
       5,   401,   428,  -106,   735,   463,   106,   237,   238,  -266,
     406,   239,   407,   298,   166,   299,  -106,   416,  -106,   417,
     231,   232,   233,   234,   235,   236,  -106,   230,   155,   297,
     231,   232,   233,   234,   235,   236,  -181,   230,  -181,   418,
     342,   343,   237,   238,     5,   452,   239,   229,   844,   780,
     844,   110,   237,   238,   111,   856,   239,   448,     6,   229,
     277,   112,  -106,   277,   124,   173,   363,   174,   277,   175,
     372,   373,   374,   228,   827,   229,   804,   377,   343,   126,
     441,   427,   872,   125,   428,   140,   231,   232,   233,   234,
     235,   236,   277,   378,   343,   277,   231,   232,   233,   234,
     235,   236,     6,   529,   230,   229,   830,   831,   237,   238,
     128,   547,   239,   229,   461,   462,   230,   146,   237,   238,
     441,   179,   239,   155,   412,   137,   364,   365,   155,   425,
     142,   866,   230,   366,   367,   368,   369,   155,   460,   869,
     370,   144,   364,   365,   149,   356,   357,   441,   358,   366,
     367,   368,   369,   150,   878,   879,   152,   880,   472,   473,
     474,   475,   230,   231,   232,   233,   234,   235,   236,   -47,
     230,   359,   360,   361,   154,   454,   232,   233,   234,   235,
     236,   469,   470,  -137,  -137,   237,   238,   -47,   157,   239,
     195,   464,   232,   233,   234,   235,   236,   237,   238,   575,
     576,   239,   399,   400,   -42,   580,   581,   587,   588,   161,
      67,   162,    68,   237,   238,   621,   588,   239,   169,   622,
     343,   464,   232,   233,   234,   235,   236,    69,   167,   231,
     232,   233,   234,   235,   236,   408,   491,   492,   493,   494,
     495,   496,   168,   237,   238,   264,   180,   239,   864,   865,
     408,   237,   238,   195,   265,   239,   105,   748,   343,   181,
     264,    70,   186,  -336,  -336,  -336,    73,   -41,   191,   265,
    -181,   860,   588,    67,   192,    68,   194,   266,  -337,  -337,
    -337,   211,   196,   108,   614,  -181,   215,  -181,   213,    71,
      69,   261,   266,    72,   264,   267,   294,     6,   296,   822,
     871,   588,  -181,   265,   301,   307,   309,   303,   410,   264,
     267,   308,  -337,  -337,  -337,   311,   317,   268,   265,  -181,
     269,   318,   319,   325,    70,   320,   266,  -337,  -337,  -337,
     328,   333,   268,   166,  -181,   269,  -181,   332,   349,   170,
     379,   266,   380,   381,   267,   196,   382,   270,  -336,  -181,
     390,  -181,    71,   271,   411,   408,    72,   413,   414,   267,
       6,   415,   270,   424,  -181,   264,   268,   422,   271,   269,
     263,    66,   445,   446,   265,   457,   519,   522,   515,   525,
     264,   268,   538,   537,   269,  -337,     6,   556,   557,   265,
    -181,    67,   566,    68,   568,   569,   270,   266,  -181,   572,
    -337,  -251,   271,   578,   408,  -181,   478,  -181,    69,   583,
     594,   270,   266,  -181,   264,   267,   597,   271,   598,   786,
     599,   122,  -181,   265,   600,   604,   602,   603,   606,   264,
     267,   607,   608,   609,  -337,   612,   616,   268,   265,  -181,
     269,    67,    70,    68,   617,   618,   266,   623,   624,  -337,
     625,  -250,   268,   619,  -181,   269,  -181,   629,    69,   626,
     631,   266,   633,   636,   267,   643,   645,   270,  -337,  -181,
      71,  -181,   648,   271,    72,   820,   658,   660,    73,   267,
     363,   669,   270,   674,  -181,   264,   268,   673,   271,   269,
     675,   677,    70,   681,   265,   688,   689,   703,   694,   693,
     700,   268,   718,   701,   269,  -337,   -13,   708,   710,   719,
    -181,   722,   723,   725,   726,   732,   270,   266,  -181,   740,
      71,   746,   271,   739,    72,   579,  -266,  -181,    73,   586,
     751,   270,   749,  -181,   753,   267,   756,   271,   758,   760,
     364,   365,   761,   155,   768,   767,   769,   366,   367,   368,
     369,   770,   772,   766,   370,   771,   773,   268,   777,   782,
     269,   785,   788,   790,   793,   795,   784,   789,   792,   801,
     805,   813,   815,   812,   814,   828,   824,   826,   825,   833,
     836,   829,   838,   841,   845,   847,   837,   270,   858,  -181,
     848,   850,   729,   271,   849,   851,   853,   863,   868,   859,
     867,   870,   873,   874,   876,   875,   877,   881,   882,   883,
     595,   682,   505,   306,   429,   692,   590,   690,   818,   654,
     293,    37,   655,   717,    38,   593,   857,   585,   573,   540,
     210,   852,   699,   755,   459,   757,   676,    56,   421,   123,
     832,   127,   310,   534,   516,   781,    59,   321,   657,   362,
     854,   178,   523,   255,   535,   536,   549,   409,   707,   668,
       0,   554,     0,   302
};

static const yytype_int16 yycheck[] =
{
      99,   184,    68,    73,   149,   307,   167,   168,   161,   170,
     406,   429,   400,   375,   239,     1,   168,     7,   170,    17,
     239,     1,   307,    48,    48,    73,    35,    38,    31,   114,
     149,   229,   230,   149,     1,    31,    38,   149,     0,   404,
       1,   127,   668,    10,     1,    31,     7,     1,   125,     3,
      36,   144,   151,   198,     1,   131,     3,   144,   144,   144,
      40,    72,   127,   129,   350,   351,   352,   353,   354,   355,
      72,     1,     1,   149,   173,   174,   175,   154,   239,   198,
      31,     1,   198,     1,     1,    94,   198,    31,   313,   154,
      70,     1,    55,   312,   313,   320,     1,   108,     3,     7,
     109,   320,   172,   264,   265,   266,    60,   268,   269,   179,
       1,    78,    66,    60,   180,    31,   119,   743,   114,   144,
     144,   119,   108,     1,   172,    88,     1,   429,    55,   119,
     196,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   505,   429,   364,   365,   366,   367,   368,
     369,   349,   371,   219,   315,    60,   521,   522,   119,   229,
     230,    88,   119,   559,   144,   119,   120,   121,   122,   123,
     124,   332,   119,   120,   121,   122,   123,   124,   145,   142,
     250,   668,   343,   127,     1,   101,   405,   141,   142,   119,
     119,   145,     1,   144,   141,   142,    28,   144,   145,   119,
     144,   119,   119,   302,     1,     7,     3,    31,   154,   119,
     154,   119,   282,    29,   119,   120,   121,   122,   123,   124,
     183,     1,   149,   384,     1,   114,   110,   111,   119,   120,
     121,   122,   123,   124,   395,   396,   397,   307,   154,   400,
     145,   119,   312,   468,   119,   470,   407,    31,   400,   468,
       1,   470,   119,     1,   145,     1,   743,   323,    50,   412,
     625,    36,   146,    60,    61,   154,   427,   420,   413,    62,
     102,   198,   425,    77,   344,   345,   346,   347,    53,   349,
     350,   351,   352,   353,   354,   355,   356,   357,   358,   359,
     360,   361,   146,     7,   364,   365,   366,   367,   368,   369,
     154,   371,   119,   127,   457,   375,   467,   460,   105,   470,
     119,   597,   598,   599,   600,    77,   108,   119,     8,     1,
     144,   123,   119,   120,   121,   122,   123,   124,   144,    11,
     154,   401,   114,    31,   404,   405,    48,     1,    36,   119,
     115,   145,   119,   127,   141,   142,   445,   146,   145,    31,
      36,   576,    66,   149,   573,   154,     1,   576,     3,   429,
     144,   119,   144,   149,   131,   761,    56,    31,   119,    59,
     154,   119,   768,   119,    38,    65,     1,    63,   597,   598,
     599,   600,   149,   145,   609,   750,   147,   148,   753,    71,
     609,    73,   810,   758,    83,     1,    21,     1,   623,     3,
       9,     1,   198,    92,   623,   119,    48,   552,    72,   125,
     108,    36,   198,    22,   513,    60,    61,   782,    43,   815,
     785,   107,   149,    32,   812,   586,   144,   117,   118,   145,
      36,    31,   145,   594,   116,   505,   154,   119,   154,   835,
      40,   154,   594,     1,   108,     3,    71,    53,    73,   119,
     549,   521,   522,   841,   146,   554,    60,    61,   144,    68,
     105,    31,   154,   150,   125,   626,   627,   154,   770,    31,
      70,   198,   633,   572,   119,   120,   121,   122,   123,   124,
     147,   148,   643,   144,   645,   770,   119,     1,   144,     3,
     651,   116,   562,   154,   119,    48,   141,   142,   154,   569,
     145,   105,    60,   573,     1,    62,    64,   144,   810,   115,
     147,    64,    55,   119,    11,   119,   120,   121,   122,   123,
     124,   674,     1,   154,     3,   810,   131,   597,   598,   599,
     600,   127,     1,   694,     3,   837,    48,   141,   142,    82,
     145,   145,   144,    77,   769,   147,    60,    61,   144,   144,
     769,   144,   837,    87,   147,   625,   119,    22,   154,   154,
     659,   119,   120,   121,   122,   123,   124,    32,   713,   145,
     636,     1,   148,   734,    71,   736,    73,   144,   648,     9,
     147,    60,   765,   141,   142,   119,     1,   145,     1,   119,
       3,    60,    22,   144,   114,    22,    11,    66,     1,    64,
       3,    32,    32,   154,   703,   119,   120,   121,   122,   123,
     124,    40,   155,   156,     0,     1,    31,   144,   145,   116,
      50,   148,   119,     9,   144,   150,   119,   141,   142,   154,
     125,   145,   127,   144,   154,   146,    22,    45,    68,    47,
     119,   120,   121,   122,   123,   124,    32,    60,   147,   148,
     119,   120,   121,   122,   123,   124,    71,    60,    73,    67,
     146,   147,   141,   142,    50,     1,   145,     3,   816,   739,
     818,   119,   141,   142,   119,   836,   145,     1,   108,     3,
     750,   119,    68,   753,    31,   124,    28,   126,   758,   128,
     141,   142,   143,     1,   793,     3,   766,   146,   147,    31,
     770,   116,   863,   144,   119,   119,   119,   120,   121,   122,
     123,   124,   782,   146,   147,   785,   119,   120,   121,   122,
     123,   124,   108,     1,    60,     3,   796,   797,   141,   142,
      31,     1,   145,     3,   146,   147,    60,    70,   141,   142,
     810,    15,   145,   147,   148,    48,    88,    89,   147,   148,
      48,   850,    60,    95,    96,    97,    98,   147,   148,   858,
     102,   145,    88,    89,    48,   125,   126,   837,   128,    95,
      96,    97,    98,    68,   873,   874,    48,   876,   344,   345,
     346,   347,    60,   119,   120,   121,   122,   123,   124,   131,
      60,   151,   152,   153,   144,   119,   120,   121,   122,   123,
     124,   146,   147,   146,   147,   141,   142,   149,   145,   145,
       1,   119,   120,   121,   122,   123,   124,   141,   142,   146,
     147,   145,   144,   145,    15,   146,   147,   146,   147,    48,
      21,   144,    23,   141,   142,   146,   147,   145,   154,   146,
     147,   119,   120,   121,   122,   123,   124,    38,   145,   119,
     120,   121,   122,   123,   124,     1,   356,   357,   358,   359,
     360,   361,   145,   141,   142,    11,   109,   145,   846,   847,
       1,   141,   142,     1,    20,   145,   144,   146,   147,   144,
      11,    72,   144,    29,    30,    31,   108,    15,   144,    20,
      36,   146,   147,    21,   145,    23,    31,    43,    29,    30,
      31,   119,    93,   119,     1,    36,    86,    53,   119,   100,
      38,   144,    43,   104,    11,    61,    36,   108,   146,     1,
     146,   147,    53,    20,   144,    15,    31,   144,    31,    11,
      61,   144,    29,    30,    31,   144,   119,    83,    20,    36,
      86,   144,   144,   119,    72,   145,    43,    29,    30,    31,
     144,   119,    83,   154,    36,    86,    53,   145,   130,   145,
     144,    43,   144,   144,    61,    93,   119,   113,   114,   115,
     144,    53,   100,   119,   144,     1,   104,   119,    31,    61,
     108,   144,   113,   144,   115,    11,    83,   146,   119,    86,
       1,     1,    21,    31,    20,    62,    48,   101,    92,   144,
      11,    83,   119,   144,    86,    31,   108,   144,   144,    20,
      36,    21,    71,    23,   125,   145,   113,    43,   115,    21,
      31,    31,   119,   144,     1,    36,   119,    53,    38,    78,
     145,   113,    43,   115,    11,    61,     8,   119,    65,     1,
     117,     1,    53,    20,   118,   144,   122,   119,   144,    11,
      61,   119,   144,   114,    31,    43,   144,    83,    20,    36,
      86,    21,    72,    23,   144,   144,    43,    45,    31,    31,
      53,    31,    83,   144,    36,    86,    53,   147,    38,     5,
     144,    43,   127,    36,    61,   127,   127,   113,   114,   115,
     100,    53,    90,   119,   104,     1,     9,    73,   108,    61,
      28,   144,   113,    62,   115,    11,    83,   129,   119,    86,
     119,   144,    72,   119,    20,    48,   144,    53,    30,   144,
     144,    83,    31,   144,    86,    31,   144,   144,   144,   144,
      36,   144,   144,   119,   119,   125,   113,    43,   115,   146,
     100,   144,   119,   147,   104,    77,   154,    53,   108,   131,
      20,   113,   122,   115,    29,    61,    43,   119,    53,    31,
      88,    89,    29,   147,   125,    36,    45,    95,    96,    97,
      98,    39,    54,   148,   102,   144,    70,    83,   144,    15,
      86,   101,    31,    31,    53,    31,   144,   144,   144,   144,
     144,    54,    29,   145,   144,   144,    43,    31,    43,    31,
     114,    36,    31,   145,   144,    31,    39,   113,    39,   115,
     144,    53,    40,   119,   144,   144,    36,   114,    31,   146,
     144,   146,    73,    73,    39,   144,   144,   144,   144,   144,
     478,   588,   376,   198,   307,   610,   468,   609,   779,   562,
     183,     9,   562,   640,    11,   470,   837,   462,   447,   405,
     153,   832,   615,   696,   323,   699,   581,    41,   298,    78,
     800,    88,   210,   401,   386,   743,    45,   218,   562,   250,
     835,   133,   391,   169,   401,   401,   410,   282,   629,   570,
      -1,   414,    -1,   194
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   158,   159,     0,     1,    50,   108,   204,   205,   206,
     207,   208,   254,   337,     1,   119,   256,   257,     1,   119,
     302,   303,     9,    22,    32,    68,   161,   163,   196,   216,
     255,   264,   265,   272,   273,   282,   299,   205,   207,   144,
     144,   147,   144,   154,   144,   147,   119,     1,   119,     1,
     119,    17,   119,    62,    48,    48,   256,     7,   119,   302,
      31,    62,    31,    48,   119,   119,     1,    21,    23,    38,
      72,   100,   104,   108,   183,   203,   231,   269,   270,   271,
     285,   322,   328,   330,   334,   336,     1,   266,   267,   268,
     320,   321,   328,   336,   154,    22,   349,   119,    32,   350,
      40,   234,   235,   162,    31,   144,   119,     1,   119,   238,
     119,   119,   119,     1,   119,     1,   119,   261,   262,   280,
     300,   301,     1,   269,    31,   144,    31,   266,    31,    35,
      94,   227,   323,   324,   325,   342,   354,    48,     7,   119,
     119,   239,    48,   239,   145,   274,    70,   276,   277,    48,
      68,   352,    48,   351,   144,   147,   148,   145,   274,   274,
     275,    48,   144,    48,   144,   144,   154,   145,   145,   154,
     145,   144,   147,   352,   352,   352,     1,   238,   323,    15,
     109,   144,   197,   198,   199,   336,   144,     1,   238,   251,
     253,   144,   145,   274,    31,     1,    93,   174,   175,   176,
     183,   203,   321,   322,   328,   330,   334,   337,   239,     1,
     234,   119,     1,   119,   331,    86,   331,     1,    10,    78,
     145,   187,   295,   335,     7,   119,   123,   332,     1,     3,
      60,   119,   120,   121,   122,   123,   124,   141,   142,   145,
     219,   220,   221,   226,   261,   281,   293,   296,   300,   312,
     313,   315,   316,   333,   219,   332,   219,   300,   239,   239,
     239,   144,   148,     1,    11,    20,    43,    61,    83,    86,
     113,   119,   167,   177,   240,   242,   258,   261,   283,   284,
     297,   298,   307,   308,   309,   318,   319,   329,   340,   341,
     344,     1,   238,   197,    36,   172,   146,   148,   144,   146,
       1,   144,   350,   144,     1,   238,   174,    15,   144,    31,
     277,   144,    77,   145,   247,   127,   144,   119,   144,   144,
     145,   247,   212,   213,   238,   119,   217,   218,   144,   261,
     281,   281,   145,   119,    66,   180,   181,   210,   211,   220,
     293,   315,   146,   147,    55,    82,   155,   156,   150,   130,
       8,    56,    59,    65,   117,   118,   125,   126,   128,   151,
     152,   153,   316,    28,    88,    89,    95,    96,    97,    98,
     102,   209,   141,   142,   143,   160,   317,   146,   146,   144,
     144,   144,   119,    31,   127,   144,   220,   220,     1,   220,
     144,   220,     1,   144,   220,    36,    63,   107,   144,   144,
     145,   148,   165,    36,    53,   115,   125,   127,     1,   309,
      31,   144,   148,   119,    31,   144,    45,    47,    67,   259,
     260,   251,   146,   239,   144,   148,     1,   116,   119,   164,
     167,   185,   188,   189,   190,   194,   195,   230,   232,   240,
     241,   261,   289,   290,   304,    21,    31,   315,     1,   293,
     294,   315,     1,   220,   119,   248,   249,    62,    31,   212,
     148,   146,   147,   150,   119,   168,   169,   131,   149,   146,
     147,   220,   226,   226,   226,   226,    77,    87,   119,   281,
     222,   296,   315,   296,   296,   223,   296,   224,   296,   225,
     296,   312,   312,   312,   312,   312,   312,   315,   315,   315,
     315,   315,   315,   315,   333,   160,    48,    64,   228,   229,
      38,    72,   326,   327,   220,    92,   297,   310,   311,    48,
      31,   101,   101,   311,   144,   144,   220,   220,   220,     1,
     169,   219,     1,   119,   283,   318,   341,   144,   119,   308,
     221,    61,   105,   220,   345,   346,   347,     1,   220,   327,
     331,   337,   338,   339,   349,   331,   144,   144,   331,   125,
     144,   220,   148,     1,    31,   195,    71,   353,   125,   145,
     291,   239,    21,   209,   146,   146,   147,   144,   144,    77,
     146,   147,   331,    78,   331,   217,   131,   146,   147,   220,
     180,   293,   315,   210,   145,   165,   166,     8,    65,   117,
     118,   333,   122,   119,   144,   239,   144,   119,   144,   114,
     178,   179,    43,   308,     1,   308,   144,   144,   144,   144,
     146,   146,   146,    45,    31,    53,     5,   114,   144,   147,
     144,   144,   239,   127,   343,   337,    36,   172,   182,   200,
     201,   202,   239,   127,   252,   127,   314,   345,    90,    21,
      36,    43,   119,   184,   189,   190,   261,   304,     9,   348,
      73,     1,   345,     1,     7,   261,   263,   292,   351,   144,
     239,   315,   293,   129,    62,   119,   248,   144,     1,    64,
     220,   119,   168,   219,   296,   296,   296,   296,    48,   144,
     181,    31,   179,   144,    30,   243,   244,   245,    31,   243,
     144,   144,   293,    53,   308,   220,   220,   347,   144,   220,
     144,     7,    66,   119,   186,   238,   250,   200,    31,   144,
     220,   220,   144,   144,   261,   119,   119,   220,     1,    40,
     236,   237,   125,   239,   114,   144,   114,   144,   146,   147,
     146,   227,   286,   287,   288,   342,   144,   331,   146,   122,
     131,    20,   220,    29,   246,   245,    43,   246,    53,   239,
      31,    29,     1,   108,   170,   171,   148,    36,   125,    45,
      39,   144,    54,    70,   278,   279,     1,   144,   220,   220,
     261,   286,    15,   308,   144,   101,     1,   308,    31,   144,
      31,   308,   144,    53,   345,    31,    22,    32,    64,   214,
     215,   144,   172,   173,   261,   144,   305,   306,   345,   293,
     164,   233,   145,    54,   144,    29,   191,   193,   191,   192,
       1,   308,     1,   308,    43,    43,    31,   239,   144,    36,
     261,   261,   279,    31,   144,   147,   114,    39,    31,     1,
     169,   145,   345,   144,   193,   144,    31,    31,   144,   144,
      53,   144,   236,    36,   305,    66,   220,   233,    39,   146,
     146,     1,   169,   114,   353,   353,   239,   144,    31,   239,
     146,   146,   220,    73,    73,   144,    39,   144,   239,   239,
     239,   144,   144,   144
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   157,   159,   158,   160,   160,   160,   162,   161,   163,
     164,   164,   164,   165,   166,   166,   167,   167,   168,   168,
     168,   169,   169,   170,   171,   171,   172,   173,   173,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   175,
     175,   176,   176,   177,   178,   178,   179,   180,   180,   180,
     181,   181,   182,   182,   183,   183,   184,   184,   185,   185,
     186,   187,   187,   187,   188,   189,   189,   190,   191,   191,
     192,   192,   193,   193,   194,   194,   194,   194,   194,   194,
     194,   194,   195,   195,   195,   195,   195,   196,   196,   197,
     198,   198,   199,   199,   200,   200,   201,   201,   202,   202,
     203,   203,   203,   203,   203,   204,   204,   205,   205,   206,
     206,   207,   207,   208,   208,   209,   209,   210,   210,   211,
     211,   212,   213,   213,   214,   214,   214,   215,   215,   216,
     216,   217,   218,   218,   219,   219,   220,   220,   221,   221,
     221,   221,   221,   221,   221,   222,   222,   223,   223,   224,
     224,   225,   225,   226,   226,   226,   226,   227,   227,   228,
     228,   229,   229,   230,   231,   232,   232,   233,   234,   234,
     235,   235,   236,   236,   237,   237,   238,   238,   239,   239,
     240,   240,   241,   242,   242,   242,   242,   243,   243,   244,
     244,   245,   245,   246,   246,   246,   247,   247,   248,   249,
     249,   250,   250,   250,   251,   252,   252,   253,   253,   254,
     254,   255,   255,   256,   257,   257,   258,   258,   258,   259,
     259,   259,   260,   260,   261,   261,   261,   261,   262,   262,
     263,   263,   264,   264,   265,   266,   266,   267,   267,   268,
     268,   269,   269,   269,   269,   269,   269,   269,   270,   270,
     271,   271,   272,   272,   273,   274,   275,   275,   276,   276,
     277,   277,   278,   278,   279,   279,   280,   281,   281,   281,
     281,   281,   281,   281,   281,   281,   281,   282,   282,   282,
     283,   283,   283,   283,   284,   284,   285,   286,   286,   287,
     287,   288,   288,   289,   290,   290,   291,   291,   291,   292,
     292,   293,   293,   293,   294,   294,   295,   296,   296,   296,
     296,   296,   296,   296,   297,   298,   298,   298,   299,   299,
     300,   300,   301,   301,   302,   302,   302,   303,   303,   304,
     305,   305,   306,   306,   307,   307,   308,   308,   309,   309,
     309,   309,   309,   309,   309,   309,   309,   309,   309,   309,
     310,   311,   311,   312,   312,   312,   312,   312,   312,   312,
     313,   313,   314,   314,   315,   315,   316,   316,   317,   317,
     318,   318,   319,   319,   320,   321,   321,   322,   323,   323,
     324,   324,   325,   325,   326,   326,   327,   327,   328,   328,
     329,   330,   331,   331,   331,   332,   332,   332,   333,   333,
     333,   333,   333,   334,   334,   334,   334,   335,   335,   336,
     336,   337,   337,   338,   338,   339,   339,   340,   340,   341,
     341,   341,   342,   342,   343,   343,   344,   344,   344,   344,
     345,   345,   346,   346,   347,   347,   347,   348,   348,   349,
     349,   350,   350,   351,   351,   352,   352,   353,   353,   354,
     354
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     0,    12,     2,
       2,     1,     2,     3,     1,     0,     3,     4,     3,     3,
       3,     3,     1,     4,     2,     0,     7,     1,     0,     6,
       1,     1,     1,     1,     1,     1,     1,     3,     2,     2,
       1,     1,     0,     7,     2,     1,     4,     1,     1,     1,
       3,     1,     7,     5,     9,     8,     1,     2,     6,     5,
       3,     4,     6,     1,     1,     7,     7,     4,     2,     1,
       1,     0,     4,     2,     1,     3,     1,     3,     1,     3,
       4,     4,     1,     1,     1,     1,     1,    11,     6,     1,
       2,     1,     1,     0,     1,     1,     2,     1,     1,     0,
       7,     5,     7,     5,     3,     1,     0,     1,     1,     2,
       1,     2,     1,     2,     1,     1,     1,     3,     1,     3,
       1,     4,     2,     1,     2,     2,     1,     1,     0,     9,
       6,     1,     3,     1,     3,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     2,     2,     6,     3,     4,
       2,     1,     0,    12,     5,     1,     1,     1,     1,     0,
       3,     5,     1,     0,     5,     5,     3,     1,     1,     0,
       2,     0,    10,     9,     9,     7,     5,     1,     0,     2,
       1,     4,     4,     2,     2,     0,     3,     3,     3,     3,
       1,     1,     1,     1,     5,     2,     0,     3,     1,     3,
       3,     1,     1,     1,     3,     1,     9,    11,     7,     1,
       1,     1,     1,     0,     1,     1,     1,     4,     4,     4,
       3,     1,     7,     7,     2,     1,     1,     2,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     2,     2,     1,
       1,     0,     7,     7,     3,     3,     1,     0,     3,     5,
       1,     0,     5,     5,     1,     0,     1,     1,     4,     1,
       1,     1,     1,     1,     2,     4,     3,     1,     1,     1,
       2,     5,     3,     5,     3,     1,     3,     1,     1,     2,
       1,     1,     0,     3,    11,    11,     3,     3,     0,     1,
       1,     3,     3,     3,     1,     3,     4,     1,     3,     3,
       3,     3,     3,     3,     4,     3,     2,     3,     1,     1,
       3,     3,     3,     1,     3,     5,     5,     3,     1,     7,
       3,     3,     3,     1,     2,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     1,     0,     1,     3,     3,     3,     3,     3,     3,
       1,     1,     2,     0,     2,     1,     1,     2,     2,     3,
       4,     8,     1,     3,     2,     8,     8,     2,     1,     1,
       2,     1,     1,     0,     1,     1,     1,     0,     1,     1,
       1,     5,     1,     2,     5,     1,     1,     1,     1,     3,
       3,     3,     3,     5,     3,     5,     3,     3,     1,     3,
       3,     3,     3,     2,     1,     1,     0,     1,     3,     4,
       4,     4,     7,     4,     2,     0,     4,     4,     4,     2,
       1,     1,     3,     1,     1,     3,     1,     1,     0,     1,
       0,     1,     0,     1,     0,     1,     0,     1,     0,     1,
       0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, yyscanner, file_path, parse_library_name, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, yyscanner, file_path, parse_library_name); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t yyscanner, const char*file_path, perm_string parse_library_name)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (yyscanner);
  YYUSE (file_path);
  YYUSE (parse_library_name);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, yyscan_t yyscanner, const char*file_path, perm_string parse_library_name)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, yyscanner, file_path, parse_library_name);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, yyscan_t yyscanner, const char*file_path, perm_string parse_library_name)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , yyscanner, file_path, parse_library_name);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, yyscanner, file_path, parse_library_name); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, yyscan_t yyscanner, const char*file_path, perm_string parse_library_name)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (yyscanner);
  YYUSE (file_path);
  YYUSE (parse_library_name);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t yyscanner, const char*file_path, perm_string parse_library_name)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, yyscanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 391 "parse.y" /* yacc.c:1646  */
    { yylloc.text = file_path; }
#line 2529 "parse.cc" /* yacc.c:1646  */
    break;

  case 4:
#line 394 "parse.y" /* yacc.c:1646  */
    { (yyval.arithmetic_op) = ExpArithmetic::PLUS; }
#line 2535 "parse.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 395 "parse.y" /* yacc.c:1646  */
    { (yyval.arithmetic_op) = ExpArithmetic::MINUS; }
#line 2541 "parse.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 396 "parse.y" /* yacc.c:1646  */
    { (yyval.arithmetic_op) = ExpArithmetic::xCONCAT; }
#line 2547 "parse.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 405 "parse.y" /* yacc.c:1646  */
    { bind_entity_to_active_scope((yyvsp[0].text), active_scope); }
#line 2553 "parse.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 409 "parse.y" /* yacc.c:1646  */
    { Architecture*tmp = new Architecture(lex_strings.make((yyvsp[-11].text)),
					    *active_scope, *(yyvsp[-4].arch_statement_list));
	FILE_NAME(tmp, (yylsp[-11]));
	bind_architecture_to_entity((yyvsp[-9].text), tmp);
	if ((yyvsp[-1].text) && tmp->get_name() != (yyvsp[-1].text))
	      errormsg((yylsp[-11]), "Architecture name doesn't match closing name.\n");
	delete[](yyvsp[-11].text);
	delete[](yyvsp[-9].text);
	delete (yyvsp[-4].arch_statement_list);
	pop_scope();
	assert(arc_scope);
	arc_scope = NULL;
	if ((yyvsp[-1].text)) delete[](yyvsp[-1].text);
      }
#line 2572 "parse.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 427 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text);
	push_scope();
	assert(!arc_scope);
	arc_scope = active_scope;
      }
#line 2582 "parse.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 439 "parse.y" /* yacc.c:1646  */
    { std::list<Architecture::Statement*>*tmp = (yyvsp[-1].arch_statement_list);
	if ((yyvsp[0].arch_statement)) tmp->push_back((yyvsp[0].arch_statement));
	(yyval.arch_statement_list) = tmp;
      }
#line 2591 "parse.cc" /* yacc.c:1646  */
    break;

  case 11:
#line 444 "parse.y" /* yacc.c:1646  */
    { std::list<Architecture::Statement*>*tmp = new std::list<Architecture::Statement*>;
	if ((yyvsp[0].arch_statement)) tmp->push_back((yyvsp[0].arch_statement));
	(yyval.arch_statement_list) = tmp;
      }
#line 2600 "parse.cc" /* yacc.c:1646  */
    break;

  case 12:
#line 450 "parse.y" /* yacc.c:1646  */
    { (yyval.arch_statement_list) = 0;
	errormsg((yylsp[-1]), "Syntax error in architecture statement.\n");
	yyerrok;
      }
#line 2609 "parse.cc" /* yacc.c:1646  */
    break;

  case 13:
#line 456 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[-1].expr_list); }
#line 2615 "parse.cc" /* yacc.c:1646  */
    break;

  case 14:
#line 459 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[0].expr_list); }
#line 2621 "parse.cc" /* yacc.c:1646  */
    break;

  case 15:
#line 460 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = 0; }
#line 2627 "parse.cc" /* yacc.c:1646  */
    break;

  case 16:
#line 465 "parse.y" /* yacc.c:1646  */
    { ReportStmt*report = dynamic_cast<ReportStmt*>((yyvsp[0].sequ));
        assert(report);
	AssertStmt*tmp = new AssertStmt((yyvsp[-1].expr), report->message(), report->severity());
        delete report;
	FILE_NAME(tmp,(yylsp[-1]));
	(yyval.sequ) = tmp;
      }
#line 2639 "parse.cc" /* yacc.c:1646  */
    break;

  case 17:
#line 473 "parse.y" /* yacc.c:1646  */
    { AssertStmt*tmp = new AssertStmt((yyvsp[-2].expr), NULL, (yyvsp[-1].severity));
	FILE_NAME(tmp,(yylsp[-2]));
	(yyval.sequ) = tmp;
      }
#line 2648 "parse.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 481 "parse.y" /* yacc.c:1646  */
    { named_expr_t*tmp = new named_expr_t(lex_strings.make((yyvsp[-2].text)), (yyvsp[0].expr));
	delete[](yyvsp[-2].text);
	(yyval.named_expr) = tmp;
      }
#line 2657 "parse.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 486 "parse.y" /* yacc.c:1646  */
    { named_expr_t*tmp = new named_expr_t(lex_strings.make((yyvsp[-2].text)), 0);
	delete[](yyvsp[-2].text);
	(yyval.named_expr) = tmp;
      }
#line 2666 "parse.cc" /* yacc.c:1646  */
    break;

  case 20:
#line 491 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[0]), "Invalid target for port map association.\n");
	yyerrok;
	(yyval.named_expr) = 0;
      }
#line 2675 "parse.cc" /* yacc.c:1646  */
    break;

  case 21:
#line 499 "parse.y" /* yacc.c:1646  */
    { std::list<named_expr_t*>*tmp = (yyvsp[-2].named_expr_list);
	tmp->push_back((yyvsp[0].named_expr));
	(yyval.named_expr_list) = tmp;
      }
#line 2684 "parse.cc" /* yacc.c:1646  */
    break;

  case 22:
#line 504 "parse.y" /* yacc.c:1646  */
    { std::list<named_expr_t*>*tmp = new std::list<named_expr_t*>;
	tmp->push_back((yyvsp[0].named_expr));
	(yyval.named_expr_list) = tmp;
      }
#line 2693 "parse.cc" /* yacc.c:1646  */
    break;

  case 23:
#line 512 "parse.y" /* yacc.c:1646  */
    { (yyval.entity_aspect) = (yyvsp[-2].entity_aspect);
	if ((yyvsp[-1].named_expr_list)) sorrymsg((yylsp[-1]), "Port map aspect not supported here. (binding_indication)\n");
	if ((yyvsp[0].named_expr_list)) sorrymsg((yylsp[0]), "Generic map aspect not supported here. (binding_indication)\n");
	delete (yyvsp[-1].named_expr_list);
	delete (yyvsp[0].named_expr_list);
      }
#line 2704 "parse.cc" /* yacc.c:1646  */
    break;

  case 24:
#line 521 "parse.y" /* yacc.c:1646  */
    { (yyval.entity_aspect) = (yyvsp[-1].entity_aspect); }
#line 2710 "parse.cc" /* yacc.c:1646  */
    break;

  case 25:
#line 522 "parse.y" /* yacc.c:1646  */
    { (yyval.entity_aspect) = 0; }
#line 2716 "parse.cc" /* yacc.c:1646  */
    break;

  case 26:
#line 530 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-5].text); }
#line 2722 "parse.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 541 "parse.y" /* yacc.c:1646  */
    { /* Save the signal declaration in the block_signals map. */
	for (std::list<perm_string>::iterator cur = (yyvsp[-4].name_list)->begin()
		   ; cur != (yyvsp[-4].name_list)->end() ; ++cur) {
	      Signal*sig = new Signal(*cur, (yyvsp[-2].vtype), (yyvsp[-1].expr) ? (yyvsp[-1].expr)->clone() : 0);
	      FILE_NAME(sig, (yylsp[-5]));
	      active_scope->bind_name(*cur, sig);
	}
	delete (yyvsp[-4].name_list);
      }
#line 2736 "parse.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 568 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error declaring signals.\n"); yyerrok; }
#line 2742 "parse.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 570 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in block declarations.\n"); yyerrok; }
#line 2748 "parse.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 593 "parse.y" /* yacc.c:1646  */
    { CaseSeqStmt* tmp = new CaseSeqStmt((yyvsp[-5].expr), (yyvsp[-3].case_alt_list));
	FILE_NAME(tmp, (yylsp[-6]));
	delete (yyvsp[-3].case_alt_list);
	(yyval.sequ) = tmp;
      }
#line 2758 "parse.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 602 "parse.y" /* yacc.c:1646  */
    { std::list<CaseSeqStmt::CaseStmtAlternative*>* tmp = (yyvsp[-1].case_alt_list);
	tmp->push_back((yyvsp[0].case_alt));
	(yyval.case_alt_list) = tmp;
      }
#line 2767 "parse.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 607 "parse.y" /* yacc.c:1646  */
    { std::list<CaseSeqStmt::CaseStmtAlternative*>*tmp
		  = new std::list<CaseSeqStmt::CaseStmtAlternative*>();
	tmp->push_back((yyvsp[0].case_alt));
	(yyval.case_alt_list) = tmp;
      }
#line 2777 "parse.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 622 "parse.y" /* yacc.c:1646  */
    { CaseSeqStmt::CaseStmtAlternative* tmp;
        std::list<ExpAggregate::choice_t*>*choices = (yyvsp[-2].choice_list);
        std::list<Expression*>*exp_list = new std::list<Expression*>;
        bool others = false;

        for(std::list<ExpAggregate::choice_t*>::iterator it = choices->begin();
                it != choices->end(); ++it) {
            if((*it)->others() || others)
                // If there is one "others", then it also covers all other alternatives
                // Continue the loop to delete every choice_t, but do not
                // bother to add the expressions to the exp_list (we are going to
                // delete them very soon)
                others = true;
            else
                exp_list->push_back((*it)->simple_expression());

            delete (*it);
        }

        if(others) {
            tmp = new CaseSeqStmt::CaseStmtAlternative(0, (yyvsp[0].sequ_list));
            for(std::list<Expression*>::iterator it = exp_list->begin();
                    it != exp_list->end(); ++it) {
                delete (*it);
            }
        } else {
            tmp = new CaseSeqStmt::CaseStmtAlternative(exp_list, (yyvsp[0].sequ_list));
        }
        if (tmp) FILE_NAME(tmp, (yylsp[-3]));

        delete choices;
        delete (yyvsp[0].sequ_list);
        (yyval.case_alt) = tmp;
      }
#line 2816 "parse.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 660 "parse.y" /* yacc.c:1646  */
    { (yyval.choice) = new ExpAggregate::choice_t((yyvsp[0].expr));}
#line 2822 "parse.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 662 "parse.y" /* yacc.c:1646  */
    { (yyval.choice) = new ExpAggregate::choice_t; }
#line 2828 "parse.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 664 "parse.y" /* yacc.c:1646  */
    { (yyval.choice) = new ExpAggregate::choice_t((yyvsp[0].range)); }
#line 2834 "parse.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 669 "parse.y" /* yacc.c:1646  */
    { std::list<ExpAggregate::choice_t*>*tmp = (yyvsp[-2].choice_list);
	tmp->push_back((yyvsp[0].choice));
	(yyval.choice_list) = tmp;
      }
#line 2843 "parse.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 674 "parse.y" /* yacc.c:1646  */
    { std::list<ExpAggregate::choice_t*>*tmp = new std::list<ExpAggregate::choice_t*>;
	tmp->push_back((yyvsp[0].choice));
	(yyval.choice_list) = tmp;
      }
#line 2852 "parse.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 685 "parse.y" /* yacc.c:1646  */
    {
    sorrymsg((yylsp[-6]), "Component configuration in not yet supported\n");
    if((yyvsp[-4].entity_aspect)) delete (yyvsp[-4].entity_aspect);
    delete (yyvsp[-5].component_specification);
      }
#line 2862 "parse.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 691 "parse.y" /* yacc.c:1646  */
    {
    errormsg((yylsp[-4]), "Error in component configuration statement.\n");
    delete (yyvsp[-3].component_specification);
      }
#line 2871 "parse.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 701 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-7].text));
	if((yyvsp[-1].text) && name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-1]), "Identifier %s doesn't match component name %s.\n",
		       (yyvsp[-1].text), name.str());
	}

	ComponentBase*comp = new ComponentBase(name);
	comp->set_interface((yyvsp[-5].interface_list), (yyvsp[-4].interface_list));
	if ((yyvsp[-5].interface_list)) delete (yyvsp[-5].interface_list);
	if ((yyvsp[-4].interface_list)) delete (yyvsp[-4].interface_list);

	active_scope->bind_name(name, comp);
	delete[](yyvsp[-7].text);
	if ((yyvsp[-1].text)) delete[] (yyvsp[-1].text);
      }
#line 2891 "parse.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 718 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Syntax error in component declaration.\n");
	delete[] (yyvsp[-6].text);
	if((yyvsp[-1].text)) delete[] (yyvsp[-1].text);
	yyerrok;
      }
#line 2901 "parse.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 727 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text); }
#line 2907 "parse.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 732 "parse.y" /* yacc.c:1646  */
    { perm_string iname = lex_strings.make((yyvsp[-5].text));
	perm_string cname = lex_strings.make((yyvsp[-3].text));
	ComponentInstantiation*tmp = new ComponentInstantiation(iname, cname, (yyvsp[-2].named_expr_list), (yyvsp[-1].named_expr_list));
	delete (yyvsp[-2].named_expr_list);
	delete (yyvsp[-1].named_expr_list);
	FILE_NAME(tmp, (yylsp[-5]));
	delete[](yyvsp[-5].text);
	delete[](yyvsp[-3].text);
	(yyval.arch_statement) = tmp;
      }
#line 2922 "parse.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 743 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Errors in component instantiation.\n");
	delete[](yyvsp[-4].text);
	delete[](yyvsp[-2].text);
	(yyval.arch_statement) = 0;
      }
#line 2932 "parse.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 752 "parse.y" /* yacc.c:1646  */
    {
    ExpName* name = dynamic_cast<ExpName*>((yyvsp[0].expr));
    std::pair<instant_list_t*, ExpName*>* tmp = new std::pair<instant_list_t*, ExpName*>((yyvsp[-2].instantiation_list), name);
    (yyval.component_specification) = tmp;
      }
#line 2942 "parse.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 762 "parse.y" /* yacc.c:1646  */
    { VTypeArray*tmp = new VTypeArray((yyvsp[0].vtype), (yyvsp[-2].range_list));
	delete (yyvsp[-2].range_list);
	(yyval.vtype) = tmp;
      }
#line 2951 "parse.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 769 "parse.y" /* yacc.c:1646  */
    { std::list<ExpRange*> r;
	// NULL boundaries indicate unbounded array type
	ExpRange*tmp = new ExpRange(NULL, NULL, ExpRange::DOWNTO);
	r.push_back(tmp);
	FILE_NAME(tmp, (yylsp[-5]));
	VTypeArray*arr = new VTypeArray((yyvsp[0].vtype), &r);
	(yyval.vtype) = arr;
      }
#line 2964 "parse.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 779 "parse.y" /* yacc.c:1646  */
    { (yyval.vtype) = (yyvsp[0].vtype); }
#line 2970 "parse.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 784 "parse.y" /* yacc.c:1646  */
    {
        /* See more explanations at IEEE 1076-2008 P11.5 */
        std::list<SequentialStmt*> stmts;
        stmts.push_back((yyvsp[0].sequ));
        stmts.push_back(new WaitStmt(WaitStmt::FINAL, NULL));
        push_scope();
        ProcessStatement*tmp = new ProcessStatement(empty_perm_string, *active_scope,
                                                    NULL, &stmts);
        pop_scope();
        FILE_NAME(tmp, (yylsp[0]));
        (yyval.arch_statement) = tmp;
      }
#line 2987 "parse.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 804 "parse.y" /* yacc.c:1646  */
    { std::list<ExpConditional::case_t*>*options;
        options = (yyvsp[-1].exp_options_list) ? (yyvsp[-1].exp_options_list) : new std::list<ExpConditional::case_t*>;
        options->push_front(new ExpConditional::case_t((yyvsp[-2].expr), (yyvsp[-4].expr_list)));

        ExpName*name = dynamic_cast<ExpName*>((yyvsp[-6].expr));
        assert(name);
        CondSignalAssignment*tmp = new CondSignalAssignment(name, *options);

        FILE_NAME(tmp, (yylsp[-6]));
        delete options;
        (yyval.arch_statement) = tmp;
      }
#line 3004 "parse.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 820 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Syntax error in waveform of conditional signal assignment.\n");
	ExpConditional*tmp = new ExpConditional((yyvsp[-2].expr), 0, (yyvsp[-1].exp_options_list));
	FILE_NAME(tmp, (yylsp[-4]));
	delete (yyvsp[-1].exp_options_list);

        ExpName*name = dynamic_cast<ExpName*> ((yyvsp[-6].expr));
	assert(name);
	SignalAssignment*tmpa = new SignalAssignment(name, tmp);
	FILE_NAME(tmpa, (yylsp[-6]));

	(yyval.arch_statement) = tmpa;
      }
#line 3021 "parse.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 836 "parse.y" /* yacc.c:1646  */
    { ExpName*name = dynamic_cast<ExpName*> ((yyvsp[-3].expr));
	assert(name);
	SignalAssignment*tmp = new SignalAssignment(name, *(yyvsp[-1].expr_list));
	FILE_NAME(tmp, (yylsp[-3]));

	(yyval.arch_statement) = tmp;
	delete (yyvsp[-1].expr_list);
      }
#line 3034 "parse.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 847 "parse.y" /* yacc.c:1646  */
    { list<ExpConditional::case_t*>*tmp = (yyvsp[-1].exp_options_list);
	tmp ->push_back((yyvsp[0].exp_options));
	(yyval.exp_options_list) = tmp;
      }
#line 3043 "parse.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 852 "parse.y" /* yacc.c:1646  */
    { list<ExpConditional::case_t*>*tmp = new list<ExpConditional::case_t*>;
	tmp->push_back((yyvsp[0].exp_options));
	(yyval.exp_options_list) = tmp;
      }
#line 3052 "parse.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 859 "parse.y" /* yacc.c:1646  */
    { (yyval.exp_options_list) = (yyvsp[0].exp_options_list); }
#line 3058 "parse.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 860 "parse.y" /* yacc.c:1646  */
    { (yyval.exp_options_list) = 0; }
#line 3064 "parse.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 866 "parse.y" /* yacc.c:1646  */
    { ExpConditional::case_t*tmp = new ExpConditional::case_t((yyvsp[0].expr), (yyvsp[-2].expr_list));
	FILE_NAME(tmp, (yylsp[-3]));
	(yyval.exp_options) = tmp;
      }
#line 3073 "parse.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 871 "parse.y" /* yacc.c:1646  */
    { ExpConditional::case_t*tmp = new ExpConditional::case_t(0,  (yyvsp[0].expr_list));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.exp_options) = tmp;
      }
#line 3082 "parse.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 881 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-2].text);
	(yyval.arch_statement) = (yyvsp[0].arch_statement);
      }
#line 3090 "parse.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 888 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-2].text);
	(yyval.arch_statement) = (yyvsp[0].arch_statement);
      }
#line 3098 "parse.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 894 "parse.y" /* yacc.c:1646  */
    { (yyval.arch_statement) = (yyvsp[0].arch_statement); }
#line 3104 "parse.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 897 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in signal assignment waveform.\n");
	delete (yyvsp[-3].expr);
	(yyval.arch_statement) = 0;
	yyerrok;
      }
#line 3114 "parse.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 903 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-3]), "Syntax error in l-value of signal assignment.\n");
	yyerrok;
	delete (yyvsp[-1].expr_list);
	(yyval.arch_statement) = 0;
      }
#line 3124 "parse.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 923 "parse.y" /* yacc.c:1646  */
    {
  if(design_entities.find(lex_strings.make((yyvsp[-7].text))) == design_entities.end())
      errormsg((yylsp[-7]), "Couldn't find entity %s used in configuration declaration\n", (yyvsp[-7].text));
  //choose_architecture_for_entity();
  sorrymsg((yylsp[-10]), "Configuration declaration is not yet supported.\n");
     }
#line 3135 "parse.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 930 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Too many errors, giving up on configuration declaration.\n");
    if((yyvsp[-1].text)) delete (yyvsp[-1].text);
    yyerrok;
      }
#line 3144 "parse.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 967 "parse.y" /* yacc.c:1646  */
    { // The syntax allows multiple names to have the same type/value.
	for (std::list<perm_string>::iterator cur = (yyvsp[-5].name_list)->begin()
		   ; cur != (yyvsp[-5].name_list)->end() ; ++cur) {
	      active_scope->bind_name(*cur, (yyvsp[-3].vtype), (yyvsp[-1].expr));
	}
	delete (yyvsp[-5].name_list);
      }
#line 3156 "parse.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 975 "parse.y" /* yacc.c:1646  */
    { sorrymsg((yylsp[-4]), "Deferred constant declarations not supported\n");
	delete (yyvsp[-3].name_list);
      }
#line 3164 "parse.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 982 "parse.y" /* yacc.c:1646  */
    { // The syntax allows multiple names to have the same type/value.
	errormsg((yylsp[-1]), "Error in value expression for constants.\n");
	yyerrok;
	for (std::list<perm_string>::iterator cur = (yyvsp[-5].name_list)->begin()
		   ; cur != (yyvsp[-5].name_list)->end() ; ++cur) {
	      active_scope->bind_name(*cur, (yyvsp[-3].vtype), 0);
	}
	delete (yyvsp[-5].name_list);
      }
#line 3178 "parse.cc" /* yacc.c:1646  */
    break;

  case 103:
#line 992 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in constant declaration type.\n");
	yyerrok;
	delete (yyvsp[-3].name_list);
      }
#line 3187 "parse.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 997 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in constant declaration.\n");
	yyerrok;
      }
#line 3195 "parse.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 1018 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[0]), "Invalid design_unit\n"); }
#line 3201 "parse.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 1027 "parse.y" /* yacc.c:1646  */
    { (yyval.range_dir) = ExpRange::TO; }
#line 3207 "parse.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 1028 "parse.y" /* yacc.c:1646  */
    { (yyval.range_dir) = ExpRange::DOWNTO; }
#line 3213 "parse.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 1033 "parse.y" /* yacc.c:1646  */
    { ExpAggregate::element_t*tmp = new ExpAggregate::element_t((yyvsp[-2].choice_list), (yyvsp[0].expr));
	delete (yyvsp[-2].choice_list);
	(yyval.element) = tmp;
      }
#line 3222 "parse.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 1038 "parse.y" /* yacc.c:1646  */
    { ExpAggregate::element_t*tmp = new ExpAggregate::element_t(0, (yyvsp[0].expr));
	(yyval.element) = tmp;
      }
#line 3230 "parse.cc" /* yacc.c:1646  */
    break;

  case 119:
#line 1045 "parse.y" /* yacc.c:1646  */
    { std::list<ExpAggregate::element_t*>*tmp = (yyvsp[-2].element_list);
	tmp->push_back((yyvsp[0].element));
	(yyval.element_list) = tmp;
      }
#line 3239 "parse.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 1050 "parse.y" /* yacc.c:1646  */
    { std::list<ExpAggregate::element_t*>*tmp = new std::list<ExpAggregate::element_t*>;
	tmp->push_back((yyvsp[0].element));
	(yyval.element_list) = tmp;
      }
#line 3248 "parse.cc" /* yacc.c:1646  */
    break;

  case 121:
#line 1058 "parse.y" /* yacc.c:1646  */
    { (yyval.record_elements) = record_elements((yyvsp[-3].name_list), (yyvsp[-1].vtype));
        delete (yyvsp[-3].name_list);
      }
#line 3256 "parse.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 1065 "parse.y" /* yacc.c:1646  */
    { (yyval.record_elements) = (yyvsp[-1].record_elements);
	(yyval.record_elements)->splice((yyval.record_elements)->end(), *(yyvsp[0].record_elements));
	delete (yyvsp[0].record_elements);
      }
#line 3265 "parse.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 1070 "parse.y" /* yacc.c:1646  */
    { (yyval.record_elements) = (yyvsp[0].record_elements); }
#line 3271 "parse.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 1076 "parse.y" /* yacc.c:1646  */
    {
    ExpName* name = dynamic_cast<ExpName*>((yyvsp[0].expr));
    entity_aspect_t* tmp = new entity_aspect_t(entity_aspect_t::ENTITY, name);
    (yyval.entity_aspect) = tmp;
      }
#line 3281 "parse.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 1082 "parse.y" /* yacc.c:1646  */
    {
    ExpName* name = dynamic_cast<ExpName*>((yyvsp[0].expr));
    entity_aspect_t* tmp = new entity_aspect_t(entity_aspect_t::CONFIGURATION, name);
    (yyval.entity_aspect) = tmp;
      }
#line 3291 "parse.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 1088 "parse.y" /* yacc.c:1646  */
    {
    entity_aspect_t* tmp = new entity_aspect_t(entity_aspect_t::OPEN, 0);
    (yyval.entity_aspect) = tmp;
      }
#line 3300 "parse.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 1095 "parse.y" /* yacc.c:1646  */
    { (yyval.entity_aspect) = (yyvsp[0].entity_aspect); }
#line 3306 "parse.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 1096 "parse.y" /* yacc.c:1646  */
    { (yyval.entity_aspect) = 0; }
#line 3312 "parse.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 1103 "parse.y" /* yacc.c:1646  */
    { Entity*tmp = new Entity(lex_strings.make((yyvsp[-7].text)));
	FILE_NAME(tmp, (yylsp[-8]));
	  // Transfer the ports
	tmp->set_interface((yyvsp[-5].interface_list), (yyvsp[-4].interface_list));
	delete (yyvsp[-5].interface_list);
	delete (yyvsp[-4].interface_list);
	  // Save the entity in the entity map.
	design_entities[tmp->get_name()] = tmp;
	delete[](yyvsp[-7].text);
	if((yyvsp[-1].text) && tmp->get_name() != (yyvsp[-1].text)) {
	      errormsg((yylsp[-8]), "Syntax error in entity clause. Closing name doesn't match.\n");
        }
        delete[](yyvsp[-1].text);
      }
#line 3331 "parse.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 1118 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-5]), "Too many errors, giving up on entity declaration.\n");
	yyerrok;
	if ((yyvsp[-1].text)) delete[](yyvsp[-1].text);
      }
#line 3340 "parse.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 1126 "parse.y" /* yacc.c:1646  */
    { list<perm_string>*tmp = new list<perm_string>;
	tmp->push_back(lex_strings.make((yyvsp[0].text)));
	delete[](yyvsp[0].text);
	(yyval.name_list) = tmp;
      }
#line 3350 "parse.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 1135 "parse.y" /* yacc.c:1646  */
    { list<perm_string>*tmp = (yyvsp[-2].name_list);
	list<perm_string>*tmp3 = (yyvsp[0].name_list);
	if (tmp3) {
	      tmp->splice(tmp->end(), *tmp3);
	      delete tmp3;
	}
	(yyval.name_list) = tmp;
      }
#line 3363 "parse.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 1144 "parse.y" /* yacc.c:1646  */
    { list<perm_string>*tmp = (yyvsp[0].name_list);
	(yyval.name_list) = tmp;
      }
#line 3371 "parse.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 1151 "parse.y" /* yacc.c:1646  */
    { list<Expression*>*tmp = (yyvsp[-2].expr_list);
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 3380 "parse.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 1156 "parse.y" /* yacc.c:1646  */
    { list<Expression*>*tmp = new list<Expression*>;
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 3389 "parse.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 1164 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3395 "parse.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 1166 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].range); }
#line 3401 "parse.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 1193 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3407 "parse.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 1195 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::AND, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3416 "parse.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 1200 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::OR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3425 "parse.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 1205 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3434 "parse.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 1210 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::NAND, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3443 "parse.cc" /* yacc.c:1646  */
    break;

  case 143:
#line 1215 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::NOR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3452 "parse.cc" /* yacc.c:1646  */
    break;

  case 144:
#line 1220 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::XNOR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3461 "parse.cc" /* yacc.c:1646  */
    break;

  case 145:
#line 1228 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3467 "parse.cc" /* yacc.c:1646  */
    break;

  case 146:
#line 1230 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::AND, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3476 "parse.cc" /* yacc.c:1646  */
    break;

  case 147:
#line 1238 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3482 "parse.cc" /* yacc.c:1646  */
    break;

  case 148:
#line 1240 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::OR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3491 "parse.cc" /* yacc.c:1646  */
    break;

  case 149:
#line 1248 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3497 "parse.cc" /* yacc.c:1646  */
    break;

  case 150:
#line 1250 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::XNOR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3506 "parse.cc" /* yacc.c:1646  */
    break;

  case 151:
#line 1258 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3512 "parse.cc" /* yacc.c:1646  */
    break;

  case 152:
#line 1260 "parse.y" /* yacc.c:1646  */
    { ExpLogical*tmp = new ExpLogical(ExpLogical::XOR, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3521 "parse.cc" /* yacc.c:1646  */
    break;

  case 153:
#line 1268 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3527 "parse.cc" /* yacc.c:1646  */
    break;

  case 154:
#line 1270 "parse.y" /* yacc.c:1646  */
    { ExpArithmetic*tmp = new ExpArithmetic(ExpArithmetic::POW, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3536 "parse.cc" /* yacc.c:1646  */
    break;

  case 155:
#line 1275 "parse.y" /* yacc.c:1646  */
    { ExpUAbs*tmp = new ExpUAbs((yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3545 "parse.cc" /* yacc.c:1646  */
    break;

  case 156:
#line 1280 "parse.y" /* yacc.c:1646  */
    { ExpUNot*tmp = new ExpUNot((yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 3554 "parse.cc" /* yacc.c:1646  */
    break;

  case 157:
#line 1288 "parse.y" /* yacc.c:1646  */
    {
	if (strcasecmp((yyvsp[-2].text), "TEXT"))
	      sorrymsg((yylsp[-5]), "file declaration currently handles only TEXT type.\n");

	for (std::list<perm_string>::iterator cur = (yyvsp[-4].name_list)->begin()
		   ; cur != (yyvsp[-4].name_list)->end() ; ++cur) {
	      Variable*var = new Variable(*cur, &primitive_INTEGER);
	      FILE_NAME(var, (yylsp[-5]));
	      active_scope->bind_name(*cur, var);

	      // there was a file name specified, so it needs an implicit call
	      // to open it at the beginning of simulation and close it at the end
	      if((yyvsp[-1].file_info)) {
		std::list<Expression*> params;

		// add file_open() call in 'initial' block
		params.push_back(new ExpScopedName(active_scope->peek_name(), new ExpName(*cur)));
		params.push_back((yyvsp[-1].file_info)->filename()->clone());
		params.push_back((yyvsp[-1].file_info)->kind()->clone());
		ProcedureCall*fopen_call = new ProcedureCall(
                                    perm_string::literal("file_open"), &params);
		arc_scope->add_initializer(fopen_call);

		// add file_close() call in 'final' block
		params.clear();
		params.push_back(new ExpScopedName(active_scope->peek_name(), new ExpName(*cur)));
		ProcedureCall*fclose_call = new ProcedureCall(
                                    perm_string::literal("file_close"), &params);
		arc_scope->add_finalizer(fclose_call);

		delete (yyvsp[-1].file_info);
	      }
	}

	delete (yyvsp[-4].name_list);
      }
#line 3595 "parse.cc" /* yacc.c:1646  */
    break;

  case 158:
#line 1325 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in file declaration.\n");
	yyerrok;
      }
#line 3603 "parse.cc" /* yacc.c:1646  */
    break;

  case 159:
#line 1332 "parse.y" /* yacc.c:1646  */
    {
        ExpName*mode = new ExpName(lex_strings.make((yyvsp[-2].text)));
        delete[](yyvsp[-2].text);
        FILE_NAME(mode, (yylsp[-3]));
        (yyval.file_info) = new file_open_info_t(new ExpString((yyvsp[0].text)), mode);
     }
#line 3614 "parse.cc" /* yacc.c:1646  */
    break;

  case 160:
#line 1339 "parse.y" /* yacc.c:1646  */
    {
        (yyval.file_info) = new file_open_info_t(new ExpString((yyvsp[0].text)));
     }
#line 3622 "parse.cc" /* yacc.c:1646  */
    break;

  case 161:
#line 1344 "parse.y" /* yacc.c:1646  */
    { (yyval.file_info) = (yyvsp[0].file_info); }
#line 3628 "parse.cc" /* yacc.c:1646  */
    break;

  case 162:
#line 1345 "parse.y" /* yacc.c:1646  */
    { (yyval.file_info) = 0; }
#line 3634 "parse.cc" /* yacc.c:1646  */
    break;

  case 163:
#line 1352 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-11].text));
	perm_string gvar = lex_strings.make((yyvsp[-8].text));
	ForGenerate*tmp = new ForGenerate(name, gvar, (yyvsp[-6].range), *(yyvsp[-4].arch_statement_list));
	FILE_NAME(tmp, (yylsp[-11]));

	if ((yyvsp[-1].text) && name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-11]), "for-generate name %s does not match closing name %s\n",
		       name.str(), (yyvsp[-1].text));
	}
	delete[](yyvsp[-11].text);
	delete[](yyvsp[-8].text);
	delete (yyvsp[-4].arch_statement_list);
	delete[](yyvsp[-1].text);
	(yyval.arch_statement) = tmp;
      }
#line 3654 "parse.cc" /* yacc.c:1646  */
    break;

  case 164:
#line 1371 "parse.y" /* yacc.c:1646  */
    { perm_string type_name = lex_strings.make((yyvsp[0].text));
	perm_string name = lex_strings.make((yyvsp[-3].text));
	const VType*type_mark = active_scope->find_type(type_name);
	touchup_interface_for_functions((yyvsp[-2].interface_list));
	SubprogramHeader*tmp = new SubprogramHeader(name, (yyvsp[-2].interface_list), type_mark);
	FILE_NAME(tmp, (yylsp[-4]));
	delete[](yyvsp[-3].text);
	delete[](yyvsp[0].text);
	(yyval.subprogram) = tmp;
      }
#line 3669 "parse.cc" /* yacc.c:1646  */
    break;

  case 167:
#line 1389 "parse.y" /* yacc.c:1646  */
    { (yyval.arch_statement_list) = (yyvsp[0].arch_statement_list); }
#line 3675 "parse.cc" /* yacc.c:1646  */
    break;

  case 168:
#line 1394 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = (yyvsp[0].interface_list);}
#line 3681 "parse.cc" /* yacc.c:1646  */
    break;

  case 169:
#line 1396 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = 0; }
#line 3687 "parse.cc" /* yacc.c:1646  */
    break;

  case 170:
#line 1401 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = (yyvsp[-1].interface_list); }
#line 3693 "parse.cc" /* yacc.c:1646  */
    break;

  case 171:
#line 1403 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Error in interface list for generic.\n");
	yyerrok;
	(yyval.interface_list) = 0;
      }
#line 3702 "parse.cc" /* yacc.c:1646  */
    break;

  case 172:
#line 1410 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = (yyvsp[0].named_expr_list); }
#line 3708 "parse.cc" /* yacc.c:1646  */
    break;

  case 173:
#line 1411 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = 0; }
#line 3714 "parse.cc" /* yacc.c:1646  */
    break;

  case 174:
#line 1416 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = (yyvsp[-1].named_expr_list); }
#line 3720 "parse.cc" /* yacc.c:1646  */
    break;

  case 175:
#line 1418 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Error in association list for generic map.\n");
	yyerrok;
	(yyval.named_expr_list) = 0;
      }
#line 3729 "parse.cc" /* yacc.c:1646  */
    break;

  case 176:
#line 1426 "parse.y" /* yacc.c:1646  */
    { std::list<perm_string>* tmp = (yyvsp[-2].name_list);
	tmp->push_back(lex_strings.make((yyvsp[0].text)));
	delete[](yyvsp[0].text);
	(yyval.name_list) = tmp;
      }
#line 3739 "parse.cc" /* yacc.c:1646  */
    break;

  case 177:
#line 1432 "parse.y" /* yacc.c:1646  */
    { std::list<perm_string>*tmp = new std::list<perm_string>;
	tmp->push_back(lex_strings.make((yyvsp[0].text)));
	delete[](yyvsp[0].text);
	(yyval.name_list) = tmp;
      }
#line 3749 "parse.cc" /* yacc.c:1646  */
    break;

  case 178:
#line 1439 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text); }
#line 3755 "parse.cc" /* yacc.c:1646  */
    break;

  case 179:
#line 1439 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = 0; }
#line 3761 "parse.cc" /* yacc.c:1646  */
    break;

  case 180:
#line 1441 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[-1].text); }
#line 3767 "parse.cc" /* yacc.c:1646  */
    break;

  case 181:
#line 1441 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = 0; }
#line 3773 "parse.cc" /* yacc.c:1646  */
    break;

  case 182:
#line 1451 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-9].text));
	IfGenerate*tmp = new IfGenerate(name, (yyvsp[-6].expr), *(yyvsp[-4].arch_statement_list));
	FILE_NAME(tmp, (yylsp[-7]));

	if ((yyvsp[-1].text) && name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-9]), "if-generate name %s does not match closing name %s\n",
		       name.str(), (yyvsp[-1].text));
	}
	delete[](yyvsp[-9].text);
	delete  (yyvsp[-4].arch_statement_list);
	delete[](yyvsp[-1].text);
	(yyval.arch_statement) = tmp;
      }
#line 3791 "parse.cc" /* yacc.c:1646  */
    break;

  case 183:
#line 1470 "parse.y" /* yacc.c:1646  */
    { IfSequential*tmp = new IfSequential((yyvsp[-7].expr), (yyvsp[-5].sequ_list), (yyvsp[-4].elsif_list), (yyvsp[-3].sequ_list));
	FILE_NAME(tmp, (yylsp[-8]));
	delete (yyvsp[-5].sequ_list);
	delete (yyvsp[-4].elsif_list);
	delete (yyvsp[-3].sequ_list);
	(yyval.sequ) = tmp;
      }
#line 3803 "parse.cc" /* yacc.c:1646  */
    break;

  case 184:
#line 1481 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-7]), "Error in if_statement condition expression.\n");
	yyerrok;
	(yyval.sequ) = 0;
	delete (yyvsp[-5].sequ_list);
      }
#line 3813 "parse.cc" /* yacc.c:1646  */
    break;

  case 185:
#line 1488 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-3]), "Too many errors in sequence within if_statement.\n");
	yyerrok;
	(yyval.sequ) = 0;
      }
#line 3822 "parse.cc" /* yacc.c:1646  */
    break;

  case 186:
#line 1494 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-3]), "Too many errors in if_statement.\n");
	yyerrok;
	(yyval.sequ) = 0;
      }
#line 3831 "parse.cc" /* yacc.c:1646  */
    break;

  case 187:
#line 1501 "parse.y" /* yacc.c:1646  */
    { (yyval.elsif_list) = (yyvsp[0].elsif_list); }
#line 3837 "parse.cc" /* yacc.c:1646  */
    break;

  case 188:
#line 1502 "parse.y" /* yacc.c:1646  */
    { (yyval.elsif_list) = 0;  }
#line 3843 "parse.cc" /* yacc.c:1646  */
    break;

  case 189:
#line 1507 "parse.y" /* yacc.c:1646  */
    { list<IfSequential::Elsif*>*tmp = (yyvsp[-1].elsif_list);
	tmp->push_back((yyvsp[0].elsif));
	(yyval.elsif_list) = tmp;
      }
#line 3852 "parse.cc" /* yacc.c:1646  */
    break;

  case 190:
#line 1512 "parse.y" /* yacc.c:1646  */
    { list<IfSequential::Elsif*>*tmp = new list<IfSequential::Elsif*>;
	tmp->push_back((yyvsp[0].elsif));
	(yyval.elsif_list) = tmp;
      }
#line 3861 "parse.cc" /* yacc.c:1646  */
    break;

  case 191:
#line 1520 "parse.y" /* yacc.c:1646  */
    { IfSequential::Elsif*tmp = new IfSequential::Elsif((yyvsp[-2].expr), (yyvsp[0].sequ_list));
	FILE_NAME(tmp, (yylsp[-3]));
	delete (yyvsp[0].sequ_list);
	(yyval.elsif) = tmp;
      }
#line 3871 "parse.cc" /* yacc.c:1646  */
    break;

  case 192:
#line 1526 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[0]), "Too many errors in elsif sub-statements.\n");
	yyerrok;
	(yyval.elsif) = 0;
      }
#line 3880 "parse.cc" /* yacc.c:1646  */
    break;

  case 193:
#line 1534 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ_list) = (yyvsp[0].sequ_list); }
#line 3886 "parse.cc" /* yacc.c:1646  */
    break;

  case 194:
#line 1536 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[0]), "Too many errors in else sub-statements.\n");
	yyerrok;
	(yyval.sequ_list) = 0;
      }
#line 3895 "parse.cc" /* yacc.c:1646  */
    break;

  case 195:
#line 1541 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ_list) = 0; }
#line 3901 "parse.cc" /* yacc.c:1646  */
    break;

  case 196:
#line 1546 "parse.y" /* yacc.c:1646  */
    { (yyval.range_list) = (yyvsp[-1].range_list); }
#line 3907 "parse.cc" /* yacc.c:1646  */
    break;

  case 197:
#line 1548 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Errors in the index constraint.\n");
	yyerrok;
	(yyval.range_list) = new list<ExpRange*>;
      }
#line 3916 "parse.cc" /* yacc.c:1646  */
    break;

  case 201:
#line 1566 "parse.y" /* yacc.c:1646  */
    {
  instant_list_t* tmp = new instant_list_t(instant_list_t::NONE, (yyvsp[0].name_list));
  (yyval.instantiation_list) = tmp;
     }
#line 3925 "parse.cc" /* yacc.c:1646  */
    break;

  case 202:
#line 1571 "parse.y" /* yacc.c:1646  */
    {
  instant_list_t* tmp = new instant_list_t(instant_list_t::OTHERS, 0);
  (yyval.instantiation_list) = tmp;
     }
#line 3934 "parse.cc" /* yacc.c:1646  */
    break;

  case 203:
#line 1576 "parse.y" /* yacc.c:1646  */
    {
  instant_list_t* tmp = new instant_list_t(instant_list_t::ALL, 0);
  (yyval.instantiation_list) = tmp;
   }
#line 3943 "parse.cc" /* yacc.c:1646  */
    break;

  case 204:
#line 1585 "parse.y" /* yacc.c:1646  */
    { std::list<InterfacePort*>*tmp = new std::list<InterfacePort*>;
	for (std::list<perm_string>::iterator cur = (yyvsp[-4].name_list)->begin()
		   ; cur != (yyvsp[-4].name_list)->end() ; ++cur) {
	      InterfacePort*port = new InterfacePort;
	      FILE_NAME(port, (yylsp[-4]));
	      port->mode = (yyvsp[-2].port_mode);
	      port->name = *(cur);
	      port->type = (yyvsp[-1].vtype);
	      port->expr = (yyvsp[0].expr);
	      tmp->push_back(port);
	}
	delete (yyvsp[-4].name_list);
	(yyval.interface_list) = tmp;
      }
#line 3962 "parse.cc" /* yacc.c:1646  */
    break;

  case 205:
#line 1602 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 3968 "parse.cc" /* yacc.c:1646  */
    break;

  case 206:
#line 1603 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = 0; }
#line 3974 "parse.cc" /* yacc.c:1646  */
    break;

  case 207:
#line 1608 "parse.y" /* yacc.c:1646  */
    { std::list<InterfacePort*>*tmp = (yyvsp[-2].interface_list);
	tmp->splice(tmp->end(), *(yyvsp[0].interface_list));
	delete (yyvsp[0].interface_list);
	(yyval.interface_list) = tmp;
      }
#line 3984 "parse.cc" /* yacc.c:1646  */
    break;

  case 208:
#line 1614 "parse.y" /* yacc.c:1646  */
    { std::list<InterfacePort*>*tmp = (yyvsp[0].interface_list);
	(yyval.interface_list) = tmp;
      }
#line 3992 "parse.cc" /* yacc.c:1646  */
    break;

  case 209:
#line 1621 "parse.y" /* yacc.c:1646  */
    { library_import((yylsp[-2]), (yyvsp[-1].name_list));
	delete (yyvsp[-1].name_list);
      }
#line 4000 "parse.cc" /* yacc.c:1646  */
    break;

  case 210:
#line 1625 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in library clause.\n"); yyerrok; }
#line 4006 "parse.cc" /* yacc.c:1646  */
    break;

  case 213:
#line 1635 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text); }
#line 4012 "parse.cc" /* yacc.c:1646  */
    break;

  case 214:
#line 1639 "parse.y" /* yacc.c:1646  */
    { std::list<perm_string>*tmp = (yyvsp[-2].name_list);
	tmp->push_back(lex_strings.make((yyvsp[0].text)));
	delete[](yyvsp[0].text);
	(yyval.name_list) = tmp;
      }
#line 4022 "parse.cc" /* yacc.c:1646  */
    break;

  case 215:
#line 1645 "parse.y" /* yacc.c:1646  */
    { std::list<perm_string>*tmp = new std::list<perm_string>;
	tmp->push_back(lex_strings.make((yyvsp[0].text)));
	delete[](yyvsp[0].text);
	(yyval.name_list) = tmp;
      }
#line 4032 "parse.cc" /* yacc.c:1646  */
    break;

  case 216:
#line 1657 "parse.y" /* yacc.c:1646  */
    { perm_string loop_name = (yyvsp[-8].text)? lex_strings.make((yyvsp[-8].text)) : perm_string() ;
	if ((yyvsp[-1].text) && !(yyvsp[-8].text)) {
	      errormsg((yylsp[-1]), "Loop statement closing name %s for un-named statement\n", (yyvsp[-1].text));
	} else if((yyvsp[-1].text) && loop_name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-8]), "Loop statement name %s doesn't match closing name %s.\n", loop_name.str(), (yyvsp[-1].text));
	}
	if((yyvsp[-8].text)) delete[](yyvsp[-8].text);
	if((yyvsp[-1].text)) delete[](yyvsp[-1].text);

	WhileLoopStatement* tmp = new WhileLoopStatement(loop_name, (yyvsp[-6].expr), (yyvsp[-4].sequ_list));
	FILE_NAME(tmp, (yylsp[-8]));
	(yyval.sequ) = tmp;
      }
#line 4050 "parse.cc" /* yacc.c:1646  */
    break;

  case 217:
#line 1674 "parse.y" /* yacc.c:1646  */
    { perm_string loop_name = (yyvsp[-10].text)? lex_strings.make((yyvsp[-10].text)) : perm_string() ;
	perm_string index_name = lex_strings.make((yyvsp[-8].text));
	if ((yyvsp[-1].text) && !(yyvsp[-10].text)) {
	      errormsg((yylsp[-1]), "Loop statement closing name %s for un-named statement\n", (yyvsp[-1].text));
	} else if((yyvsp[-1].text) && loop_name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-10]), "Loop statement name %s doesn't match closing name %s.\n", loop_name.str(), (yyvsp[-1].text));
	}
	if((yyvsp[-10].text))  delete[] (yyvsp[-10].text);
	delete[] (yyvsp[-8].text);
	if((yyvsp[-1].text)) delete[] (yyvsp[-1].text);

	ForLoopStatement* tmp = new ForLoopStatement(loop_name, index_name, (yyvsp[-6].range), (yyvsp[-4].sequ_list));
	FILE_NAME(tmp, (yylsp[-10]));
	(yyval.sequ) = tmp;
      }
#line 4070 "parse.cc" /* yacc.c:1646  */
    break;

  case 218:
#line 1692 "parse.y" /* yacc.c:1646  */
    { perm_string loop_name = (yyvsp[-6].text)? lex_strings.make((yyvsp[-6].text)) : perm_string() ;
	if ((yyvsp[-1].text) && !(yyvsp[-6].text)) {
	      errormsg((yylsp[-1]), "Loop statement closing name %s for un-named statement\n", (yyvsp[-1].text));
	} else if((yyvsp[-1].text) && loop_name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-6]), "Loop statement name %s doesn't match closing name %s.\n", loop_name.str(), (yyvsp[-1].text));
	}
	if((yyvsp[-6].text)) delete[](yyvsp[-6].text);
	if((yyvsp[-1].text)) delete[](yyvsp[-1].text);

	BasicLoopStatement* tmp = new BasicLoopStatement(loop_name, (yyvsp[-4].sequ_list));
	FILE_NAME(tmp, (yylsp[-5]));

	(yyval.sequ) = tmp;
      }
#line 4089 "parse.cc" /* yacc.c:1646  */
    break;

  case 219:
#line 1708 "parse.y" /* yacc.c:1646  */
    { (yyval.port_mode) = PORT_IN; }
#line 4095 "parse.cc" /* yacc.c:1646  */
    break;

  case 220:
#line 1709 "parse.y" /* yacc.c:1646  */
    { (yyval.port_mode) = PORT_OUT; }
#line 4101 "parse.cc" /* yacc.c:1646  */
    break;

  case 221:
#line 1710 "parse.y" /* yacc.c:1646  */
    { (yyval.port_mode) = PORT_INOUT; }
#line 4107 "parse.cc" /* yacc.c:1646  */
    break;

  case 222:
#line 1713 "parse.y" /* yacc.c:1646  */
    {(yyval.port_mode) = (yyvsp[0].port_mode);}
#line 4113 "parse.cc" /* yacc.c:1646  */
    break;

  case 223:
#line 1713 "parse.y" /* yacc.c:1646  */
    {(yyval.port_mode) = PORT_NONE;}
#line 4119 "parse.cc" /* yacc.c:1646  */
    break;

  case 224:
#line 1717 "parse.y" /* yacc.c:1646  */
    { Expression*tmp;
        /* Check if the IDENTIFIER is one of CHARACTER enums (LF, CR, etc.) */
        tmp = parse_char_enums((yyvsp[0].text));
        if(!tmp) {
            perm_string name = lex_strings.make((yyvsp[0].text));
            /* There are functions that have the same name types, e.g. integer */
            if(!active_scope->find_subprogram(name).empty() && !parse_type_by_name(name))
                tmp = new ExpFunc(name);
            else
                tmp = new ExpName(name);
        }
        FILE_NAME(tmp, (yylsp[0]));
        delete[](yyvsp[0].text);
        (yyval.expr) = tmp;
      }
#line 4139 "parse.cc" /* yacc.c:1646  */
    break;

  case 225:
#line 1734 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 4145 "parse.cc" /* yacc.c:1646  */
    break;

  case 226:
#line 1737 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 4151 "parse.cc" /* yacc.c:1646  */
    break;

  case 227:
#line 1740 "parse.y" /* yacc.c:1646  */
    {
        ExpName*name = dynamic_cast<ExpName*>((yyvsp[-3].expr));
        assert(name);
        name->add_index((yyvsp[-1].expr_list));
        delete (yyvsp[-1].expr_list);  // contents of the list is moved to the selected_name
    }
#line 4162 "parse.cc" /* yacc.c:1646  */
    break;

  case 228:
#line 1754 "parse.y" /* yacc.c:1646  */
    { Expression*tmp;
        perm_string name = lex_strings.make((yyvsp[-3].text));
        delete[](yyvsp[-3].text);
        if (active_scope->is_vector_name(name) || is_subprogram_param(name))
            tmp = new ExpName(name, (yyvsp[-1].expr_list));
        else
            tmp = new ExpFunc(name, (yyvsp[-1].expr_list));
        FILE_NAME(tmp, (yylsp[-3]));
        (yyval.expr) = tmp;
      }
#line 4177 "parse.cc" /* yacc.c:1646  */
    break;

  case 229:
#line 1765 "parse.y" /* yacc.c:1646  */
    { ExpName*name = dynamic_cast<ExpName*>((yyvsp[-3].expr));
        assert(name);
        name->add_index((yyvsp[-1].expr_list));
        (yyval.expr) = (yyvsp[-3].expr);
      }
#line 4187 "parse.cc" /* yacc.c:1646  */
    break;

  case 230:
#line 1775 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>*tmp = (yyvsp[-2].expr_list);
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 4196 "parse.cc" /* yacc.c:1646  */
    break;

  case 231:
#line 1780 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>*tmp = new std::list<Expression*>;
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 4205 "parse.cc" /* yacc.c:1646  */
    break;

  case 232:
#line 1790 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-6].text));
	if((yyvsp[-1].text) && name != (yyvsp[-1].text)) {
	      errormsg((yylsp[-6]), "Identifier %s doesn't match package name %s.\n",
		       (yyvsp[-1].text), name.str());
        }
	Package*tmp = new Package(name, *active_scope);
	FILE_NAME(tmp, (yylsp[-6]));
	delete[](yyvsp[-6].text);
        if ((yyvsp[-1].text)) delete[](yyvsp[-1].text);
	pop_scope();
	  /* Put this package into the work library, or the currently
	     parsed library. Note that parse_library_name is an
	     argument to the parser. */
	library_save_package(parse_library_name, tmp);
      }
#line 4225 "parse.cc" /* yacc.c:1646  */
    break;

  case 233:
#line 1806 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Syntax error in package clause.\n");
      yyerrok;
      pop_scope();
    }
#line 4234 "parse.cc" /* yacc.c:1646  */
    break;

  case 234:
#line 1814 "parse.y" /* yacc.c:1646  */
    { push_scope();
	(yyval.text) = (yyvsp[0].text);
      }
#line 4242 "parse.cc" /* yacc.c:1646  */
    break;

  case 247:
#line 1843 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in package declarative item.\n");
	yyerrok;
      }
#line 4250 "parse.cc" /* yacc.c:1646  */
    break;

  case 252:
#line 1862 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-6].text));
	if ((yyvsp[-1].text) && name != (yyvsp[-1].text))
	      errormsg((yylsp[-1]), "Package name (%s) doesn't match closing name (%s).\n", (yyvsp[-6].text), (yyvsp[-1].text));
	delete[] (yyvsp[-6].text);
	if((yyvsp[-1].text)) delete[](yyvsp[-1].text);
	pop_scope();
      }
#line 4262 "parse.cc" /* yacc.c:1646  */
    break;

  case 253:
#line 1871 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-6]), "Errors in package %s body.\n", (yyvsp[-6].text));
	yyerrok;
	pop_scope();
      }
#line 4271 "parse.cc" /* yacc.c:1646  */
    break;

  case 254:
#line 1883 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[0].text));
	push_scope();
	Package*pkg = library_recall_package(parse_library_name, name);
	if (pkg != 0) {
	      active_scope->set_package_header(pkg);
	} else {
	      errormsg((yylsp[-2]), "Package body for %s has no matching header.\n", (yyvsp[0].text));
	}
	(yyval.text) = (yyvsp[0].text);
      }
#line 4286 "parse.cc" /* yacc.c:1646  */
    break;

  case 255:
#line 1896 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = (yyvsp[-1].interface_list); }
#line 4292 "parse.cc" /* yacc.c:1646  */
    break;

  case 256:
#line 1900 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = (yyvsp[0].interface_list); }
#line 4298 "parse.cc" /* yacc.c:1646  */
    break;

  case 257:
#line 1901 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = 0; }
#line 4304 "parse.cc" /* yacc.c:1646  */
    break;

  case 258:
#line 1906 "parse.y" /* yacc.c:1646  */
    { (yyval.interface_list) = (yyvsp[-1].interface_list); }
#line 4310 "parse.cc" /* yacc.c:1646  */
    break;

  case 259:
#line 1908 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Syntax error in port list.\n");
	yyerrok;
	(yyval.interface_list) = 0;
      }
#line 4319 "parse.cc" /* yacc.c:1646  */
    break;

  case 260:
#line 1914 "parse.y" /* yacc.c:1646  */
    {(yyval.interface_list) = (yyvsp[0].interface_list);}
#line 4325 "parse.cc" /* yacc.c:1646  */
    break;

  case 261:
#line 1914 "parse.y" /* yacc.c:1646  */
    {(yyval.interface_list) = 0;}
#line 4331 "parse.cc" /* yacc.c:1646  */
    break;

  case 262:
#line 1918 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = (yyvsp[-1].named_expr_list); }
#line 4337 "parse.cc" /* yacc.c:1646  */
    break;

  case 263:
#line 1920 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Syntax error in port map aspect.\n");
	yyerrok;
      }
#line 4345 "parse.cc" /* yacc.c:1646  */
    break;

  case 264:
#line 1926 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = (yyvsp[0].named_expr_list); }
#line 4351 "parse.cc" /* yacc.c:1646  */
    break;

  case 265:
#line 1927 "parse.y" /* yacc.c:1646  */
    { (yyval.named_expr_list) = 0; }
#line 4357 "parse.cc" /* yacc.c:1646  */
    break;

  case 266:
#line 1933 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 4363 "parse.cc" /* yacc.c:1646  */
    break;

  case 267:
#line 1938 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 4369 "parse.cc" /* yacc.c:1646  */
    break;

  case 268:
#line 1940 "parse.y" /* yacc.c:1646  */
    { ExpAttribute*tmp = NULL;
	perm_string attr = lex_strings.make((yyvsp[-1].text));
	ExpName*base = dynamic_cast<ExpName*>((yyvsp[-3].expr));
	const VType*type = parse_type_by_name(base->peek_name());

	if(type) {
	    tmp = new ExpTypeAttribute(type, attr, (yyvsp[0].expr_list));
	} else {
	    tmp = new ExpObjAttribute(base, attr, (yyvsp[0].expr_list));
	}

	FILE_NAME(tmp, (yylsp[-3]));
	delete[](yyvsp[-1].text);
	(yyval.expr) = tmp;
      }
#line 4389 "parse.cc" /* yacc.c:1646  */
    break;

  case 269:
#line 1956 "parse.y" /* yacc.c:1646  */
    { ExpCharacter*tmp = new ExpCharacter((yyvsp[0].text)[0]);
	FILE_NAME(tmp,(yylsp[0]));
	delete[](yyvsp[0].text);
	(yyval.expr) = tmp;
      }
#line 4399 "parse.cc" /* yacc.c:1646  */
    break;

  case 270:
#line 1962 "parse.y" /* yacc.c:1646  */
    { ExpInteger*tmp = new ExpInteger((yyvsp[0].uni_integer));
	FILE_NAME(tmp, (yylsp[0]));
	(yyval.expr) = tmp;
      }
#line 4408 "parse.cc" /* yacc.c:1646  */
    break;

  case 271:
#line 1967 "parse.y" /* yacc.c:1646  */
    { ExpReal*tmp = new ExpReal((yyvsp[0].uni_real));
	FILE_NAME(tmp, (yylsp[0]));
	(yyval.expr) = tmp;
      }
#line 4417 "parse.cc" /* yacc.c:1646  */
    break;

  case 272:
#line 1972 "parse.y" /* yacc.c:1646  */
    { ExpString*tmp = new ExpString((yyvsp[0].text));
	FILE_NAME(tmp,(yylsp[0]));
	delete[](yyvsp[0].text);
	(yyval.expr) = tmp;
      }
#line 4427 "parse.cc" /* yacc.c:1646  */
    break;

  case 273:
#line 1978 "parse.y" /* yacc.c:1646  */
    { ExpBitstring*tmp = new ExpBitstring((yyvsp[0].text));
	FILE_NAME(tmp, (yylsp[0]));
	delete[](yyvsp[0].text);
	(yyval.expr) = tmp;
      }
#line 4437 "parse.cc" /* yacc.c:1646  */
    break;

  case 274:
#line 1984 "parse.y" /* yacc.c:1646  */
    { ExpTime::timeunit_t unit = ExpTime::FS;

        if(!strcasecmp((yyvsp[0].text), "us"))
            unit = ExpTime::US;
        else if(!strcasecmp((yyvsp[0].text), "ms"))
            unit = ExpTime::MS;
        else if(!strcasecmp((yyvsp[0].text), "ns"))
            unit = ExpTime::NS;
        else if(!strcasecmp((yyvsp[0].text), "s"))
            unit = ExpTime::S;
        else if(!strcasecmp((yyvsp[0].text), "ps"))
            unit = ExpTime::PS;
        else if(!strcasecmp((yyvsp[0].text), "fs"))
            unit = ExpTime::FS;
        else
            errormsg((yylsp[0]), "Invalid time unit (accepted are fs, ps, ns, us, ms, s).\n");

        if((yyvsp[-1].uni_integer) < 0)
            errormsg((yylsp[-1]), "Time cannot be negative.\n");

        ExpTime*tmp = new ExpTime((yyvsp[-1].uni_integer), unit);
        FILE_NAME(tmp, (yylsp[-1]));
        delete[] (yyvsp[0].text);
        (yyval.expr) = tmp;
      }
#line 4467 "parse.cc" /* yacc.c:1646  */
    break;

  case 275:
#line 2019 "parse.y" /* yacc.c:1646  */
    { sorrymsg((yylsp[-3]), "Function calls not supported\n");
	delete[] (yyvsp[-3].text);
	(yyval.expr) = 0;
      }
#line 4476 "parse.cc" /* yacc.c:1646  */
    break;

  case 276:
#line 2027 "parse.y" /* yacc.c:1646  */
    { Expression*tmp = aggregate_or_primary((yylsp[-2]), (yyvsp[-1].element_list));
	(yyval.expr) = tmp;
      }
#line 4484 "parse.cc" /* yacc.c:1646  */
    break;

  case 280:
#line 2040 "parse.y" /* yacc.c:1646  */
    {
    ProcedureCall* tmp = new ProcedureCall(lex_strings.make((yyvsp[-1].text)));
    FILE_NAME(tmp, (yylsp[-1]));
    delete[] (yyvsp[-1].text);
    (yyval.sequ) = tmp;
      }
#line 4495 "parse.cc" /* yacc.c:1646  */
    break;

  case 281:
#line 2047 "parse.y" /* yacc.c:1646  */
    {
    ProcedureCall* tmp = new ProcedureCall(lex_strings.make((yyvsp[-4].text)), (yyvsp[-2].named_expr_list));
    FILE_NAME(tmp, (yylsp[-4]));
    delete[] (yyvsp[-4].text);
    (yyval.sequ) = tmp;
      }
#line 4506 "parse.cc" /* yacc.c:1646  */
    break;

  case 282:
#line 2054 "parse.y" /* yacc.c:1646  */
    {
    ProcedureCall* tmp = new ProcedureCall(lex_strings.make((yyvsp[-2].text)), (yyvsp[-1].expr_list));
    FILE_NAME(tmp, (yylsp[-2]));
    delete[] (yyvsp[-2].text);
    delete (yyvsp[-1].expr_list); // parameters are copied in this variant
    (yyval.sequ) = tmp;
      }
#line 4518 "parse.cc" /* yacc.c:1646  */
    break;

  case 283:
#line 2062 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Errors in procedure call.\n");
	yyerrok;
	delete[](yyvsp[-4].text);
	(yyval.sequ) = 0;
      }
#line 4528 "parse.cc" /* yacc.c:1646  */
    break;

  case 284:
#line 2071 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-2].text);
	(yyval.sequ) = (yyvsp[0].sequ);
      }
#line 4536 "parse.cc" /* yacc.c:1646  */
    break;

  case 285:
#line 2074 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4542 "parse.cc" /* yacc.c:1646  */
    break;

  case 286:
#line 2079 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-1].text));
	touchup_interface_for_functions((yyvsp[0].interface_list));
	SubprogramHeader*tmp = new SubprogramHeader(name, (yyvsp[0].interface_list), NULL);
	FILE_NAME(tmp, (yylsp[-2]));
	delete[](yyvsp[-1].text);
	(yyval.subprogram) = tmp;
      }
#line 4554 "parse.cc" /* yacc.c:1646  */
    break;

  case 293:
#line 2105 "parse.y" /* yacc.c:1646  */
    { push_scope();
         (yyval.text) = (yyvsp[-2].text);
       }
#line 4562 "parse.cc" /* yacc.c:1646  */
    break;

  case 294:
#line 2115 "parse.y" /* yacc.c:1646  */
    { perm_string iname = (yyvsp[-10].text)? lex_strings.make((yyvsp[-10].text)) : empty_perm_string;
	if ((yyvsp[-10].text)) delete[](yyvsp[-10].text);
	if ((yyvsp[-1].text)) {
	      if (iname.nil()) {
		    errormsg((yylsp[-1]), "Process end name %s for un-named processes.\n", (yyvsp[-1].text));
	      } else if (iname != (yyvsp[-1].text)) {
		    errormsg((yylsp[-1]), "Process name %s does not match opening name %s.\n",
			     (yyvsp[-1].text), (yyvsp[-10].text));
	      }
	      delete[](yyvsp[-1].text);
	}

	ProcessStatement*tmp = new ProcessStatement(iname, *active_scope, (yyvsp[-9].expr_list), (yyvsp[-5].sequ_list));
        arc_scope->bind_scope(tmp->peek_name(), tmp);
        pop_scope();
	FILE_NAME(tmp, (yylsp[-8]));
	delete (yyvsp[-9].expr_list);
	delete (yyvsp[-5].sequ_list);
	(yyval.arch_statement) = tmp;
      }
#line 4587 "parse.cc" /* yacc.c:1646  */
    break;

  case 295:
#line 2140 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-4]), "Too many errors in process sequential statements.\n");
	yyerrok;
	(yyval.arch_statement) = 0;
      }
#line 4596 "parse.cc" /* yacc.c:1646  */
    break;

  case 296:
#line 2153 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[-1].expr_list); }
#line 4602 "parse.cc" /* yacc.c:1646  */
    break;

  case 297:
#line 2155 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Error in process sensitivity list\n");
	yyerrok;
	(yyval.expr_list) = 0;
      }
#line 4611 "parse.cc" /* yacc.c:1646  */
    break;

  case 298:
#line 2160 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = 0; }
#line 4617 "parse.cc" /* yacc.c:1646  */
    break;

  case 299:
#line 2165 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>*tmp = new std::list<Expression*>;
	ExpName*all = new ExpNameALL;
	FILE_NAME(all, (yylsp[0]));
	tmp->push_back(all);
	(yyval.expr_list) = tmp;
      }
#line 4628 "parse.cc" /* yacc.c:1646  */
    break;

  case 300:
#line 2172 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[0].expr_list); }
#line 4634 "parse.cc" /* yacc.c:1646  */
    break;

  case 301:
#line 2177 "parse.y" /* yacc.c:1646  */
    { ExpRange* tmp = new ExpRange((yyvsp[-2].expr), (yyvsp[0].expr), (yyvsp[-1].range_dir));
	FILE_NAME(tmp, (yylsp[-2]));
	(yyval.range) = tmp;
      }
#line 4643 "parse.cc" /* yacc.c:1646  */
    break;

  case 302:
#line 2182 "parse.y" /* yacc.c:1646  */
    {
        ExpRange*tmp = NULL;
        ExpName*name = NULL;
        if((name = dynamic_cast<ExpName*>((yyvsp[-2].expr)))) {
            tmp = new ExpRange(name, false);
            FILE_NAME(tmp, (yylsp[-2]));
        } else {
	    errormsg((yylsp[-2]), "'range attribute can be used with named expressions only");
        }
        (yyval.range) = tmp;
      }
#line 4659 "parse.cc" /* yacc.c:1646  */
    break;

  case 303:
#line 2194 "parse.y" /* yacc.c:1646  */
    {
        ExpRange*tmp = NULL;
        ExpName*name = NULL;
        if((name = dynamic_cast<ExpName*>((yyvsp[-2].expr)))) {
            tmp = new ExpRange(name, true);
            FILE_NAME(tmp, (yylsp[-2]));
        } else {
	    errormsg((yylsp[-2]), "'reverse_range attribute can be used with named expressions only");
        }
        (yyval.range) = tmp;
      }
#line 4675 "parse.cc" /* yacc.c:1646  */
    break;

  case 304:
#line 2209 "parse.y" /* yacc.c:1646  */
    { list<ExpRange*>*tmp = new list<ExpRange*>;
	tmp->push_back((yyvsp[0].range));
	(yyval.range_list) = tmp;
      }
#line 4684 "parse.cc" /* yacc.c:1646  */
    break;

  case 305:
#line 2214 "parse.y" /* yacc.c:1646  */
    { list<ExpRange*>*tmp = (yyvsp[-2].range_list);
	tmp->push_back((yyvsp[0].range));
	(yyval.range_list) = tmp;
      }
#line 4693 "parse.cc" /* yacc.c:1646  */
    break;

  case 306:
#line 2222 "parse.y" /* yacc.c:1646  */
    { VTypeRecord*tmp = new VTypeRecord((yyvsp[-2].record_elements));
	(yyval.vtype) = tmp;
      }
#line 4701 "parse.cc" /* yacc.c:1646  */
    break;

  case 307:
#line 2229 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 4707 "parse.cc" /* yacc.c:1646  */
    break;

  case 308:
#line 2231 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::EQ, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4716 "parse.cc" /* yacc.c:1646  */
    break;

  case 309:
#line 2236 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::LT, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4725 "parse.cc" /* yacc.c:1646  */
    break;

  case 310:
#line 2241 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::GT, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4734 "parse.cc" /* yacc.c:1646  */
    break;

  case 311:
#line 2246 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::LE, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4743 "parse.cc" /* yacc.c:1646  */
    break;

  case 312:
#line 2251 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::GE, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4752 "parse.cc" /* yacc.c:1646  */
    break;

  case 313:
#line 2256 "parse.y" /* yacc.c:1646  */
    { ExpRelation*tmp = new ExpRelation(ExpRelation::NEQ, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 4761 "parse.cc" /* yacc.c:1646  */
    break;

  case 314:
#line 2264 "parse.y" /* yacc.c:1646  */
    { ReportStmt*tmp = new ReportStmt((yyvsp[-2].expr), (yyvsp[-1].severity));
	FILE_NAME(tmp,(yylsp[-2]));
	(yyval.sequ) = tmp;
      }
#line 4770 "parse.cc" /* yacc.c:1646  */
    break;

  case 315:
#line 2271 "parse.y" /* yacc.c:1646  */
    { ReturnStmt*tmp = new ReturnStmt((yyvsp[-1].expr));
	FILE_NAME(tmp, (yylsp[-2]));
	(yyval.sequ) = tmp;
      }
#line 4779 "parse.cc" /* yacc.c:1646  */
    break;

  case 316:
#line 2276 "parse.y" /* yacc.c:1646  */
    { ReturnStmt*tmp = new ReturnStmt(0);
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.sequ) = tmp;
      }
#line 4788 "parse.cc" /* yacc.c:1646  */
    break;

  case 317:
#line 2281 "parse.y" /* yacc.c:1646  */
    { ReturnStmt*tmp = new ReturnStmt(0);
	FILE_NAME(tmp, (yylsp[-2]));
	(yyval.sequ) = tmp;
	errormsg((yylsp[-1]), "Error in expression in return statement.\n");
	yyerrok;
      }
#line 4799 "parse.cc" /* yacc.c:1646  */
    break;

  case 320:
#line 2296 "parse.y" /* yacc.c:1646  */
    { Expression*pfx = (yyvsp[-2].expr);
	ExpName*pfx1 = dynamic_cast<ExpName*>(pfx);
	assert(pfx1);
	perm_string tmp = lex_strings.make((yyvsp[0].text));
	(yyval.expr) = new ExpName(pfx1, tmp);
	FILE_NAME((yyval.expr), (yylsp[0]));
	delete[](yyvsp[0].text);
      }
#line 4812 "parse.cc" /* yacc.c:1646  */
    break;

  case 321:
#line 2305 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in prefix in front of \"%s\".\n", (yyvsp[0].text));
        yyerrok;
	(yyval.expr) = new ExpName(lex_strings.make((yyvsp[0].text)));
	FILE_NAME((yyval.expr), (yylsp[0]));
	delete[](yyvsp[0].text);
      }
#line 4823 "parse.cc" /* yacc.c:1646  */
    break;

  case 322:
#line 2315 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>* tmp = (yyvsp[-2].expr_list);
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 4832 "parse.cc" /* yacc.c:1646  */
    break;

  case 323:
#line 2320 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>* tmp = new std::list<Expression*>();
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 4841 "parse.cc" /* yacc.c:1646  */
    break;

  case 324:
#line 2331 "parse.y" /* yacc.c:1646  */
    { library_use((yylsp[-2]), active_scope, 0, (yyvsp[-2].text), 0);
	delete[](yyvsp[-2].text);
      }
#line 4849 "parse.cc" /* yacc.c:1646  */
    break;

  case 325:
#line 2335 "parse.y" /* yacc.c:1646  */
    { library_use((yylsp[-4]), active_scope, (yyvsp[-4].text), (yyvsp[-2].text), 0);
	delete[](yyvsp[-4].text);
	delete[](yyvsp[-2].text);
      }
#line 4858 "parse.cc" /* yacc.c:1646  */
    break;

  case 326:
#line 2340 "parse.y" /* yacc.c:1646  */
    { library_use((yylsp[-4]), active_scope, (yyvsp[-4].text), (yyvsp[-2].text), (yyvsp[0].text));
	delete[](yyvsp[-4].text);
	delete[](yyvsp[-2].text);
	delete[](yyvsp[0].text);
      }
#line 4868 "parse.cc" /* yacc.c:1646  */
    break;

  case 329:
#line 2354 "parse.y" /* yacc.c:1646  */
    { ExpSelected*tmp = new ExpSelected((yyvsp[-5].expr), (yyvsp[-1].exp_options_list));
	FILE_NAME(tmp, (yylsp[-4]));
        delete (yyvsp[-5].expr);
	delete (yyvsp[-1].exp_options_list);

	ExpName*name = dynamic_cast<ExpName*>((yyvsp[-3].expr));
	assert(name);
	SignalAssignment*tmpa = new SignalAssignment(name, tmp);
	FILE_NAME(tmpa, (yylsp[-6]));

	(yyval.arch_statement) = tmpa;
      }
#line 4885 "parse.cc" /* yacc.c:1646  */
    break;

  case 330:
#line 2370 "parse.y" /* yacc.c:1646  */
    { ExpConditional::case_t*tmp = new ExpConditional::case_t((yyvsp[0].expr), (yyvsp[-2].expr_list));
	FILE_NAME(tmp, (yylsp[-2]));
	(yyval.exp_options) = tmp;
      }
#line 4894 "parse.cc" /* yacc.c:1646  */
    break;

  case 331:
#line 2375 "parse.y" /* yacc.c:1646  */
    { ExpConditional::case_t*tmp = new ExpConditional::case_t(0,  (yyvsp[-2].expr_list));
	FILE_NAME(tmp, (yylsp[-2]));
	(yyval.exp_options) = tmp;
      }
#line 4903 "parse.cc" /* yacc.c:1646  */
    break;

  case 332:
#line 2383 "parse.y" /* yacc.c:1646  */
    { list<ExpConditional::case_t*>*tmp = (yyvsp[-2].exp_options_list);
	tmp->push_back((yyvsp[0].exp_options));
	(yyval.exp_options_list) = tmp;
      }
#line 4912 "parse.cc" /* yacc.c:1646  */
    break;

  case 333:
#line 2388 "parse.y" /* yacc.c:1646  */
    { list<ExpConditional::case_t*>*tmp = new list<ExpConditional::case_t*>;
	tmp->push_back((yyvsp[0].exp_options));
	(yyval.exp_options_list) = tmp;
      }
#line 4921 "parse.cc" /* yacc.c:1646  */
    break;

  case 334:
#line 2396 "parse.y" /* yacc.c:1646  */
    { std::list<SequentialStmt*>*tmp = (yyvsp[-1].sequ_list);
    if((yyvsp[0].sequ))
        tmp->push_back((yyvsp[0].sequ));
	(yyval.sequ_list) = tmp;
      }
#line 4931 "parse.cc" /* yacc.c:1646  */
    break;

  case 335:
#line 2402 "parse.y" /* yacc.c:1646  */
    { std::list<SequentialStmt*>*tmp = new std::list<SequentialStmt*>;
    if((yyvsp[0].sequ))
        tmp->push_back((yyvsp[0].sequ));
	(yyval.sequ_list) = tmp;
      }
#line 4941 "parse.cc" /* yacc.c:1646  */
    break;

  case 336:
#line 2410 "parse.y" /* yacc.c:1646  */
    { (yyvsp[0].sequ_list) = (yyval.sequ_list); }
#line 4947 "parse.cc" /* yacc.c:1646  */
    break;

  case 337:
#line 2411 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ_list) = NULL; }
#line 4953 "parse.cc" /* yacc.c:1646  */
    break;

  case 338:
#line 2415 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4959 "parse.cc" /* yacc.c:1646  */
    break;

  case 339:
#line 2416 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4965 "parse.cc" /* yacc.c:1646  */
    break;

  case 340:
#line 2417 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4971 "parse.cc" /* yacc.c:1646  */
    break;

  case 341:
#line 2418 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4977 "parse.cc" /* yacc.c:1646  */
    break;

  case 342:
#line 2419 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4983 "parse.cc" /* yacc.c:1646  */
    break;

  case 343:
#line 2420 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4989 "parse.cc" /* yacc.c:1646  */
    break;

  case 344:
#line 2421 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 4995 "parse.cc" /* yacc.c:1646  */
    break;

  case 345:
#line 2422 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 5001 "parse.cc" /* yacc.c:1646  */
    break;

  case 346:
#line 2423 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 5007 "parse.cc" /* yacc.c:1646  */
    break;

  case 347:
#line 2424 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = (yyvsp[0].sequ); }
#line 5013 "parse.cc" /* yacc.c:1646  */
    break;

  case 348:
#line 2425 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ) = 0; }
#line 5019 "parse.cc" /* yacc.c:1646  */
    break;

  case 349:
#line 2427 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in sequential statement.\n");
	(yyval.sequ) = 0;
	yyerrok;
      }
#line 5028 "parse.cc" /* yacc.c:1646  */
    break;

  case 350:
#line 2435 "parse.y" /* yacc.c:1646  */
    { if(!strcasecmp((yyvsp[0].text), "NOTE"))
        (yyval.severity) = ReportStmt::NOTE;
    else if(!strcasecmp((yyvsp[0].text), "WARNING"))
        (yyval.severity) = ReportStmt::WARNING;
    else if(!strcasecmp((yyvsp[0].text), "ERROR"))
        (yyval.severity) = ReportStmt::ERROR;
    else if(!strcasecmp((yyvsp[0].text), "FAILURE"))
        (yyval.severity) = ReportStmt::FAILURE;
    else {
        errormsg((yylsp[-1]), "Invalid severity level (possible values: NOTE, WARNING, ERROR, FAILURE).\n");
        (yyval.severity) = ReportStmt::UNSPECIFIED;
    }
    delete[] (yyvsp[0].text);
  }
#line 5047 "parse.cc" /* yacc.c:1646  */
    break;

  case 351:
#line 2451 "parse.y" /* yacc.c:1646  */
    { (yyval.severity) = (yyvsp[0].severity); }
#line 5053 "parse.cc" /* yacc.c:1646  */
    break;

  case 352:
#line 2452 "parse.y" /* yacc.c:1646  */
    { (yyval.severity) = ReportStmt::UNSPECIFIED; }
#line 5059 "parse.cc" /* yacc.c:1646  */
    break;

  case 354:
#line 2457 "parse.y" /* yacc.c:1646  */
    { ExpShift*tmp = new ExpShift(ExpShift::SRL, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5068 "parse.cc" /* yacc.c:1646  */
    break;

  case 355:
#line 2462 "parse.y" /* yacc.c:1646  */
    { ExpShift*tmp = new ExpShift(ExpShift::SLL, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5077 "parse.cc" /* yacc.c:1646  */
    break;

  case 356:
#line 2467 "parse.y" /* yacc.c:1646  */
    { ExpShift*tmp = new ExpShift(ExpShift::SRA, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5086 "parse.cc" /* yacc.c:1646  */
    break;

  case 357:
#line 2472 "parse.y" /* yacc.c:1646  */
    { ExpShift*tmp = new ExpShift(ExpShift::SLA, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5095 "parse.cc" /* yacc.c:1646  */
    break;

  case 358:
#line 2477 "parse.y" /* yacc.c:1646  */
    { sorrymsg((yylsp[-1]), "ROR is not supported.\n");
        ExpShift*tmp = new ExpShift(ExpShift::ROR, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5105 "parse.cc" /* yacc.c:1646  */
    break;

  case 359:
#line 2483 "parse.y" /* yacc.c:1646  */
    { sorrymsg((yylsp[-1]), "ROL is not supported.\n");
        ExpShift*tmp = new ExpShift(ExpShift::ROL, (yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.expr) = tmp;
      }
#line 5115 "parse.cc" /* yacc.c:1646  */
    break;

  case 362:
#line 2496 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5121 "parse.cc" /* yacc.c:1646  */
    break;

  case 363:
#line 2497 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = 0;  }
#line 5127 "parse.cc" /* yacc.c:1646  */
    break;

  case 364:
#line 2521 "parse.y" /* yacc.c:1646  */
    { sorrymsg((yylsp[-1]), "Unary expression +- not supported.\n");
	(yyval.expr) = (yyvsp[0].expr);
      }
#line 5135 "parse.cc" /* yacc.c:1646  */
    break;

  case 365:
#line 2525 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5141 "parse.cc" /* yacc.c:1646  */
    break;

  case 366:
#line 2530 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5147 "parse.cc" /* yacc.c:1646  */
    break;

  case 367:
#line 2532 "parse.y" /* yacc.c:1646  */
    { Expression*tmp = (yyvsp[-1].expr);
	list<struct adding_term>*lst = (yyvsp[0].adding_terms);
	while (! lst->empty()) {
	      struct adding_term item = lst->front();
	      lst->pop_front();
	      if (item.op == ExpArithmetic::xCONCAT)
		    tmp = new ExpConcat(tmp, item.term);
	      else
		    tmp = new ExpArithmetic(item.op, tmp, item.term);
	}
	delete lst;
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 5166 "parse.cc" /* yacc.c:1646  */
    break;

  case 368:
#line 2550 "parse.y" /* yacc.c:1646  */
    { struct adding_term item;
	item.op = (yyvsp[-1].arithmetic_op);
	item.term = (yyvsp[0].expr);
	list<adding_term>*tmp = new list<adding_term>;
	tmp->push_back(item);
	(yyval.adding_terms) = tmp;
      }
#line 5178 "parse.cc" /* yacc.c:1646  */
    break;

  case 369:
#line 2558 "parse.y" /* yacc.c:1646  */
    { list<adding_term>*tmp = (yyvsp[-2].adding_terms);
	struct adding_term item;
	item.op = (yyvsp[-1].arithmetic_op);
	item.term = (yyvsp[0].expr);
	tmp->push_back(item);
	(yyval.adding_terms) = tmp;
      }
#line 5190 "parse.cc" /* yacc.c:1646  */
    break;

  case 370:
#line 2569 "parse.y" /* yacc.c:1646  */
    { SignalSeqAssignment*tmp = new SignalSeqAssignment((yyvsp[-3].expr), (yyvsp[-1].expr_list));
	FILE_NAME(tmp, (yylsp[-3]));
	delete (yyvsp[-1].expr_list);
	(yyval.sequ) = tmp;
      }
#line 5200 "parse.cc" /* yacc.c:1646  */
    break;

  case 371:
#line 2575 "parse.y" /* yacc.c:1646  */
    { SignalSeqAssignment*tmp = new SignalSeqAssignment((yyvsp[-7].expr), (yyvsp[-5].expr_list));
	FILE_NAME(tmp, (yylsp[-7]));
	sorrymsg((yylsp[-4]), "Conditional signal assignment not supported.\n");
	(yyval.sequ) = tmp;
      }
#line 5210 "parse.cc" /* yacc.c:1646  */
    break;

  case 373:
#line 2585 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-2].text);
	(yyval.sequ) = (yyvsp[0].sequ);
      }
#line 5218 "parse.cc" /* yacc.c:1646  */
    break;

  case 374:
#line 2591 "parse.y" /* yacc.c:1646  */
    { assert(!active_sub);
        active_sub = (yyvsp[-1].subprogram);
        (yyval.subprogram) = (yyvsp[-1].subprogram); }
#line 5226 "parse.cc" /* yacc.c:1646  */
    break;

  case 375:
#line 2603 "parse.y" /* yacc.c:1646  */
    { SubprogramHeader*prog = (yyvsp[-7].subprogram);
	SubprogramHeader*tmp = active_scope->recall_subprogram(prog);
	if (tmp) {
	      delete prog;
	      prog = tmp;
	}

	SubprogramBody*body = new SubprogramBody();
	body->transfer_from(*active_scope, ScopeBase::VARIABLES);
	body->set_statements((yyvsp[-4].sequ_list));

	prog->set_body(body);
	active_scope->bind_subprogram(prog->name(), prog);
	active_sub = NULL;
      }
#line 5246 "parse.cc" /* yacc.c:1646  */
    break;

  case 376:
#line 2623 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-6]), "Syntax errors in subprogram body.\n");
	yyerrok;
	active_sub = NULL;
	if ((yyvsp[-7].subprogram)) delete (yyvsp[-7].subprogram);
	if ((yyvsp[-1].text)) delete[](yyvsp[-1].text);
      }
#line 5257 "parse.cc" /* yacc.c:1646  */
    break;

  case 377:
#line 2633 "parse.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].subprogram)) active_scope->bind_subprogram((yyvsp[-1].subprogram)->name(), (yyvsp[-1].subprogram)); }
#line 5263 "parse.cc" /* yacc.c:1646  */
    break;

  case 388:
#line 2659 "parse.y" /* yacc.c:1646  */
    { (yyval.subprogram) = (yyvsp[0].subprogram); }
#line 5269 "parse.cc" /* yacc.c:1646  */
    break;

  case 389:
#line 2660 "parse.y" /* yacc.c:1646  */
    { (yyval.subprogram) = (yyvsp[0].subprogram); }
#line 5275 "parse.cc" /* yacc.c:1646  */
    break;

  case 390:
#line 2668 "parse.y" /* yacc.c:1646  */
    { (yyval.sequ_list) = (yyvsp[0].sequ_list); }
#line 5281 "parse.cc" /* yacc.c:1646  */
    break;

  case 391:
#line 2673 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-3].text));
	if ((yyvsp[-1].vtype) == 0) {
	      errormsg((yylsp[-4]), "Failed to declare type name %s.\n", name.str());
	} else {
	      VTypeDef*tmp;
	      map<perm_string,VTypeDef*>::iterator cur = active_scope->incomplete_types.find(name);
	      if (cur == active_scope->incomplete_types.end()) {
		    tmp = new VSubTypeDef(name, (yyvsp[-1].vtype));
		    active_scope->bind_name(name, tmp);
	      } else {
		    tmp = cur->second;
		    tmp->set_definition((yyvsp[-1].vtype));
		    active_scope->incomplete_types.erase(cur);
	      }
	}
	delete[](yyvsp[-3].text);
      }
#line 5303 "parse.cc" /* yacc.c:1646  */
    break;

  case 392:
#line 2694 "parse.y" /* yacc.c:1646  */
    { const VType*tmp = parse_type_by_name(lex_strings.make((yyvsp[0].text)));
	if (tmp == 0) {
	      errormsg((yylsp[0]), "Can't find type name `%s'\n", (yyvsp[0].text));
	      tmp = new VTypeERROR;
	}
	delete[](yyvsp[0].text);
	(yyval.vtype) = tmp;
      }
#line 5316 "parse.cc" /* yacc.c:1646  */
    break;

  case 393:
#line 2703 "parse.y" /* yacc.c:1646  */
    { const VType*tmp = calculate_subtype_array((yylsp[-1]), (yyvsp[-1].text), active_scope, (yyvsp[0].range_list));
	if (tmp == 0) {
	      errormsg((yylsp[-1]), "Unable to calculate bounds for array of %s.\n", (yyvsp[-1].text));
	}
	delete[](yyvsp[-1].text);
	delete  (yyvsp[0].range_list);
	(yyval.vtype) = tmp;
      }
#line 5329 "parse.cc" /* yacc.c:1646  */
    break;

  case 394:
#line 2712 "parse.y" /* yacc.c:1646  */
    { const VType*tmp = calculate_subtype_range((yylsp[-4]), (yyvsp[-4].text), active_scope, (yyvsp[-2].expr), (yyvsp[-1].range_dir), (yyvsp[0].expr));
	if (tmp == 0) {
	      errormsg((yylsp[-4]), "Unable to calculate bounds for range of %s.\n", (yyvsp[-4].text));
	}
	delete[](yyvsp[-4].text);
	(yyval.vtype) = tmp;
      }
#line 5341 "parse.cc" /* yacc.c:1646  */
    break;

  case 395:
#line 2723 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text); }
#line 5347 "parse.cc" /* yacc.c:1646  */
    break;

  case 396:
#line 2725 "parse.y" /* yacc.c:1646  */
    { (yyval.text) = (yyvsp[0].text); }
#line 5353 "parse.cc" /* yacc.c:1646  */
    break;

  case 397:
#line 2727 "parse.y" /* yacc.c:1646  */
    { //do not have now better idea than using char constant
	(yyval.text) = strdup("all");
      }
#line 5361 "parse.cc" /* yacc.c:1646  */
    break;

  case 398:
#line 2734 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5367 "parse.cc" /* yacc.c:1646  */
    break;

  case 399:
#line 2736 "parse.y" /* yacc.c:1646  */
    { ExpArithmetic*tmp = new ExpArithmetic(ExpArithmetic::MULT, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 5376 "parse.cc" /* yacc.c:1646  */
    break;

  case 400:
#line 2741 "parse.y" /* yacc.c:1646  */
    { ExpArithmetic*tmp = new ExpArithmetic(ExpArithmetic::DIV, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 5385 "parse.cc" /* yacc.c:1646  */
    break;

  case 401:
#line 2746 "parse.y" /* yacc.c:1646  */
    { ExpArithmetic*tmp = new ExpArithmetic(ExpArithmetic::MOD, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 5394 "parse.cc" /* yacc.c:1646  */
    break;

  case 402:
#line 2751 "parse.y" /* yacc.c:1646  */
    { ExpArithmetic*tmp = new ExpArithmetic(ExpArithmetic::REM, (yyvsp[-2].expr), (yyvsp[0].expr));
	FILE_NAME(tmp, (yylsp[-1]));
	(yyval.expr) = tmp;
      }
#line 5403 "parse.cc" /* yacc.c:1646  */
    break;

  case 403:
#line 2759 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-3].text));
	if ((yyvsp[-1].vtype) == 0) {
	      errormsg((yylsp[-4]), "Failed to declare type name %s.\n", name.str());
	} else {
	      VTypeDef*tmp;
	      map<perm_string,VTypeDef*>::iterator cur = active_scope->incomplete_types.find(name);
	      if (cur == active_scope->incomplete_types.end()) {
		    tmp = new VTypeDef(name, (yyvsp[-1].vtype));
		    active_scope->bind_name(name, tmp);
	      } else {
		    tmp = cur->second;
		    tmp->set_definition((yyvsp[-1].vtype));
		    active_scope->incomplete_types.erase(cur);
	      }
	}
	delete[](yyvsp[-3].text);
      }
#line 5425 "parse.cc" /* yacc.c:1646  */
    break;

  case 404:
#line 2777 "parse.y" /* yacc.c:1646  */
    { perm_string name = lex_strings.make((yyvsp[-1].text));
	VTypeDef*tmp = new VTypeDef(name);
	active_scope->incomplete_types[name] = tmp;
	active_scope->bind_name(name, tmp);
	delete[](yyvsp[-1].text);
      }
#line 5436 "parse.cc" /* yacc.c:1646  */
    break;

  case 405:
#line 2784 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Error in type definition for %s\n", (yyvsp[-3].text));
	yyerrok;
	delete[](yyvsp[-3].text);
      }
#line 5445 "parse.cc" /* yacc.c:1646  */
    break;

  case 406:
#line 2789 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Error in type definition\n");
	yyerrok;
      }
#line 5453 "parse.cc" /* yacc.c:1646  */
    break;

  case 407:
#line 2796 "parse.y" /* yacc.c:1646  */
    { VTypeEnum*tmp = new VTypeEnum((yyvsp[-1].name_list));
	active_scope->use_enum(tmp);
	delete (yyvsp[-1].name_list);
	(yyval.vtype) = tmp;
      }
#line 5463 "parse.cc" /* yacc.c:1646  */
    break;

  case 408:
#line 2802 "parse.y" /* yacc.c:1646  */
    { (yyval.vtype) = (yyvsp[0].vtype); }
#line 5469 "parse.cc" /* yacc.c:1646  */
    break;

  case 409:
#line 2808 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[-1].expr_list); }
#line 5475 "parse.cc" /* yacc.c:1646  */
    break;

  case 410:
#line 2810 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in use clause.\n"); yyerrok; }
#line 5481 "parse.cc" /* yacc.c:1646  */
    break;

  case 412:
#line 2816 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in use clause.\n"); yyerrok; }
#line 5487 "parse.cc" /* yacc.c:1646  */
    break;

  case 418:
#line 2832 "parse.y" /* yacc.c:1646  */
    { delete[] (yyvsp[-2].text);
	(yyval.sequ) = (yyvsp[0].sequ);
      }
#line 5495 "parse.cc" /* yacc.c:1646  */
    break;

  case 419:
#line 2838 "parse.y" /* yacc.c:1646  */
    { VariableSeqAssignment*tmp = new VariableSeqAssignment((yyvsp[-3].expr), (yyvsp[-1].expr));
	FILE_NAME(tmp, (yylsp[-3]));
	(yyval.sequ) = tmp;
      }
#line 5504 "parse.cc" /* yacc.c:1646  */
    break;

  case 420:
#line 2843 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-1]), "Syntax error in r-value expression of assignment.\n");
	yyerrok;
	delete (yyvsp[-3].expr);
	(yyval.sequ) = 0;
      }
#line 5514 "parse.cc" /* yacc.c:1646  */
    break;

  case 421:
#line 2849 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-3]), "Syntax error in l-value expression of assignment.\n");
	yyerrok;
	delete (yyvsp[-1].expr);
	(yyval.sequ) = 0;
      }
#line 5524 "parse.cc" /* yacc.c:1646  */
    break;

  case 422:
#line 2859 "parse.y" /* yacc.c:1646  */
    { /* Save the signal declaration in the block_signals map. */
	for (std::list<perm_string>::iterator cur = (yyvsp[-4].name_list)->begin()
		   ; cur != (yyvsp[-4].name_list)->end() ; ++cur) {
	      Variable*sig = new Variable(*cur, (yyvsp[-2].vtype), (yyvsp[-1].expr));
	      FILE_NAME(sig, (yylsp[-5]));
	      active_scope->bind_name(*cur, sig);
	}
	delete (yyvsp[-4].name_list);
      }
#line 5538 "parse.cc" /* yacc.c:1646  */
    break;

  case 423:
#line 2869 "parse.y" /* yacc.c:1646  */
    { errormsg((yylsp[-2]), "Syntax error in variable declaration.\n");
	yyerrok;
      }
#line 5546 "parse.cc" /* yacc.c:1646  */
    break;

  case 424:
#line 2875 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5552 "parse.cc" /* yacc.c:1646  */
    break;

  case 425:
#line 2876 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = 0;  }
#line 5558 "parse.cc" /* yacc.c:1646  */
    break;

  case 426:
#line 2881 "parse.y" /* yacc.c:1646  */
    { WaitForStmt*tmp = new WaitForStmt((yyvsp[-1].expr));
	FILE_NAME(tmp, (yylsp[-3]));
	(yyval.sequ) = tmp;
      }
#line 5567 "parse.cc" /* yacc.c:1646  */
    break;

  case 427:
#line 2886 "parse.y" /* yacc.c:1646  */
    { WaitStmt*tmp = new WaitStmt(WaitStmt::ON, (yyvsp[-1].expr));
        FILE_NAME(tmp, (yylsp[-3]));
        (yyval.sequ) = tmp;
      }
#line 5576 "parse.cc" /* yacc.c:1646  */
    break;

  case 428:
#line 2891 "parse.y" /* yacc.c:1646  */
    { WaitStmt*tmp = new WaitStmt(WaitStmt::UNTIL, (yyvsp[-1].expr));
        FILE_NAME(tmp, (yylsp[-3]));
        (yyval.sequ) = tmp;
      }
#line 5585 "parse.cc" /* yacc.c:1646  */
    break;

  case 429:
#line 2896 "parse.y" /* yacc.c:1646  */
    { WaitStmt*tmp = new WaitStmt(WaitStmt::FINAL, NULL);
        FILE_NAME(tmp, (yylsp[-1]));
        (yyval.sequ) = tmp;
      }
#line 5594 "parse.cc" /* yacc.c:1646  */
    break;

  case 430:
#line 2904 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = (yyvsp[0].expr_list); }
#line 5600 "parse.cc" /* yacc.c:1646  */
    break;

  case 431:
#line 2906 "parse.y" /* yacc.c:1646  */
    { (yyval.expr_list) = 0; }
#line 5606 "parse.cc" /* yacc.c:1646  */
    break;

  case 432:
#line 2911 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>*tmp = (yyvsp[-2].expr_list);
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 5615 "parse.cc" /* yacc.c:1646  */
    break;

  case 433:
#line 2916 "parse.y" /* yacc.c:1646  */
    { std::list<Expression*>*tmp = new std::list<Expression*>;
	tmp->push_back((yyvsp[0].expr));
	(yyval.expr_list) = tmp;
      }
#line 5624 "parse.cc" /* yacc.c:1646  */
    break;

  case 434:
#line 2924 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 5630 "parse.cc" /* yacc.c:1646  */
    break;

  case 435:
#line 2926 "parse.y" /* yacc.c:1646  */
    { ExpDelay*tmp = new ExpDelay((yyvsp[-2].expr), (yyvsp[0].expr));
        FILE_NAME(tmp, (yylsp[-2]));
        (yyval.expr) = tmp; }
#line 5638 "parse.cc" /* yacc.c:1646  */
    break;

  case 436:
#line 2930 "parse.y" /* yacc.c:1646  */
    { (yyval.expr) = 0; }
#line 5644 "parse.cc" /* yacc.c:1646  */
    break;


#line 5648 "parse.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, yyscanner, file_path, parse_library_name, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, yyscanner, file_path, parse_library_name, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, yyscanner, file_path, parse_library_name);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, yyscanner, file_path, parse_library_name);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, yyscanner, file_path, parse_library_name, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, yyscanner, file_path, parse_library_name);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, yyscanner, file_path, parse_library_name);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 2942 "parse.y" /* yacc.c:1906  */


static void yyerror(YYLTYPE*loc, yyscan_t, const char*, bool, const char*msg)
{
      fprintf(stderr, "%s:%u: %s\n", loc->text, loc->first_line, msg);
      parse_errors += 1;
}

void errormsg(const YYLTYPE&loc, const char*fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);

      fprintf(stderr, "%s:%u: error: ", loc.text, loc.first_line);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
      parse_errors += 1;
}

void sorrymsg(const YYLTYPE&loc, const char*fmt, ...)
{
      va_list ap;
      va_start(ap, fmt);

      fprintf(stderr, "%s:%u: sorry: ", loc.text, loc.first_line);
      vfprintf(stderr, fmt, ap);
      va_end(ap);
      parse_sorrys += 1;
}


/*
 * The reset_lexor function takes the fd and makes it the input file
 * for the lexor. The path argument is used in lexor/parser error messages.
 */
extern yyscan_t prepare_lexor(FILE*fd);
extern void destroy_lexor(yyscan_t scanner);

int parse_source_file(const char*file_path, perm_string parse_library_name)
{
      FILE*fd = fopen(file_path, "r");
      if (fd == 0) {
	    perror(file_path);
	    return -1;
      }

      yyscan_t scanner = prepare_lexor(fd);
      int rc = yyparse(scanner, file_path, parse_library_name);
      fclose(fd);
      destroy_lexor(scanner);

      return rc;
}
