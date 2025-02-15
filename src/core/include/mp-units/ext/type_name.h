// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138

// To be replaced with:
// P2830: Standardized Constexpr Type Ordering

#pragma once

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <string_view>
#include <type_traits>
#endif
#endif

namespace mp_units::detail {

template<typename T>
[[nodiscard]] consteval std::string_view type_name()
{
  std::string_view name, prefix, suffix;
#ifdef __clang__
  name = __PRETTY_FUNCTION__;
  prefix = "auto type_name() [T = ";
  suffix = "]";
#elif defined(__GNUC__)
  name = __PRETTY_FUNCTION__;
  prefix = "constexpr auto type_name() [with T = ";
  suffix = "]";
#elif defined(_MSC_VER)
  name = __FUNCSIG__;
  prefix = "auto __cdecl type_name<";
  suffix = ">(void)";
#endif
  name.remove_prefix(prefix.size());
  name.remove_suffix(suffix.size());
  return name;
}

template<typename Lhs, typename Rhs>
struct type_name_less : std::bool_constant<type_name<Lhs>() < type_name<Rhs>()> {};

// This is typically used to deterministically chose one of the alternatives
// to guarantee the commutation of the operation (e.g., `a + b` should return
// the same type as `b + a`).
template<typename T1, typename T2>
[[nodiscard]] consteval auto better_type_name(T1 v1, T2 v2)
{
  // NOLINTBEGIN(bugprone-branch-clone)
  if constexpr (type_name<T1>().size() < type_name<T2>().size())
    return v1;
  else if constexpr (type_name<T2>().size() < type_name<T1>().size())
    return v2;
  else if constexpr (type_name<T1>() < type_name<T2>())
    return v1;
  else
    return v2;
  // NOLINTEND(bugprone-branch-clone)
}

}  // namespace mp_units::detail
