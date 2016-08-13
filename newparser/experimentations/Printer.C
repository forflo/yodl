/*** BNFC-Generated Pretty Printer and Abstract Syntax Viewer ***/

#include <string>
#include "Printer.H"
#define INDENT_WIDTH 2


//You may wish to change render
void PrintAbsyn::render(Char c)
{
  if (c == '{')
  {
     bufAppend('\n');
     indent();
     bufAppend(c);
     _n_ = _n_ + INDENT_WIDTH;
     bufAppend('\n');
     indent();
  }
  else if (c == '(' || c == '[')
     bufAppend(c);
  else if (c == ')' || c == ']')
  {
     backup();
     bufAppend(c);
  }
  else if (c == '}')
  {
     int t;
     _n_ = _n_ - INDENT_WIDTH;
     for (t=0; t<INDENT_WIDTH; t++) {
       backup();
     }
     bufAppend(c);
     bufAppend('\n');
     indent();
  }
  else if (c == ',')
  {
     backup();
     bufAppend(c);
     bufAppend(' ');
  }
  else if (c == ';')
  {
     backup();
     bufAppend(c);
     bufAppend('\n');
     indent();
  }
  else if (c == 0) return;
  else
  {
     bufAppend(' ');
     bufAppend(c);
     bufAppend(' ');
  }
}

void PrintAbsyn::render(String s_)
{
  const char *s = s_.c_str() ;
  if(strlen(s) > 0)
  {
    bufAppend(s);
    bufAppend(' ');
  }
}
void PrintAbsyn::render(char *s)
{
  if(strlen(s) > 0)
  {
    bufAppend(s);
    bufAppend(' ');
  }
}

void PrintAbsyn::indent()
{
  int n = _n_;
  while (n > 0)
  {
    bufAppend(' ');
    n--;
  }
}

void PrintAbsyn::backup()
{
  if (buf_[cur_ - 1] == ' ')
  {
    buf_[cur_ - 1] = 0;
    cur_--;
  }
}

PrintAbsyn::PrintAbsyn(void)
{
  _i_ = 0; _n_ = 0;
  buf_ = 0;
  bufReset();
}

PrintAbsyn::~PrintAbsyn(void)
{
}

char *PrintAbsyn::print(Visitable *v)
{
  _i_ = 0; _n_ = 0;
  bufReset();
  v->accept(this);
  return buf_;
}

void PrintAbsyn::visitName(Name *p) {} //abstract class

void PrintAbsyn::visitNameSimple(NameSimple *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->simple_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameOp(NameOp *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->operator_symbol_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameChar(NameChar *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->character_literal_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameSelected(NameSelected *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->selected_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameIndexed(NameIndexed *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->indexed_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameSlice(NameSlice *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->slice_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameAttr(NameAttr *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->attribute_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitFunction_call(Function_call *p) {} //abstract class

void PrintAbsyn::visitFunctionCallNyadic(FunctionCallNyadic *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitFunctionCallPolyadic(FunctionCallPolyadic *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);
  render('(');
  if(p->listassociation_element_) {_i_ = 0; p->listassociation_element_->accept(this);}
  render(')');

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitListAssociation_element(ListAssociation_element *listassociation_element)
{
  for (ListAssociation_element::const_iterator i = listassociation_element->begin() ; i != listassociation_element->end() ; ++i)
  {
    (*i)->accept(this);
    if (i != listassociation_element->end() - 1) render(',');
  }
}void PrintAbsyn::visitListExpression(ListExpression *listexpression)
{
  for (ListExpression::const_iterator i = listexpression->begin() ; i != listexpression->end() ; ++i)
  {
    (*i)->accept(this);
    if (i != listexpression->end() - 1) render(',');
  }
}void PrintAbsyn::visitAssociation_element(Association_element *p) {} //abstract class

void PrintAbsyn::visitAssocElementPositional(AssocElementPositional *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->actual_part_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitAssocElementNamedParam(AssocElementNamedParam *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);
  render("=>");
  _i_ = 0; p->actual_part_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitActual_part(Actual_part *p) {} //abstract class

void PrintAbsyn::visitActualPartD(ActualPartD *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->actual_designator_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitAcutal_part(Acutal_part *p) {} //abstract class

void PrintAbsyn::visitActualFoo(ActualFoo *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);
  render('(');
  _i_ = 0; p->actual_designator_->accept(this);
  render(')');

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitExpression(Expression *p) {} //abstract class

void PrintAbsyn::visitExpressionName(ExpressionName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitExpressionNumber(ExpressionNumber *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  visitInteger(p->integer_);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitActual_designator(Actual_designator *p) {} //abstract class

void PrintAbsyn::visitExpressionDesignator(ExpressionDesignator *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->expression_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitAcutal_designator(Acutal_designator *p) {} //abstract class

void PrintAbsyn::visitNameDesignator(NameDesignator *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitPrefix(Prefix *p) {} //abstract class

void PrintAbsyn::visitNamePrefix(NamePrefix *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitFunctionPrefix(FunctionPrefix *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->function_call_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitSelected_name(Selected_name *p) {} //abstract class

void PrintAbsyn::visitSelectedName(SelectedName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->prefix_->accept(this);
  render('.');
  _i_ = 0; p->suffix_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitSuffix(Suffix *p) {} //abstract class

void PrintAbsyn::visitAllSuffix(AllSuffix *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  render("all");

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitAllSuffixfoo(AllSuffixfoo *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  render("foobar");
  render("all");

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitNameSuffix(NameSuffix *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->simple_name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitAttribute_name(Attribute_name *p) {} //abstract class

void PrintAbsyn::visitAttributeName(AttributeName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->prefix_->accept(this);
  render('\'');
  _i_ = 0; p->name_->accept(this);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitSlice_name(Slice_name *p) {} //abstract class

void PrintAbsyn::visitSliceName(SliceName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->prefix_->accept(this);
  render('(');
  _i_ = 0; p->expression_->accept(this);
  render(')');

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitIndexed_name(Indexed_name *p) {} //abstract class

void PrintAbsyn::visitIndexedName(IndexedName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  _i_ = 0; p->prefix_->accept(this);
  render('(');
  if(p->listexpression_) {_i_ = 0; p->listexpression_->accept(this);}
  render(')');

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitCharacter_literal(Character_literal *p) {} //abstract class

void PrintAbsyn::visitCharLiteral(CharLiteral *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  render('\'');
  visitChar(p->char_);
  render('\'');

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitGraphic_character(Graphic_character *p) {} //abstract class

void PrintAbsyn::visitGraphicChar(GraphicChar *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  visitChar(p->char_);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitOperator_symbol(Operator_symbol *p) {} //abstract class

void PrintAbsyn::visitOperatorSymbol(OperatorSymbol *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  visitString(p->string_);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitSimple_name(Simple_name *p) {} //abstract class

void PrintAbsyn::visitSimpleName(SimpleName *p)
{
  int oldi = _i_;
  if (oldi > 0) render(_L_PAREN);

  visitIdent(p->ident_);

  if (oldi > 0) render(_R_PAREN);

  _i_ = oldi;
}

void PrintAbsyn::visitInteger(Integer i)
{
  char tmp[16];
  sprintf(tmp, "%d", i);
  bufAppend(tmp);
}

void PrintAbsyn::visitDouble(Double d)
{
  char tmp[16];
  sprintf(tmp, "%g", d);
  bufAppend(tmp);
}

void PrintAbsyn::visitChar(Char c)
{
  bufAppend('\'');
  bufAppend(c);
  bufAppend('\'');
}

void PrintAbsyn::visitString(String s)
{
  bufAppend('\"');
  bufAppend(s);
  bufAppend('\"');
}

void PrintAbsyn::visitIdent(String s)
{
  render(s);
}

void PrintAbsyn::visitDecimal_Literal(String s)
{
  render(s);
}


void PrintAbsyn::visitBase_Spec(String s)
{
  render(s);
}


ShowAbsyn::ShowAbsyn(void)
{
  buf_ = 0;
  bufReset();
}

ShowAbsyn::~ShowAbsyn(void)
{
}

char *ShowAbsyn::show(Visitable *v)
{
  bufReset();
  v->accept(this);
  return buf_;
}

void ShowAbsyn::visitName(Name *p) {} //abstract class

void ShowAbsyn::visitNameSimple(NameSimple *p)
{
  bufAppend('(');
  bufAppend("NameSimple");
  bufAppend(' ');
  bufAppend('[');
  if (p->simple_name_)  p->simple_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameOp(NameOp *p)
{
  bufAppend('(');
  bufAppend("NameOp");
  bufAppend(' ');
  bufAppend('[');
  if (p->operator_symbol_)  p->operator_symbol_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameChar(NameChar *p)
{
  bufAppend('(');
  bufAppend("NameChar");
  bufAppend(' ');
  bufAppend('[');
  if (p->character_literal_)  p->character_literal_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameSelected(NameSelected *p)
{
  bufAppend('(');
  bufAppend("NameSelected");
  bufAppend(' ');
  bufAppend('[');
  if (p->selected_name_)  p->selected_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameIndexed(NameIndexed *p)
{
  bufAppend('(');
  bufAppend("NameIndexed");
  bufAppend(' ');
  bufAppend('[');
  if (p->indexed_name_)  p->indexed_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameSlice(NameSlice *p)
{
  bufAppend('(');
  bufAppend("NameSlice");
  bufAppend(' ');
  bufAppend('[');
  if (p->slice_name_)  p->slice_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitNameAttr(NameAttr *p)
{
  bufAppend('(');
  bufAppend("NameAttr");
  bufAppend(' ');
  bufAppend('[');
  if (p->attribute_name_)  p->attribute_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitFunction_call(Function_call *p) {} //abstract class

void ShowAbsyn::visitFunctionCallNyadic(FunctionCallNyadic *p)
{
  bufAppend('(');
  bufAppend("FunctionCallNyadic");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitFunctionCallPolyadic(FunctionCallPolyadic *p)
{
  bufAppend('(');
  bufAppend("FunctionCallPolyadic");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->listassociation_element_)  p->listassociation_element_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend(')');
}
void ShowAbsyn::visitListAssociation_element(ListAssociation_element *listassociation_element)
{
  for (ListAssociation_element::const_iterator i = listassociation_element->begin() ; i != listassociation_element->end() ; ++i)
  {
    (*i)->accept(this);
    if (i != listassociation_element->end() - 1) bufAppend(", ");
  }
}

void ShowAbsyn::visitListExpression(ListExpression *listexpression)
{
  for (ListExpression::const_iterator i = listexpression->begin() ; i != listexpression->end() ; ++i)
  {
    (*i)->accept(this);
    if (i != listexpression->end() - 1) bufAppend(", ");
  }
}

void ShowAbsyn::visitAssociation_element(Association_element *p) {} //abstract class

void ShowAbsyn::visitAssocElementPositional(AssocElementPositional *p)
{
  bufAppend('(');
  bufAppend("AssocElementPositional");
  bufAppend(' ');
  bufAppend('[');
  if (p->actual_part_)  p->actual_part_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitAssocElementNamedParam(AssocElementNamedParam *p)
{
  bufAppend('(');
  bufAppend("AssocElementNamedParam");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->actual_part_)  p->actual_part_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitActual_part(Actual_part *p) {} //abstract class

void ShowAbsyn::visitActualPartD(ActualPartD *p)
{
  bufAppend('(');
  bufAppend("ActualPartD");
  bufAppend(' ');
  bufAppend('[');
  if (p->actual_designator_)  p->actual_designator_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitAcutal_part(Acutal_part *p) {} //abstract class

void ShowAbsyn::visitActualFoo(ActualFoo *p)
{
  bufAppend('(');
  bufAppend("ActualFoo");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->actual_designator_)  p->actual_designator_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend(')');
}
void ShowAbsyn::visitExpression(Expression *p) {} //abstract class

void ShowAbsyn::visitExpressionName(ExpressionName *p)
{
  bufAppend('(');
  bufAppend("ExpressionName");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitExpressionNumber(ExpressionNumber *p)
{
  bufAppend('(');
  bufAppend("ExpressionNumber");
  bufAppend(' ');
  visitInteger(p->integer_);
  bufAppend(')');
}
void ShowAbsyn::visitActual_designator(Actual_designator *p) {} //abstract class

void ShowAbsyn::visitExpressionDesignator(ExpressionDesignator *p)
{
  bufAppend('(');
  bufAppend("ExpressionDesignator");
  bufAppend(' ');
  bufAppend('[');
  if (p->expression_)  p->expression_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitAcutal_designator(Acutal_designator *p) {} //abstract class

void ShowAbsyn::visitNameDesignator(NameDesignator *p)
{
  bufAppend('(');
  bufAppend("NameDesignator");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitPrefix(Prefix *p) {} //abstract class

void ShowAbsyn::visitNamePrefix(NamePrefix *p)
{
  bufAppend('(');
  bufAppend("NamePrefix");
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitFunctionPrefix(FunctionPrefix *p)
{
  bufAppend('(');
  bufAppend("FunctionPrefix");
  bufAppend(' ');
  bufAppend('[');
  if (p->function_call_)  p->function_call_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitSelected_name(Selected_name *p) {} //abstract class

void ShowAbsyn::visitSelectedName(SelectedName *p)
{
  bufAppend('(');
  bufAppend("SelectedName");
  bufAppend(' ');
  bufAppend('[');
  if (p->prefix_)  p->prefix_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->suffix_)  p->suffix_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitSuffix(Suffix *p) {} //abstract class

void ShowAbsyn::visitAllSuffix(AllSuffix *p)
{
  bufAppend("AllSuffix");
}
void ShowAbsyn::visitAllSuffixfoo(AllSuffixfoo *p)
{
  bufAppend("AllSuffixfoo");
}
void ShowAbsyn::visitNameSuffix(NameSuffix *p)
{
  bufAppend('(');
  bufAppend("NameSuffix");
  bufAppend(' ');
  bufAppend('[');
  if (p->simple_name_)  p->simple_name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitAttribute_name(Attribute_name *p) {} //abstract class

void ShowAbsyn::visitAttributeName(AttributeName *p)
{
  bufAppend('(');
  bufAppend("AttributeName");
  bufAppend(' ');
  bufAppend('[');
  if (p->prefix_)  p->prefix_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->name_)  p->name_->accept(this);
  bufAppend(']');
  bufAppend(')');
}
void ShowAbsyn::visitSlice_name(Slice_name *p) {} //abstract class

void ShowAbsyn::visitSliceName(SliceName *p)
{
  bufAppend('(');
  bufAppend("SliceName");
  bufAppend(' ');
  bufAppend('[');
  if (p->prefix_)  p->prefix_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->expression_)  p->expression_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend(')');
}
void ShowAbsyn::visitIndexed_name(Indexed_name *p) {} //abstract class

void ShowAbsyn::visitIndexedName(IndexedName *p)
{
  bufAppend('(');
  bufAppend("IndexedName");
  bufAppend(' ');
  bufAppend('[');
  if (p->prefix_)  p->prefix_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend('[');
  if (p->listexpression_)  p->listexpression_->accept(this);
  bufAppend(']');
  bufAppend(' ');
  bufAppend(')');
}
void ShowAbsyn::visitCharacter_literal(Character_literal *p) {} //abstract class

void ShowAbsyn::visitCharLiteral(CharLiteral *p)
{
  bufAppend('(');
  bufAppend("CharLiteral");
  bufAppend(' ');
  visitChar(p->char_);
  bufAppend(' ');
  bufAppend(')');
}
void ShowAbsyn::visitGraphic_character(Graphic_character *p) {} //abstract class

void ShowAbsyn::visitGraphicChar(GraphicChar *p)
{
  bufAppend('(');
  bufAppend("GraphicChar");
  bufAppend(' ');
  visitChar(p->char_);
  bufAppend(')');
}
void ShowAbsyn::visitOperator_symbol(Operator_symbol *p) {} //abstract class

void ShowAbsyn::visitOperatorSymbol(OperatorSymbol *p)
{
  bufAppend('(');
  bufAppend("OperatorSymbol");
  bufAppend(' ');
  visitString(p->string_);
  bufAppend(')');
}
void ShowAbsyn::visitSimple_name(Simple_name *p) {} //abstract class

void ShowAbsyn::visitSimpleName(SimpleName *p)
{
  bufAppend('(');
  bufAppend("SimpleName");
  bufAppend(' ');
  visitIdent(p->ident_);
  bufAppend(')');
}
void ShowAbsyn::visitInteger(Integer i)
{
  char tmp[16];
  sprintf(tmp, "%d", i);
  bufAppend(tmp);
}
void ShowAbsyn::visitDouble(Double d)
{
  char tmp[16];
  sprintf(tmp, "%g", d);
  bufAppend(tmp);
}
void ShowAbsyn::visitChar(Char c)
{
  bufAppend('\'');
  bufAppend(c);
  bufAppend('\'');
}
void ShowAbsyn::visitString(String s)
{
  bufAppend('\"');
  bufAppend(s);
  bufAppend('\"');
}
void ShowAbsyn::visitIdent(String s)
{
  bufAppend('\"');
  bufAppend(s);
  bufAppend('\"');
}

void ShowAbsyn::visitDecimal_Literal(String s)
{
  bufAppend('\"');
  bufAppend(s);
  bufAppend('\"');
}


void ShowAbsyn::visitBase_Spec(String s)
{
  bufAppend('\"');
  bufAppend(s);
  bufAppend('\"');
}



