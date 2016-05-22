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
ostream &operator<<(ostream &os, typedef_topo_t fun);
ostream &operator<<(ostream &os, VTypePrimitive::type_t fun);
ostream &operator<<(ostream &os, entity_aspect_t::entity_aspect_type_t fun);
ostream &operator<<(ostream &os, instant_list_t::application_domain_t fun);
ostream &operator<<(ostream &os, port_mode_t fun);

#endif /* IVL_ENUM_OVL */
