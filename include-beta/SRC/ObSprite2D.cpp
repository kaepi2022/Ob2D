/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObSprite2D.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <map>
#include <vector>
#include <variant>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "System.hpp"
#include "Object.hpp"

namespace ob2d::object{
    bool ob2d::object::ObSprite2D::setTexture(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr){
        if(!object) return false;     //クラッシュ対策

                    
        if(std::holds_alternative<ObTexture*>(texture_path_or_texture_ptr)){
            mode_ObTexture = true;        //ObTextureモード^^!
            mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;

            if(!std::get<ObTexture*>(texture_path_or_texture_ptr))  return false; //エラコ
            auto obj_texture = std::get<ObTexture*>(texture_path_or_texture_ptr)->sf_getTexture();
                        
            object.get()->setTextureRect({{0,0},{static_cast<int>(obj_texture->getSize().x),static_cast<int>(obj_texture->getSize().y)}});
            object.get()->setTexture(obj_texture);        //テクスチャをセット

            if(animation_clock != nullptr) animation_clock = nullptr;
        }
        else if(std::holds_alternative<ObAnimation*>(texture_path_or_texture_ptr)){
            mode_ObAnimation = true;        //ObAnimationモード> ~!
            mode_ObTexture = false; obj_texture = nullptr;

            if(!std::get<ObAnimation*>(texture_path_or_texture_ptr))  return false; //エラコ
            obj_animation_box = std::get<ObAnimation*>(texture_path_or_texture_ptr);
            if(obj_animation_box->has_SpriteSheet()) object.get()->setTexture(obj_animation_box->dt_getSpriteSheet()->sf_getTexture());
                        
            auto init_textutre = *(obj_animation_box->dt_getFirst_data()->begin());

            if(std::holds_alternative<ObTexture*>(init_textutre)){
                auto ob_texture = std::get<ObTexture*>(init_textutre)->sf_getTexture();
                object.get()->setTexture(ob_texture);
                object.get()->setTextureRect({{0,0},{static_cast<int>(ob_texture->getSize().x),static_cast<int>(ob_texture->getSize().y)}});
            }
            else if(std::holds_alternative<ObFrameRect>(init_textutre)){
                object.get()->setTextureRect(std::get<ObFrameRect>(init_textutre).sf_getIntRect());
            }

            if(!animation_clock) animation_clock = std::make_unique<sf::Clock>();
        }
        else if(std::holds_alternative<std::nullptr_t>(texture_path_or_texture_ptr)){
            mode_ObTexture   = false; obj_texture = nullptr;
            mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;
            if(animation_clock != nullptr) animation_clock = nullptr;

            object.get()->setTexture(nullptr);
        }

        if(is_fadeing_in){
            fade_sum_delta++;       //計算回数を加算
            setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

            if(fade_count <= fade_sum_delta){
                fade_sum_delta = 0;         //初期化の値そのものにする。
                is_fadeing_in = false;      //フェードイン処理完了！
                fade_count    = 0;          //初期化
                fade_delta    = {0,0};      //増加量の初期化
            }
        }

        if(is_fadeing_out){
            fade_sum_delta++;       //計算回数を加算
            setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

            if(fade_count <= fade_sum_delta){
                fade_sum_delta = 0;         //初期化の値そのものにする。
                is_fadeing_out = false;      //フェードアウト処理完了！
                fade_count    = 0;          //初期化
                fade_delta    = {0,0};      //増加量の初期化
                fade_color    = {0,0};
                setUnVisible(true);         //非表示に 
                }
        }

        return true;

    }

    void ob2d::object::ObSprite2D::dt_getMemory(){
        if(!object){
            object = std::make_unique<sf::RectangleShape>();

            //コンストラクター
            setSize(init_size);
            setPosition(init_pos);
            object.get()->setFillColor(init_color);
            setTexture(init_texture_data);

            is_active_object = true;        //オブジェクトを生存状態にする。
        }
    }

    void ob2d::object::ObSprite2D::dt_removeMemory(){
        if(object){
            object = nullptr;

            now_scale = {1,1};              //初期化

            //テクスチャ系を直々に開放
            mode_ObTexture   = false; obj_texture = nullptr;
            mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;
            if(animation_clock) animation_clock = nullptr;  //強制退去

            is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。
        }
    }

    void ob2d::object::ObSprite2D::dt_update(){
        if(!object) return;

        //アップデート関数の実行
        if((user_update_event != nullptr) && enable_update_function) user_update_event();

        //カスタム原点に変更されたなら中心に戻す。
        if(seted_custom_origin){
            auto object_size = object.get()->getSize();
            object.get()->setOrigin({object_size.x/2,object_size.y/2});
            seted_custom_origin = false;
        }

        if(is_collision_obj){
            if (col_isConflicting_fixed(ObDirection::DOWN)) object.get()->move({0,-(OverLap_d)});
            if (col_isConflicting_fixed(ObDirection::UP)) object.get()->move({0,(OverLap_u)});
            if (col_isConflicting_fixed(ObDirection::RIGHT)) object.get()->move({-(OverLap_r),0});
            if (col_isConflicting_fixed(ObDirection::LEFT)) object.get()->move({(OverLap_l),0});
        }

        if(mode_ObAnimation && animation_clock != nullptr){
            if(mode_animation_is_playing && !mode_animation_is_stoped){
            //FPSに応じて何回めの更新かを計算する。
            int animation_current_frame = animation_clock->getElapsedTime().asMilliseconds() / (1000 / mode_animation_FPS);
            if(mode_animation_kaisuu_loop) mode_animation_loop_i++;

            mode_animation_setting_frame =  animation_current_frame % mode_animation_frame_count;
            auto frame    = (obj_now_animation->begin() + mode_animation_setting_frame);


            if(std::holds_alternative<ObTexture*>(*frame)){
                auto texture = std::get<ObTexture*>(*frame)->sf_getTexture();

                object.get()->setTextureRect({{0,0},{static_cast<int>(texture->getSize().x),static_cast<int>(texture->getSize().y)}});
                if(object.get()->getTexture() != texture) object.get()->setTexture(std::get<ObTexture*>(*frame)->sf_getTexture());

            }
            else if(std::holds_alternative<ObFrameRect>(*frame)){
                object.get()->setTexture(obj_animation_box->dt_getSpriteSheet()->sf_getTexture());
                object.get()->setTextureRect(std::get<ObFrameRect>(*frame).sf_getIntRect());
            }
                            
            if(mode_animation_setting_frame == 0 && mode_animation_kaisuu_loop && mode_animation_loop_count <= mode_animation_loop_i){
                mode_animation_is_playing = false;
                mode_animation_done_play = true;
                animation_clock.get()->reset();
                mode_animation_kaisuu_loop = false;
                mode_animation_loop_i = 0;
                mode_animation_FPS    = 0;
                }
            }
        }

        if(is_fadeing_in){
            fade_sum_delta++;       //計算回数を加算
            setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

            if(fade_count <= fade_sum_delta){
                fade_sum_delta = 0;         //初期化の値そのものにする。
                is_fadeing_in = false;      //フェードイン処理完了！
                fade_count    = 0;          //初期化
                fade_delta    = {0,0};      //増加量の初期化
            }
        }

        if(is_fadeing_out){
            fade_sum_delta++;       //計算回数を加算
            setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

            if(fade_count <= fade_sum_delta){
                fade_sum_delta = 0;         //初期化の値そのものにする。
                is_fadeing_out = false;      //フェードアウト処理完了！
                fade_count    = 0;          //初期化
                fade_delta    = {0,0};      //増加量の初期化
                fade_color    = {0,0};
                setUnVisible(true);         //非表示に 
            }
        }

        //非表示の場合は表示しない。
        if(!is_unvisible) ob2d::window::sf_getWindow().draw( *object.get() );
    }

    ob2d::object::ObSprite2D::ObSprite2D(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr,ob2d::Vector2T<float> size,ob2d::Vector2T<float> position, ob2d::color::Colors color) : ObRect2D(size,position,color){
        init_texture_data = texture_path_or_texture_ptr;
    }

    ob2d::object::ObSprite2D::~ObSprite2D(){
        *(is_died.get()) = true;
        if(is_collision_obj) disableCollision();
        dt_removeMemory();
    }

    void ob2d::object::ObSprite2D::flipTexture_x(bool flag) {
        //クラッシュ対策
        if(!object) return;

        //原点が中央でなければ
        if(seted_custom_origin){
            auto size = object.get()->getSize();
            object.get()->setOrigin({size.x / 2.f,size.y / 2.f});
        }

        if (!is_flip_x && flag) {
            is_flip_x = true;
            object.get()->scale({-1, 1});
        }
        else if (is_flip_x && !flag) {
            is_flip_x = false;
            object.get()->scale({-1, 1});
        }
    }

    void ob2d::object::ObSprite2D::flipTexture_y(bool flag) {
        //クラッシュ対策
        if(!object) return;

        //原点が中央でなければ
        if(seted_custom_origin){
            auto size = object.get()->getSize();
            object.get()->setOrigin({size.x / 2.f,size.y / 2.f});
        }

        if (!is_flip_y && flag) {
            is_flip_y = true;
            object.get()->scale({1, -1});
        }
        else if (is_flip_y && !flag) {
            is_flip_y = false;
            object.get()->scale({1, -1});
        }
    }

    void ob2d::object::ObSprite2D::anime_setAnimarion(const std::string animation_name){
        if(!object || !animation_clock || !mode_ObAnimation) return;     //オブジェクトが無効かまたはアニメーションモードではなければ終了。
        if(!obj_animation_box->find_animation(animation_name)) return; //アニメーションがなければ終了。

        auto set_animation = obj_animation_box->dt_getAnimation_data(animation_name);
        obj_now_animation = set_animation;                          //アニメーション装着
        mode_animation_frame_count = set_animation->size();         //アニメーションの数をカウント
        animation_clock.get()->reset();
    }

    void ob2d::object::ObSprite2D::anime_playAnimation(uint FPS,bool loop,uint loop_count){
        if(!object || !animation_clock || !mode_ObAnimation) return;     //オブジェクトが無効かまたはアニメーションモードではなければ終了。
        mode_animation_is_playing = true;                           //再生判定
        mode_animation_is_stoped  = false;
        mode_animation_loop       = loop;                           //アニメーションループをするかどうか
        mode_animation_loop_count = loop ? 256 : std::clamp(loop_count,1u,255u);       //ループは最大255回まで
        mode_animation_kaisuu_loop = (loop_count <= 255);
        mode_animation_FPS = static_cast<int>(FPS);
        animation_clock.get()->start();
    }

    void ob2d::object::ObSprite2D::anime_pauseAnimation(){
        if(!object || !animation_clock || !mode_ObAnimation) return;
        mode_animation_is_playing = false;
        mode_animation_is_stoped  = true;
        animation_clock.get()->stop();
    }

    bool ob2d::object::ObSprite2D::anime_is_playing_animation(){
        return mode_animation_is_playing && mode_ObAnimation;
    }

    bool ob2d::object::ObSprite2D::anime_is_pausing_animation(){
        return mode_animation_is_stoped && mode_ObAnimation;
    }

    bool ob2d::object::ObSprite2D::anime_is_play_done_animation(){
        return mode_animation_done_play;
    }

    uint ob2d::object::ObSprite2D::anime_getAnimation_frame(){
        return mode_ObAnimation ? mode_animation_setting_frame : 0u;
    } 
}