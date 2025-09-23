#include <iostream>
#include <SFML/System.hpp>
#include "System.hpp" 

#include "Time.hpp"

namespace ob2d::time{

    namespace detail {
        static sf::Clock delta_time_c;
        static sf::Time delta_time;

            /// @brief 毎フレームの経過時間を更新（内部専用）
        void dt_updateDeltatime() {
            delta_time = delta_time_c.restart();
            ob2d::time::detail::window_fps = 1.f / delta_time.asSeconds();
        }
    }

    float getDeltaTime(ob2d::time::Timetypes TIME_TYPE) {
        
        switch (TIME_TYPE) {
            case Timetypes::SECONDS:
                return detail::delta_time.asSeconds();
            case Timetypes::MINUTES:
                return detail::delta_time.asSeconds() / 60.0f;
            case Timetypes::MILLI_SECONDS:
                return static_cast<float>(detail::delta_time.asMilliseconds());
            default:
                return 0.f;
        }

    }


            /// @brief 計測を開始（すでに開始していたら無視）
            void ob2d::time::ObTimer::start() {
                if (!is_measurement) {
                    clock.restart();
                    is_measurement = true;
                }
            }

            /// @brief 計測を一時停止
            void ob2d::time::ObTimer::stop() {
                if (is_measurement) {
                    ProgressTime += clock.getElapsedTime();
                    is_measurement = false;
                }
            }

            /// @brief 計測をリスタート（時間リセット）
            void ob2d::time::ObTimer::restart() {
                ProgressTime = sf::Time::Zero;
                clock.restart();
                is_measurement = true;
            }

                /**
                 * @brief 現在までの計測時間を取得
                 * @param TIME_TYPE 秒・分・ミリ秒のいずれか
                 * @return float 測定時間
                 */
            float ob2d::time::ObTimer::getProgressTime(Timetypes TIME_TYPE) {
                sf::Time result = is_measurement ? ProgressTime + clock.getElapsedTime() : ProgressTime;
                    switch (TIME_TYPE) {
                        case Timetypes::SECONDS:
                            return result.asSeconds();
                        case Timetypes::MINUTES:
                            return result.asSeconds() / 60.0f;
                        case Timetypes::MILLI_SECONDS:
                            return static_cast<float>(result.asMilliseconds());
                        default:
                            std::cerr << "[ob2d::time::timer] Error: Unknown Timetype\n";
                            return 0.f;
                }
            }
                
            ob2d::time::ObTimer::ObTimer(bool auto_start){
                if(auto_start) start();
            }
}