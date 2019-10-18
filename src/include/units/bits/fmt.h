#pragma once

#include "unicode/uchriter.h"
#include <fmt/format.h>
#include <unicode/measfmt.h>
#include <unicode/measunit.h>
#include <limits.h>
#include <cuchar>
#include <locale>

namespace units::details {

  template<typename FormatContext>
  auto format_localized_unit(const icu::Measure& m, FormatContext& ctx)
  {
    auto locale = ctx.locale().template get<std::locale>();

    UErrorCode uc = {};
    icu::MeasureFormat uFmt(locale.name() == "*" ? nullptr : locale.name().c_str(), UMEASFMT_WIDTH_SHORT, uc);
    icu::UnicodeString s;
    icu::FieldPosition p(icu::FieldPosition::DONT_CARE);
    uFmt.formatMeasures(&m, 1, s, p, uc);
    char out[MB_LEN_MAX]{};
    icu::UCharCharacterIterator iter(s.getTerminatedBuffer(), s.length());
    while(iter.hasNext()) {
      std::mbstate_t state{};
      auto c = iter.next32PostInc();
      std::size_t rc = std::c32rtomb(out, c, &state);
      if(rc != std::size_t(-1)) std::copy_n(std::begin(out), rc, ctx.out());
    }
    return ctx.out();
  }

  template<typename T>
  icu::MeasureUnit* create_icu_unit(UErrorCode&)
  {
    return nullptr;
  }

}  // namespace units::details