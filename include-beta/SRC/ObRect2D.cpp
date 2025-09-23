/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObRect2D.cpp

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

#include "System.hpp"
#include "Object.hpp"


namespace ob2d::object{

    void ob2d::object::ObRect2D::disableCollision(){
        if(!is_collision_obj) return;

            if(is_playable_obj){
                detail::collision_playable_object.erase(std::find(detail::collision_playable_object.begin(),detail::collision_playable_object.end(),this));
            }
                    
            if(is_fixed_obj){
                detail::collision_object.erase(std::find(detail::collision_object.begin(),detail::collision_object.end(),this));
            }

            is_collision_obj = false;
            is_playable_obj   = false;
            is_fixed_obj      = false;
            is_GhostColision = false;
    }
                
    ob2d::object::ObRect2D::ObRect2D(ob2d::Vector2T<float> size, ob2d::Vector2T<float> position, ob2d::color::Colors color){
                    is_died = std::make_shared<bool>(false);
                    init_pos    = position;
                    init_size   = size;
                    init_color  = color;
    };

    ob2d::object::ObRect2D::~ObRect2D(){
        *(is_died.get()) = true;
        if(is_collision_obj) disableCollision();
        dt_removeMemory();

    }

    void ob2d::object::ObRect2D::dt_getMemory(){
        if(!object){
            object = std::make_unique<sf::RectangleShape>();

            //コンストラクター
            setSize(init_size);
            setPosition(init_pos);
            object.get()->setFillColor(init_color);


            is_active_object = true;        //オブジェクトを生存状態にする。

        }
    }

    void ob2d::object::ObRect2D::dt_removeMemory(){
        if(object){
            object = nullptr;

            now_scale = {1,1};              //初期化

            is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。

        }
    }

    void ob2d::object::ObRect2D::dt_update(){
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

    //オブジェクト操作関数▼
    void ob2d::object::ObRect2D::setScale(ob2d::Vector2T<float> scale){
        //クラッシュ対策
        if(!object) return;
        sf::Vector2f obj_size = object.get()->getSize();
                    
        obj_size = object.get()->getSize();
        object.get()->setSize({obj_size.x*scale.x,obj_size.y*scale.y});                           //スケールを設定する。

                    //原点が中央でなければ
        if(seted_custom_origin){
            auto size = object.get()->getSize();
            object.get()->setOrigin({size.x / 2.f,size.x / 2.f});
        }

    }

    void ob2d::object::ObRect2D::setSize(ob2d::Vector2T<float> size){
        if(!object) return;                    //クラッシュ対策

        object.get()->setSize(size.sf_getVector());
        object.get()->setOrigin({size.x / 2.f, size.y / 2.f});
    }

    void ob2d::object::ObRect2D::setPosition(ob2d::Vector2T<float> pos){
        if(!object) return;                    //クラッシュ対策
        auto obj_size = object.get()->getSize();
        object.get()->setPosition({pos.x + obj_size.x / 2.f,pos.y + obj_size.y / 2.f});
    }

    void ob2d::object::ObRect2D::move(ob2d::Vector2T<float> velocity){
        if(!object) return;                    //クラッシュ対策
        if (is_collision_obj) {
            if (col_isConflicting_fixed(ObDirection::RIGHT) && velocity.x > 0 && OverLap_r <= 0.2 * std::abs(velocity.x)) {
                object.get()->move({-(OverLap_r), 0});
                velocity.x = 0;
            }
            if (col_isConflicting_fixed(ObDirection::LEFT) && velocity.x < 0 && OverLap_l <= 0.2 * std::abs(velocity.x)) {
                object.get()->move({(OverLap_l), 0});
                velocity.x = 0;
            }
            if (col_isConflicting_fixed(ObDirection::DOWN) && velocity.y > 0 && OverLap_d <= 0.2 * std::abs(velocity.y)) {
                object.get()->move({0, -(OverLap_d)});
                velocity.y = 0;

            }
            if (col_isConflicting_fixed(ObDirection::UP) && velocity.y < 0 && OverLap_u <= 0.2 * std::abs(velocity.y)) {
                object.get()->move({0, (OverLap_u)});
                velocity.y = 0;
            }

        }
                    
        object.get()->move({velocity.x, velocity.y});
    }

    void ob2d::object::ObRect2D::setColor(ob2d::color::Colors color){
        if(!object) return;                    //クラッシュ対策

        object.get()->setFillColor(color);
    }

    void ob2d::object::ObRect2D::setBorder(ob2d::color::Colors color,float size){
        if(!object) return;                    //クラッシュ対策
        object.get()->setOutlineThickness(size);
        object.get()->setOutlineColor(color);
    }

    void ob2d::object::ObRect2D::setOrigin(uint8_t up_or_down, uint8_t left_or_right){
        //クラッシュ対策
        if(!object) return;

        if (up_or_down == ObDirection::UP) {
            if (left_or_right == ObDirection::LEFT) object.get()->setOrigin({0, 0});
            else if (left_or_right == ObDirection::RIGHT) object.get()->setOrigin({object.get()->getSize().x, 0});
            seted_custom_origin = true;        //任意の原点に設定されたので、true
        } 
        else if (up_or_down == ObDirection::DOWN) {
            if (left_or_right == ObDirection::LEFT)       object.get()->setOrigin({0, object.get()->getSize().y});
            else if (left_or_right == ObDirection::RIGHT) object.get()->setOrigin({object.get()->getSize().x, object.get()->getSize().y});
            seted_custom_origin = true;        //任意の原点に設定されたので、true
        }
    }

    void ob2d::object::ObRect2D::setAlpha(uint alpha_body,bool with_border,uint alpha_border){
        if(!object) return;                    //クラッシュ対策

        uint level_body = std::clamp(alpha_body,0u,255u);   //0<= x <= 255 　に収める
        uint level_border = with_border ? std::clamp(alpha_body,0u,255u) : std::clamp(alpha_border,0u,255u);   //0<= x <= 255 　に収める
                    
        auto color = object.get()->getFillColor();
        auto b_color = object.get()->getOutlineColor();
        color.a = level_body;
        b_color.a = level_border;

        object.get()->setOutlineColor(b_color);
        object.get()->setFillColor(color);
    }

                //x = オブジェクトそのものの透明度、y = 縁の透明度
    ob2d::Vector2T<uint> ob2d::object::ObRect2D::getAlpha(){
        if(!object) return {0u,0u};
        return {object.get()->getFillColor().a,object.get()->getOutlineColor().a};
    }
                
    ob2d::Vector2T<ob2d::color::Colors> ob2d::object::ObRect2D::getColor(){
        if(!object) return {ob2d::color::Colors(255,255,255),ob2d::color::Colors(255,255,255)};

        return {object.get()->getFillColor(),object.get()->getOutlineColor()};
    }

    void ob2d::object::ObRect2D::setUnVisible(bool flag){
        if(!object) return;

            is_unvisible = flag;
            auto obj_color_body     = object.get()->getFillColor();
            auto obj_color_border   = object.get()->getOutlineColor();

            switch (flag){
                case true:
                    object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,0u});
                    object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,0u});
                    break;
                case false:
                    object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,255u});
                    object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,255u});
                    break;
            }

    }

    void ob2d::object::ObRect2D::setRote(bool flag , float angle){
        if(!object) return;
        if(flag){
            object.get()->setRotation(sf::degrees(angle));
            if(is_collision_obj) col_setGhost(true);
        }
        else{
            object.get()->setRotation(sf::degrees(0.f));
            if(is_collision_obj) col_setGhost(false); 
            } 
    }

                
    float ob2d::object::ObRect2D::getRoteAngle(){
        if(!object) return 0.f;
        return object.get()->getRotation().asDegrees();
    }

    void ob2d::object::ObRect2D::startFadeIn(float prosess_time,uint alpha_body,uint alpha_border){
        if(!object) return;            
        if(!(is_fadeing_in || is_fadeing_out) && is_unvisible){
            setUnVisible(false);
            is_fadeing_in = true;
            fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
            fade_delta      = {static_cast<float>(alpha_body) / fade_count ,static_cast<float>(alpha_border) / fade_count};

        }
    }

    void ob2d::object::ObRect2D::startFadeOut(float prosess_time){
        if(!object) return;
        if(!(is_fadeing_in || is_fadeing_out) && !is_unvisible){
            is_fadeing_out = true;
            fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
            fade_color = {getAlpha().x,getAlpha().y};
            fade_delta = {static_cast<float>(fade_color.x) / fade_count ,static_cast<float>(fade_color.y) / fade_count};
        }
    }

    ob2d::ObStatus<float> ob2d::object::ObRect2D::getStatus(){
        if(!object) return {{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f}};

        auto object_pos = object.get()->getPosition();
        auto object_size = object.get()->getGlobalBounds().size;
        ob2d::Vector2T<float> obj_size = {object_size.x,object_size.y};

        return {
            obj_size,
                {object_pos.x ,object_pos.y},
                {object_pos.x - obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                {object_pos.x - obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                {object_pos.x + obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                {object_pos.x + obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f}
                                
            };
    }

                //当たり判定関連▼
    void ob2d::object::ObRect2D::enableCollision_Playable(){
    //当たり判定が無効ならば当たり判定を有効にする
        is_collision_obj = true;
        is_playable_obj   = true;

        object::detail::collision_playable_object.push_back(this);
    }

    void ob2d::object::ObRect2D::enableCollision_Fixed(){
        //当たり判定が無効ならば当たり判定を有効にする
        is_collision_obj = true;
        is_playable_obj   = false;

        object::detail::collision_object.push_back(this);
    }
        
}//end ob2d::object::ObRect2D