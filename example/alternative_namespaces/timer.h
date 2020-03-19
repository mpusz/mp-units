#pragma once

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

#include "./time.h"

#include <ctime>

namespace units{ namespace experimental{
 
    class timer{
        
    public:
        timer(): running(true)
        {
            start_time = std::clock();
        }

        void restart() 
        {
            running = true;
            start_time = std::clock();
        }

        void stop() 
        {
            if (running){
                stop_time = std::clock();
                running = false;
            }
        }
        
        q_time::ms<> operator ()()const
        {
            std::clock_t const wanted = running ? std::clock() : stop_time;
            static constexpr auto divider = 1000.0 / CLOCKS_PER_SEC;
           
            return q_time::ms<>( 
                difftime(wanted , start_time) 
            ) * divider;
        }
        bool is_running() const {return running;}
        bool is_stopped() const {return !running;}
    private:
        bool running;
        std::clock_t start_time,stop_time;
    };

} }//units::experimental
