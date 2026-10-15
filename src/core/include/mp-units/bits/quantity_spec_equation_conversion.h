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

#include <mp-units/bits/quantity_spec_hierarchy.h>
#include <mp-units/bits/quantity_spec_hierarchy_conversion.h>
#include <mp-units/bits/ratio.h>
#include <mp-units/bits/type_list.h>
#include <mp-units/compat_macros.h>
#include <mp-units/ext/algorithm.h>
#include <mp-units/ext/type_name.h>
#include <mp-units/framework/quantity_spec_definitions.h>
#include <mp-units/framework/symbolic_expression.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <cstddef>
#include <type_traits>
#endif
#endif

// The equation rule: at least one side is a derived quantity specification. The rules named below
// ("exact equation first", "different roots stay apart", "a product of its own tree", "pairing",
// "self-cancel only against an empty side", "reduce the deepest factor") are the ones the user's
// guide page on quantity specification conversions describes, and the cap on the rank is the kind
// boundary described there.
//
// Each side is a signed multiset of factors kept as two sorted `type_list`s (numerator and
// denominator) of `T` or `power<T, Num, Den>` elements, as `derived_quantity_spec` stores them, and
// the two sides stay apart (a From factor never merges with a To factor). The question "is From
// convertible to To" is answered by reducing From * To^-1 to a plain number with three operations, each
// with a rank:
//  - pair:        a From factor against a To factor of the same tree and sign (the tree rule),
//  - self-cancel: a numerator against a denominator factor of one side and one dimension,
//  - expand:      a factor replaced by its equation (`implicit` on the From side, the target expansion
//                 rank on the To side).
// The rank of the conversion is the weakest operation taken, capped by the kind boundary: a declared
// kind that one side carries in excess of the other lowers the rank regardless of the operations. Lists
// are never turned back into a `derived_quantity_spec`.
namespace mp_units::detail {

// The numerator and the denominator factors of a quantity specification: a derived one lists its
// own, `dimensionless` is the empty product, and any other is a single factor.
template<typename Q>
struct quantity_spec_factors {
  using num = type_list<Q>;
  using den = type_list<>;
};

template<DerivedQuantitySpec Q>
struct quantity_spec_factors<Q> {
  using num = Q::_num_;
  using den = Q::_den_;
};

template<>
struct quantity_spec_factors<struct dimensionless> {
  using num = type_list<>;
  using den = type_list<>;
};

// Both inputs are sorted by `quantity_spec_less` with one element per base, and so is the result,
// which the pairing walk and the exact equation check of the equation rule rely on.
template<typename Lhs, typename Rhs>
using merge_quantity_spec_factors = expr_consolidate<type_list_merge_sorted<Lhs, Rhs, type_list_of_quantity_spec_less>>;

// A factor present in the numerator and the denominator of one side is the same quantity twice,
// so the exponents just subtract.
template<typename Num, typename Den>
using simplify_quantity_spec_factors = expr_simplify<Num, Den, type_list_of_quantity_spec_less>;

// the kind boundary

template<typename Num, typename Den>
struct carried_kinds_result {
  using num = Num;
  using den = Den;
};

// Every `is_kind` ancestor-or-self of a quantity, as a sorted factor list. The nearest one is the
// kind root, so the walk jumps straight to it, and a quantity whose kind root is the undeclared root
// of its tree has none.
template<typename Q>
[[nodiscard]] consteval auto declared_kinds_of()
{
  using nearest = kind_root<Q>;
  if constexpr (!DeclaredKind<nearest>)
    return type_list<>{};
  else if constexpr (!HasParent<nearest>)
    return type_list<nearest>{};
  else
    return merge_quantity_spec_factors<type_list<nearest>, decltype(detail::declared_kinds_of<parent_of<nearest>>())>{};
}

template<typename Q>
struct carried_kinds;

// The declared kinds a factor list carries, exponents multiplied in.
template<typename List>
[[nodiscard]] consteval auto list_carried_kinds()
{
  if constexpr (type_list_size<List> == 0)
    return carried_kinds_result<type_list<>, type_list<>>{};
  else {
    using element = type_list_front<List>;
    using carried = carried_kinds<expr_type<element>>;
    constexpr ratio exponent = detail::get_exponent(element{});
    using rest = decltype(detail::list_carried_kinds<type_list_pop_front<List>>());
    return carried_kinds_result<
      merge_quantity_spec_factors<rational_power_list<typename carried::num, exponent>, typename rest::num>,
      merge_quantity_spec_factors<rational_power_list<typename carried::den, exponent>, typename rest::den>>{};
  }
}

// Whether any factor of a list carries a declared kind. Most factors carry none, and a side made
// only of such factors carries none either, so its merge walk is skipped.
template<typename... Ts>
[[nodiscard]] consteval bool any_carries_kinds(type_list<Ts...>)
{
  return (... || (type_list_size<typename carried_kinds<expr_type<Ts>>::num> +
                    type_list_size<typename carried_kinds<expr_type<Ts>>::den> >
                  0));
}

// The declared kinds a side `Num / Den` carries, with what appears in both parts canceled.
template<typename Num, typename Den,
         bool Carries = detail::any_carries_kinds(Num{}) || detail::any_carries_kinds(Den{})>
struct side_carried_kinds {
  using normalised = carried_kinds_result<type_list<>, type_list<>>;
  using num = type_list<>;
  using den = type_list<>;
};

template<typename Num, typename Den>
struct side_carried_kinds<Num, Den, true> {
  using num_part = decltype(detail::list_carried_kinds<Num>());
  using den_part = decltype(detail::list_carried_kinds<Den>());
  using normalised =
    simplify_quantity_spec_factors<merge_quantity_spec_factors<typename num_part::num, typename den_part::den>,
                                   merge_quantity_spec_factors<typename num_part::den, typename den_part::num>>;
  using num = normalised::num;
  using den = normalised::den;
};

// What a named quantity carries: its `is_kind` chain (every `is_kind` ancestor-or-self) and
// whatever its equation carries, recursively. A base quantity not declared `is_kind` carries
// nothing. It is a class template, so the compiler computes it once per quantity.
template<typename Q>
struct carried_kinds : carried_kinds_result<decltype(detail::declared_kinds_of<Q>()), type_list<>> {};

template<HasEquation Q>
struct carried_kinds<Q> :
    carried_kinds_result<
      merge_quantity_spec_factors<
        decltype(detail::declared_kinds_of<Q>()),
        typename side_carried_kinds<typename equation_of<Q>::_num_, typename equation_of<Q>::_den_>::normalised::num>,
      typename side_carried_kinds<typename equation_of<Q>::_num_, typename equation_of<Q>::_den_>::normalised::den> {};

// `lost` holds the declared kinds that From carries in excess of To, and `gained` those that To
// carries in excess of From. The positive and the negative parts are compared separately, so an
// angle in a denominator that the target lacks counts as lost. A source `kind_of<K>` is exempt from
// the cap on gained kinds. A kind lost together with one gained is `none`, and that includes the same
// kind lost in one part and gained in the other (`angular_measure` to `inverse(angular_measure)`).
template<typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank kind_boundary_rank()
{
  using enum conversion_rank;
  using from = carried_kinds_result<FromNum, FromDen>;
  using to = carried_kinds_result<ToNum, ToDen>;
  using num_diff = simplify_quantity_spec_factors<typename from::num, typename to::num>;
  using den_diff = simplify_quantity_spec_factors<typename from::den, typename to::den>;
  using lost = type_list_join<typename num_diff::num, typename den_diff::num>;
  using gained = type_list_join<typename num_diff::den, typename den_diff::den>;
  constexpr bool dissolved = type_list_size<lost> > 0;
  constexpr bool created = type_list_size<gained> > 0;
  if constexpr (dissolved && created)
    // one declared kind traded for another, like
    // `isq::angular_measure * isq::length -> isq::solid_angular_measure * isq::length`
    return none;
  else if constexpr (dissolved)
    return explicit_only;
  else if constexpr (!SourceIsKind && created)
    return explicit_only;
  else
    return implicit;
}

// The kind boundary of a conversion from the side `FromNum / FromDen` to the side `ToNum / ToDen`.
// Both sides most often carry the same declared kinds, usually none, and then nothing is lost or
// gained.
template<typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank kind_boundary()
{
  using enum conversion_rank;
  using from = side_carried_kinds<FromNum, FromDen>;
  using to = side_carried_kinds<ToNum, ToDen>;
  if constexpr (std::is_same_v<typename from::num, typename to::num> &&
                std::is_same_v<typename from::den, typename to::den>)
    // both sides carry the same declared kinds (most often none)
    return implicit;
  else
    return detail::kind_boundary_rank<typename from::num, typename from::den, typename to::num, typename to::den,
                                      SourceIsKind>();
}

// The tree rule looks only at positions in a hierarchy, so on its own it would let a conversion drop
// a declared kind that only the source's equation carries: `isq::phase_coefficient`, defined as
// `phase_angle / path_length`, below `isq::propagation_coefficient`, which carries no angle. The kind
// boundary of the two quantities lowers every answer of the tree rule, so a pair and its square rank
// alike wherever a kind is only lost or only gained, and a kind source keeps its exemption for a
// gained kind.
// Between two single quantities the boundary needs no merge: if the target carries no declared kind,
// the source can only lose one, and if the source carries none, it can only gain one. Only when both
// carry kinds that differ is the comparison by parts needed, and a kind lost together with one gained
// is a trade only between quantities that are not one another's ancestors. On a hierarchy edge the
// two are ancestor and descendant, not unrelated kinds, so there the boundary lowers the rank to
// `explicit_only` and never to `none`.
template<typename From, typename To, conversion_rank TreeRank, bool SourceIsKind = false>
[[nodiscard]] consteval conversion_rank with_kind_boundary()
{
  using enum conversion_rank;
  using from = carried_kinds<From>;
  using to = carried_kinds<To>;
  constexpr bool from_carries = type_list_size<typename from::num> + type_list_size<typename from::den> > 0;
  constexpr bool to_carries = type_list_size<typename to::num> + type_list_size<typename to::den> > 0;
  if constexpr (TreeRank == none)
    return none;
  else if constexpr (std::is_same_v<typename from::num, typename to::num> &&
                     std::is_same_v<typename from::den, typename to::den>)
    return TreeRank;
  else if constexpr (!to_carries)
    return detail::weaker(TreeRank, explicit_only);
  else if constexpr (!from_carries)
    return SourceIsKind ? TreeRank : detail::weaker(TreeRank, explicit_only);
  else {
    constexpr conversion_rank boundary = detail::kind_boundary_rank<typename from::num, typename from::den,
                                                                    typename to::num, typename to::den, SourceIsKind>();
    if constexpr (boundary == none && TreeRank >= explicit_only)
      return explicit_only;
    else
      return detail::weaker(TreeRank, boundary);
  }
}

// operation ranks

// Expanding a To factor `M` is the tree rule `A(M) -> M`: `implicit` when `M` defines its equation,
// else a downcast, which a source `kind_of<K>` takes implicitly.
template<typename M, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank target_expansion_rank()
{
  return detail::defines_equation<M>() || SourceIsKind ? conversion_rank::implicit : conversion_rank::explicit_only;
}

// Exact equation first: one side is a single factor `N^e` and the other side is exactly `eq(N)^e`.
// That factor is expanded before any pairing, otherwise a quantity whose
// equation mentions its own tree would pair with it as a sibling.

// Whether `OtherNum / OtherDen` is exactly `EqNum / EqDen` raised to the exponent of the factor `E`.
// A function rather than a constrained partial specialization: gcc-12 crashes on a class-type
// non-type template argument spelled as a call in a base-specifier (`rational_power_list<EqNum,
// detail::get_exponent(E{})>`), and a named constant sidesteps it.
template<typename E, typename EqNum, typename EqDen, typename OtherNum, typename OtherDen>
[[nodiscard]] consteval bool recipe_matches()
{
  if constexpr (type_list_size<EqNum> == type_list_size<OtherNum> &&
                type_list_size<EqDen> == type_list_size<OtherDen>) {
    constexpr ratio exponent = detail::get_exponent(E{});
    return std::is_same_v<OtherNum, rational_power_list<EqNum, exponent>> &&
           std::is_same_v<OtherDen, rational_power_list<EqDen, exponent>>;
  } else
    return false;
}

// A side of any other shape than one factor with an equation has no equation for the other side to
// match.
template<typename SideNum, typename SideDen, typename OtherNum, typename OtherDen>
struct is_recipe : std::false_type {};

template<typename E, typename OtherNum, typename OtherDen>
  requires HasEquation<expr_type<E>>
struct is_recipe<type_list<E>, type_list<>, OtherNum, OtherDen> :
    std::bool_constant<detail::recipe_matches<E, typename equation_of<expr_type<E>>::_num_,
                                              typename equation_of<expr_type<E>>::_den_, OtherNum, OtherDen>()> {};

// a factor in the denominator carries a negative exponent, which inverts its equation
template<typename E, typename OtherNum, typename OtherDen>
  requires HasEquation<expr_type<E>>
struct is_recipe<type_list<>, type_list<E>, OtherNum, OtherDen> :
    std::bool_constant<detail::recipe_matches<E, typename equation_of<expr_type<E>>::_den_,
                                              typename equation_of<expr_type<E>>::_num_, OtherNum, OtherDen>()> {};

template<typename Root, typename... Ts>
[[nodiscard]] consteval bool all_of_tree(type_list<Ts...>)
{
  return (... && std::is_same_v<hierarchy_root<expr_type<Ts>>, Root>);
}

// A product of its own tree: one side is a single factor whose (non-empty) equation consists
// entirely of quantities of its own tree. In the ISQ this happens in the `dimensionless` tree,
// where every pure number is a sibling of every other, and for `intrinsic_carrier_density`, defined
// as `sqrt(electron_density * hole_density)` within the number density tree. Nothing here restricts
// it to these trees: a user's `geometric_mean_length` defined over `sqrt(width * height)` triggers
// it in the `length` tree.

template<typename Q>
struct is_own_tree_product_quantity : std::false_type {};

template<HasEquation Q>
  requires(type_list_size<typename equation_of<Q>::_num_> + type_list_size<typename equation_of<Q>::_den_> > 0)
struct is_own_tree_product_quantity<Q> :
    std::bool_constant<detail::all_of_tree<hierarchy_root<Q>>(typename equation_of<Q>::_num_{}) &&
                       detail::all_of_tree<hierarchy_root<Q>>(typename equation_of<Q>::_den_{})> {};

template<typename SideNum, typename SideDen>
struct is_own_tree_product : std::false_type {};

template<typename E>
struct is_own_tree_product<type_list<E>, type_list<>> : is_own_tree_product_quantity<expr_type<E>> {};

template<typename E>
struct is_own_tree_product<type_list<>, type_list<E>> : is_own_tree_product_quantity<expr_type<E>> {};

// pair

// `has_cast` records whether a tree ranked `cast`, the only case in which `pair_lists` reads `All`.
template<conversion_rank Rank, typename FromRest, typename ToRest, bool Paired, bool HasCast = false>
struct pairing_result {
  static constexpr conversion_rank rank = Rank;
  using from_rest = FromRest;
  using to_rest = ToRest;
  static constexpr bool paired = Paired;
  static constexpr bool has_cast = HasCast;
};

template<typename FromFactors, typename ToFactors, bool SourceIsKind>
[[nodiscard]] consteval auto pair_tree_factors();

// Pairs `F` with every candidate of `ToPost` in turn and keeps the best complete pairing of the
// tree's factors. A partially consumed source factor goes back to the front of the source factors,
// so it pairs again with the next candidate, and the leftovers keep their sorted order.
template<typename F, typename FromRest, typename ToPre, typename ToPost, bool SourceIsKind>
[[nodiscard]] consteval auto best_match()
{
  using enum conversion_rank;
  if constexpr (type_list_size<ToPost> == 0)
    // the sentinel every real pairing beats or ties, so it is never the one returned
    return pairing_result<none, type_list<>, type_list<>, true>{};
  else {
    using T = type_list_front<ToPost>;
    using from_base = expr_type<F>;
    using to_base = expr_type<T>;
    constexpr ratio from_exp = detail::get_exponent(F{});
    constexpr ratio to_exp = detail::get_exponent(T{});
    constexpr ratio common = from_exp < to_exp ? from_exp : to_exp;
    using from_after = type_list_join<power_or_T_list<from_base, from_exp - common>, FromRest>;
    using to_after = type_list_join<ToPre, power_or_T_list<to_base, to_exp - common>, type_list_pop_front<ToPost>>;
    using continued = decltype(detail::pair_tree_factors<from_after, to_after, SourceIsKind>());
    constexpr conversion_rank rank =
      detail::weaker(detail::convertible_named<from_base, to_base, SourceIsKind>(), continued::rank);
    if constexpr (rank == implicit)
      return pairing_result<implicit, typename continued::from_rest, typename continued::to_rest, true>{};
    else {
      using alternative = decltype(detail::best_match<F, FromRest, type_list_push_back<ToPre, T>,
                                                      type_list_pop_front<ToPost>, SourceIsKind>());
      if constexpr (alternative::rank > rank)
        return alternative{};
      else
        return pairing_result<rank, typename continued::from_rest, typename continued::to_rest, true>{};
    }
  }
}

// The sum of the exponents of a factor list.
template<typename... Ts>
[[nodiscard]] consteval ratio total_exponent(type_list<Ts...>)
{
  ratio sum{0};
  ((sum = sum + detail::get_exponent(Ts{})), ...);
  return sum;
}

// Pairs the source and the target factors of one tree. Pairing is maximal (the whole exponent of the
// smaller side is matched), but which factors of the larger side take part is a choice: a source
// factor may stay unpaired, for a later self-cancel, as long as the remaining source factors still
// outweigh the target factors. Ties go to pairing the factor.
template<typename FromFactors, typename ToFactors, bool SourceIsKind>
[[nodiscard]] consteval auto pair_tree_factors()
{
  using enum conversion_rank;
  if constexpr (type_list_size<FromFactors> == 0 || type_list_size<ToFactors> == 0)
    return pairing_result<implicit, FromFactors, ToFactors, false>{};
  else {
    using F = type_list_front<FromFactors>;
    using rest = type_list_pop_front<FromFactors>;
    using paired = decltype(detail::best_match<F, rest, type_list<>, ToFactors, SourceIsKind>());
    if constexpr (paired::rank == implicit)
      return paired{};
    else if constexpr (detail::total_exponent(rest{}) >= detail::total_exponent(ToFactors{})) {
      using skipped = decltype(detail::pair_tree_factors<rest, ToFactors, SourceIsKind>());
      if constexpr (skipped::rank > paired::rank)
        return pairing_result<skipped::rank, type_list_push_front<typename skipped::from_rest, F>,
                              typename skipped::to_rest, true>{};
      else
        return paired{};
    } else
      return paired{};
  }
}

template<typename Factors, typename Rest>
struct tree_factors_split {
  using factors = Factors;
  using rest = Rest;
};

// The leading factors of `List` whose tree is `Root`, and what follows them.
template<typename Root, typename List>
[[nodiscard]] consteval auto split_tree_factors()
{
  if constexpr (type_list_size<List> == 0)
    return tree_factors_split<type_list<>, type_list<>>{};
  else {
    using head = type_list_front<List>;
    if constexpr (std::is_same_v<hierarchy_root<expr_type<head>>, Root>) {
      using tail = decltype(detail::split_tree_factors<Root, type_list_pop_front<List>>());
      return tree_factors_split<type_list_push_front<typename tail::factors, head>, typename tail::rest>{};
    } else
      return tree_factors_split<type_list<>, List>{};
  }
}

template<typename Root, typename Q>
[[nodiscard]] consteval bool equation_mentions()
{
  if constexpr (!HasEquation<Q>)
    return false;
  else
    return []<typename... Ts>(type_list<Ts...>) {
      return (... || std::is_same_v<hierarchy_root<expr_type<Ts>>, Root>);
    }(type_list_join<typename equation_of<Q>::_num_, typename equation_of<Q>::_den_>{});
}

// Whether a factor outside the tree `Root` has an equation that mentions that tree. The factors of
// the tree itself are the ones being paired, so they are not "remaining" and do not count.
template<typename Root, typename... Ts>
[[nodiscard]] consteval bool any_remaining_equation_mentions(type_list<Ts...>)
{
  return (... ||
          (!std::is_same_v<hierarchy_root<expr_type<Ts>>, Root> && detail::equation_mentions<Root, expr_type<Ts>>()));
}

// Pairing: one merge walk over two lists of the same sign pairs every tree that appears on both
// sides. Both lists are sorted by the root name first, so the factors of one tree are adjacent and
// aligned across the lists. `All` holds every factor of both sides: a tree that would rank `cast` is
// deferred while a remaining factor (one of another tree) has an equation that mentions this tree,
// so that the decision between siblings is made with every factor of the tree present.
template<typename From, typename To, typename All, bool SourceIsKind>
[[nodiscard]] consteval auto pair_lists()
{
  using enum conversion_rank;
  if constexpr (type_list_size<From> == 0 || type_list_size<To> == 0)
    return pairing_result<implicit, From, To, false>{};
  else {
    using from_root = hierarchy_root<expr_type<type_list_front<From>>>;
    using to_root = hierarchy_root<expr_type<type_list_front<To>>>;
    if constexpr (std::is_same_v<from_root, to_root>) {
      using from_split = decltype(detail::split_tree_factors<from_root, From>());
      using to_split = decltype(detail::split_tree_factors<to_root, To>());
      using matched =
        decltype(detail::pair_tree_factors<typename from_split::factors, typename to_split::factors, SourceIsKind>());
      using tail =
        decltype(detail::pair_lists<typename from_split::rest, typename to_split::rest, All, SourceIsKind>());
      using paired = pairing_result<detail::weaker(matched::rank, tail::rank),
                                    type_list_join<typename matched::from_rest, typename tail::from_rest>,
                                    type_list_join<typename matched::to_rest, typename tail::to_rest>, true,
                                    matched::rank == cast || tail::has_cast>;
      // nested so that the scan of `All` is instantiated only for a tree that actually ranks `cast`
      if constexpr (matched::rank == cast) {
        if constexpr (detail::any_remaining_equation_mentions<from_root>(All{}))
          return pairing_result<tail::rank, type_list_join<typename from_split::factors, typename tail::from_rest>,
                                type_list_join<typename to_split::factors, typename tail::to_rest>, tail::paired,
                                true>{};
        else
          return paired{};
      } else
        return paired{};
    } else if constexpr (quantity_spec_sort_key<from_root>::name < quantity_spec_sort_key<to_root>::name) {
      using tail = decltype(detail::pair_lists<type_list_pop_front<From>, To, All, SourceIsKind>());
      return pairing_result<tail::rank, type_list_push_front<typename tail::from_rest, type_list_front<From>>,
                            typename tail::to_rest, tail::paired, tail::has_cast>{};
    } else {
      using tail = decltype(detail::pair_lists<From, type_list_pop_front<To>, All, SourceIsKind>());
      return pairing_result<tail::rank, typename tail::from_rest,
                            type_list_push_front<typename tail::to_rest, type_list_front<To>>, tail::paired,
                            tail::has_cast>{};
    }
  }
}

template<bool HasCast, typename NumAlone, typename DenAlone, typename FromNum, typename FromDen, typename ToNum,
         typename ToDen, bool SourceIsKind>
struct paired_sides {
  using num = NumAlone;
  using den = DenAlone;
};

template<typename NumAlone, typename DenAlone, typename FromNum, typename FromDen, typename ToNum, typename ToDen,
         bool SourceIsKind>
struct paired_sides<true, NumAlone, DenAlone, FromNum, FromDen, ToNum, ToDen, SourceIsKind> {
  using all = type_list_join<FromNum, FromDen, ToNum, ToDen>;
  using num = decltype(detail::pair_lists<FromNum, ToNum, all, SourceIsKind>());
  using den = decltype(detail::pair_lists<FromDen, ToDen, all, SourceIsKind>());
};

// Different roots stay apart: two named factors of one sign and one dimension from different trees,
// left over after pairing, block the conversion with `none` (`frequency * width -> activity * length`).
// The exception is a pair whose equations connect them, where the equation of either factor has a
// factor in the other's tree (`radiant_intensity = radiant_flux / solid_angular_measure` against
// `radiant_flux`): such factors don't block and the equations decide. The check itself looks only
// at the two quantities, so the order of the operations doesn't change its result.
template<typename Lhs, typename Rhs>
[[nodiscard]] consteval bool is_blocking_pair()
{
  if constexpr (std::is_same_v<hierarchy_root<Lhs>, hierarchy_root<Rhs>>)
    return false;
  else if constexpr (!std::is_same_v<MP_UNITS_NONCONST_TYPE(Lhs::_dimension_),
                                     MP_UNITS_NONCONST_TYPE(Rhs::_dimension_)>)
    return false;
  else
    return !detail::equation_mentions<hierarchy_root<Rhs>, Lhs>() &&
           !detail::equation_mentions<hierarchy_root<Lhs>, Rhs>();
}

template<typename F, typename... Ts>
[[nodiscard]] consteval bool blocks_any(type_list<Ts...>)
{
  return (... || detail::is_blocking_pair<expr_type<F>, expr_type<Ts>>());
}

template<typename To, typename... Fs>
[[nodiscard]] consteval bool any_blocking_pair(type_list<Fs...>)
{
  return (... || detail::blocks_any<Fs>(To{}));
}

// self-cancel

template<bool Found, typename Num, typename Den>
struct cancel_result {
  static constexpr bool found = Found;
  using num = Num;
  using den = Den;
};

// The first denominator factor of the same dimension as `N`.
template<typename N, typename DenPre, typename DenPost>
[[nodiscard]] consteval auto cancel_against()
{
  if constexpr (type_list_size<DenPost> == 0)
    return cancel_result<false, type_list<N>, DenPre>{};
  else {
    using D = type_list_front<DenPost>;
    if constexpr (std::is_same_v<MP_UNITS_NONCONST_TYPE(expr_type<N>::_dimension_),
                                 MP_UNITS_NONCONST_TYPE(expr_type<D>::_dimension_)>) {
      constexpr ratio num_exp = detail::get_exponent(N{});
      constexpr ratio den_exp = detail::get_exponent(D{});
      constexpr ratio common = num_exp < den_exp ? num_exp : den_exp;
      return cancel_result<
        true, power_or_T_list<expr_type<N>, num_exp - common>,
        type_list_join<DenPre, power_or_T_list<expr_type<D>, den_exp - common>, type_list_pop_front<DenPost>>>{};
    } else
      return detail::cancel_against<N, type_list_push_back<DenPre, D>, type_list_pop_front<DenPost>>();
  }
}

// One self-cancel on one side, the first available in list order (every self-cancel on a side has
// the same rank, so which one goes first doesn't change the result).
template<typename NumPre, typename Num, typename Den>
[[nodiscard]] consteval auto self_cancel()
{
  if constexpr (type_list_size<Num> == 0 || type_list_size<Den> == 0)
    return cancel_result<false, type_list_join<NumPre, Num>, Den>{};
  else {
    using N = type_list_front<Num>;
    using attempt = decltype(detail::cancel_against<N, type_list<>, Den>());
    if constexpr (attempt::found)
      return cancel_result<true, type_list_join<NumPre, typename attempt::num, type_list_pop_front<Num>>,
                           typename attempt::den>{};
    else
      return detail::self_cancel<type_list_push_back<NumPre, N>, type_list_pop_front<Num>, Den>();
  }
}

// expand

// Replaces the factor at `I` of `This` with its equation: the equation's numerator joins `This`
// and its denominator joins `Other` (the opposite list of the same side), both raised to the
// factor's exponent, and then the two lists of the side are normalised against each other.
// The factor and the rest come from one `type_list_extract`.
template<typename This, typename Other, std::size_t I>
struct substituted_side {
  using extracted = type_list_extract<This, I>;
  using element = extracted::element;
  using factor = expr_type<element>;
  static constexpr ratio exponent = detail::get_exponent(element{});
  using eq = equation_of<factor>;
  using merged_this =
    merge_quantity_spec_factors<typename extracted::rest, rational_power_list<typename eq::_num_, exponent>>;
  using merged_other = merge_quantity_spec_factors<Other, rational_power_list<typename eq::_den_, exponent>>;
  using normalised = simplify_quantity_spec_factors<merged_this, merged_other>;
  using this_list = normalised::num;
  using other_list = normalised::den;
};

// A side `Num / Den` after the factor at `I` of its numerator (`InNum`) or of its denominator is
// replaced by its equation. Expanding a From factor and expanding a To factor are this one
// substitution. They differ only in rank.
template<typename Num, typename Den, std::size_t I, bool InNum>
struct substituted : substituted_side<Num, Den, I> {
  using num = substituted_side<Num, Den, I>::this_list;
  using den = substituted_side<Num, Den, I>::other_list;
};

template<typename Num, typename Den, std::size_t I>
struct substituted<Num, Den, I, false> : substituted_side<Den, Num, I> {
  using num = substituted_side<Den, Num, I>::other_list;
  using den = substituted_side<Den, Num, I>::this_list;
};

template<conversion_rank Rank, typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank reduce();

// Expands the From factor at `I` of the numerator (`InNum`) or the denominator, always `implicit`.
template<conversion_rank Rank, typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool InNum,
         std::size_t I, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank expand_source()
{
  using side = substituted<FromNum, FromDen, I, InNum>;
  return detail::reduce<Rank, typename side::num, typename side::den, ToNum, ToDen, SourceIsKind>();
}

// Expands the To factor at `I` of the numerator (`InNum`) or the denominator, at the target expansion
// rank.
template<conversion_rank Rank, typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool InNum,
         std::size_t I, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank expand_target()
{
  using side = substituted<ToNum, ToDen, I, InNum>;
  return detail::reduce<detail::weaker(Rank, detail::target_expansion_rank<typename side::factor, SourceIsKind>()),
                        FromNum, FromDen, typename side::num, typename side::den, SourceIsKind>();
}

// Every factor of depth `Depth` in one list, from position `I` on, is expanded in turn and the best
// rank is kept. Which of equally deep factors comes first in a list follows their names, so trying
// only the first would let a name decide the verdict. The search stops at the first `implicit`.
template<int Depth, bool Source, bool InNum, std::size_t I, conversion_rank Rank, typename FromNum, typename FromDen,
         typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank best_expansion()
{
  using list =
    std::conditional_t<Source, std::conditional_t<InNum, FromNum, FromDen>, std::conditional_t<InNum, ToNum, ToDen>>;
  if constexpr (I == type_list_size<list>)
    return conversion_rank::none;
  else {
    constexpr conversion_rank here = [] {
      if constexpr (quantity_spec_depth<expr_type<type_list_element<list, I>>>::value != Depth)
        return conversion_rank::none;
      else if constexpr (Source)
        return detail::expand_source<Rank, FromNum, FromDen, ToNum, ToDen, InNum, I, SourceIsKind>();
      else
        return detail::expand_target<Rank, FromNum, FromDen, ToNum, ToDen, InNum, I, SourceIsKind>();
    }();
    if constexpr (here == conversion_rank::implicit)
      return here;
    else {
      constexpr conversion_rank rest =
        detail::best_expansion<Depth, Source, InNum, I + 1, Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
      return here > rest ? here : rest;
    }
  }
}

// Reduce the deepest factor: nothing pairs and nothing self-cancels, so a factor of greatest depth is
// expanded. The list it comes from is the first one holding that depth, in the order From numerator,
// From denominator, To numerator, To denominator; within that list every factor of that depth is tried
// (see `best_expansion`). The deepest factor is reduced first, so the two sides meet at the most
// specific common equation. Flattening both sides to base quantities would rank
// `inverse(repetency) -> height` `explicit_only` instead of `cast`.
template<conversion_rank Rank, typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank reduce_deepest()
{
  constexpr int from_num_depth = detail::max_factor_depth(FromNum{});
  constexpr int from_den_depth = detail::max_factor_depth(FromDen{});
  constexpr int to_num_depth = detail::max_factor_depth(ToNum{});
  constexpr int to_den_depth = detail::max_factor_depth(ToDen{});
  constexpr int deepest = detail::max({from_num_depth, from_den_depth, to_num_depth, to_den_depth});
  if constexpr (deepest == 0)
    // nothing left has an equation and nothing pairs or self-cancels
    return conversion_rank::none;
  else if constexpr (from_num_depth == deepest)
    return detail::best_expansion<deepest, true, true, 0, Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
  else if constexpr (from_den_depth == deepest)
    return detail::best_expansion<deepest, true, false, 0, Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
  else if constexpr (to_num_depth == deepest)
    return detail::best_expansion<deepest, false, true, 0, Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
  else
    return detail::best_expansion<deepest, false, false, 0, Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
}

// The two single-factor rules that come before pairing, in their order: exact equation first, then
// a product of its own tree, the From side before the To side in each. Both are the same
// substitution of the one factor. After an exact equation the two sides are the same product.
enum class leading_operation : std::int8_t { none, expand_source, expand_target };

template<typename FromNum, typename FromDen, typename ToNum, typename ToDen>
[[nodiscard]] consteval leading_operation find_leading_operation()
{
  using enum leading_operation;
  // both rules need a side of a single factor, so the traits are not even instantiated otherwise
  constexpr bool from_single = type_list_size<FromNum> + type_list_size<FromDen> == 1;
  constexpr bool to_single = type_list_size<ToNum> + type_list_size<ToDen> == 1;
  if constexpr (from_single) {
    if constexpr (is_recipe<FromNum, FromDen, ToNum, ToDen>::value) return expand_source;
  }
  if constexpr (to_single) {
    if constexpr (is_recipe<ToNum, ToDen, FromNum, FromDen>::value) return expand_target;
  }
  if constexpr (from_single) {
    if constexpr (is_own_tree_product<FromNum, FromDen>::value) return expand_source;
  }
  if constexpr (to_single) {
    if constexpr (is_own_tree_product<ToNum, ToDen>::value) return expand_target;
  }
  return none;
}

// One step of the procedure: done, a single-factor rule, pair (a cross-tree pair ends it with `none`),
// self-cancel against an empty counterpart, reduce the deepest. Every step removes a factor or
// replaces one by factors of strictly smaller depth, so it terminates.
template<conversion_rank Rank, typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind>
[[nodiscard]] consteval conversion_rank reduce()
{
  using enum conversion_rank;
  if constexpr (std::is_same_v<FromNum, ToNum> && std::is_same_v<FromDen, ToDen>)
    // both sides are empty, or the same product (right after an exact equation)
    return Rank;
  else if constexpr (constexpr leading_operation operation =
                       detail::find_leading_operation<FromNum, FromDen, ToNum, ToDen>();
                     operation == leading_operation::expand_source)
    return detail::expand_source<Rank, FromNum, FromDen, ToNum, ToDen, type_list_size<FromNum> == 1, 0, SourceIsKind>();
  else if constexpr (operation == leading_operation::expand_target)
    return detail::expand_target<Rank, FromNum, FromDen, ToNum, ToDen, type_list_size<ToNum> == 1, 0, SourceIsKind>();
  else {
    constexpr std::size_t from_size = type_list_size<FromNum> + type_list_size<FromDen>;
    constexpr std::size_t to_size = type_list_size<ToNum> + type_list_size<ToDen>;
    // Paired first without `All`, which keeps the pairing of one list pair the same instantiation in
    // every step that meets it. Only when a tree ranks `cast` does the deferral need every factor.
    using num_alone = decltype(detail::pair_lists<FromNum, ToNum, type_list<>, SourceIsKind>());
    using den_alone = decltype(detail::pair_lists<FromDen, ToDen, type_list<>, SourceIsKind>());
    using sides = paired_sides<num_alone::has_cast || den_alone::has_cast, num_alone, den_alone, FromNum, FromDen,
                               ToNum, ToDen, SourceIsKind>;
    using num = sides::num;
    using den = sides::den;
    // same-tree factors pair first (the best pairing), so only what they leave can pair across trees
    if constexpr (detail::any_blocking_pair<typename num::to_rest>(typename num::from_rest{}) ||
                  detail::any_blocking_pair<typename den::to_rest>(typename den::from_rest{}))
      return none;
    else if constexpr (num::paired || den::paired) {
      constexpr conversion_rank rank = detail::weaker(Rank, detail::weaker(num::rank, den::rank));
      if constexpr (rank == none)
        return none;
      else
        return detail::reduce<rank, typename num::from_rest, typename den::from_rest, typename num::to_rest,
                              typename den::to_rest, SourceIsKind>();
    } else if constexpr (to_size == 0) {
      // a ratio of two quantities of one dimension is a plain number
      using cancel = decltype(detail::self_cancel<type_list<>, FromNum, FromDen>());
      if constexpr (cancel::found)
        return detail::reduce<Rank, typename cancel::num, typename cancel::den, ToNum, ToDen, SourceIsKind>();
      else
        return detail::reduce_deepest<Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
    } else if constexpr (from_size == 0) {
      // the user asserts that a number is such a ratio
      using cancel = decltype(detail::self_cancel<type_list<>, ToNum, ToDen>());
      if constexpr (cancel::found)
        return detail::reduce<SourceIsKind ? Rank : detail::weaker(Rank, explicit_only), FromNum, FromDen,
                              typename cancel::num, typename cancel::den, SourceIsKind>();
      else
        return detail::reduce_deepest<Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
    } else
      return detail::reduce_deepest<Rank, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
  }
}

// Neither side is a kind here. `convertible_kinds` strips `kind_of<>` and passes `SourceIsKind`,
// which exempts a source kind from the kind boundary's cap on gained kinds.
// Whether a side is a single named quantity with exponent one.
template<typename Num, typename Den>
inline constexpr bool is_single_named = false;

template<typename Q>
  requires(!is_specialization_of_power<Q>)
inline constexpr bool is_single_named<type_list<Q>, type_list<>> = true;

template<typename FromNum, typename FromDen, typename ToNum, typename ToDen, bool SourceIsKind, bool TargetIsKind>
[[nodiscard]] consteval conversion_rank convertible_factors()
{
  if constexpr (is_single_named<FromNum, FromDen> && is_single_named<ToNum, ToDen>) {
    // what is left are two named quantities: they relate as the bare pair does
    using from = type_list_front<FromNum>;
    using to = type_list_front<ToNum>;
    if constexpr (SourceIsKind)
      return detail::with_kind_boundary<from, to, detail::convertible_from_kind<from, to>(), true>();
    else if constexpr (TargetIsKind)
      return detail::with_kind_boundary<from, to, detail::convertible_to_kind<from, to>()>();
    else
      return detail::with_kind_boundary<from, to, detail::convertible_named<from, to>()>();
  } else if constexpr (constexpr conversion_rank rank =
                         detail::reduce<conversion_rank::implicit, FromNum, FromDen, ToNum, ToDen, SourceIsKind>();
                       rank == conversion_rank::none)
    // the kind boundary can only lower the rank
    return conversion_rank::none;
  else
    return detail::weaker(rank, detail::kind_boundary<FromNum, FromDen, ToNum, ToDen, SourceIsKind>());
}

// Whether two factor lists have a base in common, the only case in which simplifying them against
// each other changes them.
template<typename E, typename List>
inline constexpr bool has_base = false;

template<typename E, typename... Ts>
inline constexpr bool has_base<E, type_list<Ts...>> = (... || std::is_same_v<expr_type<Ts>, expr_type<E>>);

template<typename Lhs, typename Rhs>
inline constexpr bool share_a_base = false;

template<typename... Ls, typename Rhs>
inline constexpr bool share_a_base<type_list<Ls...>, Rhs> = (... || has_base<Ls, Rhs>);

// A factor common to both sides doesn't change how they relate: a factor present on both sides,
// in the same part, is canceled before anything else, so `A * X -> B * X` is decided exactly as
// `A -> B`, the kind boundary included, as long as no factor of `X` cancels against `A` or `B`.
template<typename From, typename To, bool SourceIsKind = false, bool TargetIsKind = false>
[[nodiscard]] consteval conversion_rank convertible_by_equations()
{
  using from = quantity_spec_factors<From>;
  using to = quantity_spec_factors<To>;
  if constexpr (!share_a_base<typename from::num, typename to::num> &&
                !share_a_base<typename from::den, typename to::den>)
    // nothing to cancel, which is the common case
    return detail::convertible_factors<typename from::num, typename from::den, typename to::num, typename to::den,
                                       SourceIsKind, TargetIsKind>();
  else {
    using num = simplify_quantity_spec_factors<typename from::num, typename to::num>;
    using den = simplify_quantity_spec_factors<typename from::den, typename to::den>;
    return detail::convertible_factors<typename num::num, typename den::num, typename num::den, typename den::den,
                                       SourceIsKind, TargetIsKind>();
  }
}

}  // namespace mp_units::detail
