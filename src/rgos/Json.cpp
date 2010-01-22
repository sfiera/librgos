// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Json.hpp"

#include <math.h>
#include "rgos/JsonVisitor.hpp"
#include "rgos/Serialize.hpp"
#include "sfz/Formatter.hpp"
#include "sfz/Macros.hpp"
#include "sfz/SmartPtr.hpp"

using sfz::FormatItem;
using sfz::ReferenceCounted;
using sfz::StringKey;
using sfz::scoped_ptr;
using sfz::quote;
using std::map;
using std::vector;

namespace rgos {

class Json::Value : public ReferenceCounted {
  public:
    virtual void accept(JsonVisitor* visitor) const = 0;
};

class Json::Object : public Json::Value {
  public:
    Object(const map<StringKey, Json>& value)
        : _value(value) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_object(_value);
    }

  private:
    const map<StringKey, Json> _value;

    DISALLOW_COPY_AND_ASSIGN(Object);
};

class Json::Array : public Json::Value {
  public:
    Array(const vector<Json>& value)
        : _value(value) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_array(_value);
    }

  private:
    const vector<Json> _value;

    DISALLOW_COPY_AND_ASSIGN(Array);
};

class Json::String : public Json::Value {
  public:
    explicit String(const sfz::String& s)
        : _value(s) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_string(_value);
    }

  private:
    const sfz::String _value;

    DISALLOW_COPY_AND_ASSIGN(String);
};

class Json::Number : public Json::Value {
  public:
    explicit Number(double value)
        : _value(value) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_number(_value);
    }

  private:
    const double _value;

    DISALLOW_COPY_AND_ASSIGN(Number);
};

class Json::Bool : public Json::Value {
  public:
    explicit Bool(bool value)
        : _value(value) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_bool(_value);
    }

  private:
    const bool _value;

    DISALLOW_COPY_AND_ASSIGN(Bool);
};

class Json::Null : public Json::Value {
  public:
    explicit Null() { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_null();
    }

  private:
    DISALLOW_COPY_AND_ASSIGN(Null);
};

Json Json::object(const map<StringKey, Json>& value) {
    return Json(new Object(value));
}

Json Json::array(const vector<Json>& value) {
    return Json(new Array(value));
}

Json Json::string(const sfz::String& value) {
    return Json(new String(value));
}

Json Json::number(double value) {
    return Json(new Number(value));
}

Json Json::bool_(bool value) {
    return Json(new Bool(value));
}

Json Json::null() {
    return Json(new Null);
}

Json::Json(Json::Value* value)
    : _value(value) { }

Json::Json(const Json& other)
    : _value(other._value) { }

Json& Json::operator=(const Json& other) {
    _value = other._value;
    return *this;
}

Json::~Json() { }

void Json::accept(JsonVisitor* visitor) const {
    _value->accept(visitor);
}

void Json::print_to(sfz::String* out) const {
    serialize(*this).print_to(out);
}

}  // namespace rgos
