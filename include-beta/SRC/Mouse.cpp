#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <SFML/OpenGL.hpp>

#include "System.hpp"
#include "Window.hpp"

#include "System.hpp"
#include "hid.hpp"
#include "Scene.hpp"

namespace ob2d::hid::mouse {

            namespace detail{
                ob2d::Vector2T<float> mouse_wheel_delta_now;
                ob2d::Vector2T<float> mouse_wheel_pos_bef;
                bool is_visible_cursor = true;
                ob2d::Vector2T<float> cursor_hotspot;

            }

            uint32_t mouse_trigger = 0;  ///< このフレームで押されたボタン
            uint32_t mouse_release = 0;  ///< このフレームで離されたボタン
            uint32_t mouse_hold    = 0;  ///< 押しっぱなしのボタン

            /// @brief 現在のマウスカーソルの座標を取得します（ウィンドウ内座標）
            ob2d::Vector2T<int> getCursorPosition(ob2d::scene::ObLayer layer_id) {
                if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

                auto mouse_pos = sf::Mouse::getPosition(ob2d::window::sf_getWindow());
                auto window_size = ob2d::window::sf_getWindow().getSize(); // ウィンドウの実ピクセルサイズ

                Vector2T<float> cam_postion; 
                switch (layer_id){
                    case ob2d::scene::ObLayer::layer_m3 :
                        cam_postion = ob2d::scene::layer_m3.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_m2 :
                        cam_postion = ob2d::scene::layer_m2.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_m1 :
                        cam_postion = ob2d::scene::layer_m1.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_pm0 :
                        cam_postion = ob2d::scene::layer_pm0.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_p1 :
                        cam_postion = ob2d::scene::layer_p1.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_p2 :
                        cam_postion = ob2d::scene::layer_p2.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::layer_p3 :
                        cam_postion = ob2d::scene::layer_p3.getCameraStatus().left_up_position;
                        break;
                    case ob2d::scene::ObLayer::NULL_LAYER :
                        cam_postion = {0.f,0.f};
                        break;
                    default:
                        break;
                }

                // オフセット（黒帯部分の量）をピクセル単位に変換してから引く
                int logic_x = (mouse_pos.x - window::detail::view_offset.x * window_size.x) / window::detail::view_render_scale;
                int logic_y = (mouse_pos.y - window::detail::view_offset.y * window_size.y) / window::detail::view_render_scale;

                return { cam_postion.x + logic_x,cam_postion.y +  logic_y };
            }

            /// @brief カーソルを指定座標に移動させます（ウィンドウ基準）
            void setCursorPosition(Vector2T<int> position) {
                if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                auto mouse_pos = sf::Mouse::getPosition(ob2d::window::sf_getWindow());
                auto window_size = ob2d::window::sf_getWindow().getSize(); // ウィンドウの実ピクセルサイズ

                // オフセット（黒帯部分の量）をピクセル単位に変換してから引く
                int logic_x = (mouse_pos.x - window::detail::view_offset.x * window_size.x) / window::detail::view_render_scale;
                int logic_y = (mouse_pos.y - window::detail::view_offset.y * window_size.y) / window::detail::view_render_scale;

                sf::Mouse::setPosition({ logic_x, logic_y  }, ob2d::window::sf_getWindow());
            }

            /// @brief ホイールの回転量を取得します（1単位 ≒ 1ノッチ）
            ob2d::Vector2T<float> getMouseWheelDelta() {
                if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                return ob2d::hid::mouse::detail::mouse_wheel_delta_now;
            }

            /// @brief システムカーソルを非表示にします（カスタムカーソル用）
            void setVisibleCursor(bool flag) {
                if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                ob2d::window::sf_getWindow().setMouseCursorVisible(flag);
                detail::is_visible_cursor = flag;
            }



            /// @brief カーソルが現在非表示かどうかを返します
            bool is_VisibleCursor() {
                return detail::is_visible_cursor;
            }


            /// @brief 指定ボタンが「このフレームで押されたか」を判定します
            bool pushButtonTrigger(uint8_t MOUSE_BUTTON) {
                return (MOUSE_BUTTON & mouse_trigger) != 0;
            }

            /// @brief 指定ボタンが「このフレームで離されたか」を判定します
            bool pushButtonRelease(uint8_t MOUSE_BUTTON) {
                return (MOUSE_BUTTON & mouse_release) != 0;
            }

            /// @brief 指定ボタンが「押され続けているか」を判定します
            bool pushButtonHold(uint8_t MOUSE_BUTTON) {
                return (MOUSE_BUTTON & mouse_hold) != 0;
            }
}

namespace ob2d::hid::detail{
    void UpdateMouseStatus(){
                uint32_t one_frame_now = 0;
                static uint32_t one_frame_ago = 0;

                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))     one_frame_now |= ob2d::hid::mouse::button::MOUSE_RIGHT;
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))      one_frame_now |= ob2d::hid::mouse::button::MOUSE_LEFT;
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))    one_frame_now |= ob2d::hid::mouse::button::MOUSE_WHEEL;
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Extra1))    one_frame_now |= ob2d::hid::mouse::button::MOUSE_BUTTON_1;
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Extra2))    one_frame_now |= ob2d::hid::mouse::button::MOUSE_BUTTON_2;

                ob2d::hid::mouse::mouse_trigger = ~one_frame_ago & one_frame_now;
                ob2d::hid::mouse::mouse_release = one_frame_ago & ~one_frame_now;
                ob2d::hid::mouse::mouse_hold    = one_frame_now;

                one_frame_ago = one_frame_now;
            }
}