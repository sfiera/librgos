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

using sfz::FormatItem;
using sfz::FormatItemPrinter;
using sfz::String;
using sfz::StringKey;
using sfz::StringPiece;
using sfz::ascii_encoding;
using sfz::format;
using sfz::quote;
using std::map;
using std::vector;

namespace rgos {

namespace {

class SerializerVisitor : public JsonVisitor {
  public:
    explicit SerializerVisitor(String* out);

    virtual void visit_object(const map<StringKey, Json>& value);
    virtual void visit_array(const vector<Json>& value);
    virtual void visit_string(const StringPiece& value);
    virtual void visit_number(double value);
    virtual void visit_bool(bool value);
    virtual void visit_null();

  protected:
    String* const _out;

  private:
    DISALLOW_COPY_AND_ASSIGN(SerializerVisitor);
};

class PrettyPrinterVisitor : public SerializerVisitor {
  public:
    explicit PrettyPrinterVisitor(String* out);

    virtual void visit_object(const map<StringKey, Json>& value);
    virtual void visit_array(const vector<Json>& value);

  private:
    int _depth;

    DISALLOW_COPY_AND_ASSIGN(PrettyPrinterVisitor);
};

SerializerVisitor::SerializerVisitor(String* out)
    : _out(out) { }

void SerializerVisitor::visit_object(const map<StringKey, Json>& value) {
    _out->append(1, '{');
    if (value.size() > 0) {
        foreach (it, value) {
            if (it != value.begin()) {
                _out->append(1, ',');
            }
            quote(it->first).print_to(_out);
            _out->append(1, ':');
            it->second.accept(this);
        }
    }
    _out->append(1, '}');
}

void SerializerVisitor::visit_array(const vector<Json>& value) {
    _out->append(1, '[');
    if (value.size() > 0) {
        foreach (it, value) {
            if (it != value.begin()) {
                _out->append(1, ',');
            }
            it->accept(this);
        }
    }
    _out->append(1, ']');
}

void SerializerVisitor::visit_string(const StringPiece& value) {
    quote(value).print_to(_out);
}

void SerializerVisitor::visit_number(double value) {
    FormatItem(value).print_to(_out);
}

void SerializerVisitor::visit_bool(bool value) {
    FormatItem(value).print_to(_out);
}

void SerializerVisitor::visit_null() {
    _out->append("null", ascii_encoding());
}

PrettyPrinterVisitor::PrettyPrinterVisitor(String* out)
    : SerializerVisitor(out),
      _depth(0) { }

void PrettyPrinterVisitor::visit_object(const map<StringKey, Json>& value) {
    _out->append(1, '{');
    if (value.size() > 0) {
        _depth += 2;
        foreach (it, value) {
            if (it != value.begin()) {
                _out->append(1, ',');
            }
            _out->append(1, '\n');
            _out->append(_depth, ' ');
            quote(it->first).print_to(_out);
            _out->append(": ", ascii_encoding());
            it->second.accept(this);
        }
        _depth -= 2;
        _out->append(1, '\n');
        _out->append(_depth, ' ');
    }
    _out->append(1, '}');
}

void PrettyPrinterVisitor::visit_array(const vector<Json>& value) {
    _out->append(1, '[');
    if (value.size() > 0) {
        _depth += 2;
        foreach (it, value) {
            if (it != value.begin()) {
                _out->append(1, ',');
            }
            _out->append(1, '\n');
            _out->append(_depth, ' ');
            it->accept(this);
        }
        _depth -= 2;
        _out->append(1, '\n');
        _out->append(_depth, ' ');
    }
    _out->append(1, ']');
}

class JsonSerializer : public FormatItemPrinter {
  public:
    JsonSerializer(const Json& value)
        : _value(value) { }

    virtual void print_to(String* out) const {
        SerializerVisitor visitor(out);
        _value.accept(&visitor);
    }

  private:
    const Json& _value;
};

class JsonPrettyPrinter : public FormatItemPrinter {
  public:
    JsonPrettyPrinter(const Json& value)
        : _value(value) { }

    virtual void print_to(String* out) const {
        PrettyPrinterVisitor visitor(out);
        _value.accept(&visitor);
        out->append(1, '\n');
    }

  private:
    const Json& _value;
};

}  // namespace

FormatItem serialize(const Json& value) {
    return FormatItem::make(new JsonSerializer(value));
}

FormatItem pretty_print(const Json& value) {
    return FormatItem::make(new JsonPrettyPrinter(value));
}

}  // namespace rgos
