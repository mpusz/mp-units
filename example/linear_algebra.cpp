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

#include <linear_algebra.hpp>
// linear_algebra.hpp has to be included first otherwise the header will fail to compile!
#include <mp_units/customization_points.h>
#include <mp_units/format.h>
#include <mp_units/quantity_io.h>
#include <mp_units/systems/isq/mechanics.h>
#include <mp_units/systems/isq/space_and_time.h>
#include <mp_units/systems/si/unit_symbols.h>
#include <mp_units/systems/si/units.h>
#include <iostream>

namespace STD_LA {

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const vector<ET, OT>& v)
{
  os << "|";
  for (auto i = 0U; i < v.size(); ++i) {
    os << STD_FMT::format(" {:>9}", v(i));
  }
  os << " |";
  return os;
}

template<class ET, class OT>
std::ostream& operator<<(std::ostream& os, const matrix<ET, OT>& v)
{
  for (auto i = 0U; i < v.rows(); ++i) {
    os << "|";
    for (auto j = 0U; j < v.columns(); ++j) {
      os << STD_FMT::format(" {:>9}", v(i, j));
    }
    os << (i != v.rows() - 1U ? " |\n" : " |");
  }
  return os;
}

}  // namespace STD_LA

template<typename Rep = double>
using vector = std::math::fs_vector<Rep, 3>;

template<typename Rep>
inline constexpr bool mp_units::is_vector<vector<Rep>> = true;

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// template<typename Rep = double>
// using matrix = std::math::fs_matrix<Rep, 3, 3>;


// constexpr auto mm = isq::length[si::milli<si::metre>];
// constexpr auto m = isq::length[si::metre];
// constexpr auto km = isq::length[si::kilo<si::metre>];
// constexpr auto N = isq::force[si::newton];

// template<Unit auto U = si::metre, Representation Rep = double>
// using length_v = quantity<isq::length[U], vector<Rep>>;

// template<Unit auto U = si::newton, Representation Rep = double>
// using force_v = quantity<isq::force[U], vector<Rep>>;

void quantity_of_vector_cast()
{
  std::cout << "\nquantity_of_vector_cast:\n";

  quantity<isq::position_vector[m], vector<int>> v{vector<int>{1001, 1002, 1003}};
  quantity<isq::position_vector[km], vector<int>> u{vector<int>{3, 2, 1}};

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";

  std::cout << "v[km] = " << value_cast<km>(v) << "\n";
  std::cout << "u[m]  = " << u[m] << "\n";
}

void quantity_of_vector_multiply_by_scalar_value()
{
  std::cout << "\nquantity_of_vector_multiply_scalar_value:\n";

  quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};

  std::cout << "v = " << v << "\n";

  std::cout << "2 * v = " << 2 * v << "\n";
}

void quantity_of_vector_divide_by_scalar_value()
{
  std::cout << "\nquantity_of_vector_divide_scalar_value:\n";

  quantity<isq::position_vector[m], vector<int>> v{vector<int>{2, 4, 6}};

  std::cout << "v = " << v << "\n";

  std::cout << "v / 2 = " << v / 2 << "\n";
}

void quantity_of_vector_add()
{
  std::cout << "\nquantity_of_vector_add:\n";

  // TODO Does not compile (LA bug)
  // auto v = vector<int>{1, 2, 3} * isq::position_vector[m];
  // auto u = vector<int>{3, 2, 1} * isq::position_vector[m];
  // auto t = vector<int>{3, 2, 1} * isq::position_vector[km];

  // quantity<isq::distance[m], vector<int>> v{vector<int>{1, 2, 3}};  // should not compile
  quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};
  quantity<isq::position_vector[m], vector<int>> u{vector<int>{3, 2, 1}};
  quantity<isq::position_vector[km], vector<int>> t{vector<int>{3, 2, 1}};

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";
  std::cout << "t = " << t << "\n";

  std::cout << "v + u = " << v + u << "\n";
  std::cout << "v + t = " << v + t << "\n";
}

void quantity_of_vector_subtract()
{
  std::cout << "\nquantity_of_vector_subtract:\n";

  quantity<isq::position_vector[m], vector<int>> v{vector<int>{1, 2, 3}};
  quantity<isq::position_vector[m], vector<int>> u{vector<int>{3, 2, 1}};
  quantity<isq::position_vector[km], vector<int>> t{vector<int>{3, 2, 1}};

  std::cout << "v = " << v << "\n";
  std::cout << "u = " << u << "\n";
  std::cout << "t = " << t << "\n";

  std::cout << "v - u = " << v - u << "\n";
  std::cout << "v - t = " << v - t << "\n";
}

void quantity_of_vector_divide_by_scalar_quantity()
{
  std::cout << "\nquantity_of_vector_divide_scalar:\n";

  quantity<isq::position_vector[km], vector<int>> pos{vector<int>{30, 20, 10}};
  auto dur = 2 * isq::duration[h];
  quantity<isq::velocity[km / h], vector<int>> v = pos / dur;

  std::cout << "pos = " << pos << "\n";
  std::cout << "dur = " << dur << "\n";

  std::cout << "pos / dur = " << pos / dur << "\n";
  std::cout << "v = " << v << "\n";
}


// void quantity_of_vector_multiply_same()
// {
//   std::cout << "\nquantity_of_vector_multiply_same:\n";

//   length_v<> v(vector<>{1, 2, 3});
//   length_v<> u(vector<>{3, 2, 1});

//   std::cout << "v = " << v << "\n";
//   std::cout << "u = " << u << "\n";

//   std::cout << "v * u    = " << v * u << "\n";
//   std::cout << "2 * m * v = " << 2. * m * v << "\n";
// }

// void quantity_of_vector_multiply_different()
// {
//   std::cout << "\nquantity_of_vector_multiply_different:\n";

//   force_v<> v(vector<>{1, 2, 3});
//   length_v<> u(vector<>{3, 2, 1});

//   std::cout << "v = " << v << "\n";
//   std::cout << "u = " << u << "\n";

//   std::cout << "v * u    = " << v * u << "\n";
//   std::cout << "2 * N * u = " << 2. * N * u << "\n";
//   std::cout << "2 * u    = " << 2 * u << "\n";
// }

// void quantity_of_vector_divide_by_scalar()
// {
//   std::cout << "\nquantity_of_vector_divide_by_scalar:\n";

//   length_v<> v(vector<>{4, 8, 12});

//   std::cout << "v = " << v << "\n";

//   // TODO Uncomment when bug in the LA is fixed
//   // std::cout << "v / 2 * s = " << v / 2 * s << "\n";
//   // std::cout << "v / 2 = " << v / 2 << "\n";
// }

void quantity_of_vector_tests()
{
  quantity_of_vector_cast();
  quantity_of_vector_multiply_by_scalar_value();
  quantity_of_vector_divide_by_scalar_value();
  quantity_of_vector_add();
  quantity_of_vector_subtract();
  quantity_of_vector_divide_by_scalar_quantity();
  // quantity_of_vector_multiply_same();
  // quantity_of_vector_multiply_different();
  // quantity_of_vector_divide_by_scalar();
}

// template<Unit auto U = si::metre, Representation Rep = double>
// using length_m = quantity<isq::length[U], matrix<Rep>>;

// void quantity_of_matrix_add()
// {
//   std::cout << "\nquantity_of_matrix_add:\n";

//   length_m<> v(matrix<>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
//   length_m<> u(matrix<>{{3, 2, 1}, {3, 2, 1}, {3, 2, 1}});
//   length_m<si::kilo<si::metre>> t(matrix<>{{3, 2, 1}, {3, 2, 1}, {3, 2, 1}});

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";
//   std::cout << "t =\n" << t << "\n";

//   std::cout << "v + u =\n" << v + u << "\n";
//   std::cout << "v + t =\n" << v + t << "\n";

//   // TODO Uncomment when fixed in the LA lib
//   // std::cout << "v[mm] =\n" << matrix<si::length<si::millimetre>>(v) << "\n";
// }

// void quantity_of_matrix_multiply_same()
// {
//   std::cout << "\nquantity_of_matrix_multiply_same:\n";

//   length_m<> v(matrix<>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
//   length_v<> u(vector<>{3, 2, 1});

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";

//   std::cout << "v * u =\n" << v * u << "\n";
//   std::cout << "2 * m * u =\n" << 2. * m * u << "\n";
// }

// void quantity_of_matrix_multiply_different()
// {
//   std::cout << "\nquantity_of_matrix_multiply_different:\n";

//   force_v<> v(vector<>{1, 2, 3});
//   length_m<> u(matrix<>{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";

//   std::cout << "v * u =\n" << v * u << "\n";
//   std::cout << "2 * N * u =\n" << 2. * N * u << "\n";
//   std::cout << "2 * u =\n" << 2 * u << "\n";
// }

// void quantity_of_matrix_divide_by_scalar()
// {
//   std::cout << "\nquantity_of_matrix_divide_by_scalar:\n";

//   length_m<> v(matrix<>{{2, 4, 6}, {4, 6, 8}, {8, 4, 2}});

//   std::cout << "v =\n" << v << "\n";

//   // TODO Uncomment when bug in the LA is fixed
//   // std::cout << "v / (2 * s) =\n" << v / (2 * s) << "\n";
//   // std::cout << "v / 2 =\n" << v / 2 << "\n";
// }

// void quantity_of_matrix_tests()
// {
//   quantity_of_matrix_add();
//   quantity_of_matrix_multiply_same();
//   quantity_of_matrix_multiply_different();
//   quantity_of_matrix_divide_by_scalar();
// }


// void vector_of_quantity_add()
// {
//   std::cout << "\nvector_of_quantity_add:\n";

//   vector<quantity<m>> v = {1 * m, 2 * m, 3 * m};
//   vector<quantity<m>> u = {3 * m, 2 * m, 1 * m};
//   vector<quantity<km>> t = {3 * km, 2 * km, 1 * km};

//   std::cout << "v = " << v << "\n";
//   std::cout << "u = " << u << "\n";
//   std::cout << "t = " << t << "\n";

//   std::cout << "v + u = " << v + u << "\n";
//   std::cout << "v + t = " << v + t << "\n";
//   std::cout << "t[m]  = " << vector<quantity<m>>(t) << "\n";
// }

// void vector_of_quantity_multiply_same()
// {
//   std::cout << "\nvector_of_quantity_multiply_same:\n";

//   vector<quantity<m>> v = {1 * m, 2 * m, 3 * m};
//   vector<quantity<m>> u = {3 * m, 2 * m, 1 * m};

//   std::cout << "v = " << v << "\n";
//   std::cout << "u = " << u << "\n";

//   std::cout << "v * u    = " << v * u << "\n";
//   std::cout << "2 * m * v = " << 2. * m * v << "\n";
// }

// void vector_of_quantity_multiply_different()
// {
//   std::cout << "\nvector_of_quantity_multiply_different:\n";

//   vector<quantity<N>> v = {1 * N, 2 * N, 3 * N};
//   vector<quantity<m>> u = {3 * m, 2 * m, 1 * m};

//   std::cout << "v = " << v << "\n";
//   std::cout << "u = " << u << "\n";

//   std::cout << "v * u    = " << v * u << "\n";
//   std::cout << "2 * N * u = " << 2. * N * u << "\n";
//   std::cout << "2 * u    = " << 2 * u << "\n";
// }

// void vector_of_quantity_divide_by_scalar()
// {
//   std::cout << "\nvector_of_quantity_divide_by_scalar:\n";

//   vector<quantity<m>> v = {4 * m, 8 * m, 12 * m};

//   std::cout << "v = " << v << "\n";

//   // TODO Uncomment when bug in the LA is fixed
//   // std::cout << "v / (2 * s) = " << v / (2 * s) << "\n";
//   // std::cout << "v / 2 = " << v / 2 << "\n";
// }

// void vector_of_quantity_tests()
// {
//   vector_of_quantity_add();
//   vector_of_quantity_multiply_same();
//   vector_of_quantity_multiply_different();
//   vector_of_quantity_divide_by_scalar();
// }

// void matrix_of_quantity_add()
// {
//   std::cout << "\nmatrix_of_quantity_add:\n";

//   matrix<quantity<m>> v = {{1 * m, 2 * m, 3 * m}, {4 * m, 5 * m, 6 * m}, {7 * m, 8 * m, 9 * m}};
//   matrix<quantity<m>> u = {{3 * m, 2 * m, 1 * m}, {3 * m, 2 * m, 1 * m}, {3 * m, 2 * m, 1 * m}};
//   matrix<quantity<mm>> t = {{3 * mm, 2 * mm, 1 * mm}, {3 * mm, 2 * mm, 1 * mm}, {3 * mm, 2 * mm, 1 * mm}};

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";
//   std::cout << "t =\n" << t << "\n";

//   std::cout << "v + u =\n" << v + u << "\n";
//   std::cout << "v + t =\n" << v + t << "\n";

//   // TODO Uncomment when fixed in the LA lib
//   // std::cout << "v[mm] =\n" << matrix<si::length<si::millimetre>>(v) << "\n";
// }

// void matrix_of_quantity_multiply_same()
// {
//   std::cout << "\nmatrix_of_quantity_multiply_same:\n";

//   matrix<quantity<m>> v = {{1 * m, 2 * m, 3 * m}, {4 * m, 5 * m, 6 * m}, {7 * m, 8 * m, 9 * m}};
//   vector<quantity<m>> u = {3 * m, 2 * m, 1 * m};

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";

//   std::cout << "v * u =\n" << v * u << "\n";
//   std::cout << "2 * m * u =\n" << 2. * m * u << "\n";
// }

// void matrix_of_quantity_multiply_different()
// {
//   std::cout << "\nmatrix_of_quantity_multiply_different:\n";

//   vector<quantity<N>> v = {1 * N, 2 * N, 3 * N};
//   matrix<quantity<m>> u = {{1 * m, 2 * m, 3 * m}, {4 * m, 5 * m, 6 * m}, {7 * m, 8 * m, 9 * m}};

//   std::cout << "v =\n" << v << "\n";
//   std::cout << "u =\n" << u << "\n";

//   std::cout << "v * u =\n" << v * u << "\n";
//   std::cout << "2 * N * u =\n" << 2. * N * u << "\n";
//   std::cout << "2 * u =\n" << 2 * u << "\n";
// }

// void matrix_of_quantity_divide_by_scalar()
// {
//   std::cout << "\nmatrix_of_quantity_divide_by_scalar:\n";

//   matrix<quantity<m>> v = {{2 * m, 4 * m, 6 * m}, {4 * m, 6 * m, 8 * m}, {8 * m, 4 * m, 2 * m}};

//   std::cout << "v =\n" << v << "\n";

//   // TODO Uncomment when bug in the LA is fixed
//   // std::cout << "v / (2 * s) =\n" << v / (2 * s) << "\n";
//   // std::cout << "v / 2 =\n" << v / 2 << "\n";
// }

// void matrix_of_quantity_tests()
// {
//   matrix_of_quantity_add();
//   matrix_of_quantity_multiply_same();
//   matrix_of_quantity_multiply_different();
//   matrix_of_quantity_divide_by_scalar();
// }

}  // namespace

int main()
{
  quantity_of_vector_tests();
  // quantity_of_matrix_tests();
  // vector_of_quantity_tests();
  // matrix_of_quantity_tests();
}
