// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#include "rgos/Json.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sfz/sfz.hpp>
#include "rgos/JsonVisitor.hpp"

using sfz::StringSlice;
using std::make_pair;
using std::map;
using std::vector;
using testing::Eq;
using testing::InSequence;
using testing::StrictMock;

namespace rgos {
namespace {

class MockJsonVisitor : public JsonVisitor {
  public:
    MOCK_METHOD0(enter_object, void());
    MOCK_METHOD1(object_key, void(const StringSlice&));
    MOCK_METHOD0(exit_object, void());
    void visit_object(const StringMap<Json>& value) {
        enter_object();
        foreach (const StringMap<Json>::value_type& item, value) {
            object_key(item.first);
            item.second.accept(this);
        }
        exit_object();
    }

    MOCK_METHOD0(enter_array, void());
    MOCK_METHOD0(exit_array, void());
    void visit_array(const vector<Json>& value) {
        enter_array();
        foreach (const Json& item, value) {
            item.accept(this);
        }
        exit_array();
    }

    MOCK_METHOD1(visit_string, void(const StringSlice& value));
    MOCK_METHOD1(visit_number, void(double value));
    MOCK_METHOD1(visit_bool, void(bool value));
    MOCK_METHOD0(visit_null, void());
};

typedef ::testing::Test JsonTest;

TEST_F(JsonTest, NullTest) {
    StrictMock<MockJsonVisitor> visitor;
    EXPECT_CALL(visitor, visit_null());
    Json().accept(&visitor);
}

TEST_F(JsonTest, StringTest) {
    StrictMock<MockJsonVisitor> visitor;
    EXPECT_CALL(visitor, visit_string(Eq<StringSlice>("Hello, world!")));
    Json::string("Hello, world!").accept(&visitor);
}

TEST_F(JsonTest, NumberTest) {
    StrictMock<MockJsonVisitor> visitor;
    EXPECT_CALL(visitor, visit_number(1.0));
    Json::number(1.0).accept(&visitor);
}

TEST_F(JsonTest, BoolTest) {
    StrictMock<MockJsonVisitor> visitor;
    EXPECT_CALL(visitor, visit_bool(true));
    Json::bool_(true).accept(&visitor);
}

// []
TEST_F(JsonTest, EmptyArrayTest) {
    StrictMock<MockJsonVisitor> visitor;
    {
        InSequence s;
        EXPECT_CALL(visitor, enter_array());
        EXPECT_CALL(visitor, exit_array());
    }
    vector<Json> a;
    Json::array(a).accept(&visitor);
}

// [1.0, 2.0, 3.0]
TEST_F(JsonTest, NonEmptyArrayTest) {
    StrictMock<MockJsonVisitor> visitor;
    {
        InSequence s;
        EXPECT_CALL(visitor, enter_array());
        EXPECT_CALL(visitor, visit_number(1.0));
        EXPECT_CALL(visitor, visit_number(2.0));
        EXPECT_CALL(visitor, visit_number(3.0));
        EXPECT_CALL(visitor, exit_array());
    }
    vector<Json> a;
    a.push_back(Json::number(1.0));
    a.push_back(Json::number(2.0));
    a.push_back(Json::number(3.0));
    Json::array(a).accept(&visitor);
}

// {}
TEST_F(JsonTest, EmptyObjectTest) {
    StrictMock<MockJsonVisitor> visitor;
    {
        InSequence s;
        EXPECT_CALL(visitor, enter_object());
        EXPECT_CALL(visitor, exit_object());
    }
    StringMap<Json> o;
    Json::object(o).accept(&visitor);
}

// {
//   "one", 1.0,
//   "two", 2.0,
//   "three", 3.0
// }
TEST_F(JsonTest, NonEmptyObjectTest) {
    StrictMock<MockJsonVisitor> visitor;
    {
        InSequence s;
        EXPECT_CALL(visitor, enter_object());
        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("one")));
        EXPECT_CALL(visitor, visit_number(1.0));
        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("three")));
        EXPECT_CALL(visitor, visit_number(3.0));
        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("two")));
        EXPECT_CALL(visitor, visit_number(2.0));
        EXPECT_CALL(visitor, exit_object());
    }
    StringMap<Json> o;
    o.insert(make_pair("one", Json::number(1.0)));
    o.insert(make_pair("two", Json::number(2.0)));
    o.insert(make_pair("three", Json::number(3.0)));
    Json::object(o).accept(&visitor);
}

// {
//   "album": "Hey Everyone",
//   "artist": "Dananananaykroyd",
//   "compilation": false,
//   "tracks": [
//     {
//       "title": "Hey Everyone",
//       "length": 151
//     },
//     {
//       "title": "Watch This!",
//       "length": 213
//     },
//     {
//       "title": "The Greater Than Symbol & The Hash",
//       "length": 281
//     }
//   ]
// }
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

TEST_F(JsonTest, ComplexObjectTest) {
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

    StrictMock<MockJsonVisitor> visitor;
    {
        InSequence s;
        EXPECT_CALL(visitor, enter_object());

        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("album")));
        EXPECT_CALL(visitor, visit_string(kAlbum.album));

        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("artist")));
        EXPECT_CALL(visitor, visit_string(kAlbum.artist));

        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("compilation")));
        EXPECT_CALL(visitor, visit_bool(kAlbum.compilation));

        EXPECT_CALL(visitor, object_key(Eq<StringSlice>("tracks")));
        EXPECT_CALL(visitor, enter_array());
        foreach (const Album::Track& track, kAlbum.tracks) {
            EXPECT_CALL(visitor, enter_object());
            EXPECT_CALL(visitor, object_key(Eq<StringSlice>("length")));
            EXPECT_CALL(visitor, visit_number(track.length));
            EXPECT_CALL(visitor, object_key(Eq<StringSlice>("title")));
            EXPECT_CALL(visitor, visit_string(track.title));
            EXPECT_CALL(visitor, exit_object());
        }
        EXPECT_CALL(visitor, exit_array());

        EXPECT_CALL(visitor, exit_object());
    }

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

    Json::object(album).accept(&visitor);
}

}  // namespace
}  // namespace rgos
