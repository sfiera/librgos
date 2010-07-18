// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef RGOS_SERIALIZE_HPP_
#define RGOS_SERIALIZE_HPP_

#include <sfz/sfz.hpp>

namespace rgos {

class Json;

struct JsonPrettyPrinter;

JsonPrettyPrinter pretty_print(const Json& value);

struct JsonPrettyPrinter { const Json& json; };
void print_to(sfz::PrintTarget out, const JsonPrettyPrinter& j);

}  // namespace rgos

#endif  // RGOS_SERIALIZE_HPP_
