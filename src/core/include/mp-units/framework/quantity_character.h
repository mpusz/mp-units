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
// tensor) and its *numeric field* (real, complex). A quantity has exactly one of each. The
// enumerators are declared in increasing rank so that comparing two of them compares their ranks
// (scalar < vector < tensor, real < complex).
MP_UNITS_EXPORT enum class quantity_tensor_order : std::int8_t { scalar, vector, tensor };
MP_UNITS_EXPORT enum class quantity_field : std::int8_t { real, complex };

namespace detail {

template<typename T>
concept CharacterAxis = std::same_as<T, quantity_tensor_order> || std::same_as<T, quantity_field>;

// What a quantity specification *admits*, as opposed to what a quantity *is*.
//
// A concrete specification admits exactly one flag per axis, but a quantity *kind* admits every one
// of them: it constrains neither axis, because a kind quantity stores whatever representation the
// user picks and nothing in the hierarchy can narrow that down -- a simple quantity never declares a
// specification at all, so the tree cannot even be consulted. Representations are described the same
// way, by the slots they can fill: a scalar representation fills a scalar, vector, or tensor slot, a
// vector one fills vector or tensor, and a tensor one only tensor. That is the degenerate-rank rule
// of ISO 80000-2:2019, 18, held as data rather than as a chain of recursive concept definitions, and
// it makes matching one intersection per axis.
//
// All of this is private, and deliberately so. The public axes above name single values, so a user
// cannot write a set, a specification cannot declare an impossible one, and an implementation stays
// free to represent "admits everything" however it likes rather than having a mask imposed on it.
// The set algebra rides on an enum nested in a class so that its operators can be hidden friends of
// the enclosing class: the class is an associated scope of the nested enum, so they are found by
// argument-dependent lookup exactly where these types appear and nowhere else.
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

// The one flag standing for a public axis value. Every query below takes the public value on the
// other side, so a flag set never has to be translated back and can never surface.
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

// Does a flag set admit `value`? Ask this when a *possibility* is what matters ("may this be a
// vector?"). This is the whole of character matching.
[[nodiscard]] consteval bool admits(order_flags set, quantity_tensor_order value)
{
  return (set & to_flags(value)) != order_flags{};
}

[[nodiscard]] consteval bool admits(field_flags set, quantity_field value)
{
  return (set & to_flags(value)) != field_flags{};
}

// Does a flag set admit `value` and nothing else? Ask this when a *certainty* is what matters ("is
// this definitely a vector?"). The two questions part company exactly at a quantity kind, which
// admits vector and therefore answers yes to the first and no to the second.
[[nodiscard]] consteval bool admits_only(order_flags set, quantity_tensor_order value)
{
  return set == to_flags(value);
}

[[nodiscard]] consteval bool admits_only(field_flags set, quantity_field value) { return set == to_flags(value); }

// Do two flag sets share a flag? Matching a representation against a specification is this question
// on each axis: the slots the representation can fill against the characters the specification admits.
[[nodiscard]] consteval bool overlaps(order_flags lhs, order_flags rhs) { return (lhs & rhs) != order_flags{}; }

[[nodiscard]] consteval bool overlaps(field_flags lhs, field_flags rhs) { return (lhs & rhs) != field_flags{}; }

// Does a flag set name a single flag, i.e. is the answer definite?
template<typename T>
  requires std::same_as<T, order_flags> || std::same_as<T, field_flags>
[[nodiscard]] consteval bool is_definite(T set)
{
  return std::has_single_bit(static_cast<std::uint8_t>(set));
}

// The highest rank a flag set admits, as a public value. Used to combine the characters of a derived
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
 * - its *tensor order* (`quantity_tensor_order`): scalar (rank 0), vector (rank 1), or
 *   tensor (rank 2), and
 * - its *numeric field* (`quantity_field`): real or complex.
 *
 * This is the character a quantity *has*, and the spelling in which a quantity specification
 * *declares* one, so each axis is a single value. What a specification *admits* is a separate,
 * private matter: a quantity kind constrains neither axis, and no public value has to express that.
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

  // Deliberately no comparison operators. Both axes are flag sets, so neither relational nor
  // equality comparison of two whole characters asks a question worth answering: a caller wants to
  // know what a character *admits*, which is `admits()` per axis, or that it admits one thing
  // and nothing else, which is `admits_only()`. Set equality would answer those correctly only by
  // accident of the encoding. Comparison is also not needed to use a character as a non-type
  // template argument: template-argument equivalence for a class type is member-wise.
};

namespace detail {

// A specification's character as the library matches it: a flag set per axis. A concrete
// specification's mask names one flag per axis, a quantity kind's names them all.
struct character_mask {
  order_flags order = to_flags(quantity_tensor_order::scalar);
  field_flags field = to_flags(quantity_field::real);
};

// The mask a declared character stands for.
[[nodiscard]] consteval character_mask to_mask(quantity_character ch)
{
  return character_mask{to_flags(ch.order), to_flags(ch.field)};
}

// The spellings of a character constraint that name a character *directly*: one axis, both axes, or
// the legacy flat enum. A quantity specification also stands for a character, but it is deliberately
// not one of these: the `…Of` concepts take a specification to mean the quantity-tree question
// (`ReferenceOf`, `implicitly_convertible`) and a character axis to mean the character question, and
// keeping the two guards disjoint by *type* is what stops one from silently widening the other.
MP_UNITS_DIAGNOSTIC_PUSH
MP_UNITS_DIAGNOSTIC_IGNORE_DEPRECATED
template<auto V>
concept CharacterValue =
  CharacterAxis<MP_UNITS_NTTP_HACK(decltype(V))> || std::same_as<MP_UNITS_NTTP_HACK(decltype(V)), quantity_character> ||
  std::same_as<MP_UNITS_NTTP_HACK(decltype(V)), quantity_character_legacy>;

// The mask such a spelling stands for. A bare axis constrains only its own axis and leaves the other
// free, which is simply every flag there.
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

// Does a mask admit exactly the declared character `ch` on both axes? The named form of what an
// equality comparison would have spelled, kept named so that the "and nothing else" is visible: a
// quantity kind is not exactly anything, and this answers `false` for one rather than looking like a
// mismatch.
[[nodiscard]] consteval bool admits_only(character_mask mask, quantity_character ch)
{
  return admits_only(mask.order, ch.order) && admits_only(mask.field, ch.field);
}

// The public character a mask stands for. Only a definite mask has one: a quantity kind admits every
// character, so there is nothing to report, and asking is a bug on the caller's side rather than a
// value to invent. Reachable only through the deprecated `character` members, which is exactly the
// case that used to answer `scalar` for a vector-backed kind quantity.
//
// The mask travels as a template argument rather than as a function argument, and the function is
// `constexpr` rather than `consteval`, because every caller is the initializer of a `static constexpr`
// member of a class template. A call spelled with the mask as an argument is a non-dependent
// expression there, so Clang checks it while parsing the template instead of on instantiation, and
// then rejects the consteval call for not being an immediate invocation. Naming the mask as a
// template argument makes the call dependent on the enclosing instantiation, which is also what keeps
// merely naming such a type free of the diagnostic below.
template<character_mask Mask>
[[nodiscard]] constexpr quantity_character definite_character()
{
  // Only has to make the evaluation non-constant, so that the initializer of the `constexpr` member
  // is ill-formed. `MP_UNITS_THROW` because a core header also has to build with exceptions disabled;
  // a string literal rather than an exception type keeps the message visible at the point of the
  // diagnostic without dragging `<stdexcept>` in.
  if constexpr (!is_definite(Mask.order) || !is_definite(Mask.field))
    MP_UNITS_THROW(
      "a quantity kind admits every character, so it has none to report; ask "
      "QuantityOf<Q, quantity_tensor_order::vector> or another character instead");
  else
    return quantity_character{highest_order(Mask.order), highest_field(Mask.field)};
}

// Is the character a definite real scalar? This is the precondition for `non_negative`: a quantity
// may only be declared non-negative when it cannot be a vector, a tensor, or complex, and a mask
// admitting several orders (a kind) is not a definite anything.
[[nodiscard]] consteval bool is_real_scalar(character_mask mask) { return admits_only(mask, quantity_character{}); }

}  // namespace detail

}  // namespace mp_units
