#ifndef PRINTER_HEADER
#define PRINTER_HEADER

#include "Absyn.H"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Certain applications may improve performance by changing the buffer size */
#define BUFFER_INITIAL 2000
/* You may wish to change _L_PAREN or _R_PAREN */
#define _L_PAREN '('
#define _R_PAREN ')'

class PrintAbsyn : public Visitor
{
 protected:
  int _n_, _i_;
  /* The following are simple heuristics for rendering terminals */
  /* You may wish to change them */
  void render(Char c);
  void render(String s);
void render(char *s);
  void indent(void);
  void backup(void);
 public:
  PrintAbsyn(void);
  ~PrintAbsyn(void);
  char *print(Visitable *v);

  void visitName(Name *p); /* abstract class */
  void visitNameSimple(NameSimple *p);
  void visitNameOp(NameOp *p);
  void visitNameChar(NameChar *p);
  void visitNameSelected(NameSelected *p);
  void visitNameIndexed(NameIndexed *p);
  void visitNameSlice(NameSlice *p);
  void visitNameAttr(NameAttr *p);
  void visitFunction_call(Function_call *p); /* abstract class */
  void visitFunctionCallNyadic(FunctionCallNyadic *p);
  void visitFunctionCallPolyadic(FunctionCallPolyadic *p);
  void visitListAssociation_element(ListAssociation_element *p);
  void visitListExpression(ListExpression *p);
  void visitAssociation_element(Association_element *p); /* abstract class */
  void visitAssocElementPositional(AssocElementPositional *p);
  void visitAssocElementNamedParam(AssocElementNamedParam *p);
  void visitActual_part(Actual_part *p); /* abstract class */
  void visitActualPartD(ActualPartD *p);
  void visitAcutal_part(Acutal_part *p); /* abstract class */
  void visitActualFoo(ActualFoo *p);
  void visitExpression(Expression *p); /* abstract class */
  void visitExpressionName(ExpressionName *p);
  void visitExpressionNumber(ExpressionNumber *p);
  void visitActual_designator(Actual_designator *p); /* abstract class */
  void visitExpressionDesignator(ExpressionDesignator *p);
  void visitAcutal_designator(Acutal_designator *p); /* abstract class */
  void visitNameDesignator(NameDesignator *p);
  void visitPrefix(Prefix *p); /* abstract class */
  void visitNamePrefix(NamePrefix *p);
  void visitFunctionPrefix(FunctionPrefix *p);
  void visitSelected_name(Selected_name *p); /* abstract class */
  void visitSelectedName(SelectedName *p);
  void visitSuffix(Suffix *p); /* abstract class */
  void visitAllSuffix(AllSuffix *p);
  void visitAllSuffixfoo(AllSuffixfoo *p);
  void visitNameSuffix(NameSuffix *p);
  void visitAttribute_name(Attribute_name *p); /* abstract class */
  void visitAttributeName(AttributeName *p);
  void visitSlice_name(Slice_name *p); /* abstract class */
  void visitSliceName(SliceName *p);
  void visitIndexed_name(Indexed_name *p); /* abstract class */
  void visitIndexedName(IndexedName *p);
  void visitCharacter_literal(Character_literal *p); /* abstract class */
  void visitCharLiteral(CharLiteral *p);
  void visitGraphic_character(Graphic_character *p); /* abstract class */
  void visitGraphicChar(GraphicChar *p);
  void visitOperator_symbol(Operator_symbol *p); /* abstract class */
  void visitOperatorSymbol(OperatorSymbol *p);
  void visitSimple_name(Simple_name *p); /* abstract class */
  void visitSimpleName(SimpleName *p);

  void visitInteger(Integer i);
  void visitDouble(Double d);
  void visitChar(Char c);
  void visitString(String s);
  void visitIdent(String s);
  void visitDecimal_Literal(String s);
  void visitBase_Spec(String s);
 protected:
  void inline bufAppend(const char *s)
  {
    int len = strlen(s);
    while (cur_ + len > buf_size)
    {
      buf_size *= 2; /* Double the buffer size */
      resizeBuffer();
    }
    for(int n = 0; n < len; n++)
    {
      buf_[cur_ + n] = s[n];
    }
    cur_ += len;
    buf_[cur_] = 0;
  }

  void inline bufAppend(const char c)
  {
    if (cur_ == buf_size)
    {
      buf_size *= 2; /* Double the buffer size */
      resizeBuffer();
    }
    buf_[cur_] = c;
    cur_++;
    buf_[cur_] = 0;
  }

  void inline bufAppend(String str)
  {
    const char *s = str.c_str();
    bufAppend(s);
  }
  void inline bufReset(void)
  {
    cur_ = 0;
    buf_size = BUFFER_INITIAL;
    resizeBuffer();
    memset(buf_, 0, buf_size);
  }

  void inline resizeBuffer(void)
  {
    char *temp = (char *) malloc(buf_size);
    if (!temp)
    {
      fprintf(stderr, "Error: Out of memory while attempting to grow buffer!\n");
      exit(1);
    }
    if (buf_)
    {
      strcpy(temp, buf_);
      free(buf_);
    }
    buf_ = temp;
  }
  char *buf_;
  int cur_, buf_size;
};



class ShowAbsyn : public Visitor
{
 public:
  ShowAbsyn(void);
  ~ShowAbsyn(void);
  char *show(Visitable *v);

  void visitName(Name *p); /* abstract class */
  void visitNameSimple(NameSimple *p);
  void visitNameOp(NameOp *p);
  void visitNameChar(NameChar *p);
  void visitNameSelected(NameSelected *p);
  void visitNameIndexed(NameIndexed *p);
  void visitNameSlice(NameSlice *p);
  void visitNameAttr(NameAttr *p);
  void visitFunction_call(Function_call *p); /* abstract class */
  void visitFunctionCallNyadic(FunctionCallNyadic *p);
  void visitFunctionCallPolyadic(FunctionCallPolyadic *p);
  void visitListAssociation_element(ListAssociation_element *p);
  void visitListExpression(ListExpression *p);
  void visitAssociation_element(Association_element *p); /* abstract class */
  void visitAssocElementPositional(AssocElementPositional *p);
  void visitAssocElementNamedParam(AssocElementNamedParam *p);
  void visitActual_part(Actual_part *p); /* abstract class */
  void visitActualPartD(ActualPartD *p);
  void visitAcutal_part(Acutal_part *p); /* abstract class */
  void visitActualFoo(ActualFoo *p);
  void visitExpression(Expression *p); /* abstract class */
  void visitExpressionName(ExpressionName *p);
  void visitExpressionNumber(ExpressionNumber *p);
  void visitActual_designator(Actual_designator *p); /* abstract class */
  void visitExpressionDesignator(ExpressionDesignator *p);
  void visitAcutal_designator(Acutal_designator *p); /* abstract class */
  void visitNameDesignator(NameDesignator *p);
  void visitPrefix(Prefix *p); /* abstract class */
  void visitNamePrefix(NamePrefix *p);
  void visitFunctionPrefix(FunctionPrefix *p);
  void visitSelected_name(Selected_name *p); /* abstract class */
  void visitSelectedName(SelectedName *p);
  void visitSuffix(Suffix *p); /* abstract class */
  void visitAllSuffix(AllSuffix *p);
  void visitAllSuffixfoo(AllSuffixfoo *p);
  void visitNameSuffix(NameSuffix *p);
  void visitAttribute_name(Attribute_name *p); /* abstract class */
  void visitAttributeName(AttributeName *p);
  void visitSlice_name(Slice_name *p); /* abstract class */
  void visitSliceName(SliceName *p);
  void visitIndexed_name(Indexed_name *p); /* abstract class */
  void visitIndexedName(IndexedName *p);
  void visitCharacter_literal(Character_literal *p); /* abstract class */
  void visitCharLiteral(CharLiteral *p);
  void visitGraphic_character(Graphic_character *p); /* abstract class */
  void visitGraphicChar(GraphicChar *p);
  void visitOperator_symbol(Operator_symbol *p); /* abstract class */
  void visitOperatorSymbol(OperatorSymbol *p);
  void visitSimple_name(Simple_name *p); /* abstract class */
  void visitSimpleName(SimpleName *p);

  void visitInteger(Integer i);
  void visitDouble(Double d);
  void visitChar(Char c);
  void visitString(String s);
  void visitIdent(String s);
  void visitDecimal_Literal(String s);
  void visitBase_Spec(String s);
 protected:
  void inline bufAppend(const char *s)
  {
    int len = strlen(s);
    while (cur_ + len > buf_size)
    {
      buf_size *= 2; /* Double the buffer size */
      resizeBuffer();
    }
    for(int n = 0; n < len; n++)
    {
      buf_[cur_ + n] = s[n];
    }
    cur_ += len;
    buf_[cur_] = 0;
  }

  void inline bufAppend(const char c)
  {
    if (cur_ == buf_size)
    {
      buf_size *= 2; /* Double the buffer size */
      resizeBuffer();
    }
    buf_[cur_] = c;
    cur_++;
    buf_[cur_] = 0;
  }

  void inline bufAppend(String str)
  {
    const char *s = str.c_str();
    bufAppend(s);
  }
  void inline bufReset(void)
  {
    cur_ = 0;
    buf_size = BUFFER_INITIAL;
    resizeBuffer();
    memset(buf_, 0, buf_size);
  }

  void inline resizeBuffer(void)
  {
    char *temp = (char *) malloc(buf_size);
    if (!temp)
    {
      fprintf(stderr, "Error: Out of memory while attempting to grow buffer!\n");
      exit(1);
    }
    if (buf_)
    {
      strcpy(temp, buf_);
      free(buf_);
    }
    buf_ = temp;
  }
  char *buf_;
  int cur_, buf_size;
};




#endif

