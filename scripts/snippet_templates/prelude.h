// What every template provides. In header mode, that is the core library, the ISQ and the
// SI, and the standard headers a sample takes for granted, since the documentation leaves
// `#include` lines out. The other systems and the utility types come from opt-in templates,
// because parsing them costs more than most samples do. codata.h alone takes over 1 GB of
// memory.
#ifdef MP_UNITS_MODULES
import mp_units;
#else
#include <mp-units/core.h>
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <concepts>
#include <cstdint>
#include <format>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numbers>
#include <numeric>
#include <optional>
#include <print>
#include <random>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#endif
