#ifndef PARSER_HEADER_FILE
#define PARSER_HEADER_FILE

#include<vector>
#include<string>


class Name;
class Function_call;
class ListAssociation_element;
class ListExpression;
class Association_element;
class Actual_part;
class Acutal_part;
class Expression;
class Actual_designator;
class Acutal_designator;
class Prefix;
class Selected_name;
class Suffix;
class Attribute_name;
class Slice_name;
class Indexed_name;
class Character_literal;
class Graphic_character;
class Operator_symbol;
class Simple_name;

typedef union
{
  int int_;
  char char_;
  double double_;
  char* string_;
  Name* name_;
  Function_call* function_call_;
  ListAssociation_element* listassociation_element_;
  ListExpression* listexpression_;
  Association_element* association_element_;
  Actual_part* actual_part_;
  Acutal_part* acutal_part_;
  Expression* expression_;
  Actual_designator* actual_designator_;
  Acutal_designator* acutal_designator_;
  Prefix* prefix_;
  Selected_name* selected_name_;
  Suffix* suffix_;
  Attribute_name* attribute_name_;
  Slice_name* slice_name_;
  Indexed_name* indexed_name_;
  Character_literal* character_literal_;
  Graphic_character* graphic_character_;
  Operator_symbol* operator_symbol_;
  Simple_name* simple_name_;
} YYSTYPE;

Name* pName(FILE *inp);
Name* pName(const char *str);
Function_call* pFunction_call(FILE *inp);
Function_call* pFunction_call(const char *str);
ListAssociation_element* pListAssociation_element(FILE *inp);
ListAssociation_element* pListAssociation_element(const char *str);
ListExpression* pListExpression(FILE *inp);
ListExpression* pListExpression(const char *str);
Association_element* pAssociation_element(FILE *inp);
Association_element* pAssociation_element(const char *str);
Actual_part* pActual_part(FILE *inp);
Actual_part* pActual_part(const char *str);
Acutal_part* pAcutal_part(FILE *inp);
Acutal_part* pAcutal_part(const char *str);
Expression* pExpression(FILE *inp);
Expression* pExpression(const char *str);
Actual_designator* pActual_designator(FILE *inp);
Actual_designator* pActual_designator(const char *str);
Acutal_designator* pAcutal_designator(FILE *inp);
Acutal_designator* pAcutal_designator(const char *str);
Prefix* pPrefix(FILE *inp);
Prefix* pPrefix(const char *str);
Selected_name* pSelected_name(FILE *inp);
Selected_name* pSelected_name(const char *str);
Suffix* pSuffix(FILE *inp);
Suffix* pSuffix(const char *str);
Attribute_name* pAttribute_name(FILE *inp);
Attribute_name* pAttribute_name(const char *str);
Slice_name* pSlice_name(FILE *inp);
Slice_name* pSlice_name(const char *str);
Indexed_name* pIndexed_name(FILE *inp);
Indexed_name* pIndexed_name(const char *str);
Character_literal* pCharacter_literal(FILE *inp);
Character_literal* pCharacter_literal(const char *str);
Graphic_character* pGraphic_character(FILE *inp);
Graphic_character* pGraphic_character(const char *str);
Operator_symbol* pOperator_symbol(FILE *inp);
Operator_symbol* pOperator_symbol(const char *str);
Simple_name* pSimple_name(FILE *inp);
Simple_name* pSimple_name(const char *str);



#define _ERROR_ 258
#define _SYMB_0 259
#define _SYMB_1 260
#define _SYMB_2 261
#define _SYMB_3 262
#define _SYMB_4 263
#define _SYMB_5 264
#define _SYMB_6 265
#define _SYMB_7 266
#define _SYMB_8 267
#define _SYMB_9 268
#define _STRING_ 269
#define _CHAR_ 270
#define _INTEGER_ 271
#define _IDENT_ 272

extern YYSTYPE yylval;

#endif
