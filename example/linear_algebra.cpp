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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! Before you commit any changes to this file please make sure to check if it !!!
// !!! renders correctly in the documentation "Examples" section.                 !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// This single source file demonstrates that the very same mp-units scenario compiles and runs
// against several linear algebra libraries used as the quantity *representation type*. The build
// system compiles it once per backend it found (one executable each), in both header and module
// modes. The mp-units code in `drone_flight()` is identical for every backend - only the thin shim
// that names the vector type and constructs it differs.

// Backend selection. The third-party libraries are included textually (they are not modularized and
// we need their vector type directly); a backend is selected from whatever is available. mp-units'
// own `cartesian_vector` is the built-in baseline: it ships with the library, needs no third-party
// dependency and no integration plugin, and is used when forced (`MP_UNITS_LA_USE_CARTESIAN`) or
// when no linear algebra library is found.
#if defined(MP_UNITS_LA_USE_CARTESIAN)
#define MP_UNITS_LA_CARTESIAN
#elif __has_include(<Eigen/Core>)
#include <Eigen/Core>
#define MP_UNITS_LA_EIGEN
#elif __has_include(<glm/vec3.hpp>)
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#define MP_UNITS_LA_GLM
#elif __has_include(<blaze/math/StaticVector.h>)
#include <blaze/math/StaticVector.h>
#define MP_UNITS_LA_BLAZE
#else
#define MP_UNITS_LA_CARTESIAN
#endif

#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <exception>
#include <iostream>
#include <string_view>
#endif

// mp-units and the matching opt-in integration adapter. For a third-party backend the
// `mp-units/integrations/<lib>.h` header (or its `mp_units.integrations.<lib>` module) wires the
// library's vector type into mp-units' representation customization points; user code then uses the
// library's native types directly. The built-in `cartesian_vector` needs no adapter - it ships with
// the library and is a representation type out of the box.
#ifdef MP_UNITS_MODULES
import mp_units;
#if defined(MP_UNITS_LA_EIGEN)
import mp_units.integrations.eigen;
#elif defined(MP_UNITS_LA_GLM)
import mp_units.integrations.glm;
#elif defined(MP_UNITS_LA_BLAZE)
import mp_units.integrations.blaze;
#endif
#else
#include <mp-units/math.h>
#include <mp-units/systems/isq/mechanics.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#if defined(MP_UNITS_LA_EIGEN)
#include <mp-units/integrations/eigen.h>
#elif defined(MP_UNITS_LA_GLM)
#include <mp-units/integrations/glm.h>
#elif defined(MP_UNITS_LA_BLAZE)
#include <mp-units/integrations/blaze.h>
#elif defined(MP_UNITS_LA_CARTESIAN)
#include <mp-units/cartesian_vector.h>
#endif
#endif

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

#if defined(MP_UNITS_LA_EIGEN)
inline constexpr const char* backend_name = "Eigen";
using vec3 = Eigen::Vector3d;
#elif defined(MP_UNITS_LA_GLM)
inline constexpr const char* backend_name = "GLM";
using vec3 = glm::dvec3;
#elif defined(MP_UNITS_LA_BLAZE)
inline constexpr const char* backend_name = "Blaze";
using vec3 = blaze::StaticVector<double, 3>;
#elif defined(MP_UNITS_LA_CARTESIAN)
inline constexpr const char* backend_name = "cartesian_vector (built-in)";
using vec3 = cartesian_vector<double>;
#endif

[[nodiscard]] vec3 make_vec3(double x, double y, double z) { return {x, y, z}; }

template<Quantity Q>
void print(std::string_view name, const Q& q)
{
  const auto& v = q.numerical_value_in(q.unit);
  std::cout << name << " = [" << v[0] << ", " << v[1] << ", " << v[2] << "] " << unit_symbol(Q::unit) << "\n";
}

void drone_flight()
{
  std::cout << "*** Linear algebra backend: " << backend_name << " ***\n\n";

  // a 3D velocity, stored as a vector quantity (vector representation x reference)
  const quantity velocity = isq::velocity(make_vec3(30, 40, 0) * km / h);
  print("velocity        ", velocity);

  // unit conversion of a vector quantity (exercises magnitude-ratio scaling of the rep)
  print("velocity [m/s]  ", velocity.in(m / s));

  // vector quantity x scalar quantity -> vector quantity (the rep arithmetic is materialized)
  const quantity time = 2. * h;
  const quantity displacement = velocity * time;
  print("displacement    ", displacement.in(km));

  // vector + vector, with an automatic unit conversion of one of the operands
  const quantity wind_drift = make_vec3(500, 0, 0) * isq::displacement[m];
  print("with wind drift ", (displacement + wind_drift).in(km));

  // scalar magnitude of a vector quantity
  std::cout << "speed            = " << magnitude(velocity).in(km / h) << "\n";
}

}  // namespace

int main()
{
  try {
    drone_flight();
  } catch (const std::exception& ex) {
    std::cerr << "Unhandled std exception caught: " << ex.what() << '\n';
  } catch (...) {
    std::cerr << "Unhandled unknown exception caught\n";
  }
}
