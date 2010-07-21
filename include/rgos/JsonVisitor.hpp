// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef RGOS_JSON_VISITOR_HPP_
#define RGOS_JSON_VISITOR_HPP_

#include <map>
#include <vector>
#include <sfz/sfz.hpp>
#include <rgos/StringMap.hpp>

namespace rgos {

class Json;

class JsonVisitor {
  public:
    virtual ~JsonVisitor();

    virtual void visit_object(const StringMap<Json>& value) = 0;
    virtual void visit_array(const std::vector<Json>& value) = 0;
    virtual void visit_string(const sfz::StringPiece& value) = 0;
    virtual void visit_number(double value) = 0;
    virtual void visit_bool(bool value) = 0;
    virtual void visit_null() = 0;
};

class JsonDefaultVisitor : public JsonVisitor {
  public:
    virtual void visit_object(const StringMap<Json>& value);
    virtual void visit_array(const std::vector<Json>& value);
    virtual void visit_string(const sfz::StringPiece& value);
    virtual void visit_number(double value);
    virtual void visit_bool(bool value);
    virtual void visit_null();

    virtual void visit_default(const char* type) = 0;
};

}  // namespace rgos

#endif  // RGOS_JSON_VISITOR_HPP
