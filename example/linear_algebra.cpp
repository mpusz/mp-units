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

#include <units/physical/si/velocity.h>
#include <units/physical/si/force.h>
#include <units/physical/si/energy.h>
#include <units/format.h>
#include <linear_algebra.hpp>
#include <iostream>

namespace {

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const std::experimental::math::vector<ET, OT>& v)
{
  os << "|";
  for (size_t i = 0;  i < v.size();  ++i) {
    os << fmt::format(" {:>9}", v(i));
  }
  os << " |";
  return os;
}

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const std::experimental::math::matrix<ET, OT>& v)
{
  for (size_t i = 0;  i < v.rows();  ++i) {
    os << "|";
    for (size_t j = 0;  j < v.columns();  ++j) {
      os << fmt::format(" {:>9}", v(i, j));
    }
    os << (i != v.rows() - 1 ? " |\n" : " |");
  }
  return os;
}


using namespace std::experimental::math;
using namespace units;
using namespace si::literals;

void vector_of_quantity_add()
{
  std::cout << "\nvector_of_quantity_add:\n";

  fs_vector<si::length<si::metre>, 3> v = { 1q_m, 2q_m, 3q_m };
  fs_vector<si::length<si::metre>, 3> u = { 3q_m, 2q_m, 1q_m };
  fs_vector<si::length<si::kilometre>, 3> t = { 3q_km, 2q_km, 1q_km };

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";
  std::cout << "t = " << t << "\n";

  std::cout << "v + u = " << v + u << "\n";
  std::cout << "v + t = " << v + t << "\n";
  std::cout << "fs_vector<si::length<si::metre>, 3>(t) = " << fs_vector<si::length<si::metre>, 3>(t) << "\n";
}

void vector_of_quantity_multiply_same()
{
  std::cout << "\nvector_of_quantity_multiply_same:\n";

  fs_vector<si::length<si::metre>, 3> v = { 1q_m, 2q_m, 3q_m };
  fs_vector<si::length<si::metre>, 3> u = { 3q_m, 2q_m, 1q_m };

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";

  std::cout << "v * u = " << v * u << "\n";
  std::cout << "2q_m * v = " << 2q_m * v << "\n";
}

void vector_of_quantity_multiply_different()
{
  std::cout << "\nvector_of_quantity_multiply_different:\n";

  fs_vector<si::force<si::newton>, 3> v = { 1q_N, 2q_N, 3q_N };
  fs_vector<si::length<si::metre>, 3> u = { 3q_m, 2q_m, 1q_m };

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";

  std::cout << "v * u = " << v * u << "\n";
  std::cout << "2q_N * u = " << 2q_N * u << "\n";
  std::cout << "2 * u = " << 2 * u << "\n";
}

void vector_of_quantity_divide_by_scalar()
{
  std::cout << "\nvector_of_quantity_divide_by_scalar:\n";

  fs_vector<si::length<si::metre>, 3> v = { 4q_m, 8q_m, 12q_m };

  std::cout << "v = " << v << "\n";

  // TODO Uncomment when bug in the LA is fixed
  // std::cout << "v / 2q_s = " << v / 2q_s << "\n";
  // std::cout << "v / 2 = " << v / 2 << "\n";
}

void vector_of_quantity_tests()
{
  vector_of_quantity_add();
  vector_of_quantity_multiply_same();
  vector_of_quantity_multiply_different();
  vector_of_quantity_divide_by_scalar();
}

void matrix_of_quantity_add()
{
  std::cout << "\nmatrix_of_quantity_add:\n";

  fs_matrix<si::length<si::metre>, 3, 3> v = {{ 1q_m, 2q_m, 3q_m }, { 4q_m, 5q_m, 6q_m }, { 7q_m, 8q_m, 9q_m }};
  fs_matrix<si::length<si::metre>, 3, 3> u = {{ 3q_m, 2q_m, 1q_m }, { 3q_m, 2q_m, 1q_m }, { 3q_m, 2q_m, 1q_m }};
  fs_matrix<si::length<si::millimetre>, 3, 3> t = {{ 3q_mm, 2q_mm, 1q_mm }, { 3q_mm, 2q_mm, 1q_mm }, { 3q_mm, 2q_mm, 1q_mm }};

  std::cout << "v =\n" << v << "\n";
  std::cout << "u =\n" << u << "\n";
  std::cout << "t =\n" << t << "\n";

  std::cout << "v + u =\n" << v + u << "\n";
  std::cout << "v + t =\n" << v + t << "\n";

  // TODO Uncomment when fixed in the LA lib
  // std::cout << "fs_matrix<si::length<si::millimetre>, 3, 3>(v) =\n" << fs_matrix<si::length<si::millimetre>, 3, 3>(v) << "\n";
}

void matrix_of_quantity_multiply_same()
{
  std::cout << "\nmatrix_of_quantity_multiply_same:\n";
  
  fs_matrix<si::length<si::metre>, 3, 3> v = {{ 1q_m, 2q_m, 3q_m }, { 4q_m, 5q_m, 6q_m }, { 7q_m, 8q_m, 9q_m }};
  fs_vector<si::length<si::metre>, 3> u = { 3q_m, 2q_m, 1q_m };

  std::cout << "v =\n" << v << "\n";
  std::cout << "u =\n" << u << "\n";

  std::cout << "v * u =\n" << v * u << "\n";
  std::cout << "2q_m * u =\n" << 2q_m * u << "\n";
}

void matrix_of_quantity_multiply_different()
{
  std::cout << "\nmatrix_of_quantity_multiply_different:\n";

  fs_vector<si::force<si::newton>, 3> v = { 1q_N, 2q_N, 3q_N };
  fs_matrix<si::length<si::metre>, 3, 3> u = {{ 1q_m, 2q_m, 3q_m }, { 4q_m, 5q_m, 6q_m }, { 7q_m, 8q_m, 9q_m }};

  std::cout << "v =\n" << v << "\n";
  std::cout << "u =\n" << u << "\n";

  std::cout << "v * u =\n" << v * u << "\n";
  std::cout << "2q_N * u =\n" << 2q_N * u << "\n";
  std::cout << "2 * u =\n" << 2 * u << "\n";
}

void matrix_of_quantity_divide_by_scalar()
{
  std::cout << "\nmatrix_of_quantity_divide_by_scalar:\n";

  fs_matrix<si::length<si::metre>, 3, 3> v = {{ 2q_m, 4q_m, 6q_m }, { 4q_m, 6q_m, 8q_m }, { 8q_m, 4q_m, 2q_m }};

  std::cout << "v =\n" << v << "\n";

  // TODO Uncomment when bug in the LA is fixed
  // std::cout << "v / 2q_s =\n" << v / 2q_s << "\n";
  // std::cout << "v / 2 =\n" << v / 2 << "\n";
}

void matrix_of_quantity_tests()
{
  matrix_of_quantity_add();
  matrix_of_quantity_multiply_same();
  matrix_of_quantity_multiply_different();
  matrix_of_quantity_divide_by_scalar();
}

}

int main()
{
  vector_of_quantity_tests();
  matrix_of_quantity_tests();
}
