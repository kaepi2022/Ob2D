/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObMic.cpp

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
    float ob2d::sounds::ObMic::getVoiceSimilarity(const std::string &sound_file,int tolerance){
        sf::SoundBuffer compare_sound;    //音声比較要ファイルこいつをベースに計算する
        //ファイルが読み込めなかったら強制終了
        if(is_recording && !has_record_data) return 0.f; 
        if(!compare_sound.loadFromFile(sound_file)) return 0.f;

        const int16_t *PCM1 = compare_sound.getSamples();
        const int16_t *PCM2 = sound_buf.getSamples();

        int pcm_count = static_cast<int>(std::min(sound_buf.getSampleCount(),compare_sound.getSampleCount()));

        int PCM1_MAX = 0;           //比較する音声ファイルの最大PCMを保存
        int PCM2_MAX = 0;           //マイク音声の最大PCMを保存

        //それぞれのPCM最大値を取得
        for(int i = 0; i < pcm_count; i++){
            if(!(i > pcm_count)) PCM1_MAX = std::max(PCM1_MAX,std::abs(PCM1[i]));
            if(!(i > pcm_count)) PCM2_MAX = std::max(PCM2_MAX,std::abs(PCM2[i]));
        }

        float PCM_scale = PCM2_MAX / PCM1_MAX;      //PCM2の比較時の音量調整に使用する。

        float best_similarity = 0.f;                //似てるPCMの数
        int max_shift = 200;                        //ズレの誤差補正用 (Ob2D既定値:200)

        //PCMを10戻して比較
        for (int shift = -max_shift; shift <= max_shift; shift += 10) {
            int match_count = 0;
            int compare_count = 0;

            for (int i = 0; i < pcm_count; i++) {
                int j = i + shift;                   
                if (j < 0 || j >= pcm_count) continue;  //jがオーバーロードしているなら処理スキップ

                int scaled_PCM2 = static_cast<int>(PCM2[j] / PCM_scale);        //PCM2 * (PCM1_MAX/PCM2_MAX)

                //
                if (std::abs(PCM1[i] - scaled_PCM2) <= tolerance) {
                    match_count++;
                }
                compare_count++;
            }

            if (compare_count > 0) {
                float similarity = (static_cast<float>(match_count) / compare_count) * 100.f;   //類似度の計算
                best_similarity = std::max(best_similarity, similarity);
            }
        }

        return best_similarity;

    }

    void ob2d::sounds::ObMic::startRecord(){
        if(!is_recording){
            bool dummy_flags = recoder.start();
            is_recording = true;        //録音のスタード
            has_record_data = false;    //録音データを破棄
        }
    }

    void ob2d::sounds::ObMic::stopRecord(){
        if(is_recording){
            recoder.stop();
            is_recording = false;       //録音を終了
            sound_buf = recoder.getBuffer();

            if(record_sound != nullptr) record_sound.get()->setBuffer(sound_buf);
            else if(record_sound == nullptr){
                record_sound = std::make_unique<sf::Sound>(sound_buf);
            } 

            has_record_data = true;     //録音データを保持
        }
    }


    ob2d::sounds::ObMic::ObMic(bool auto_record){
        if(auto_record){
            startRecord();
        } 
    }


    bool ob2d::sounds::ObMic::saveToFile(const std::string &file_path){
        if(!is_recording && has_record_data){
            return sound_buf.saveToFile(file_path);
        }

        return false;
    }

    float ob2d::sounds::ObMic::getRecordlen(ob2d::time::Timetypes time_types){
        if(!is_recording && has_record_data && record_sound){
            switch (time_types){
                case ob2d::time::Timetypes::MINUTES :
                    return sound_buf.getDuration().asSeconds() / 60.f;
                case ob2d::time::Timetypes::SECONDS :
                    return sound_buf.getDuration().asSeconds();
                case ob2d::time::Timetypes::MILLI_SECONDS :
                    return sound_buf.getDuration().asMilliseconds();         
            }
        }
                    
        return 0.f;
    }

    float ob2d::sounds::ObMic::getDB(){
        if(!is_recording && has_record_data && record_sound){
            int max_voice_PCM = 0;

            const int16_t* PCM = sound_buf.getSamples();

            int length = sound_buf.getSampleCount();   //Hz×音声の秒数から計算量の計算

                //ループで最大周波数の取得
                for(int i = 0; i < length; i++){
                    max_voice_PCM = std::max(max_voice_PCM,std::abs(PCM[i]));
                }

                //65536.fはint16_tから2^16を計算した値
                return 20 * std::log10( static_cast<double>(max_voice_PCM) / 65536.0);
            }

        return 0.f;
    }

    void ob2d::sounds::ObMic::play(){
        if(has_record_data && record_sound) record_sound.get()->play();
    }


    void ob2d::sounds::ObMic::setPitch(float pitch){
        if(has_record_data && record_sound) record_sound.get()->setPitch(pitch);
    }

    bool ob2d::sounds::ObMic::is_Recoding() const{
        return is_recording;
    }

    bool ob2d::sounds::ObMic::has_RecordData() const{
        return has_record_data;
    }
}