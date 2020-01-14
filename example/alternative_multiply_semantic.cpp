
/*
 Copyright (c) 2003-2020 Andy Little.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see http://www.gnu.org/licenses./
*/


#include <units/physical/us/length.h>
#include <units/physical/iau/length.h>
#include <iostream>
#include <cmath>

// functions to calc higher powers
template <int N,typename Q>
  requires N == 1
auto constexpr qq_to_power( Q const & q)
{
   return q;
}

template <int N,typename Q>
  requires N == 0
auto constexpr qq_to_power( Q const & q)
{
   return 1;
}

template <int N,typename Q>
  requires N > 1
auto constexpr qq_to_power( Q const & q)
{
   return qq_multiply(qq_to_power<N-1>(q), q);
}

int main()
{
    auto x = units::si::length<units::us::foot,double>{1};

    auto xx = x * x ;
    auto xxx = xx * x;
    auto xxxx = xxx * x;

    std::cout << xxxx << '\n';



    //#################################################
    // uncomment to show compile fail issue
   // auto xxxxx = xxxx * x;

    //##################################################

    //--------------------------------------------------
    // alternative multiply semantic using si quantities

    auto x2 = qq_multiply(x,x);

    std::cout << x2 <<'\n';
    // check result is same for the power 2 case
    // N.b numeric value wont be same since x2 is a si quantity
    // but we can compare ratio 
    // N.B cant do this obviously for higher powers as q*q will compile fail)
    auto d = x2 / xx;

    std::cout << "ratio x2 / xx == " << d << " //(should be 1)\n";

   // now try multiplying to higher powers using the alternative semantic above...
   
    //x^3
    auto x3 = qq_multiply(x,x2);
    std::cout << x3 <<'\n';

    //x^4
    auto x4 = qq_multiply(x,x3);
    std::cout << x4 <<'\n';

    //x^5
    auto x5 = qq_multiply(x,x4);
    std::cout << x5 <<'\n';

    //x^6
    auto x6 = qq_multiply(x,x5);
    std::cout << x6 <<'\n';

    //x^7
    auto x7 = qq_multiply(x,x6);
    std::cout << x7 <<'\n';

    //x^8
    auto x8 = qq_multiply(x,x7);
    std::cout << x8 <<'\n';

    auto xN = qq_to_power<101>(x);

    std::cout << xN <<'\n';

   // etc ....
   // see also https://github.com/kwikius/quan-trunk/blob/master/quan_matters/examples/high_power_quantities.cpp#L37

   // Try other units

    auto constexpr z = units::si::length<units::iau::parsec,double>{1};

   //#############################################################
   // see existing useage example here
   // https://github.com/kwikius/quan-trunk/blob/master/quan_matters/examples/gravity.cpp#L78

   // auto zz = z * z;  // compile fail due to integer overflow

   //##############################################################

    // OK
    auto constexpr z1 = qq_multiply(z,z);

    std::cout << "z1 = " << z1 << '\n';
}


