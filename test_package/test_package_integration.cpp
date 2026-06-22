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

// Verifies that an installed linear algebra integration can be consumed - both as a header
// (`#include <mp-units/integrations/<lib>.h>`) and, in a modules build, as a module
// (`import mp_units.integrations.<lib>;`). The backend is selected from whatever library the
// linked target makes available.

#include <mp-units/compat_macros.h>

#if __has_include(<Eigen/Core>)
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
#error "No supported linear algebra library found on the include path"
#endif

#if MP_UNITS_HOSTED
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <iostream>
#endif
#endif

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
#include <mp-units/systems/isq.h>
#include <mp-units/systems/si.h>
#if defined(MP_UNITS_LA_EIGEN)
#include <mp-units/integrations/eigen.h>
#elif defined(MP_UNITS_LA_GLM)
#include <mp-units/integrations/glm.h>
#elif defined(MP_UNITS_LA_BLAZE)
#include <mp-units/integrations/blaze.h>
#endif
#endif

using namespace mp_units;

#if defined(MP_UNITS_LA_EIGEN)
using vec3 = Eigen::Vector3d;
#elif defined(MP_UNITS_LA_GLM)
using vec3 = glm::dvec3;
#elif defined(MP_UNITS_LA_BLAZE)
using vec3 = blaze::StaticVector<double, 3>;
#endif

int main()
{
  using namespace mp_units::si::unit_symbols;
  const quantity velocity = vec3{3., 4., 0.} * isq::velocity[km / h];
  const quantity speed = magnitude(velocity.numerical_value_in(km / h)) * (km / h);
#if MP_UNITS_HOSTED
  std::cout << "speed = " << speed.numerical_value_in(km / h) << " km/h\n";
#else
  [[maybe_unused]] auto value = speed;
#endif
}
