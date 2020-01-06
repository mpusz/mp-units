

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
       using m = units::si::length<units::si::metre,double>;
       using mm = units::si::length<units::si::millimetre,double>;
   }
   
   namespace acceleration{
      using m_per_s2 = units::si::acceleration<units::si::metre_per_second_sq,double>;
      
      constexpr m_per_s2 g{9.80665};
   }

   namespace force{
       using N = units::si::force<units::si::newton,double>;
   }

   namespace mass {
      using kg = units::si::mass<units::si::kilogram,double>;
   }

   namespace density {
      using kg_per_m3 = units::si::density<units::si::kilogram_per_metre_cub,double>;
   }

   namespace volume {
      using m3 = units::si::volume<units::si::cubic_metre,double>;
   }
}

struct Box{

    Box(length::m const& l,
        length::m const& w,
        length::m const& h
    ): length{l},width{w},height{h}{}

    force::N filled_weight()const
    {
        volume::m3 volume
         = this->length * this->width * this->height;
        mass::kg mass = this->contents.density * volume;
        return mass * acceleration::g;
    }
    length::m fill_level(mass::kg const & measured_mass)const
    {
        return this->height
         * (measured_mass * acceleration::g) / filled_weight();
    }

    volume::m3 spare_capacity(mass::kg const & measured_mass)const
    {
      return (height - fill_level(measured_mass)) * width * length;
    }
    struct contents{
        contents():density{1.225}/*air density*/{}
        density::kg_per_m3 density;
    }contents;

    void set_contents_density(density::kg_per_m3 const & density_in)
    {
        assert( density_in > density::kg_per_m3{1.225} );
        this->contents.density = density_in;
    }

    length::m const length;
    length::m const width;
    length::m const height;

};

#include <iostream>

using namespace units::si::literals;
int main()
{
   
   auto box = Box{1000.0mm, 500.0mm, 200.0mm};
   box.set_contents_density(1000.0kgpm_cub);

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
