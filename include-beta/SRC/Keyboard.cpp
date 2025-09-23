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
#include "scene.hpp"
#include "Window.hpp"

#include "System.hpp"
#include "hid.hpp"

namespace ob2d::hid::keyboard{
// 現在のキー入力状態をビット列で管理

            // 現在のキー入力状態をビット列で管理
            uint64_t key_trigger ;  ///< このフレームで押されたキー
            uint64_t key_release ;  ///< このフレームで離されたキー
            uint64_t key_hold    ;  ///< 押しっぱなしのキー（継続）

            /// @brief 指定されたキーが「このフレームで押されたか」を判定します。
            /// @note 内部的にはビットマスクを使って高速に処理されています。
            /// @warning パフォーマンス重視の場合は直接 `(key_trigger & KEY)` を使うのが望ましいです。
            bool pushKeyTrigger(uint32_t KEY) {
                return (KEY & key_trigger) != 0;
            }

            /// @brief 指定されたキーが「このフレームで離されたか」を判定します。
            bool pushKeyRelease(uint32_t KEY) {
                return (KEY & key_release) != 0;
            }

            /// @brief 指定されたキーが「現在押され続けているか」を判定します。
            bool pushKeyHold(uint32_t KEY) {
                return (KEY & key_hold) != 0;
            }
        }

    namespace ob2d::hid::detail{
    void UpdateKeyStatus(){
                uint32_t one_frame_now = 0;
                static uint32_t one_frame_ago = 0;

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))   one_frame_now |= ob2d::hid::keyboard::key::KEY_UP;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_DOWN;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_RIGHT;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_LEFT;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_W;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_S;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_A;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_D;

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_SPACE;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_LSHIFT;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_RSHIFT;

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_Z;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_X;

                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_ESCAPE;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_BACKSPACE;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_ENTER;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_1;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_2;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_3;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_4;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_5;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_6;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_7;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_8;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_9;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0))   one_frame_now |=  ob2d::hid::keyboard::key::KEY_0;


                 ob2d::hid::keyboard::key_trigger = ~one_frame_ago & one_frame_now;
                 ob2d::hid::keyboard::key_release = one_frame_ago  & ~one_frame_now;
                 ob2d::hid::keyboard::key_hold    = one_frame_now;

                one_frame_ago = one_frame_now;

            }
    }
