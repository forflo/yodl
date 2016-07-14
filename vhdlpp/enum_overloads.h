/* FM. MA */

#ifndef IVL_ENUM_OVL
#define IVL_ENUM_OVL

#include "expression.h"
#include "subprogram.h"
#include "parse_types.h"
#include "scope.h"
#include "simple_tree/simple_tree.h"
#include "generate_graph.h"

std::ostream &operator<<(std::ostream &os, ExpLogical::fun_t fun);
std::ostream &operator<<(std::ostream &os, ExpArithmetic::fun_t fun);
std::ostream &operator<<(std::ostream &os, ExpRelation::fun_t fun);
std::ostream &operator<<(std::ostream &os, ExpShift::shift_t fun);
std::ostream &operator<<(std::ostream &os, ExpRange::range_dir_t fun);
std::ostream &operator<<(std::ostream &os, ExpTime::timeunit_t fun);
std::ostream &operator<<(std::ostream &os, ExpEdge::fun_t fun);
std::ostream &operator<<(std::ostream &os, typedef_topo_t fun);
std::ostream &operator<<(std::ostream &os, VTypePrimitive::type_t fun);
std::ostream &operator<<(std::ostream &os, entity_aspect_t::entity_aspect_type_t fun);
std::ostream &operator<<(std::ostream &os, instant_list_t::application_domain_t fun);
std::ostream &operator<<(std::ostream &os, port_mode_t fun);
std::ostream &operator<<(std::ostream &os, DotGraphGenerator::color s);
std::ostream &operator<<(std::ostream &os, DotGraphGenerator::shape s);
std::ostream &operator<<(std::ostream &os, DotGraphGenerator::arrowHead s);

#endif /* IVL_ENUM_OVL */
