/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObMusic.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <cmath>

#include "System.hpp"
#include "Sounds.hpp"

namespace ob2d::sounds{

    ob2d::sounds::ObMusic::ObMusic(const std::string& music_path,bool auto_play) {
        bool test = music_.openFromFile(music_path); // 無視してもOK
        if(auto_play) music_.play();
    }

                /**
                 * @brief 音楽ファイルを読み込む
                 * @param music_path 音楽ファイルのパス
                 * @return ログコード（成功 / 失敗）
                 */
                bool ob2d::sounds::ObMusic::setMusic(const std::string& music_path) {
                    return music_.openFromFile(music_path);
                }

                /// @brief 再生開始（停止中 or 一時停止中から）
                void ob2d::sounds::ObMusic::play()        { music_.play(); }

                /// @brief 一時停止
                void ob2d::sounds::ObMusic::pause()       { music_.pause(); }

                /// @brief 停止（再開できない）
                void ob2d::sounds::ObMusic::stop()        { music_.stop(); }

                /// @brief 再生中か確認
                bool ob2d::sounds::ObMusic::is_Playing() const { return music_.getStatus() == sf::SoundSource::Status::Playing; }

                /// @brief 停止中か確認
                bool ob2d::sounds::ObMusic::is_Stopped() const { return music_.getStatus() == sf::SoundSource::Status::Stopped; }

                /// @brief 一時停止中か確認
                bool ob2d::sounds::ObMusic::is_Paused()  const { return music_.getStatus() == sf::SoundSource::Status::Paused; }

                /**
                 * @brief 再生位置を設定（秒指定）
                 * @param seconds_time 開始位置（秒）
                 */
                void ob2d::sounds::ObMusic::setMusicPosition(float seconds_time) {
                    music_.setPlayingOffset(sf::seconds(seconds_time));
                }

                /// @brief ループ再生を有効化
                void ob2d::sounds::ObMusic::setLoop(bool flag) {
                    music_.setLooping(flag);
                }

                /**
                 * @brief ループ再生範囲を指定
                 * @param start_seconds 開始位置（秒）
                 * @param end_seconds 終了位置（秒）
                 */
                void ob2d::sounds::ObMusic::setLoopPoint(float start_seconds, float end_seconds) {
                    setLoop(true);
                    music_.setLoopPoints({ sf::seconds(start_seconds), sf::seconds(end_seconds) });
                }

                /**
                 * @brief 音量を設定
                 * @param volume 0〜100の範囲で指定
                 */
                void ob2d::sounds::ObMusic::setVolume(float volume) {
                    music_.setVolume(volume);
                }

                /**
                 * @brief ピッチを設定（再生速度にも影響）
                 * @param pitch 通常は 1.0（1.5で1.5倍速など）
                 */
                void ob2d::sounds::ObMusic::setPitch(float pitch) {
                    music_.setPitch(pitch);
                }
}