
#include <iostream>
#include "./timer.h"

/*
   simple timer, useful for perf timing etc
*/

using namespace units::experimental;
using namespace units::physical::si::literals;

int main()
{
   std::cout << "Simple timer using mpusz/units ...\n";

   auto const period = 0.5_q_s;
   auto const duration = 10 * period;

   timer t;

   auto const start_time = t();

   std::cout << "Started at " << start_time <<'\n';

   auto prev = start_time;
   for (auto now = t(); (now - start_time) < duration; now = t() ) {
      if ( (now - prev ) >= period ){
         prev = now;
         std::cout << "tick (" << now << ")\n";;
      }
   }
   t.stop();

   std::cout << "finished at " << t() << '\n';
}
