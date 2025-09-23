#ifndef SYSTEMBASE_HPP
#define SYSTEMBASE_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <cstdlib>
#include <map>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <optional>


namespace ob2d {
    
    using uint = unsigned int;

    namespace scene{
        enum class ObLayer{
            layer_m3 = 0,
            layer_m2 = 1,
            layer_m1 = 2,
            layer_pm0= 3,
            layer_p1 = 4,
            layer_p2 = 5,
            layer_p3 = 6,
            NULL_LAYER = -1
        };

        namespace detail{
            extern uint16_t scene_system_manager;       //システム的にシーンを管理する。
            extern uint16_t now_scene_id;               //ゲームで設定されているシーンID

            //オブジェクトが同一シーンかつレイヤーにいるかどうかを調べるための道具みたいなものww
            struct object_scene_info{
                ObLayer now_layer;
                bool active;

                bool operator==(const object_scene_info& tar){
                    return (this->now_layer == tar.now_layer) && (this->active == tar.active);
                }

                bool operator!=(const object_scene_info& tar){
                    return (this->now_layer != tar.now_layer) || (this->active != tar.active);
                }
            };
        }


    }

    namespace time{
        namespace detail{
            extern float window_fps;
        }
        /// @brief 時間の単位を指定する列挙型
        enum class Timetypes {
            SECONDS = 0,
            MINUTES = 1,
            MILLI_SECONDS = 2
        };
    }

    namespace detail{
        extern bool resized_rendorsize;
        extern bool inited_window;
        enum class error{
            WINDOW_EMPTY,
        };

        void errorPrintf(error codes);

        

    };

    void openURL(const std::string &web_url);


    template <typename T>
    struct Vector2T {
        public:
            T x;
            T y;

            Vector2T<T>(T value_x = 0,T value_y = 0){
                x = value_x;
                y = value_y;
            }

            /**
             * @brief 同値比較演算子
             */
            bool operator==(const Vector2T<T>& other) const {
                return other.x == this->x && other.y == this->y;
            }

            bool operator!=(const Vector2T<T>& other) const {
                return other.x != this->x || other.y != this->y;
            }

            sf::Vector2<T> sf_getVector() const{
                return {x,y};
            }

            Vector2T<T> operator+(const Vector2T<T>& other){
                return {other.x  + this->x , other.y  + this->y };
            }
            Vector2T<T> operator-(const Vector2T<T>& other){
                return {other.x  - this->x , other.y  - this->y };
            }
            Vector2T<T> operator*(const Vector2T<T>& other){
                return {other.x * this->x , other.y * this->y };
            }
            Vector2T<T> operator/(const Vector2T<T>& other){
                return {other.x  / this->x , other.y  / this->y };
            }
    };

    template <typename T>
    struct Vector3T {
        public:
            T x;
            T y;
            T z;

            /**
             * @brief 同値比較演算子
             */
            bool operator==(const Vector3T<T>& other) const {
                return other.x == this->x && other.y == this->y &&  other.z == this->z;
            }

            bool operator!=(const Vector3T<T>& other) const {
                return other.x != this->x || other.y != this->y ||  other.z != this->z;
            }

            Vector3T<T> operator+(const Vector3T<T>& other){
                return {other.x  + this->x , other.y  + this->y, other.z  + this->z };
            }
            Vector3T<T> operator-(const Vector3T<T>& other){
                return {other.x  - this->x , other.y  - this->y, other.z  - this->z };
            }
            Vector3T<T> operator*(const Vector3T<T>& other){
                return {other.x * this->x , other.y * this->y , other.z * this->z };
            }
            Vector3T<T> operator/(const Vector3T<T>& other){
                return {other.x  / this->x , other.y  / this->y ,other.z  / this->z};
            }
    };

    template <typename T>
    struct ObStatus{
        ob2d::Vector2T<T> object_size;
        ob2d::Vector2T<T> center_position;
        ob2d::Vector2T<T> left_up_position;
        ob2d::Vector2T<T> left_down_position;
        ob2d::Vector2T<T> right_up_position;
        ob2d::Vector2T<T> right_down_position;
    };

    namespace sfml_tool{
        sf::String sf_convertString(const std::string &string);
    };

    namespace color {

        using Colors = sf::Color;  ///< SFML互換の色型（r, g, b, a）

        // === 一般色定義 ===
        // === 一般色定義 ===
        const Colors White  = Colors(255, 255, 255); ///< 白色
        const Colors Black  = Colors(0, 0, 0);       ///< 黒色
        const Colors Blue   = Colors(0, 0, 255);     ///< 青色
        const Colors Red    = Colors(255, 0, 0);     ///< 赤色
        const Colors Yellow = Colors(255, 255, 0);   ///< 黄色
        const Colors Green  = Colors(0, 128, 0);     ///< 緑色

                // === KAEPIオリジナル色（ブランドカラー）===
        const Colors KAEPI = Colors(255, 148, 134); ///< みねクラカラー（#ff9486）

        namespace color_114514{
            const Colors T = Colors(161, 115, 107);
            const Colors D = Colors(151, 159, 155);
            const Colors K = Colors(62, 52, 63);
            const Colors R = Colors(201, 191, 182);
        }

        namespace color_usagi{
            const Colors Cocoa = Colors(252,155,172);   ///< 心愛さん
            const Colors Chino = Colors(161,163,240);   ///< 智乃さん
            const Colors Rize = Colors(179,135,233);    ///< 理世さん
            const Colors Syaro = Colors(240,215,151);   ///< 紗路さん
            const Colors Chiya = Colors(83,158,154);    ///< 千夜さん
            const Colors Maya = Colors(184,234,243);    ///< 麻耶さん
            const Colors Megu = Colors(248,154,146);    ///< 恵さん
        }


        ob2d::color::Colors HexColor(std::string color_code_hex);

    }

}

#endif 