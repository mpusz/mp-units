

#include <units/quantity.h>

#include <iostream>

#include <units/physical/dimensions.h>
#include <units/physical/si/prefixes.h>
#include <units/quantity.h>

/**
*  In some case a constant times a dimensionless ratio of 2 quantities is required
*  // https://www.sciencedirect.com/topics/engineering/standing-correlation
*  The class provides a means of output the ratio and to get at the underlying numeric value of the constant
*/

template <units::Quantity Num, units::Quantity Den>
requires units::DimensionlessQuantity<decltype( Num{}/ Den{})>
struct quantity_ratio{
   using rep = std::common_type_t<float,typename Num::rep,typename Den::rep>;
   constexpr quantity_ratio( const rep & v)
   : m_value{v}{};
   // convert it to a number
   constexpr rep numeric_value ()const
   {
      return (m_value * Num::one()) / Den::one();
   }
 private:
   friend std::ostream& operator << (std::ostream & os,quantity_ratio const & r)
   {
      // get at the units text of the quantity, without its numeric value
      auto unit_str = [] (const units::Quantity AUTO& q)
      {
        typedef std::remove_cvref_t<decltype(q)> qtype;
        return units::detail::unit_text<typename qtype::dimension, typename qtype::unit>().standard();
      };
      return os <<  r.m_value << ' ' << unit_str(Num{}) << '/' << unit_str(Den{})  ;
   }
   rep m_value;
};

//-----------------------

#include <units/physical/si/volume.h>
#include <units/physical/international/volume.h>

namespace {

using namespace units::physical;
using namespace units::physical::si::literals;
using namespace units::physical::international::literals;

}

// https://www.sciencedirect.com/topics/engineering/standing-correlation

int main()
{
    // using decltype on the quantity constants is a lazy way to get the definitions
    using CF_STB_ratio = quantity_ratio<decltype(0q_ft3),decltype(0q_stb)> ;

    constexpr CF_STB_ratio Rs = 456;

    std::cout << "Rs = " << Rs <<'\n'; 

    constexpr auto v = Rs.numeric_value();

    std::cout << "Rs numeric value = " << v << '\n'; 

}