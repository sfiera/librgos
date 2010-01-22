// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef RGOS_SERIALIZE_HPP_
#define RGOS_SERIALIZE_HPP_

namespace sfz { class FormatItem; }

namespace rgos {

class Json;

sfz::FormatItem serialize(const Json& value);
sfz::FormatItem pretty_print(const Json& value);

}  // namespace rgos

#endif  // RGOS_SERIALIZE_HPP_
