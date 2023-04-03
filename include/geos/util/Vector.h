/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2022 ISciences LLC
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <cstdint>
#include <cstring>
#include <iterator>

namespace geos {
namespace util {

/**
 * \brief
 * A partial re-implementation of std::vector intended for use by
 * CoordinateSequence. Unlike std::vector, it can be backed by an
 * external buffer or an internally-owned buffed.
 */
template<typename T>
class Vector {

public:
    Vector() :
        m_buf(nullptr),
        m_capacity(0),
        m_size(0)
    {}

    Vector(std::size_t sz) :
        m_buf(nullptr),
        m_capacity(0),
        m_size(static_cast<std::uint32_t>(0))
    {
        resize(sz);
    }

    Vector(std::size_t sz, T* buf) :
        m_buf(buf),
        m_capacity(0),
        m_size(static_cast<std::uint32_t>(sz))
    {}

    Vector(const Vector& other) : Vector() {
        if (!other.empty()) {
            m_buf = make_space(nullptr, other.size());
            std::memcpy(m_buf, other.m_buf, other.size()*sizeof(T));
        }
    }

    Vector& operator=(const Vector& other) {
        clear();

        if (!other.empty()) {
            m_buf = make_space(nullptr, other.size());
            std::memcpy(m_buf, other.m_buf, other.size()*sizeof(T));
        }

        return *this;
    }

    Vector(Vector&& other) :
        m_buf(other.m_buf),
        m_capacity(other.m_capacity),
        m_size(other.m_size)
    {
        other.m_buf = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
    }

    Vector& operator=(Vector&& other) {
        if (owned()) {
            delete[] m_buf;
        }

        m_buf = other.m_buf;
        m_capacity = other.m_capacity;
        m_size = other.m_size;
        other.m_buf = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;

        return *this;
    }

    ~Vector() {
        if (owned()) {
            delete[] m_buf;
        }
    }

    void push_back(T item) {
        growIfNeeded(1);
        assert(size() < capacity());
        m_buf[m_size++] = item;
    }

    void pop_back() {
        assert(size() > 0);
        m_size--;
    }

    T* make_space(T* pos, std::size_t n) {
        auto loc = pos == nullptr ? 0 : pos - begin();
        growIfNeeded(n);
        pos = begin() + loc;

        if (pos != end()) {
            auto num_to_move = static_cast<std::size_t>(end() - pos);
            std::memmove(pos + n, pos, num_to_move*sizeof(T));
        }
        m_size += static_cast<std::uint32_t>(n);

        return pos;
    }

    void insert(T* pos, std::size_t n, const T& value) {
        pos = make_space(pos, n);
        std::fill(pos, pos + n, value);
    }

    template<typename Iter>
    void insert(T* pos, Iter from, Iter to) {
        auto n = static_cast<std::size_t>(to - from);

        if (from >= begin() && from < end()) {
            // from and to may be invalidated
            auto from_n = from - begin();
            auto to_n = to - begin();

            pos = make_space(pos, n);

            from = begin() + from_n;
            to = begin() + to_n;
        } else {
            pos = make_space(pos, n);
        }

        std::copy(from, to, pos);
    }

    template<typename Iter>
    void assign(Iter from, Iter to) {
        assert(static_cast<std::size_t>(to - from) <= size());
        std::copy(from, to, begin());
    }

    void reserve(std::size_t sz) {
        if (sz <= capacity()) {
            return;
        }

        T* tmp = sz > 0 ? new T[sz] : nullptr;
        if (tmp && !empty()) {
            std::memcpy(tmp, m_buf, m_size * sizeof(T));
        }
        if (owned()) {
            delete[] m_buf;
        }
        m_buf = tmp;
        m_capacity = static_cast<std::uint32_t>(sz);
    }

    void resize(std::size_t sz) {
        reserve(sz);
        m_size = static_cast<std::uint32_t>(sz);
    }

    void clear() {
        m_size = 0;
    }

    std::size_t capacity() const {
        return m_capacity;
    }

    std::size_t size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }

    bool owned() const {
        return data() == nullptr || !(capacity() == 0 && size() > 0);
    }

    const T& operator[](std::size_t i) const {
        return *(data() + i);
    }

    T& operator[](std::size_t i) {
        return *(data() + i);
    }

    T* release() {
        m_capacity = 0;
        return m_buf;
    }

    T* data() {
        return m_buf;
    }

    const T* data() const {
        return m_buf;
    }

    T* begin() {
        return data();
    };

    T* end() {
        return data() + size();
    }

    const T* begin() const {
        return data();
    };

    const T* end() const {
        return data() + size();
    }

private:

    void growIfNeeded(std::size_t num_to_add) {
        if (size() + num_to_add > capacity()) {
            auto new_capacity = capacity() == 0 ?
                        std::max(size() + num_to_add, static_cast<std::size_t>(4)) :
                        static_cast<std::size_t>(static_cast<double>(capacity()) * 1.5);
            new_capacity = std::max(new_capacity, capacity() + num_to_add);
            reserve(new_capacity);
        }
    }

    T* m_buf;
    std::uint32_t m_capacity;
    std::uint32_t m_size;
};


}
}
