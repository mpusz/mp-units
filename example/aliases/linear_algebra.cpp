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

#include <units/format.h>
#include <units/isq/si/energy.h>  // IWYU pragma: keep
#include <units/isq/si/force.h>
#include <units/isq/si/length.h>
#include <units/isq/si/speed.h>  // IWYU pragma: keep
#include <units/quantity_io.h>
#include <iostream>
#include <matrix>

template<typename Rep = double>
using vector = STD_LA::fixed_size_column_vector<Rep, 3>;

template<typename Rep = double>
using matrix = STD_LA::fixed_size_matrix<Rep, 3, 3>;

namespace STD_LA {

template<typename Rep>
std::ostream& operator<<(std::ostream& os, const ::vector<Rep>& v)
{
  os << "|";
  for (auto i = 0U; i < v.size(); ++i) {
    os << UNITS_STD_FMT::format(" {:>9}", v(i));
  }
  os << " |";
  return os;
}

template<typename Rep>
std::ostream& operator<<(std::ostream& os, const ::matrix<Rep>& v)
{
  for (auto i = 0U; i < v.rows(); ++i) {
    os << "|";
    for (auto j = 0U; j < v.columns(); ++j) {
      os << UNITS_STD_FMT::format(" {:>9}", v(i, j));
    }
    os << (i != v.rows() - 1U ? " |\n" : " |");
  }
  return os;
}

}  // namespace STD_LA

namespace {

using namespace units::aliases::isq::si;

void vector_of_quantity_add()
{
  std::cout << "\nvector_of_quantity_add:\n";

  vector<length::m<>> v = {m<>(4), m<>(8), m<>(12)};
  vector<length::m<>> u = {m<>(3), m<>(2), m<>(1)};
  vector<length::km<>> t = {km<>(3), km<>(2), km<>(1)};

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";
  std::cout << "t = " << t << "\n";

  std::cout << "v + u = " << v + u << "\n";
  std::cout << "v + t = " << v + t << "\n";
  std::cout << "t[m]  = " << vector<length::m<>>(t) << "\n";
}

void vector_of_quantity_divide_by_scalar()
{
  std::cout << "\nvector_of_quantity_divide_by_scalar:\n";

  vector<length::m<>> v = {m<>(4), m<>(8), m<>(12)};

  std::cout << "v = " << v << "\n";

  std::cout << "v / s<>(2) = " << v / s<>(2) << "\n";
  std::cout << "v / 2 = " << v / 2 << "\n";
}

void vector_of_quantity_tests()
{
  vector_of_quantity_add();
  vector_of_quantity_divide_by_scalar();
}

void matrix_of_quantity_add()
{
  std::cout << "\nmatrix_of_quantity_add:\n";

  matrix<length::m<>> v = {{m<>(1), m<>(2), m<>(3)}, {m<>(4), m<>(5), m<>(6)}, {m<>(7), m<>(8), m<>(9)}};
  matrix<length::m<>> u = {{m<>(3), m<>(2), m<>(1)}, {m<>(3), m<>(2), m<>(1)}, {m<>(3), m<>(2), m<>(1)}};
  matrix<length::mm<>> t = {{mm<>(3), mm<>(2), mm<>(1)}, {mm<>(3), mm<>(2), mm<>(1)}, {mm<>(3), mm<>(2), mm<>(1)}};

  std::cout << "v =\n" << v << "\n";
  std::cout << "u =\n" << u << "\n";
  std::cout << "t =\n" << t << "\n";

  std::cout << "v + u =\n" << v + u << "\n";
  std::cout << "v + t =\n" << v + t << "\n";

  std::cout << "v[mm] =\n" << matrix<length::mm<>>(v) << "\n";
}

void matrix_of_quantity_divide_by_scalar()
{
  std::cout << "\nmatrix_of_quantity_divide_by_scalar:\n";

  matrix<length::m<>> v = {{m<>(2), m<>(4), m<>(6)}, {m<>(4), m<>(6), m<>(8)}, {m<>(8), m<>(4), m<>(2)}};

  std::cout << "v =\n" << v << "\n";

  std::cout << "v / s<>(2) =\n" << v / s<>(2) << "\n";
  std::cout << "v / 2 =\n" << v / 2 << "\n";
}

void matrix_of_quantity_tests()
{
  matrix_of_quantity_add();
  matrix_of_quantity_divide_by_scalar();
}

using namespace units::isq;

template<units::Unit U = si::metre, units::Representation Rep = double>
using length_v = si::length<U, vector<Rep>>;

template<units::Unit U = si::newton, units::Representation Rep = double>
using force_v = si::force<U, vector<Rep>>;

void quantity_of_vector_add()
{
  std::cout << "\nquantity_of_vector_add:\n";

  length_v<> v(vector<>{4, 8, 12});
  length_v<> u(vector<>{3, 2, 1});
  length_v<si::kilometre> t(vector<>{3, 2, 1});

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";
  std::cout << "t = " << t << "\n";

  std::cout << "v + u = " << v + u << "\n";
  std::cout << "v + t = " << v + t << "\n";
  std::cout << "t[m]  = " << quantity_cast<si::metre>(t) << "\n";
}

void quantity_of_vector_divide_by_scalar()
{
  std::cout << "\nquantity_of_vector_divide_by_scalar:\n";

  length_v<> v(vector<>{4, 8, 12});

  std::cout << "v = " << v << "\n";

  std::cout << "v / s<>(2) = " << v / s<>(2) << "\n";
  std::cout << "v / 2 = " << v / 2 << "\n";
}

void quantity_of_vector_tests()
{
  quantity_of_vector_add();
  quantity_of_vector_divide_by_scalar();
}

template<units::Unit U = si::metre, units::Representation Rep = double>
using length_m = si::length<U, matrix<Rep>>;

void quantity_of_matrix_add()
{
  std::cout << "\nquantity_of_matrix_add:\n";

  length_m<> v(matrix<>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
  length_m<> u(matrix<>{{3, 2, 1}, {3, 2, 1}, {3, 2, 1}});
  length_m<si::kilometre> t(matrix<>{{3, 2, 1}, {3, 2, 1}, {3, 2, 1}});

  std::cout << "v =\n" << v << "\n";
  std::cout << "u =\n" << u << "\n";
  std::cout << "t =\n" << t << "\n";

  std::cout << "v + u =\n" << v + u << "\n";
  std::cout << "v + t =\n" << v + t << "\n";

  // TODO Fix it
  // std::cout << "v[mm] =\n" << matrix<length::mm<>>(v) << "\n";
}

void quantity_of_matrix_divide_by_scalar()
{
  std::cout << "\nquantity_of_matrix_divide_by_scalar:\n";

  length_m<> v(matrix<>{{2, 4, 6}, {4, 6, 8}, {8, 4, 2}});

  std::cout << "v =\n" << v << "\n";

  std::cout << "v / s<>(2) =\n" << v / s<>(2) << "\n";
  std::cout << "v / 2 =\n" << v / 2 << "\n";
}

void quantity_of_matrix_tests()
{
  quantity_of_matrix_add();
  quantity_of_matrix_divide_by_scalar();
}

}  // namespace

int main()
{
  vector_of_quantity_tests();
  matrix_of_quantity_tests();
  quantity_of_vector_tests();
  quantity_of_matrix_tests();
}
