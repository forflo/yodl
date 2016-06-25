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

#include "std_types.h"
#include "scope.h"

#include <map>


void StandardTypes::add_global_types_to(ActiveScope *scope){
    scope->use_name(type_BOOLEAN.peek_name(), &type_BOOLEAN);
    scope->use_name(perm_string::literal("bit"), &primitive_BIT);
    // TODO: why doesn't this work
    //scope->use_name(perm_string::literal("std_logic_vector"), &primitive_STDLOGIC_VECTOR);
    scope->use_name(perm_string::literal("bit_vector"), &primitive_BIT_VECTOR);
    scope->use_name(perm_string::literal("integer"), &primitive_INTEGER);
    scope->use_name(perm_string::literal("real"), &primitive_REAL);
    scope->use_name(perm_string::literal("std_logic"), &primitive_STDLOGIC);
    scope->use_name(perm_string::literal("character"), &primitive_CHARACTER);
    scope->use_name(perm_string::literal("string"), &primitive_STRING);
    scope->use_name(perm_string::literal("natural"), &primitive_NATURAL);
    scope->use_name(perm_string::literal("time"), &primitive_TIME);
}

void StandardTypes::generate_global_types() {
    // boolean
    list<perm_string> enum_BOOLEAN_vals;
    enum_BOOLEAN_vals.push_back(perm_string::literal("false"));
    enum_BOOLEAN_vals.push_back(perm_string::literal("true"));
    VTypeEnum *enum_BOOLEAN = new VTypeEnum(&enum_BOOLEAN_vals);

    type_BOOLEAN.set_definition(enum_BOOLEAN);
    std_types[type_BOOLEAN.peek_name()] = &type_BOOLEAN;
    std_enums.push_back(enum_BOOLEAN);

    // file_open_kind
    list<perm_string> enum_FILE_OPEN_KIND_vals;
    enum_FILE_OPEN_KIND_vals.push_back(perm_string::literal("read_mode"));
    enum_FILE_OPEN_KIND_vals.push_back(perm_string::literal("write_mode"));
    enum_FILE_OPEN_KIND_vals.push_back(perm_string::literal("append_mode"));
    VTypeEnum *enum_FILE_OPEN_KIND = new VTypeEnum(&enum_FILE_OPEN_KIND_vals);

    type_FILE_OPEN_KIND.set_definition(enum_FILE_OPEN_KIND);
    std_types[type_FILE_OPEN_KIND.peek_name()] = &type_FILE_OPEN_KIND;
    std_enums.push_back(enum_FILE_OPEN_KIND);

    // file_open_status
    list<perm_string> enum_FILE_OPEN_STATUS_vals;
    enum_FILE_OPEN_STATUS_vals.push_back(perm_string::literal("open_ok"));
    enum_FILE_OPEN_STATUS_vals.push_back(perm_string::literal("status_error"));
    enum_FILE_OPEN_STATUS_vals.push_back(perm_string::literal("name_error"));
    enum_FILE_OPEN_STATUS_vals.push_back(perm_string::literal("mode_error"));
    VTypeEnum *enum_FILE_OPEN_STATUS = new VTypeEnum(&enum_FILE_OPEN_STATUS_vals);

    type_FILE_OPEN_STATUS.set_definition(enum_FILE_OPEN_STATUS);
    std_types[type_FILE_OPEN_STATUS.peek_name()] = &type_FILE_OPEN_STATUS;
    std_enums.push_back(enum_FILE_OPEN_STATUS);
}

void StandardTypes::delete_global_types() {
    typedef_context_t typedef_ctx;

    // FM. MA| changed to c++11 looping syntax
    for (auto &i : std_types){
        i.second->peek_definition();
        delete i.second;
    }

    // std_enums are destroyed above
}

const VTypeEnum *StandardTypes::find_std_enum_name(perm_string name) {
    for (list<const VTypeEnum *>::const_iterator it = std_enums.begin();
         it != std_enums.end(); ++it) {
        if ((*it)->has_name(name)) {
            return *it;
        }
    }

    return NULL;
}

void StandardType::semit_std_types(ostream& fd) const {
    fd << "`ifndef __VHDL_STD_TYPES" << endl;
    fd << "`define __VHDL_STD_TYPES" << endl;
    typedef_context_t typedef_ctx;

    for (auto &i : std_types)
        i.second->emit_typedef(fd, typedef_cts);

    fd << "`endif" << endl;
}

// FM. MA TODO: TMP?
static bool StandardTypes::is_global_type(perm_string name) {
    if (name == "boolean") {
        return true;
    }
    if (name == "bit") {
        return true;
    }
    if (name == "bit_vector") {
        return true;
    }
    if (name == "integer") {
        return true;
    }
    if (name == "real") {
        return true;
    }
    if (name == "std_logic") {
        return true;
    }
    if (name == "std_logic_vector") {
        return true;
    }
    if (name == "character") {
        return true;
    }
    if (name == "string") {
        return true;
    }
    if (name == "natural") {
        return true;
    }
    if (name == "signed") {
        return true;
    }
    if (name == "unsigned") {
        return true;
    }
    if (name == "time") {
        return true;
    }

    return std_types.count(name) > 0;
}
