// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// NOLINTBEGIN(bugprone-reserved-identifier, cppcoreguidelines-macro-usage)
#pragma once

#include <mp-units/bits/compiler.h>

// Hack for compiler bugs: `decltype(V)` of a class-type non-type template parameter should be
// the parameter's type, but two compilers get it wrong. GCC 12 gives `const T` (GCC bug 98820, fixed
// in GCC 13). LLVM Clang 23 gives `const T&` once a constraint passes `V` on to another concept
// (https://github.com/llvm/llvm-project/issues/230945). Wrap such a `decltype(V)` in this macro: it
// adds the trait that undoes the bug on those two compilers and leaves the type alone everywhere else.
// Newer GCC still yields `const T` for a `V` mapped from a static member such as `T::_quantity_spec_`,
// which needs no hack because the classification concepts accept a const-qualified type.
// Extend a branch to a newer version only after that version is confirmed to have the same bug.
#if MP_UNITS_COMP_CLANG == 23 && !MP_UNITS_COMP_APPLE_CLANG
#define MP_UNITS_NTTP_HACK(...) std::remove_cvref_t<__VA_ARGS__>
#elif MP_UNITS_COMP_GCC == 12
#define MP_UNITS_NTTP_HACK(...) std::remove_const_t<__VA_ARGS__>
#else
#define MP_UNITS_NTTP_HACK(...) __VA_ARGS__
#endif

// Hack for a compiler bug: Clang 16 rejects some valid immediate invocations, e.g. a `consteval`
// operator called in the initializer of a static data member of a class template ("cannot take
// address of consteval function ... outside of an immediate invocation"). Clang 17 accepts them.
// Apple Clang 15 and 16 keep the hack they always had, as nobody has tested them without it.
// Extend the branch to a newer version only after that version is confirmed to have the same bug.
#if (MP_UNITS_COMP_CLANG == 16 && !MP_UNITS_COMP_APPLE_CLANG) || MP_UNITS_COMP_APPLE_CLANG == 15 || \
  MP_UNITS_COMP_APPLE_CLANG == 16
#define MP_UNITS_CONSTEVAL_HACK constexpr
#else
#define MP_UNITS_CONSTEVAL_HACK consteval
#endif

// Hack for a compiler bug: Apple Clang 15 (Xcode 15) crashes or takes very long to compile the
// representation concepts when they require `WeaklyRegular`, so those concepts take a simpler form
// there. Apple Clang 16 does not need it.
#if MP_UNITS_COMP_APPLE_CLANG == 15
#define MP_UNITS_XCODE15_HACKS
#endif

// Hack for a library bug: libstdc++ 15.1 in C++23 mode derives some `std::formatter`
// specializations from `std::formatter<__format::__disabled, wchar_t>` for an incomplete
// `__disabled`. Matching that against a constrained program-defined `std::formatter` partial
// specialization checks its constraint on the incomplete type, which is a hard error for a
// concept such as `Dimension` (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=120625, fixed in
// 15.2). Wrap such a constraint in this macro: on the affected library it first checks that
// `T` is complete.
#if _GLIBCXX_RELEASE == 15 && __GLIBCXX__ < 20250808

namespace mp_units::detail {

template<typename T>
inline constexpr bool is_complete = requires { sizeof(T) > 0; };

}  // namespace mp_units::detail

#define MP_UNITS_FORMATTER_HACK(T, ...) (mp_units::detail::is_complete<T> && (__VA_ARGS__))

#else

#define MP_UNITS_FORMATTER_HACK(T, ...) (__VA_ARGS__)

#endif

// Hack for a compiler bug: in a module build, Clang 18 warns about a deprecated data member from
// the implicitly defined copy constructor of its class, which it defines in the importing
// translation unit, outside the `MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED` region around the class.
// Header builds and Clang 16, 17 and 20 to 23 are not affected. Wrap the member's `[[deprecated]]`
// in this macro: on Clang 18 it drops the attribute, so the member stays available without the
// warning.
#if MP_UNITS_COMP_CLANG == 18 && !MP_UNITS_COMP_APPLE_CLANG
#define MP_UNITS_DEPRECATED_MEMBER_HACK(...)
#else
#define MP_UNITS_DEPRECATED_MEMBER_HACK(...) __VA_ARGS__
#endif

// Hack for compiler bugs: during constant evaluation `std::bit_cast` leaves an empty base subobject
// uninitialized, so its result is unusable in a constant expression. GCC 12 and 13 and LLVM Clang 16
// to 18 are affected, GCC 14 and Clang 20 are not. Apple Clang below 19 keeps the hack it always had,
// as nobody has tested it without.
#if MP_UNITS_COMP_GCC == 12 || MP_UNITS_COMP_GCC == 13 ||                                   \
  (MP_UNITS_COMP_CLANG >= 16 && MP_UNITS_COMP_CLANG <= 18 && !MP_UNITS_COMP_APPLE_CLANG) || \
  (MP_UNITS_COMP_APPLE_CLANG && MP_UNITS_COMP_APPLE_CLANG < 19)
#define MP_UNITS_BIT_CAST_HACK 1
#else
#define MP_UNITS_BIT_CAST_HACK 0
#endif

// Hack for a compiler bug: LLVM Clang 19 to 21 mis-evaluate constant expressions that reach a
// pack-indexed type through a module interface, reporting an initializer as non-constant with no
// cause. Header-mode builds are unaffected, and Clang 22 evaluates them correctly. Apple Clang below
// 22 keeps the hack it always had, as nobody has tested it without.
#if (MP_UNITS_COMP_CLANG >= 19 && MP_UNITS_COMP_CLANG <= 21 && !MP_UNITS_COMP_APPLE_CLANG) || \
  (MP_UNITS_COMP_APPLE_CLANG && MP_UNITS_COMP_APPLE_CLANG < 22)
#define MP_UNITS_PACK_INDEXING_HACK 1
#else
#define MP_UNITS_PACK_INDEXING_HACK 0
#endif

// Hack for a compiler bug: GCC 12 parses `t[i, j]` in a requires-expression as the deprecated comma
// subscript `t[(i, j)]`, so a probe for the C++23 multidimensional subscript is satisfied by a type
// with only a one-index `operator[]`. GCC 13 parses it correctly.
#if MP_UNITS_COMP_GCC == 12
#define MP_UNITS_MULTIDIM_SUBSCRIPT_HACK 1
#else
#define MP_UNITS_MULTIDIM_SUBSCRIPT_HACK 0
#endif

// Hack for compiler bugs: GCC 12 and 13 ICE on `decltype(auto(expr))` (in tsubst, at cp/pt.cc).
#if MP_UNITS_COMP_GCC == 12 || MP_UNITS_COMP_GCC == 13
#define MP_UNITS_AUTO_CAST_HACK 1
#else
#define MP_UNITS_AUTO_CAST_HACK 0
#endif

// Hack for a compiler bug: MSVC crashes with an internal compiler error (C1001) when `mag_ratio`
// passes `detail::ratio{N, D}` to `make_magnitude` as a non-type template argument, so `mag_ratio`
// builds the magnitude from prime factorizations there. Confirmed on MSVC 19.51 (`_MSC_VER` 1951).
// Older versions were not tested and keep the hack. Extend the branch to a newer version only after
// that version is confirmed to have the same bug.
#if MP_UNITS_COMP_MSVC && MP_UNITS_COMP_MSVC <= 1951
#define MP_UNITS_MAG_RATIO_HACK 1
#else
#define MP_UNITS_MAG_RATIO_HACK 0
#endif

// Hack for a compiler bug: MSVC omits the implicit copy deduction candidate of a class template with
// a constrained `auto` non-type template parameter whose constraint depends on an earlier parameter,
// like `PO` of `quantity_point`. `quantity_point qp = other_qp;` then fails with C2641, so the hack
// adds an explicit copy deduction guide. Minimal reproducer, accepted by GCC and Clang:
//   template<typename T, int V> concept C = true;
//   template<int R, C<R> auto PO> struct X {};
//   X b = X<0, 1>{};  // error C2641
// Confirmed on MSVC 19.51 (`_MSC_VER` 1951). Older versions were not tested and keep the hack.
// Extend the branch to a newer version only after that version is confirmed to have the same bug.
#if MP_UNITS_COMP_MSVC && MP_UNITS_COMP_MSVC <= 1951
#define MP_UNITS_QUANTITY_POINT_CTAD_HACK 1
#else
#define MP_UNITS_QUANTITY_POINT_CTAD_HACK 0
#endif

// Hack for a compiler bug: MSVC instantiates the initializer of a `constexpr` static data member of
// a class template together with the class, instead of only when the member is used ([temp.inst]).
// The deprecated `character` members of quantity specifications, `quantity`, and `quantity_point` are
// ill-formed when the character isn't definite (a quantity kind, or a specification defined by a kind
// equation), so on MSVC every such type fails (C3615 or C2131). The hack omits all of those members there. Confirmed on
// MSVC 19.51
// (`_MSC_VER` 1951). Older versions are untested and keep the hack. Extend the branch to a newer version only after
// that version is confirmed to have the same bug.
#if MP_UNITS_COMP_MSVC && MP_UNITS_COMP_MSVC <= 1951
#define MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK 1
#else
#define MP_UNITS_EAGER_STATIC_MEMBER_INIT_HACK 0
#endif

// Hack: before C++23 the standard library has no `std::from_range`, which the `fixed_string`
// constructor taking a range relies on. Declaring it in `namespace std` is undefined behavior
// ([namespace.std]), accepted here so that C++20 users can spell the same constructor call.
#if !defined __cpp_lib_ranges_to_container

namespace std {

struct from_range_t {
  explicit from_range_t() = default;
};
inline constexpr from_range_t from_range{};

}  // namespace std

#endif

// NOLINTEND(bugprone-reserved-identifier, cppcoreguidelines-macro-usage)
