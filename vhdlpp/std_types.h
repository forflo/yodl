/*
 * Copyright CERN 2015
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
// FM. MA| Encapsulated every standard type inside a new class StandardTypes

#include "vtype.h"

class ActiveScope;



class StandardTypes {
public:
    StandardTypes() {};
    ~StandardTypes() {};

    // this list contains enums used by typedefs in the std_types map
    list<const VTypeEnum *> std_enums;
    map<perm_string, VTypeDef *> std_types;

public:
    VTypeDef type_BOOLEAN(perm_string::literal("boolean"));
    VTypeDef type_FILE_OPEN_KIND(perm_string::literal("file_open_kind"));
    VTypeDef type_FILE_OPEN_STATUS(perm_string::literal("file_open_status"));

    const VTypePrimitive primitive_BIT(VTypePrimitive::BIT, true);
    const VTypePrimitive primitive_INTEGER(VTypePrimitive::INTEGER);
    const VTypePrimitive primitive_NATURAL(VTypePrimitive::NATURAL);
    const VTypePrimitive primitive_REAL(VTypePrimitive::REAL);

    const VTypePrimitive primitive_STDLOGIC(VTypePrimitive::STDLOGIC, true);
    const VTypePrimitive primitive_TIME(VTypePrimitive::TIME, true);

    const VTypeArray primitive_CHARACTER(
        &primitive_BIT, 7, 0);
    const VTypeArray primitive_BIT_VECTOR(
        &primitive_BIT, vector<VTypeArray::range_t> (1));
    const VTypeArray primitive_BOOL_VECTOR(
        &type_BOOLEAN, vector<VTypeArray::range_t> (1));
    const VTypeArray primitive_STDLOGIC_VECTOR(
        &primitive_STDLOGIC, vector<VTypeArray::range_t> (1));
    const VTypeArray primitive_STRING(
        &primitive_CHARACTER, vector<VTypeArray::range_t> (1));
    const VTypeArray primitive_SIGNED(
        &primitive_STDLOGIC, vector<VTypeArray::range_t> (1), true);
    const VTypeArray primitive_UNSIGNED(
        &primitive_STDLOGIC, vector<VTypeArray::range_t> (1), false);

public:
    void add_global_types_to(ActiveScope *scope);

    void generate_global_types();

    void delete_global_types();

    const VTypeEnum *find_std_enum_name(perm_string name);

    void emit_std_types(ostream& fd) const;

    static bool is_global_type(perm_string name);
};
