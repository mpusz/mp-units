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

#pragma once

// IWYU pragma: private, include <mp-units/framework.h>
#include <mp-units/bits/diagnostics.h>
#include <mp-units/bits/module_macros.h>
#include <mp-units/compat_macros.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>
#endif  // MP_UNITS_IMPORT_STD
#endif  // MP_UNITS_IN_MODULE_INTERFACE

namespace mp_units {

// A quantity's character splits into two orthogonal axes: its *tensor order* (scalar, vector,
// tensor) and its *numeric field* (real, complex). The enumerators are declared in increasing order
// (scalar < vector < tensor, real < complex) because combining the characters of a derived quantity's
// factors takes their maximum.
MP_UNITS_EXPORT enum class quantity_tensor_order : std::int8_t { scalar, vector, tensor };
MP_UNITS_EXPORT enum class quantity_field : std::int8_t { real, complex };

namespace detail {

template<typename T>
concept CharacterAxis = std::same_as<T, quantity_tensor_order> || std::same_as<T, quantity_field>;

// The characters a quantity specification or a representation admits, as a flag set per axis. A
// concrete specification admits one flag per axis. A quantity kind admits all of them, so only the
// representation can restrict the character of a simple quantity such as `42 * m`. A representation
// admits every slot of its own order or higher, a library rule generalizing clause 18 of
// ISO 80000-2:2019 (a scalar is a tensor of order zero, a vector one of order one). The recursive
// `Scalar`, `Vector`, and `Tensor` concepts hold that rule, and representation masks are derived from
// them.
//
// The operators are hidden friends of a class enclosing the enum, so that argument-dependent lookup
// finds them only where these types appear.
struct order_flags_detail {
  enum flags : std::uint8_t { scalar = 1 << 0, vector = 1 << 1, tensor = 1 << 2 };

  [[nodiscard]] friend consteval flags operator|(flags lhs, flags rhs)
  {
    return static_cast<flags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
  }
  [[nodiscard]] friend consteval flags operator&(flags lhs, flags rhs)
  {
    return static_cast<flags>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
  }
};
using order_flags = order_flags_detail::flags;

struct field_flags_detail {
  enum flags : std::uint8_t { real = 1 << 0, complex = 1 << 1 };

  [[nodiscard]] friend consteval flags operator|(flags lhs, flags rhs)
  {
    return static_cast<flags>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
  }
  [[nodiscard]] friend consteval flags operator&(flags lhs, flags rhs)
  {
    return static_cast<flags>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
  }
};
using field_flags = field_flags_detail::flags;

inline constexpr order_flags any_order =
  order_flags_detail::scalar | order_flags_detail::vector | order_flags_detail::tensor;
inline constexpr field_flags any_field = field_flags_detail::real | field_flags_detail::complex;

// The one flag standing for a public axis value.
[[nodiscard]] consteval order_flags to_flags(quantity_tensor_order order)
{
  switch (order) {
    case quantity_tensor_order::vector:
      return order_flags_detail::vector;
    case quantity_tensor_order::tensor:
      return order_flags_detail::tensor;
    default:
      return order_flags_detail::scalar;
  }
}

[[nodiscard]] consteval field_flags to_flags(quantity_field field)
{
  return field == quantity_field::complex ? field_flags_detail::complex : field_flags_detail::real;
}

// Whether `set` admits `value`, for when a possibility matters ("may this be a vector?").
[[nodiscard]] consteval bool admits(order_flags set, quantity_tensor_order value)
{
  return (set & to_flags(value)) != order_flags{};
}

[[nodiscard]] consteval bool admits(field_flags set, quantity_field value)
{
  return (set & to_flags(value)) != field_flags{};
}

// Whether `set` admits `value` and nothing else, for when a certainty matters ("is this definitely a
// vector?"). The two answers differ for a quantity kind, for example: it admits vector, but not only
// vector.
[[nodiscard]] consteval bool admits_only(order_flags set, quantity_tensor_order value)
{
  return set == to_flags(value);
}

[[nodiscard]] consteval bool admits_only(field_flags set, quantity_field value) { return set == to_flags(value); }

// Whether two flag sets share a flag. `RepresentationOf` and `QuantitySpecOf` match on this per axis.
[[nodiscard]] consteval bool overlaps(order_flags lhs, order_flags rhs) { return (lhs & rhs) != order_flags{}; }

[[nodiscard]] consteval bool overlaps(field_flags lhs, field_flags rhs) { return (lhs & rhs) != field_flags{}; }

template<typename T>
  requires std::same_as<T, order_flags> || std::same_as<T, field_flags>
[[nodiscard]] consteval bool is_definite(T set)
{
  return std::has_single_bit(static_cast<std::uint8_t>(set));
}

// The highest order a flag set admits, as a public value. Used to combine the characters of a derived
// quantity's factors.
[[nodiscard]] consteval quantity_tensor_order highest_order(order_flags set)
{
  if (admits(set, quantity_tensor_order::tensor)) return quantity_tensor_order::tensor;
  if (admits(set, quantity_tensor_order::vector)) return quantity_tensor_order::vector;
  return quantity_tensor_order::scalar;
}

[[nodiscard]] consteval quantity_field highest_field(field_flags set)
{
  return admits(set, quantity_field::complex) ? quantity_field::complex : quantity_field::real;
}

}  // namespace detail

// The pre-2.6.0 flat character enumeration, kept as a user-facing compatibility spelling
// (`quantity_character::real_scalar`, `::vector`, ...). The library itself uses the two-axis
// form; this enum is provided only so existing user code keeps compiling. It is deprecated to
// steer new code toward the `quantity_tensor_order` and `quantity_field` axes.
#define MP_UNITS_QCL_DEPRECATED \
  [[deprecated("2.6.0: use the 'quantity_tensor_order' and 'quantity_field' axes instead")]]
MP_UNITS_EXPORT enum class MP_UNITS_QCL_DEPRECATED quantity_character_legacy : std::int8_t {
  real_scalar MP_UNITS_QCL_DEPRECATED,
  complex_scalar MP_UNITS_QCL_DEPRECATED,
  vector MP_UNITS_QCL_DEPRECATED,
  tensor MP_UNITS_QCL_DEPRECATED
};
#undef MP_UNITS_QCL_DEPRECATED

/**
 * @brief Quantity character
 *
 * A quantity character classifies a quantity along two orthogonal axes:
 *
 * - its *tensor order* (`quantity_tensor_order`): scalar (order 0), vector (order 1), or
 *   tensor (order 2), and
 * - its *numeric field* (`quantity_field`): real or complex.
 *
 * A quantity specification declares its character in this form, so each axis is a single value. A
 * quantity kind declares none, as it constrains neither axis.
 *
 * Scalars, vectors, and tensors are mathematical objects independent of the choice of a
 * coordinate system, whereas their components depend on it. A scalar has magnitude but no
 * direction; a vector possesses both magnitude and direction; a tensor (for example the Cauchy
 * stress tensor) additionally carries orientation. The numeric field separately distinguishes real
 * quantities from complex ones (such as a voltage phasor or a complex permittivity), which affects
 * the operations a quantity admits.
 *
 * The pre-2.6.0 flat spelling (`quantity_character::real_scalar`, `::complex_scalar`, `::vector`,
 * `::tensor`) remains available for backward compatibility and maps onto the two axes.
 */
MP_UNITS_EXPORT struct quantity_character {
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
  using enum quantity_character_legacy;
  MP_UNITS_DIAGNOSTIC_POP

  quantity_tensor_order order = quantity_tensor_order::scalar;
  quantity_field field = quantity_field::real;

  consteval quantity_character() = default;
  consteval quantity_character(quantity_tensor_order tensor_order, quantity_field numeric_field) :
      order(tensor_order), field(numeric_field)
  {
  }
  consteval quantity_character(quantity_tensor_order tensor_order) : order(tensor_order) {}
  consteval quantity_character(quantity_field numeric_field) : field(numeric_field) {}
  MP_UNITS_DIAGNOSTIC_PUSH
  MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
  consteval quantity_character(quantity_character_legacy legacy)
  {
    switch (legacy) {
      case real_scalar:
        order = quantity_tensor_order::scalar;
        field = quantity_field::real;
        break;
      case complex_scalar:
        order = quantity_tensor_order::scalar;
        field = quantity_field::complex;
        break;
      case vector:
        order = quantity_tensor_order::vector;
        field = quantity_field::real;
        break;
      case tensor:
        order = quantity_tensor_order::tensor;
        field = quantity_field::real;
        break;
    }
  }
  MP_UNITS_DIAGNOSTIC_POP

  // No comparison operators: matching asks `admits()` or `admits_only()` of a specification's mask.
  // A non-type template argument doesn't need `==`, as template-argument equivalence is member-wise.
};

namespace detail {

// A specification's character as the library matches it.
struct character_mask {
  order_flags order = to_flags(quantity_tensor_order::scalar);
  field_flags field = to_flags(quantity_field::real);
};

[[nodiscard]] consteval character_mask to_mask(quantity_character ch)
{
  return character_mask{to_flags(ch.order), to_flags(ch.field)};
}

// The spellings that name a character directly: one axis, both axes, or the legacy flat enum. A
// quantity specification is excluded, because the `…Of` concepts give it the quantity-tree reading
// (see `QuantityOf`).
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
template<auto V>
concept CharacterValue =
  CharacterAxis<MP_UNITS_NTTP_HACK(decltype(V))> || std::same_as<MP_UNITS_NTTP_HACK(decltype(V)), quantity_character> ||
  std::same_as<MP_UNITS_NTTP_HACK(decltype(V)), quantity_character_legacy>;

// A bare axis constrains only its own axis, so the other one gets every flag.
template<auto V>
  requires CharacterValue<V>
[[nodiscard]] consteval character_mask axis_mask()
{
  using T = MP_UNITS_NTTP_HACK(decltype(V));
  if constexpr (std::same_as<T, quantity_tensor_order>)
    return character_mask{to_flags(V), any_field};
  else if constexpr (std::same_as<T, quantity_field>)
    return character_mask{any_order, to_flags(V)};
  else
    return to_mask(quantity_character{V});
}
MP_UNITS_DIAGNOSTIC_POP

// An exact match on both axes, so a quantity kind never matches.
[[nodiscard]] consteval bool admits_only(character_mask mask, quantity_character ch)
{
  return admits_only(mask.order, ch.order) && admits_only(mask.field, ch.field);
}

// The public character of a definite mask, for the deprecated `character` members. A quantity kind
// admits every character, so asking for one is a bug on the caller's side.
//
// It is `constexpr` and takes the mask as a template argument. Written as a `consteval` function of a
// mask argument, it was rejected by Clang 16 in the `quantity` and `quantity_point` members, where the
// argument is itself a `consteval` call. Where the mask is a constant, the caller keeps it dependent
// (see `kind_of_::character`), as Clang evaluates a non-dependent call while parsing the template.
template<character_mask Mask>
[[nodiscard]] constexpr quantity_character definite_character()
{
  // Only has to make the evaluation non-constant, so that the initializer of the `constexpr` member
  // is ill-formed. `MP_UNITS_THROW` because a core header also has to build with exceptions disabled.
  if constexpr (!is_definite(Mask.order) || !is_definite(Mask.field))
    MP_UNITS_THROW(
      "a quantity kind admits every character, so it has none to report; ask "
      "QuantityOf<Q, quantity_tensor_order::vector> or another character instead");
  else
    return quantity_character{highest_order(Mask.order), highest_field(Mask.field)};
}

// The precondition for `non_negative`: only a quantity that can't be a vector, a tensor, or complex
// may be declared non-negative, so a quantity kind doesn't qualify.
[[nodiscard]] consteval bool is_real_scalar(character_mask mask) { return admits_only(mask, quantity_character{}); }

}  // namespace detail

}  // namespace mp_units
