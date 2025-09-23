#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <SFML/System.hpp>
#include "System.hpp"

namespace ob2d {
    namespace time {

        namespace detail {
            void dt_updateDeltatime();
        }

        float getDeltaTime(ob2d::time::Timetypes TIME_TYPE);

        class ObTimer {
            private:
                sf::Clock clock;
                sf::Time ProgressTime;
                bool is_measurement  ;
            public:
                void start();
                void stop();
                void restart();
                float getProgressTime(Timetypes TIME_TYPE);
                
                ObTimer(bool auto_start = false);
        };

    } // namespace time
} // namespace ob2d

#endif