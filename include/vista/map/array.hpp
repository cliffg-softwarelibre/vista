#ifndef VISTA_MAP_ARRAY_HPP
#define VISTA_MAP_ARRAY_HPP

///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2020 Bjorn Reese <breese@users.sourceforge.net>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <array>
#include <vista/map/span.hpp>

namespace vista
{
namespace map
{

template <typename T1, typename T2>
using pair = std::pair<T1, T2>;

//! @brief Fixed-capacity associative array.
//!
//! Stores all values in-place in continguous memory embedded into the array.
//! Empty slots contains default constructed values. Dynamic memory is only
//! allocated if required by the key or value constructors.
//!
//! Violation of any precondition results in undefined behavior.

template <typename Key,
          typename T,
          std::size_t N,
          typename Compare = std::less<Key>>
class array
    : protected map::span<Key, T, N, Compare>
{
    using super = map::template span<Key, T, N, Compare>;

public:
    using key_type = typename super::key_type;
    using mapped_type = typename super::mapped_type;
    using value_type = typename super::value_type;
    using key_compare = typename super::key_compare;
    using value_compare = typename super::value_compare;
    using size_type = typename super::size_type;
    using pointer = typename super::pointer;
    using iterator = typename super::iterator;
    using const_iterator = typename super::const_iterator;

    //! @brief Creates associative array.
    //!
    //! @post capacity() == N
    //! @post size() == 0

    constexpr array() noexcept;

    //! @brief Checks if span is empty.
    //!
    //! Span is empty when size() == 0

    using super::empty;

    //! @brief Checks if span is full.
    //!
    //! Span is full when size() == capacity()

    using super::full;

    //! @brief Returns the maximum possible number of elements in span.

    using super::capacity;

    //! @brief Returns the number of elements in span.

    using super::size;

    //! @brief Clears the span.
    //!
    //! The storage of the cleared elements will be left in a default-constructed state.
    //!
    //! @post size() == 0

    VISTA_CXX14_CONSTEXPR
    void clear() noexcept(std::is_trivially_destructible<value_type>::value || std::is_nothrow_default_constructible<value_type>::value);

    //! @brief Inserts element with given key.
    //!
    //! Notice that the return type differs from std::map.
    //!
    //! Linear time complexity.
    //!
    //! @returns Iterator to inserted element, or end() if span already is full.

    using super::insert;

    //! @brief Inserts element with given key.
    //!
    //! Linear time complexity.
    //!
    //! @returns Iterator to inserted element, or end() if span already is full.

    VISTA_CXX14_CONSTEXPR
    iterator insert(iterator hint, value_type) noexcept(std::is_nothrow_move_assignable<value_type>::value && vista::detail::is_nothrow_swappable<value_type>::value);

    //! @brief Inserts in-place constructed element.
    //!
    //! Linear time complexity.
    //!
    //! @returns Iterator to inserted element, or end() if span already is full.

    template <typename... Args>
    VISTA_CXX14_CONSTEXPR
    iterator emplace(Args&&...) noexcept(std::is_nothrow_move_assignable<value_type>::value && vista::detail::is_nothrow_swappable<value_type>::value);

    //! @brief Inserts in-place constructed element.
    //!
    //! Linear time complexity.
    //!
    //! @returns Iterator to inserted element, or end() if span already is full.

    template <typename... Args>
    VISTA_CXX14_CONSTEXPR
    iterator emplace_hint(iterator, Args&&...) noexcept(std::is_nothrow_move_assignable<value_type>::value && vista::detail::is_nothrow_swappable<value_type>::value);


    //! @brief Erases all elements with given key.
    //!
    //! The storage of the erased element will be left in a default-constructed state.
    //!
    //! Linear time complexity.

    VISTA_CXX14_CONSTEXPR
    size_type erase(const key_type&) noexcept(std::is_nothrow_move_assignable<value_type>::value);

    //! @brief Erases the element at given position.
    //!
    //! The storage of the erased element will be left in a default-constructed state.
    //!
    //! Linear time complexity.
    //!
    //! @pre position points to valid element.

    VISTA_CXX14_CONSTEXPR
    iterator erase(iterator position) noexcept(std::is_nothrow_move_assignable<value_type>::value);

    //! @brief Checks if span contains key.
    //!
    //! Logarithmic time complexity.

    VISTA_CXX14_CONSTEXPR
    bool contains(const key_type&) const noexcept;

    //! @brief Returns iterator to element with given key.
    //!
    //! If key not found, return end()
    //!
    //! Logarithmic time complexity.

    VISTA_CXX14_CONSTEXPR
    iterator find(const key_type&) noexcept;

    VISTA_CXX14_CONSTEXPR
    const_iterator find(const key_type&) const noexcept;

    //! @brief Returns iterator to element with given key.
    //!
    //! If key not found, then returns iterator to the entry at the position where
    //! the searched-for entry should have been, or the end iterator.
    //!
    //! Logarithmic time complexity.

    using super::lower_bound;

    //! @brief Returns reference to element with given key.
    //!
    //! If key not found, insert element with copied key and default-constructed value.
    //!
    //! Logarithmic time complexity.
    //!
    //! @pre contains(key) or !full()
    //!
    //! @returns Reference to value associated with key.

    VISTA_CXX14_CONSTEXPR
    mapped_type& operator[](const key_type& key) noexcept;

    //! @brief Returns reference to element with given key.
    //!
    //! If key not found, insert element with moved key and default-constructed value.
    //!
    //! Logarithmic time complexity.
    //!
    //! @pre contains(key) or !full()
    //!
    //! @returns Reference to value associated with key.

    VISTA_CXX14_CONSTEXPR
    mapped_type& operator[](key_type&& key) noexcept;

    //! @brief Returns iterator to the beginning of the span.

    using super::begin;
    using super::cbegin;

    //! @brief Returns iterator to the ending of the span.

    using super::end;
    using super::cend;

    using super::key_comp;
    using super::value_comp;

private:
    struct
    {
        value_type storage[N];
    } member;

private:
    static_assert(std::is_default_constructible<value_type>::value, "value_type must be default constructible");
};

} // namespace map
} // namespace vista

#include <vista/map/detail/array.ipp>

#endif // VISTA_MAP_ARRAY_HPP
