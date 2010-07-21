// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Json.hpp"

#include <math.h>
#include <sfz/sfz.hpp>
#include "rgos/JsonVisitor.hpp"
#include "rgos/Serialize.hpp"

using sfz::ReferenceCounted;
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
    Object(const StringMap<Json>& value)
        : _value(value) { }

    virtual void accept(JsonVisitor* visitor) const {
        visitor->visit_object(_value);
    }

  private:
    const StringMap<Json> _value;

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
    explicit String(const sfz::PrintItem& s)
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

Json Json::object(const StringMap<Json>& value) {
    return Json(new Object(value));
}

Json Json::array(const vector<Json>& value) {
    return Json(new Array(value));
}

Json Json::string(const sfz::PrintItem& value) {
    return Json(new String(value));
}

Json Json::number(double value) {
    return Json(new Number(value));
}

Json Json::bool_(bool value) {
    return Json(new Bool(value));
}

Json::Json() { }

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
    if (_value.get()) {
        _value->accept(visitor);
    } else {
        visitor->visit_null();
    }
}

}  // namespace rgos
