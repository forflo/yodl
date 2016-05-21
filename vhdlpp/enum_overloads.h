/* FM. MA */

#ifndef IVL_ENUM_OVL
#define IVL_ENUM_OVL

#include "expression.h"
#include "subprogram.h"
#include "parse_types.h"
#include "scope.h"
#include "simple_tree/simple_tree.h"

using namespace std;

ostream &operator<<(ostream &os, ExpLogical::fun_t fun);
ostream &operator<<(ostream &os, ExpArithmetic::fun_t fun);
ostream &operator<<(ostream &os, ExpRelation::fun_t fun);
ostream &operator<<(ostream &os, ExpShift::shift_t fun);
ostream &operator<<(ostream &os, ExpRange::range_dir_t fun);
ostream &operator<<(ostream &os, ExpTime::timeunit_t fun);
ostream &operator<<(ostream &os, ExpEdge::fun_t fun);

#endif /* IVL_ENUM_OVL */
