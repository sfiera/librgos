// Copyright (c) 2009 Chris Pickel <sfiera@gmail.com>
//
// This file is part of librgos, a free software project.  You can redistribute it and/or modify it
// under the terms of the MIT License.

#ifndef RGOS_STRING_MAP_HPP_
#define RGOS_STRING_MAP_HPP_

#include <map>
#include <utility>
#include <stdlib.h>
#include <sfz/sfz.hpp>

namespace rgos {

struct StringPieceLess;

template <typename T, typename Compare = StringPieceLess>
class StringMap {
  public:
    typedef sfz::StringPiece                        key_type;
    typedef T                                       mapped_type;
    typedef std::pair<const key_type, mapped_type>  value_type;
    typedef size_t                                  size_type;

    class iterator;
    class const_iterator;

    StringMap() { }
    explicit StringMap(const StringMap& other);
    ~StringMap() { }

    mapped_type& operator[](const key_type& key);
    std::pair<iterator, bool> insert(const value_type& pair);

    size_type size() const { return _map.size(); }
    bool empty() const { return _map.empty(); }

    void clear() { _map.clear(); }
    void erase(iterator pos) { _map.erase(pos); }
    void erase(iterator start, iterator end) { _map.erase(start, end); }
    size_type erase(const key_type& key) { return _map.erase(key); }

    iterator find(const key_type& key) { return _map.find(key); }
    const_iterator find(const key_type& key) const { return _map.find(key); }

    iterator begin() { return _map.begin(); }
    const_iterator begin() const { return _map.begin(); }
    iterator end() { return _map.end(); }
    const_iterator end() const { return _map.end(); }
    iterator rbegin() { return _map.rbegin(); }
    const_iterator rbegin() const { return _map.rbegin(); }
    iterator rend() { return _map.rend(); }
    const_iterator rend() const { return _map.rend(); }

    void swap(StringMap& from) { _map.swap(from._map); }

  private:
    struct WrappedValue;
    typedef std::map<sfz::StringPiece, sfz::linked_ptr<WrappedValue>, Compare> internal_map;
    typedef typename internal_map::iterator wrapped_iterator;
    typedef typename internal_map::const_iterator wrapped_const_iterator;

    struct WrappedValue {
        const sfz::String key_storage;
        std::pair<const sfz::StringPiece, mapped_type> pair;

        WrappedValue(const sfz::StringPiece& k)
            : key_storage(k),
              pair(key_storage, mapped_type()) { }

        WrappedValue(const sfz::StringPiece& k, const mapped_type& v)
            : key_storage(k),
              pair(key_storage, v) { }

        DISALLOW_COPY_AND_ASSIGN(WrappedValue);
    };

    template <typename wrapped_iterator>
    class iterator_base {
      public:
        typedef typename wrapped_iterator::iterator_category    iterator_category;
        typedef typename StringMap::value_type                  value_type;
        typedef typename wrapped_iterator::difference_type      difference_type;
        typedef value_type*                                     pointer;
        typedef value_type&                                     reference;

        iterator_base() { }
        iterator_base(wrapped_iterator it) : _it(it) { }

        reference operator*() const { return _it->second->pair; }
        pointer operator->() const { return &_it->second->pair; }

        iterator_base& operator++() { ++_it; return *this; }
        iterator_base operator++(int) { return _it++; }
        iterator_base& operator--() { --_it; return *this; }
        iterator_base operator--(int) { return _it--; }

        bool operator==(iterator_base it) { return _it == it._it; }
        bool operator!=(iterator_base it) { return _it != it._it; }

      private:
        wrapped_iterator _it;
    };

    internal_map _map;

    StringMap& operator=(const StringMap&);  // DISALLOW_ASSIGN
};

template <typename T, typename Compare>
class StringMap<T, Compare>::iterator : public iterator_base<wrapped_iterator> {
  public:
    iterator() { }
    iterator(const_iterator it) : iterator_base<wrapped_iterator>(it._it) { }

  private:
    friend class StringMap;
    iterator(wrapped_iterator it) : iterator_base<wrapped_iterator>(it) { }
};

template <typename T, typename Compare>
class StringMap<T, Compare>::const_iterator : public iterator_base<wrapped_const_iterator> {
  public:
    const_iterator() { }

  private:
    friend class StringMap;
    friend class iterator;
    const_iterator(wrapped_const_iterator it) : iterator_base<wrapped_const_iterator>(it) { }
};

template <typename T, typename Compare>
StringMap<T, Compare>::StringMap(const StringMap& other) {
    foreach (it, other) {
        insert(*it);
    }
}

template <typename T, typename Compare>
typename StringMap<T, Compare>::mapped_type& StringMap<T, Compare>::operator[](
        const key_type& key) {
    wrapped_iterator it = _map.find(key);
    if (it == _map.end()) {
        sfz::linked_ptr<WrappedValue> value(new WrappedValue(key, value));
        _map.insert(std::make_pair(value->key_storage, value));
        return value->pair.second;
    }
    return it->pair.second;
}

template <typename T, typename Compare>
std::pair<typename StringMap<T, Compare>::iterator, bool> StringMap<T, Compare>::insert(
        const value_type& pair) {
    const sfz::StringPiece& key = pair.first;
    const mapped_type& value = pair.second;
    wrapped_iterator it = _map.find(key);
    if (it == _map.end()) {
        sfz::linked_ptr<WrappedValue> inserted(new WrappedValue(key, value));
        it = _map.insert(typename internal_map::value_type(inserted->key_storage, inserted)).first;
        return make_pair(iterator(it), true);
    } else {
        return make_pair(iterator(it), false);
    }
}

struct StringPieceLess {
    bool operator()(const sfz::StringPiece& lhs, const sfz::StringPiece& rhs) const {
        for (sfz::StringPiece::const_iterator it = lhs.begin(), jt = rhs.begin(),
                it_end = rhs.end(), jt_end = rhs.end(); true; ++it, ++jt) {
            if (jt == jt_end) {
                return false;
            } else if (it == it_end) {
                return true;
            } else if (*it < *jt) {
                return true;
            } else if (*jt < *it) {
                return false;
            }
        }
    }
};

}  // namespace rgos

#endif  // RGOS_STRING_MAP_HPP_
