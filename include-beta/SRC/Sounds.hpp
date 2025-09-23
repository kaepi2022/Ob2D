/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObMic.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#ifndef SOUNDS_HPP
#define SOUNDS_HPP

#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <cmath>

#include "System.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "Object.hpp"

namespace ob2d {

    namespace sounds {
        class ObMic{
            private:
                sf::SoundBufferRecorder recoder;
                std::unique_ptr<sf::Sound>  record_sound; //コンストラクタが～とかうっさいのでスマポを使用
                sf::SoundBuffer             sound_buf;
                bool is_recording;
                bool has_record_data;

            public:

                float getVoiceSimilarity(const std::string &sound_file,int tolerance);

                void startRecord();

                void stopRecord();

                ObMic(bool auto_record = false);

                bool saveToFile(const std::string &file_path);

                float getRecordlen(ob2d::time::Timetypes time_types);

                float getDB();                

                void play();

                void setPitch(float pitch);

                bool is_Recoding() const;

                bool has_RecordData() const;

        };

        class ObMusic {
            private:
                sf::Music music_;

            public:
                ObMusic(const std::string& music_path,bool auto_play = false);
                bool setMusic(const std::string& music_path);
                void play();
                void pause();
                void stop();
                bool is_Playing() const;
                bool is_Stopped() const;
                bool is_Paused()  const;
                void setMusicPosition(float seconds_time);
                void setLoop(bool flag);
                void setLoopPoint(float start_seconds, float end_seconds);
                void setVolume(float volume);
                void setPitch(float pitch);

        };

        class ObSE {
            private:
                sf::SoundBuffer soundbuf_;
                sf::Sound sound;

            public:
                ObSE(const std::string& sound_path,bool auto_play = false);

                bool setSound(const std::string& sound_path);

                void setVolume(float volume);

                void setPitch(float pitch);

                void play();

        };

        //マイクの録音をサポートしているか
        bool is_SupportMic();

        void setGrobalSoundVolume(float volume);

        float getGrobalSoundVolume();
    }
} 


#endif