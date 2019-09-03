/* Copyright 2019 WobLight
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

#include <memory>

template <class T>
struct AutoPtr {
    using disposer_t = void(*)(T*);

    T *pointer = nullptr;
    disposer_t disposer;
    AutoPtr(disposer_t d) noexcept : disposer(d){ }
    AutoPtr(const AutoPtr &other) = delete;
    AutoPtr(AutoPtr &&other) noexcept : pointer(std::move(other.pointer)), disposer(std::forward<disposer_t>(other.disposer)) { other.pointer = nullptr; }
    AutoPtr(T* p, disposer_t d) noexcept : pointer(p), disposer(d){ }

    void reset(T* t = nullptr) { disposer(pointer); pointer = t; }

    ~AutoPtr(){ disposer(pointer); }
    AutoPtr& operator =(AutoPtr &&other){ disposer(pointer); pointer = std::move(other.pointer); disposer(std::move(other.disposer)); other.pointer = nullptr; return *this; }
    AutoPtr& operator =(const AutoPtr &other) = delete ;

    operator T*&() noexcept { return pointer; }
    T** operator &() noexcept { return &pointer; }
    bool operator !() noexcept { return !pointer; }
    T*& operator ->() noexcept { return pointer; }
};

#endif // UTILS_H
