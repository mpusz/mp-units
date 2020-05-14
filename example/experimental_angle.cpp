


#include <units/physical/si/length.h>
#include <units/physical/si/torque.h>
#include <units/physical/si/energy.h>
#include <units/format.h>
#include <iostream>

using namespace units;
using namespace units::physical::si::literals;

int main()
{
   auto torque = 20.0q_Nm;
   auto energy = 20.0q_J;

   physical::Angle angle = torque/energy;

   std::cout << angle <<'\n';
}

