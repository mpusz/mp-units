

#include <units/physical/si/acceleration.h>
#include <units/physical/si/length.h>
#include <units/physical/si/volume.h>
#include <units/physical/si/time.h>
#include <units/physical/si/force.h>
#include <units/physical/si/mass.h>
#include <units/physical/si/density.h>
#include <cassert>

namespace{

   namespace length{

       template <typename Rep = double>
       using m = units::si::length<units::si::metre,Rep>;

       template <typename Rep = double>
       using mm = units::si::length<units::si::millimetre,Rep>;
   }
   
   namespace acceleration{

      template <typename Rep = double>
      using m_per_s2 = units::si::acceleration<units::si::metre_per_second_sq,Rep>;

      template <typename Rep = double>
      constexpr m_per_s2<> g{static_cast<Rep>(9.80665)};
   }

   namespace force{

       template <typename Rep = double>
       using N = units::si::force<units::si::newton,Rep>;
   }

   namespace mass {

      template <typename Rep = double>
      using kg = units::si::mass<units::si::kilogram,Rep>;
   }

   namespace density {

      template <typename Rep = double>
      using kg_per_m3 = units::si::density<units::si::kilogram_per_metre_cub,Rep>;
   }

   namespace volume {

      template <typename Rep = double>
      using m3 = units::si::volume<units::si::cubic_metre,Rep>;
   }
}

struct Box{

    Box(length::m<> const& l,
        length::m<> const& w,
        length::m<> const& h
    ): length{l},width{w},height{h}{}

    force::N<> filled_weight()const
    {
        volume::m3<> const volume
           = length * width * height;
        mass::kg<> const mass = contents.density * volume;
        return mass * acceleration::g<>;
    }

    length::m<> fill_level(mass::kg<> const & measured_mass)const
    {
       return height
          * (measured_mass * acceleration::g<>) / filled_weight();
    }

    volume::m3<> spare_capacity(mass::kg<> const & measured_mass)const
    {
       return (height - fill_level(measured_mass)) * width * length;
    }

    struct contents{
        contents():density{air_density}{}
        density::kg_per_m3<> density;
    }contents;

    void set_contents_density(density::kg_per_m3<> const & density_in)
    {
        assert( density_in > air_density );
        contents.density = density_in;
    }

    static constexpr density::kg_per_m3<> air_density{1.225};

    length::m<> length;
    length::m<> width;
    length::m<> height;
};

#include <iostream>

using namespace units::si::literals;
int main()
{
   auto box = Box{1000.0mm, 500.0mm, 200.0mm};
   box.set_contents_density(1000.0kg_per_m3);

   auto fill_time = 200.0s;      // time since starting fill
   auto measured_mass = 20.0kg;  // measured mass at fill_time

   std::cout << "mpusz/units box example...\n";
   std::cout << "fill height at " << fill_time << " = "
   << box.fill_level(measured_mass) << "( " << (box.fill_level(measured_mass)/ box.height)*100 << "% full)\n";
   std::cout << "spare_capacity at " << fill_time << " = "
   << box.spare_capacity(measured_mass) <<'\n';
   std::cout << "input flow rate after " << fill_time
   << " = " << measured_mass / fill_time <<'\n';
   std::cout << "float rise rate = "
   << box.fill_level(measured_mass) / fill_time <<'\n';
   auto fill_time_left
   = (box.height / box.fill_level(measured_mass) - 1) * fill_time ;
   std::cout << "box full E.T.A. at current flow rate = " << fill_time_left <<'\n';

}
