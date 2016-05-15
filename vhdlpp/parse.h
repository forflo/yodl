/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
#line 210 "parse.y" /* yacc.c:1909  */

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

#line 255 "parse.hh" /* yacc.c:1909  */
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
