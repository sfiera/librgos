// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef RGOS_JSON_HPP_
#define RGOS_JSON_HPP_

#include <map>
#include <vector>
#include "sfz/ReferenceCounted.hpp"
#include "sfz/String.hpp"

namespace rgos {

class JsonVisitor;

class Json {
  public:
    static Json object(const std::map<sfz::StringKey, Json>& value);
    static Json array(const std::vector<Json>& value);
    static Json string(const sfz::String& value);
    static Json number(double value);
    static Json bool_(bool value);
    static Json null();

    Json(const Json& other);
    Json& operator=(const Json& other);
    ~Json();

    void accept(JsonVisitor* visitor) const;
    void print_to(sfz::String* out) const;

  private:
    class Value;
    class Object;
    class Array;
    class String;
    class Number;
    class Bool;
    class Null;

    Json(Value* value);

    sfz::RefPtr<const Value> _value;

    // ALLOW_COPY_AND_ASSIGN
};

}  // namespace rgos

#endif  // RGOS_JSON_HPP_
