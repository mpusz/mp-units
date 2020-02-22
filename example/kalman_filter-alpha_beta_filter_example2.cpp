

#include <vector>
#include <iostream>
#include <iomanip>

#include <units/physical/si/length.h>
#include <units/physical/si/time.h>
#include <units/physical/si/velocity.h>

/*
    kalman filter tutorial
    1d aircraft α-β filter example2 from https://www.kalmanfilter.net/alphabeta.html#ex2
*/

using namespace units;
using namespace units::si::literals;

template <Quantity Q>
struct state_variable{
    Q estimated_current_state;
    Q predicted_next_state;
};

namespace {
   constexpr auto radar_transmit_interval = 5.0q_s;
   constexpr double kalman_range_gain = 0.2;
   constexpr double kalman_speed_gain = 0.1;
}

struct state{

   state_variable<si::length<si::metre> > range;
   state_variable<si::velocity<si::metre_per_second> > speed;

   void estimate(const state & previous_state, const si::length<si::metre>& measurement)
   {
       auto const innovation = measurement - previous_state.range.predicted_next_state;
       range.estimated_current_state = previous_state.range.predicted_next_state + kalman_range_gain * innovation;
       speed.estimated_current_state = previous_state.speed.predicted_next_state + kalman_speed_gain * innovation / radar_transmit_interval;
   }

   void predict()
   {
      range.predicted_next_state = range.estimated_current_state + speed.estimated_current_state * radar_transmit_interval;
      speed.predicted_next_state = speed.estimated_current_state;
   }
};

int main()
{
   std::cout << "\n\n1d aircraft α-β filter example2 from https://www.kalmanfilter.net/alphabeta.html#ex2";
   std::cout << "\n\n";

   std::vector<si::length<si::metre> > measurements {
      0.0q_m,    // N.B measurement[0] is unknown and unused
      30110.0q_m, 
      30265.0q_m,
      30740.0q_m,
      30750.0q_m, 
      31135.0q_m,
      31015.0q_m, 
      31180.0q_m, 
      31610.0q_m,
      31960.0q_m, 
      31865.0q_m
   };

   const auto num_measurements = measurements.size();

   std::vector<state> track{num_measurements};

   //We need an initial estimate of track[0] as there is no previous state to get a prediction from
   track[0].range.estimated_current_state = 30'000q_m;
   track[0].speed.estimated_current_state = 40.0q_mps;
   
   for ( auto n = 0U; n < num_measurements;++n){

      if ( n > 0){
          track[n].estimate(track[n-1],measurements[n]);
      }
      track[n].predict();

      std::cout << std::fixed;
      std::cout << "measurement[" << n << "]                    = " << std::setprecision(0) << measurements[n] <<'\n';
      std::cout << "range.estimated_current_state[" << n << "]  = " << std::setprecision(1) << track[n].range.estimated_current_state<<'\n';
      std::cout << "speed.estimated_current_state[" << n << "]  = " << track[n].speed.estimated_current_state  <<'\n';
      std::cout << "range.predicted_next_state[" << n << "]     = " << track[n].range.predicted_next_state << '\n';
      std::cout << "speed.predicted_next_state[" << n << "]     = " << track[n].speed.predicted_next_state << "\n\n";
   }

}
