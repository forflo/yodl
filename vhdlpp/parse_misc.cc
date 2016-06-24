/*
 * Copyright (c) 2011,2013 Stephen Williams (steve@icarus.com)
 * Copyright CERN 2013 / Stephen Williams (steve@icarus.com)
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
 *    Picture Elements, Inc., 777 Panoramic Way, Berkeley, CA 94704.
 */
#include <iostream>
#include <cassert>
#include <cstring>

#include "parse_context.h"
#include "parse_misc.h"
#include "parse_types.h"
#include "parse_api.h"
#include "entity.h"
#include "architec.h"
#include "expression.h"
#include "vtype.h"
#include "compiler.h"

using namespace std;

static const VType *calculate_subtype_array(ParserContext *c,
                                            const YYLTYPE& loc,
                                            const char *base_name,
                                            ScopeBase * /* scope */,
                                            Expression *array_left,
                                            bool downto,
                                            Expression *array_right) {
    const VType *base_type = parse_type_by_name(lex_strings.make(base_name));

    if (base_type == 0) {
        ParserUtil::errormsg(c, loc, "Unable to find base type %s of array.\n", base_name);
        return NULL;
    }

    assert(array_left == 0 || array_right != 0);

    // unfold typedef, there might be VTypeArray inside
    const VType    *origin_type = base_type;
    const VTypeDef *type_def    = dynamic_cast<const VTypeDef *>(base_type);
    if (type_def) {
        base_type = type_def->peek_definition();
    }

    const VTypeArray *base_array = dynamic_cast<const VTypeArray *> (base_type);
    if (base_array) {
        assert(array_left && array_right);

        vector<VTypeArray::range_t> range(base_array->dimensions());

        // FIXME: For now, I only know how to handle 1 dimension
        assert(base_array->dimensions() == 1);

        range[0] = VTypeArray::range_t(array_left, array_right, downto);

        // use typedef as the element type if possible
        const VType *element = type_def
            ? origin_type
            : base_array->element_type();

        VTypeArray *subtype = new VTypeArray(element, range,
                                             base_array->signed_vector());

        subtype->set_parent_type(base_array);
        return subtype;
    }

    return base_type;
}


const VType *calculate_subtype_array(ParserContext *c,
                                     const YYLTYPE& loc,
                                     const char *base_name,
                                     ScopeBase *scope,
                                     list<ExpRange *> *ranges) {
    if (ranges->size() == 1) {
        ExpRange   *tmpr = ranges->front();
        Expression *lef  = tmpr->left();
        Expression *rig  = tmpr->right();
        // FM. MA| Fixed bug tmpr->direction() is 0 if downto
        return calculate_subtype_array(c, loc,
                                       base_name,
                                       scope,
                                       lef,
                                       (tmpr->direction() == ExpRange::range_dir_t::DOWNTO
                                           ? true
                                           : false),
                                       rig);
    }

    ParserUtil::sorrymsg(c, loc, "Don't know how to handle multiple ranges here.\n");
    return NULL;
}


const VType *calculate_subtype_range(ParserContext *c,
                                     const YYLTYPE& loc, const char *base_name,
                                     ScopeBase *scope, Expression *range_left,
                                     int direction, Expression *range_right) {
    const VType *base_type = parse_type_by_name(lex_strings.make(base_name));

    if (base_type == 0) {
        ParserUtil::errormsg(c, loc, "Unable to find base type %s of range.\n", base_name);
        return 0;
    }

    assert(range_left && range_right);

    int64_t    left_val, right_val;
    VTypeRange *subtype;

    if (range_left->evaluate(scope, left_val) && range_right->evaluate(scope, right_val)) {
        subtype = new VTypeRangeConst(base_type, left_val, right_val);
    }else  {
        subtype = new VTypeRangeExpr(base_type, range_left, range_right, direction);
    }

    return subtype;
}


ExpString *parse_char_enums(const char *str) {
    if (!strcasecmp(str, "LF")) {
        return new ExpString("\\n");
    }

    if (!strcasecmp(str, "CR")) {
        return new ExpString("\\r");
    }

    return NULL;
}
