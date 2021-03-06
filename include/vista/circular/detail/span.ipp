///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2018 Bjorn Reese <breese@users.sourceforge.net>
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>

namespace vista
{
namespace circular
{

//-----------------------------------------------------------------------------
// span<T>
//-----------------------------------------------------------------------------

template <typename T, std::size_t E>
constexpr span<T, E>::span() noexcept
{
}

template <typename T, std::size_t E>
template <typename OtherT,
          std::size_t OtherExtent,
          typename std::enable_if<(E == OtherExtent || E == dynamic_extent) && std::is_convertible<OtherT (*)[], T (*)[]>::value, int>::type>
constexpr span<T, E>::span(const span<OtherT, OtherExtent>& other) noexcept
    : member(other)
{
}

template <typename T, std::size_t E>
template <typename ContiguousIterator>
constexpr span<T, E>::span(ContiguousIterator begin,
                           ContiguousIterator end) noexcept
    : member(std::move(begin), std::move(end))
{
}

template <typename T, std::size_t E>
template <typename ContiguousIterator>
constexpr span<T, E>::span(ContiguousIterator begin,
                           ContiguousIterator end,
                           ContiguousIterator first,
                           size_type length) noexcept
    : member(std::move(begin), std::move(end), std::move(first), length)
{
}

template <typename T, std::size_t E>
template <std::size_t N,
          typename std::enable_if<(E == N || E == dynamic_extent), int>::type>
constexpr span<T, E>::span(value_type (&array)[N]) noexcept
    : member(array)
{
}

template <typename T, std::size_t E>
constexpr span<T, E>::span(const span& other, pointer data) noexcept
    : member(other.member, data)
{
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::assign(const span& other, pointer data) noexcept
{
    member.assign(other.member, data);
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::operator=(std::initializer_list<value_type> input) noexcept(std::is_nothrow_move_assignable<value_type>::value) -> span&
{
    static_assert(std::is_move_assignable<T>::value, "T must be MoveAssignable");

    assign(std::move(input));
    return *this;
}

template <typename T, std::size_t E>
constexpr bool span<T, E>::empty() const noexcept
{
    return size() == 0;
}

template <typename T, std::size_t E>
constexpr bool span<T, E>::full() const noexcept
{
    return size() == capacity();
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::capacity() const noexcept -> size_type
{
    return member.capacity();
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::size() const noexcept -> size_type
{
    return member.size;
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::front() noexcept -> reference
{
    assert(!empty());

    return at(front_index());
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::front() const noexcept -> const_reference
{
    VISTA_CXX14(assert(!empty()));

    return at(front_index());
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::back() noexcept -> reference
{
    assert(!empty());

    return at(back_index());
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::back() const noexcept -> const_reference
{
    VISTA_CXX14(assert(!empty()));

    return at(back_index());
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::operator[](size_type position) noexcept -> reference
{
    return at(front_index() + position);
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::operator[](size_type position) const noexcept -> const_reference
{
    return at(front_index() + position);
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::clear() noexcept
{
    member.size = 0;
    member.next = member.capacity();
}

template <typename T, std::size_t E>
template <typename InputIterator>
VISTA_CXX14_CONSTEXPR
void span<T, E>::assign(InputIterator first, InputIterator last) noexcept(std::is_nothrow_copy_assignable<value_type>::value)
{
    clear();
    push_back(std::move(first), std::move(last));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::assign(std::initializer_list<value_type> input) noexcept(std::is_nothrow_move_assignable<value_type>::value)
{
    static_assert(std::is_move_assignable<T>::value, "T must be MoveAssignable");

    clear();
    for (auto&& value : input)
    {
        push_back(std::move(value));
    }
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::push_front(value_type input) noexcept(std::is_nothrow_move_assignable<value_type>::value)
{
    static_assert(std::is_move_assignable<T>::value, "T must be MoveAssignable");

    expand_front();
    front() = std::move(input);
}

template <typename T, std::size_t E>
template <typename InputIterator>
VISTA_CXX14_CONSTEXPR
void span<T, E>::push_front(InputIterator first,
                            InputIterator last) noexcept(std::is_nothrow_copy_assignable<value_type>::value)
{
    static_assert(std::is_copy_assignable<T>::value, "T must be CopyAssignable");

    while (first != last)
    {
        push_front(*first);
        ++first;
    }
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::push_back(value_type input) noexcept(std::is_nothrow_move_assignable<value_type>::value)
{
    static_assert(std::is_move_assignable<T>::value, "T must be MoveAssignable");

    expand_back();
    back() = std::move(input);
}

template <typename T, std::size_t E>
template <typename InputIterator>
VISTA_CXX14_CONSTEXPR
void span<T, E>::push_back(InputIterator first,
                           InputIterator last) noexcept(std::is_nothrow_copy_assignable<value_type>::value)
{
    static_assert(std::is_copy_assignable<T>::value, "T must be CopyAssignable");

    while (first != last)
    {
        push_back(*first);
        ++first;
    }
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::pop_front() noexcept(std::is_nothrow_move_constructible<value_type>::value) -> value_type
{
    static_assert(std::is_move_constructible<T>::value, "T must be MoveConstructible");

    auto& old_front = front();
    remove_front(); // Item still lingers in storage
    return std::move(old_front);
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::pop_back() noexcept(std::is_nothrow_move_constructible<value_type>::value) -> value_type
{
    static_assert(std::is_move_constructible<T>::value, "T must be MoveConstructible");

    auto& old_back = back();
    remove_back(); // Item still lingers in storage
    return std::move(old_back);
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::expand_front(size_type count) noexcept
{
    assert(count <= capacity());

    const auto unused = capacity() - size();
    if (count > unused)
    {
        member.next = member.capacity() + index(member.next - count + unused);
        member.size = member.capacity();
    }
    else
    {
        member.size += count;
    }
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::expand_back(size_type count) noexcept
{
    assert(count <= capacity());

    member.next = member.capacity() + index(member.next + count);
    if (count > capacity() - size())
    {
        member.size = capacity();
    }
    else
    {
        member.size += count;
    }
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::remove_front(size_type count) noexcept
{
    assert(size() > 0);
    assert(count <= size());

    member.size -= count;
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::remove_back(size_type count) noexcept
{
    assert(size() > 0);
    assert(count <= size());

    member.next = member.capacity() + index(member.next - count);
    member.size -= count;
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::rotate_front() noexcept(vista::detail::is_nothrow_swappable<value_type>::value)
{
    if (empty())
        return;

    const auto first = index(front_index());
    if (first == 0)
        return;

    const auto last = capacity() - first;
    rotate_range(first, last);
    member.next = member.capacity() + size();
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::begin() noexcept -> iterator
{
    return iterator(this, vindex(front_index()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::begin() const noexcept -> const_iterator
{
    return const_iterator(this, vindex(front_index()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::cbegin() const noexcept -> const_iterator
{
    return const_iterator(this, vindex(front_index()));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::end() noexcept -> iterator
{
    return iterator(this, vindex(member.next));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::end() const noexcept -> const_iterator
{
    return const_iterator(this, vindex(member.next));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::cend() const noexcept -> const_iterator
{
    return const_iterator(this, vindex(member.next));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::rbegin() noexcept -> reverse_iterator
{
    return reverse_iterator(std::move(end()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::rbegin() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(std::move(end()));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::rend() noexcept -> reverse_iterator
{
    return reverse_iterator(std::move(begin()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::rend() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(std::move(begin()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::crbegin() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(std::move(end()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::crend() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(std::move(begin()));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::first_segment() noexcept -> segment
{
    return (empty())
        ? segment()
        : (wraparound()
           ? segment(member.data + front_index(),
                     member.data + capacity())
           : segment(member.data + index(front_index()),
                     member.data + index(back_index()) + 1));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::first_segment() const noexcept -> const_segment
{
    return (empty())
        ? const_segment()
        : (wraparound()
           ? const_segment(member.data + front_index(),
                           member.data + capacity())
           : const_segment(member.data + index(front_index()),
                           member.data + index(back_index()) + 1));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::last_segment() noexcept -> segment
{
    return wraparound() && (index(member.next) < size())
        ? segment(member.data,
                  member.data + index(back_index()) + 1)
        : segment();
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::last_segment() const noexcept -> const_segment
{
    return wraparound() && (index(member.next) < size())
        ? const_segment(member.data,
                        member.data + index(back_index()) + 1)
        : const_segment();
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::first_unused_segment() noexcept -> segment
{
    return (full())
        ? segment()
        : segment(member.data + member.next - capacity(),
                  member.data + std::min(front_index(), capacity()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::first_unused_segment() const noexcept -> const_segment
{
    return (full())
        ? const_segment()
        : (wraparound()
           ? const_segment(member.data + back_index() + 1,
                           member.data + capacity())
           : const_segment(member.data + back_index() + 1,
                           member.data + capacity()));
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::last_unused_segment() noexcept -> segment
{
    return (full() || !unused_wraparound())
        ? segment()
        : segment(member.data,
                  member.data + index(front_index()));
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::last_unused_segment() const noexcept -> const_segment
{
    return (full() || !unused_wraparound())
        ? const_segment()
        : const_segment(member.data,
                        member.data + index(front_index()));
}

//-----------------------------------------------------------------------------

template <typename T, std::size_t E>
constexpr auto span<T, E>::index(size_type position) const noexcept -> size_type
{
    return member.modulo(position, member.capacity());
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::vindex(size_type position) const noexcept -> size_type
{
    return member.modulo(position, 2 * member.capacity());
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::front_index() const noexcept -> size_type
{
    return member.next - member.size;
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::back_index() const noexcept -> size_type
{
    return member.next - 1;
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::at(size_type position) noexcept -> reference
{
    return member.data[index(position)];
}

template <typename T, std::size_t E>
constexpr auto span<T, E>::at(size_type position) const noexcept -> const_reference
{
    return member.data[index(position)];
}

template <typename T, std::size_t E>
constexpr bool span<T, E>::wraparound() const noexcept
{
    return index(front_index()) > index(back_index());
}

template <typename T, std::size_t E>
constexpr bool span<T, E>::unused_wraparound() const noexcept
{
    return front_index() > capacity();
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::rotate_range(size_type lower_length,
                              size_type upper_length) noexcept(vista::detail::is_nothrow_swappable<value_type>::value)
{
    // Based on Gries-Mills block swapping rotate
    if (lower_length == 0 || upper_length == 0)
        return;
    const auto position = lower_length;
    while (lower_length != upper_length)
    {
        if (lower_length > upper_length)
        {
            swap_range(position - lower_length, position, upper_length);
            lower_length -= upper_length;
        }
        else
        {
            swap_range(position - lower_length, position + upper_length - lower_length, lower_length);
            upper_length -= lower_length;
        }
    }
    swap_range(position - lower_length, position, lower_length);
}

template <typename T, std::size_t E>
VISTA_CXX14_CONSTEXPR
void span<T, E>::swap_range(size_type lhs,
                            size_type rhs,
                            size_type length) noexcept(vista::detail::is_nothrow_swappable<value_type>::value)
{
    for (size_type k = 0; k < length; ++k)
    {
        using std::swap;
        swap(at(lhs + k), at(rhs + k));
    }
 }

//-----------------------------------------------------------------------------
// span<T>::member_storage fixed extent
//-----------------------------------------------------------------------------

// std::addressof(x) and std::distance(a, b) are not constexpr before C++17, so
// we use &x and b - a instead, which ought to work for ContiguousIterator.

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
constexpr span<T, E>::member_storage<T1, E1>::member_storage() noexcept
    : data(nullptr),
      size(0),
      next(0)
{
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
constexpr span<T, E>::member_storage<T1, E1>::member_storage(pointer data,
                                                             size_type size,
                                                             size_type next) noexcept
    : data(data),
      size(size),
      next(next)
{
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
constexpr span<T, E>::member_storage<T1, E1>::member_storage(const member_storage& other,
                                                             pointer data) noexcept
    : data(data),
      size(other.size),
      next(other.next)
{
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
template <typename OtherT, std::size_t OtherExtent>
constexpr span<T, E>::member_storage<T1, E1>::member_storage(const span<OtherT,
                                                             OtherExtent>& other) noexcept
    : data(other.member.data),
      size(other.member.size),
      next(other.member.next)
{
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
template <typename ContiguousIterator>
VISTA_CXX14_CONSTEXPR
span<T, E>::member_storage<T1, E1>::member_storage(ContiguousIterator begin,
                                                   ContiguousIterator end) noexcept
    : data(begin == end ? nullptr : &*begin),
      size(0),
      next(size_type(end - begin))
{
    assert(size_type(end - begin) == capacity());
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
template <typename ContiguousIterator>
VISTA_CXX14_CONSTEXPR
span<T, E>::member_storage<T1, E1>::member_storage(ContiguousIterator begin,
                                                   ContiguousIterator end,
                                                   ContiguousIterator first,
                                                   size_type length) noexcept
    : data(begin == end ? nullptr : &*begin),
      size(length),
      next(size_type(first - begin) + length)
{
    assert(size_type(end - begin) == capacity());
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
template <std::size_t N>
constexpr span<T, E>::member_storage<T1, E1>::member_storage(value_type (&array)[N]) noexcept
    : member_storage(array, array + N)
{
    static_assert(N >= E1, "N cannot be smaller than capacity");
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
constexpr auto span<T, E>::member_storage<T1, E1>::capacity() const noexcept -> size_type
{
    return E1;
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
VISTA_CXX14_CONSTEXPR
void span<T, E>::member_storage<T1, E1>::capacity(size_type) noexcept
{
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
VISTA_CXX14_CONSTEXPR
void span<T, E>::member_storage<T1, E1>::assign(const member_storage& other,
                                                pointer data) noexcept
{
    this->data = data;
    this->size = other.size;
    this->next = other.next;
}

template <typename T, std::size_t E>
template <typename T1, std::size_t E1>
constexpr auto span<T, E>::member_storage<T1, E1>::modulo(size_type value,
                                                          size_type n) const noexcept -> size_type
{
    return ((n & (n - 1)) == 0)
        ? value & (n - 1) // Power of two
        : (value % n);
}

//-----------------------------------------------------------------------------
// span<T>::member_storage dynamic extent
//-----------------------------------------------------------------------------

template <typename T, std::size_t E>
template <typename T1>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage() noexcept
    : data(nullptr),
      cap(0),
      size(0),
      next(0)
{
}

template <typename T, std::size_t E>
template <typename T1>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(pointer data,
                                                                         size_type capacity,
                                                                         size_type size,
                                                                         size_type next) noexcept
    : data(data),
      cap(capacity),
      size(size),
      next(next)
{
}

template <typename T, std::size_t E>
template <typename T1>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(const member_storage& other,
                                                                         pointer data) noexcept
    : data(data),
      cap(other.cap),
      size(other.size),
      next(other.next)
{
}

template <typename T, std::size_t E>
template <typename T1>
template <typename OtherT, std::size_t OtherExtent>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(const span<OtherT, OtherExtent>& other) noexcept
    : data(other.member.data),
      cap(other.member.capacity()),
      size(other.member.size),
      next(other.member.next)
{
}

template <typename T, std::size_t E>
template <typename T1>
template <typename ContiguousIterator>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(ContiguousIterator begin,
                                                                         ContiguousIterator end) noexcept
    : data(begin == end ? nullptr : &*begin),
      cap(size_type(end - begin)),
      size(0),
      next(size_type(end - begin))
{
}

template <typename T, std::size_t E>
template <typename T1>
template <typename ContiguousIterator>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(ContiguousIterator begin,
                                                                         ContiguousIterator end,
                                                                         ContiguousIterator first,
                                                                         size_type length) noexcept
    : data(begin == end ? nullptr : &*begin),
      cap(size_type(end - begin)),
      size(length),
      next(size_type(first - begin) + length)
{
}

template <typename T, std::size_t E>
template <typename T1>
template <std::size_t N>
constexpr span<T, E>::member_storage<T1, dynamic_extent>::member_storage(value_type (&array)[N]) noexcept
    : member_storage(array, array + N)
{
}

template <typename T, std::size_t E>
template <typename T1>
constexpr auto span<T, E>::member_storage<T1, dynamic_extent>::capacity() const noexcept -> size_type
{
    return cap;
}

template <typename T, std::size_t E>
template <typename T1>
VISTA_CXX14_CONSTEXPR
void span<T, E>::member_storage<T1, dynamic_extent>::capacity(size_type value) noexcept
{
    cap = value;
}

template <typename T, std::size_t E>
template <typename T1>
VISTA_CXX14_CONSTEXPR
void span<T, E>::member_storage<T1, dynamic_extent>::assign(const member_storage& other,
                                                            pointer data) noexcept
{
    this->data = data;
    capacity(other.capacity());
    this->size = other.size;
    this->next = other.next;
}

template <typename T, std::size_t E>
template <typename T1>
constexpr auto span<T, E>::member_storage<T1, dynamic_extent>::modulo(size_type value,
                                                                      size_type n) const noexcept -> size_type
{
    return ((n & (n - 1)) == 0)
        ? value & (n - 1) // Power of two
        : (value % n);
}

//-----------------------------------------------------------------------------
// span<T>::basic_iterator
//-----------------------------------------------------------------------------

template <typename T, std::size_t E>
template <typename U>
constexpr span<T, E>::basic_iterator<U>::basic_iterator(span_pointer parent,
                                                        size_type position) noexcept
    : parent(parent),
      current(position)
{
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator++() noexcept -> iterator_type&
{
    assert(parent);

    current = parent->vindex(current + 1);
    return *this;
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator++(int) noexcept -> iterator_type
{
    assert(parent);

    auto before = *this;
    current = parent->vindex(current + 1);
    return before;
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator--() noexcept -> iterator_type&
{
    assert(parent);

    current = parent->vindex(current - 1);
    return *this;
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator--(int) noexcept -> iterator_type
{
    assert(parent);

    auto before = *this;
    current = parent->vindex(current - 1);
    return before;
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator+=(difference_type amount) noexcept -> iterator_type&
{
    assert(parent);

    current = parent->vindex(current + amount);
    return *this;
}

template <typename T, std::size_t E>
template <typename U>
constexpr auto span<T, E>::basic_iterator<U>::operator+(difference_type amount) const noexcept -> iterator_type
{
    VISTA_CXX14(assert(parent));

    return iterator_type(parent, current + amount);
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator-=(difference_type amount) noexcept -> iterator_type&
{
    assert(parent);

    current = parent->vindex(current - amount);
    return *this;
}

template <typename T, std::size_t E>
template <typename U>
constexpr auto span<T, E>::basic_iterator<U>::operator-(difference_type amount) const noexcept -> iterator_type
{
    VISTA_CXX14(assert(parent));

    return iterator_type(parent, current - amount);
}

template <typename T, std::size_t E>
template <typename U>
constexpr auto span<T, E>::basic_iterator<U>::operator-(const iterator_type& other) const noexcept -> difference_type
{
    VISTA_CXX14(assert(parent));

    return current - other.current;
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator[](difference_type amount) noexcept -> reference
{
    assert(parent);

    return parent->at(current + amount);
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator-> () noexcept -> pointer
{
    assert(parent);

    return *parent->at(current);
}

template <typename T, std::size_t E>
template <typename U>
VISTA_CXX14_CONSTEXPR
auto span<T, E>::basic_iterator<U>::operator*() noexcept -> reference
{
    assert(parent);

    return parent->at(current);
}

template <typename T, std::size_t E>
template <typename U>
constexpr auto span<T, E>::basic_iterator<U>::operator*() const noexcept -> const_reference
{
    VISTA_CXX14(assert(parent));

    return parent->at(current);
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator==(const iterator_type& other) const noexcept
{
    VISTA_CXX14(assert(parent));
    VISTA_CXX14(assert(parent == other.parent));

    return current == other.current;
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator!=(const iterator_type& other) const noexcept
{
    return !operator==(other);
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator<(const iterator_type& other) const noexcept
{
    VISTA_CXX14(assert(parent));
    VISTA_CXX14(assert(parent == other.parent));

    return current < other.current;
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator<=(const iterator_type& other) const noexcept
{
    VISTA_CXX14(assert(parent));
    VISTA_CXX14(assert(parent == other.parent));

    return current <= other.current;
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator>(const iterator_type& other) const noexcept
{
    VISTA_CXX14(assert(parent));
    VISTA_CXX14(assert(parent == other.parent));

    return current > other.current;
}

template <typename T, std::size_t E>
template <typename U>
constexpr bool span<T, E>::basic_iterator<U>::operator>=(const iterator_type& other) const noexcept
{
    VISTA_CXX14(assert(parent));
    VISTA_CXX14(assert(parent == other.parent));

    return current >= other.current;
}

} // namespace circular
} // namespace vista
