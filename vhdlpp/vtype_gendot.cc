// FM. MA

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <cassert>
#include <algorithm>

#include "vtype.h"
#include "enum_overloads.h"
#include "parse_types.h"
#include "simple_tree.h"
#include "compiler.h"

using namespace std;

SimpleTree<map<string, string>> *VTypePrimitive::emit_strinfo_tree() const {
    return new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VTypePrimitive"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"type", (dynamic_cast<stringstream&>(
                stringstream{} << type_)).str()},
            {"packed?", (dynamic_cast<stringstream&>(
                stringstream{} << packed_)).str()}});
}

SimpleTree<map<string, string>> *VTypeArray::range_t::emit_strinfo_tree() const {
    auto result =  new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Range"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"direction", (direction_ ? "downto" : "to")}});

    if (msb_)
        result->forest.push_back(msb_->emit_strinfo_tree());
    if (lsb_)
        result->forest.push_back(lsb_->emit_strinfo_tree());
    return result;
}

SimpleTree<map<string, string>> *VTypeArray::emit_strinfo_tree() const {
    auto result =  new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VTypeArray"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"signed?", (dynamic_cast<stringstream&>(
                stringstream{} << signed_flag_)).str()}});

    result->forest.push_back(etype_->emit_strinfo_tree());
    for (auto &i : ranges_)
        result->forest.push_back(i.emit_strinfo_tree());

    if (parent_)
        result->forest.push_back(parent_->emit_strinfo_tree());
    return result;
}

SimpleTree<map<string, string>> *VTypeRange::emit_strinfo_tree() const {
    auto result =  new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "VTypeRange"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    result->forest.push_back(base_->emit_strinfo_tree());
    return result;
}

SimpleTree<map<string, string>> *VTypeRangeConst::emit_strinfo_tree() const {
    auto result =  new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VTypeRangeConst"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"start", std::to_string(start_)},
            {"end", std::to_string(end_)}});

    result->forest.push_back(base_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *VTypeRangeExpr::emit_strinfo_tree() const {
    auto result =  new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VTypeRangeExpr"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"downto?", std::to_string(downto_)}});

    result->forest.push_back(start_->emit_strinfo_tree());
    result->forest.push_back(end_->emit_strinfo_tree());
    result->forest.push_back(base_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *VTypeEnum::emit_strinfo_tree() const {
    auto temp = map<string, string>{
        {"node-type", "VTypeEnum"},
        {"node-pointer", static_cast<stringstream&>(
                (stringstream{} << this)).str()}};

    for (auto &i : names_)
        temp[i.str()] = "";

    return new SimpleTree<map<string, string>>(temp);
}

SimpleTree<map<string, string>> *VTypeRecord::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{{"node-type", "VTypeRecord"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()}});

    for (auto &i : elements_)
        result->forest.push_back(i->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *VTypeRecord::element_t::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "Record Element"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"element name", name_.str()}});

    result->forest.push_back(type_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *VTypeDef::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VTypeDef"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"type name", name_.str()}});

    result->forest.push_back(type_->emit_strinfo_tree());

    return result;
}

SimpleTree<map<string, string>> *VSubTypeDef::emit_strinfo_tree() const {
    auto result = new SimpleTree<map<string, string>>(
        map<string, string>{
            {"node-type", "VSubTypeDef"},
            {"node-pointer", static_cast<stringstream&>(
                    (stringstream{} << this)).str()},
            {"type name", name_.str()}});

    result->forest.push_back(type_->emit_strinfo_tree());

    return result;
}
