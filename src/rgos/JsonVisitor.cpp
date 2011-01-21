// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/JsonVisitor.hpp"

using sfz::String;
using sfz::StringSlice;
using std::map;
using std::vector;

namespace rgos {

JsonVisitor::~JsonVisitor() { }

void JsonDefaultVisitor::visit_object(const StringMap<Json>& value) {
    visit_default("object");
}

void JsonDefaultVisitor::visit_array(const vector<Json>& value) {
    visit_default("array");
}

void JsonDefaultVisitor::visit_string(const StringSlice& value) {
    visit_default("string");
}

void JsonDefaultVisitor::visit_number(double value) {
    visit_default("number");
}

void JsonDefaultVisitor::visit_bool(bool value) {
    visit_default("bool");
}

void JsonDefaultVisitor::visit_null() {
    visit_default("null");
}

}  // namespace rgos
