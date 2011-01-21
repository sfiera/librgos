// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Serialize.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sfz/sfz.hpp>
#include "rgos/Json.hpp"

using sfz::CString;
using sfz::StringSlice;
using sfz::format;
using std::make_pair;
using std::map;
using std::vector;
using testing::Eq;
using testing::InSequence;
using testing::StrictMock;

namespace rgos {
namespace {

typedef ::testing::Test SerializeTest;

MATCHER_P(SerializesTo, representation, "") {
    sfz::String actual(arg);
    CString actual_c_str(actual);
    sfz::String expected(representation);
    CString expected_c_str(expected);
    *result_listener
        << "actual " << actual_c_str.data() << " vs. expected " << expected_c_str.data();
    return actual == expected;
}

TEST_F(SerializeTest, NullTest) {
    EXPECT_THAT(Json(), SerializesTo("null"));
}

TEST_F(SerializeTest, StringTest) {
    EXPECT_THAT(Json::string(""), SerializesTo("\"\""));
    EXPECT_THAT(Json::string("Hello, world!"), SerializesTo("\"Hello, world!\""));
    EXPECT_THAT(Json::string("Multiple\nLines"), SerializesTo("\"Multiple\\nLines\""));
}

TEST_F(SerializeTest, NumberTest) {
    EXPECT_THAT(Json::number(1.0), SerializesTo(1.0));
    EXPECT_THAT(Json::number(2.0), SerializesTo(2.0));
    EXPECT_THAT(Json::number(3.0), SerializesTo(3.0));
}

TEST_F(SerializeTest, BoolTest) {
    EXPECT_THAT(Json::bool_(true), SerializesTo("true"));
    EXPECT_THAT(Json::bool_(false), SerializesTo("false"));
}

TEST_F(SerializeTest, EmptyArrayTest) {
    vector<Json> a;
    EXPECT_THAT(Json::array(a), SerializesTo("[]"));
}

TEST_F(SerializeTest, NonEmptyArrayTest) {
    vector<Json> a;
    a.push_back(Json::number(1.0));
    a.push_back(Json::number(2.0));
    a.push_back(Json::number(3.0));
    EXPECT_THAT(Json::array(a), SerializesTo(format("[{0},{1},{2}]", 1.0, 2.0, 3.0)));
}

TEST_F(SerializeTest, EmptyObjectTest) {
    StringMap<Json> o;
    EXPECT_THAT(Json::object(o), SerializesTo("{}"));
}

TEST_F(SerializeTest, NonEmptyObjectTest) {
    StringMap<Json> o;
    o.insert(make_pair("one", Json::number(1.0)));
    o.insert(make_pair("two", Json::number(2.0)));
    o.insert(make_pair("three", Json::number(3.0)));
    EXPECT_THAT(Json::object(o), SerializesTo(format(
                "{{"
                    "\"one\":{0},"
                    "\"three\":{1},"
                    "\"two\":{2}"
                "}}",
                1.0, 3.0, 2.0)));
}

struct Album {
    struct Track {
        const StringSlice title;
        double length;
    };

    const StringSlice album;
    const StringSlice artist;
    bool compilation;
    Track tracks[3];
};

TEST_F(SerializeTest, ComplexObjectTest) {
    const Album kAlbum = {
        "Hey Everyone",
        "Dananananaykroyd",
        false,
        {
            { "Hey Everyone", 151 },
            { "Watch This!", 213 },
            { "The Greater Than Symbol & The Hash", 281 },
        },
    };

    vector<Json> tracks;
    foreach (const Album::Track& track, kAlbum.tracks) {
        StringMap<Json> object;
        object.insert(make_pair("title", Json::string(track.title)));
        object.insert(make_pair("length", Json::number(track.length)));
        tracks.push_back(Json::object(object));
    }

    StringMap<Json> album;
    album.insert(make_pair("album", Json::string(kAlbum.album)));
    album.insert(make_pair("artist", Json::string(kAlbum.artist)));
    album.insert(make_pair("compilation", Json::bool_(kAlbum.compilation)));
    album.insert(make_pair("tracks", Json::array(tracks)));

    EXPECT_THAT(Json::object(album), SerializesTo(format(
                    "{{"
                        "\"album\":\"Hey Everyone\","
                        "\"artist\":\"Dananananaykroyd\","
                        "\"compilation\":false,"
                        "\"tracks\":["
                            "{{"
                                "\"length\":{0},"
                                "\"title\":\"Hey Everyone\""
                            "}},"
                            "{{"
                                "\"length\":{1},"
                                "\"title\":\"Watch This!\""
                            "}},"
                            "{{"
                                "\"length\":{2},"
                                "\"title\":\"The Greater Than Symbol & The Hash\""
                            "}}"
                        "]"
                    "}}",
                    151.0, 213.0, 281.0)));
}

}  // namespace
}  // namespace rgos
