#pragma once

#include <units/physical/si/prefixes.h>
#include <units/quantity.h>
// get at the units text of the quantity, without its numeric value
inline auto constexpr units_str(const units::Quantity auto& q)
{
  typedef std::remove_cvref_t<decltype(q)> qtype;
  return units::detail::unit_text<typename qtype::dimension, typename qtype::unit>();
}
