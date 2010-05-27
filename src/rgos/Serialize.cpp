// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Serialize.hpp"

#include <math.h>
#include "rgos/Json.hpp"
#include "rgos/JsonVisitor.hpp"
#include "sfz/Foreach.hpp"
#include "sfz/Format.hpp"
#include "sfz/Formatter.hpp"
#include "sfz/Macros.hpp"
#include "sfz/String.hpp"
#include "sfz/StringUtilities.hpp"

using sfz::PrintItem;
using sfz::PrintTarget;
using sfz::StringKey;
using sfz::StringPiece;
using sfz::quote;
using std::map;
using std::vector;

namespace rgos {

namespace {

class SerializerVisitor : public JsonVisitor {
  public:
    explicit SerializerVisitor(PrintTarget out);

    virtual void visit_object(const map<StringKey, Json>& value);
    virtual void visit_array(const vector<Json>& value);
    virtual void visit_string(const StringPiece& value);
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

    virtual void visit_object(const map<StringKey, Json>& value);
    virtual void visit_array(const vector<Json>& value);

  private:
    int _depth;

    DISALLOW_COPY_AND_ASSIGN(PrettyPrinterVisitor);
};

SerializerVisitor::SerializerVisitor(PrintTarget out)
    : _out(out) { }

void SerializerVisitor::visit_object(const map<StringKey, Json>& value) {
    _out.append(1, '{');
    if (value.size() > 0) {
        foreach (it, value) {
            if (it != value.begin()) {
                _out.append(1, ',');
            }
            print_to(_out, quote(it->first));
            _out.append(1, ':');
            it->second.accept(this);
        }
    }
    _out.append(1, '}');
}

void SerializerVisitor::visit_array(const vector<Json>& value) {
    _out.append(1, '[');
    if (value.size() > 0) {
        foreach (it, value) {
            if (it != value.begin()) {
                _out.append(1, ',');
            }
            it->accept(this);
        }
    }
    _out.append(1, ']');
}

void SerializerVisitor::visit_string(const StringPiece& value) {
    print_to(_out, quote(value));
}

void SerializerVisitor::visit_number(double value) {
    PrintItem(value).print_to(_out);
}

void SerializerVisitor::visit_bool(bool value) {
    PrintItem(value).print_to(_out);
}

void SerializerVisitor::visit_null() {
    _out.append("null");
}

PrettyPrinterVisitor::PrettyPrinterVisitor(PrintTarget out)
    : SerializerVisitor(out),
      _depth(0) { }

void PrettyPrinterVisitor::visit_object(const map<StringKey, Json>& value) {
    _out.append(1, '{');
    if (value.size() > 0) {
        _depth += 2;
        foreach (it, value) {
            if (it != value.begin()) {
                _out.append(1, ',');
            }
            _out.append(1, '\n');
            _out.append(_depth, ' ');
            print_to(_out, quote(it->first));
            _out.append(": ");
            it->second.accept(this);
        }
        _depth -= 2;
        _out.append(1, '\n');
        _out.append(_depth, ' ');
    }
    _out.append(1, '}');
}

void PrettyPrinterVisitor::visit_array(const vector<Json>& value) {
    _out.append(1, '[');
    if (value.size() > 0) {
        _depth += 2;
        foreach (it, value) {
            if (it != value.begin()) {
                _out.append(1, ',');
            }
            _out.append(1, '\n');
            _out.append(_depth, ' ');
            it->accept(this);
        }
        _depth -= 2;
        _out.append(1, '\n');
        _out.append(_depth, ' ');
    }
    _out.append(1, ']');
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
