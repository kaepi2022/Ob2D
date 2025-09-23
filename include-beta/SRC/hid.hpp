#ifndef HID_HPP
#define HID_HPP

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


namespace ob2d{


    namespace hid{

        namespace detail{

            struct ControllerStickPosition{
                Vector2T<int> L_Stick;
                Vector2T<int> R_Stick;
            };

        };

        namespace keyboard {

            // 現在のキー入力状態をビット列で管理
            extern uint64_t key_trigger ;  ///< このフレームで押されたキー
            extern uint64_t key_release ;  ///< このフレームで離されたキー
            extern uint64_t key_hold    ;  ///< 押しっぱなしのキー（継続）

            namespace key {
                // 各キーに対応するビットマスク定義（論理演算で使用）
                constexpr uint32_t KEY_UP           = 1<<0;
                constexpr uint32_t KEY_DOWN         = 1<<1;
                constexpr uint32_t KEY_RIGHT        = 1<<2;
                constexpr uint32_t KEY_LEFT         = 1<<3;

                constexpr  uint32_t KEY_W        = 1<<4;
                constexpr  uint32_t KEY_S        = 1<<5;
                constexpr  uint32_t KEY_A        = 1<<6;
                constexpr  uint32_t KEY_D        = 1<<7;

                constexpr  uint32_t KEY_SPACE    = 1<<8;
                constexpr  uint32_t KEY_LSHIFT   = 1<<9;
                constexpr  uint32_t KEY_RSHIFT   = 1<<10;
                constexpr  uint32_t KEY_ESCAPE   = 1<<11;
                constexpr  uint32_t KEY_ENTER    = 1<<12;
                constexpr  uint32_t KEY_BACKSPACE  = 1<<13;
                constexpr  uint32_t KEY_Z        = 1<<14;
                constexpr  uint32_t KEY_X        = 1<<15;

                constexpr  uint32_t KEY_1 = 1<<16;
                constexpr  uint32_t KEY_2 = 1<<17;
                constexpr  uint32_t KEY_3 = 1<<18;
                constexpr  uint32_t KEY_4 = 1<<19;
                constexpr  uint32_t KEY_5 = 1<<20;
                constexpr  uint32_t KEY_6 = 1<<21;
                constexpr  uint32_t KEY_7 = 1<<22;
                constexpr  uint32_t KEY_8 = 1<<23;
                constexpr  uint32_t KEY_9 = 1<<24;
                constexpr  uint32_t KEY_0 = 1<<25;
            };

            bool pushKeyTrigger(uint32_t KEY);
            bool pushKeyRelease(uint32_t KEY);
            bool pushKeyHold(uint32_t KEY);
        };
      
        namespace mouse {
            namespace detail{
                extern ob2d::Vector2T<float> mouse_wheel_delta_now;
                extern ob2d::Vector2T<float> mouse_wheel_pos_bef;
                extern ob2d::Vector2T<float>        cursor_hotspot;
            }

            // マウスボタンの状態をビット列で管理
            extern uint32_t mouse_trigger ;  ///< このフレームで押されたボタン
            extern uint32_t mouse_release ;  ///< このフレームで離されたボタン
            extern uint32_t mouse_hold    ;  ///< 押しっぱなしのボタン

            /// @brief マウスボタンの識別用ビットマスク
            namespace button {
                constexpr uint8_t MOUSE_RIGHT     = 1 << 0;  ///< 右クリック
                constexpr uint8_t MOUSE_LEFT      = 1 << 1;  ///< 左クリック
                constexpr uint8_t MOUSE_WHEEL     = 1 << 2;  ///< ホイールクリック
                constexpr uint8_t MOUSE_BUTTON_1  = 1 << 3;  ///< 拡張ボタン1
                constexpr uint8_t MOUSE_BUTTON_2  = 1 << 4;  ///< 拡張ボタン2
            };


            ob2d::Vector2T<int> getCursorPosition(ob2d::scene::ObLayer layer_id);
            ob2d::Vector2T<float> getCursorHotspot();
            ob2d::Vector2T<float> getWheelDelta();
            void setCursorPosition(Vector2T<int> position);
            void setVisibleCursor(bool flag);
            bool is_VisibleCursor();
            bool pushButtonTrigger(uint8_t MOUSE_BUTTON);
            bool pushButtonRelease(uint8_t MOUSE_BUTTON);
            bool pushButtonHold(uint8_t MOUSE_BUTTON);


        };

        namespace detail{

            void UpdateKeyStatus();

            void UpdateMouseStatus();



        };

        namespace controller{
        namespace Buttons{
            constexpr uint16_t BUTTON_A = (1 << 0);
            constexpr uint16_t BUTTON_B = (1 << 1);
            constexpr uint16_t BUTTON_X = (1 << 2);
            constexpr uint16_t BUTTON_Y = (1 << 3);

            constexpr uint16_t BUTTON_L  = (1 << 4);
            constexpr uint16_t BUTTON_R  = (1 << 5);
            constexpr uint16_t BUTTON_ZL = (1 << 6);
            constexpr uint16_t BUTTON_ZR = (1 << 7);

            constexpr uint16_t BUTTON_UP      = (1 << 8);
            constexpr uint16_t BUTTON_DOWN    = (1 << 9);
            constexpr uint16_t BUTTON_LEFT    = (1 << 10);
            constexpr uint16_t BUTTON_RIGHT   = (1 << 11);

            constexpr uint16_t BUTTON_START    = (1 << 12);
            constexpr uint16_t BUTTON_SERECT   = (1 << 13);

            constexpr uint16_t BUTTON_STICK_L   = (1 << 14);
            constexpr uint16_t BUTTON_STICK_R   = (1 << 15);
        };

        enum class PlayerID{
            PLAYER_1,
            PLAYER_2,
            PLAYER_3,
            PLAYER_4            
        };

        enum class StickType{
            STICK_TYPE_A = static_cast<int>(sf::Joystick::Axis::X),
            STICK_TYPE_B = static_cast<int>(sf::Joystick::Axis::Y),
            STICK_TYPE_C = static_cast<int>(sf::Joystick::Axis::Z),
            STICK_TYPE_D = static_cast<int>(sf::Joystick::Axis::R),
            STICK_TYPE_E = static_cast<int>(sf::Joystick::Axis::U),
            STICK_TYPE_F = static_cast<int>(sf::Joystick::Axis::V)
        };

        extern const std::string nulldevice;

        class Player{
            static constexpr uint8_t nullController = 15;   //プレイヤー外

            private:
                uint8_t SFML_buttonID = nullController;
                uint16_t now_button_status;
                uint16_t bef_button_status;

                //std::arrayは0がL、1がRとする。
                std::array<Vector2T<sf::Joystick::Axis>,2> stick_status = {Vector2T<sf::Joystick::Axis>(static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_A),static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_B)),
                                                                            Vector2T<sf::Joystick::Axis>(static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_C),static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_D))
                                                                           };

                std::array<sf::Joystick::Axis,2> ZLR_trigger_status = {static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_E),static_cast<sf::Joystick::Axis>(ob2d::hid::controller::StickType::STICK_TYPE_F)};

                std::map<uint16_t,uint8_t> key_map = {
                    {Buttons::BUTTON_A,2},
                    {Buttons::BUTTON_B,1},
                    {Buttons::BUTTON_X,3},
                    {Buttons::BUTTON_Y,0},
                    {Buttons::BUTTON_L,4},
                    {Buttons::BUTTON_R,5},
                    {Buttons::BUTTON_ZL,6},
                    {Buttons::BUTTON_ZR,7},
                    {Buttons::BUTTON_START,0},
                    {Buttons::BUTTON_SERECT,0},
                    {Buttons::BUTTON_STICK_L,0},
                    {Buttons::BUTTON_STICK_R,0}
                }; 

            public:
                uint16_t button_trigger;
                uint16_t button_release;
                uint16_t button_hold;

                float ZL_pressure;
                float ZR_pressure;


                Vector2T<float> stick_L_position;
                Vector2T<float> stick_R_position;

                Player();
                ~Player();

                bool is_ActiveController();
                bool disconnectController();

                void dt_update();
                uint8_t* dt_getdata();

                void setStickTypes(ob2d::Vector2T<ob2d::hid::controller::StickType> L_stick,ob2d::Vector2T<ob2d::hid::controller::StickType> R_stick);
                void setTriggerPressureType(ob2d::hid::controller::StickType BUTTON_ZL,ob2d::hid::controller::StickType BUTTON_ZR);

                std::map<uint16_t,uint8_t>& getKeyMap();

                std::string getControllerDeviceName();
        };

        namespace detail{
            extern std::array<bool,8> player_sfml_id_used_status;
            extern std::array<Player*,8> players_list;
            extern Player* wait_player;
            extern bool is_waiting_join_player;
        }

        void setPlayer(ob2d::hid::controller::Player &target_player);
    
    }
    };
};


#endif