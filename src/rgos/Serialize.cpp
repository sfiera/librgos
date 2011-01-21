// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Serialize.hpp"

#include <math.h>
#include <sfz/sfz.hpp>
#include "rgos/Json.hpp"
#include "rgos/JsonVisitor.hpp"

using sfz::PrintItem;
using sfz::PrintTarget;
using sfz::StringSlice;
using sfz::quote;
using std::map;
using std::vector;

namespace rgos {

namespace {

class SerializerVisitor : public JsonVisitor {
  public:
    explicit SerializerVisitor(PrintTarget out);

    virtual void visit_object(const StringMap<Json>& value);
    virtual void visit_array(const vector<Json>& value);
    virtual void visit_string(const StringSlice& value);
    virtual void visit_number(double value);
    virtual void visit_bool(bool value);
    virtual void visit_null();

  protected:
    PrintTarget _out;

  private:
    DISALLOW_COPY_AND_ASSIGN(SerializerVisitor);
};

class PrettyPrinterVisitor : public SerializerVisitor {
  public:
    explicit PrettyPrinterVisitor(PrintTarget out);

    virtual void visit_object(const StringMap<Json>& value);
    virtual void visit_array(const vector<Json>& value);

  private:
    int _depth;

    DISALLOW_COPY_AND_ASSIGN(PrettyPrinterVisitor);
};

SerializerVisitor::SerializerVisitor(PrintTarget out)
    : _out(out) { }

void SerializerVisitor::visit_object(const StringMap<Json>& value) {
    _out.push(1, '{');
    if (value.size() > 0) {
        bool first = true;
        foreach (const StringMap<Json>::value_type& item, value) {
            if (first) {
                first = false;
            } else {
                _out.push(1, ',');
            }
            print_to(_out, quote(item.first));
            _out.push(1, ':');
            item.second.accept(this);
        }
    }
    _out.push(1, '}');
}

void SerializerVisitor::visit_array(const vector<Json>& value) {
    _out.push(1, '[');
    if (value.size() > 0) {
        bool first = true;
        foreach (const Json& item, value) {
            if (first) {
                first = false;
            } else {
                _out.push(1, ',');
            }
            item.accept(this);
        }
    }
    _out.push(1, ']');
}

void SerializerVisitor::visit_string(const StringSlice& value) {
    print_to(_out, quote(value));
}

void SerializerVisitor::visit_number(double value) {
    PrintItem(value).print_to(_out);
}

void SerializerVisitor::visit_bool(bool value) {
    PrintItem(value).print_to(_out);
}

void SerializerVisitor::visit_null() {
    _out.push("null");
}

PrettyPrinterVisitor::PrettyPrinterVisitor(PrintTarget out)
    : SerializerVisitor(out),
      _depth(0) { }

void PrettyPrinterVisitor::visit_object(const StringMap<Json>& value) {
    _out.push(1, '{');
    if (value.size() > 0) {
        _depth += 2;
        bool first = true;
        foreach (const StringMap<Json>::value_type& item, value) {
            if (first) {
                first = false;
            } else {
                _out.push(1, ',');
            }
            _out.push(1, '\n');
            _out.push(_depth, ' ');
            print_to(_out, quote(item.first));
            _out.push(": ");
            item.second.accept(this);
        }
        _depth -= 2;
        _out.push(1, '\n');
        _out.push(_depth, ' ');
    }
    _out.push(1, '}');
}

void PrettyPrinterVisitor::visit_array(const vector<Json>& value) {
    _out.push(1, '[');
    if (value.size() > 0) {
        _depth += 2;
        bool first = true;
        foreach (const Json& item, value) {
            if (first) {
                first = false;
            } else {
                _out.push(1, ',');
            }
            _out.push(1, '\n');
            _out.push(_depth, ' ');
            item.accept(this);
        }
        _depth -= 2;
        _out.push(1, '\n');
        _out.push(_depth, ' ');
    }
    _out.push(1, ']');
}

}  // namespace

JsonPrettyPrinter pretty_print(const Json& value) {
    JsonPrettyPrinter result = { value };
    return result;
}

void print_to(sfz::PrintTarget out, const Json& json) {
    SerializerVisitor visitor(out);
    json.accept(&visitor);
}

void print_to(sfz::PrintTarget out, const JsonPrettyPrinter& json) {
    PrettyPrinterVisitor visitor(out);
    json.json.accept(&visitor);
}

}  // namespace rgos
