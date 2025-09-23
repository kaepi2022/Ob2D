/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObSE.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <cmath>

#include "Sounds.hpp"

namespace ob2d::sounds{
            /**
             * @brief コンストラクタ（ファイル読み込み付き）
             * @param sound_path SEファイルのパス
             */
            ob2d::sounds::ObSE::ObSE(const std::string& sound_path,bool auto_play) : sound(soundbuf_){
                if(auto_play) sound.play();
                bool test = soundbuf_.loadFromFile(sound_path);
            }

            /**
             * @brief 効果音ファイルを読み込む
             * @param sound_path ファイルパス
             * @return ログコード（成功 / 失敗）
             */
            bool ob2d::sounds::ObSE::setSound(const std::string& sound_path) {
                if (soundbuf_.loadFromFile(sound_path)) {
                    sound.setBuffer(soundbuf_);
                    return true;
                } else {
                    return false;
                }
            }

            /**
             * @brief 音量を設定（0〜100）
             * @param volume 音量値
             */
            void ob2d::sounds::ObSE::setVolume(float volume) {
                sound.setVolume(volume);
            }

            /**
             * @brief ピッチを設定（再生速度にも影響）
             * @param pitch 倍率（1.0が通常）
             */
            void ob2d::sounds::ObSE::setPitch(float pitch) {
                sound.setPitch(pitch);
            }

            /// @brief 効果音を再生（非同期再生）
            void ob2d::sounds::ObSE::play() {
                sound.play();
            }
}