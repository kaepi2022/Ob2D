/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObText2D.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <vector>
#include <variant>
#include <memory>

#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "Object.hpp"



namespace ob2d::object{
    void ob2d::object::ObText2D::disableCollision(){
        if(!is_collision_obj) return;

        if(is_playable_obj){
            detail::collision_playable_object.erase(std::find(detail::collision_playable_object.begin(),detail::collision_playable_object.end(),this));
        }
        if(is_fixed_obj){
            detail::collision_object.erase(std::find(detail::collision_object.begin(),detail::collision_object.end(),this));
        }

        is_collision_obj = false;
        is_playable_obj   = false;
        is_GhostColision = false;
    }


    void ob2d::object::ObText2D::setOrigin(uint8_t oligin){
        auto text_size = object.get()->getLocalBounds().size;

        switch(oligin){
            case ObDirection::CENTER:
                now_ObDirection = ObDirection::CENTER;
                object.get()->setOrigin({text_size.x / 2.f,text_size.y / 2.f});
                break;
            case ObDirection::LEFT:
                now_ObDirection = ObDirection::LEFT;
                object.get()->setOrigin({0.f,text_size.y / 2.f});
                break;
            case ObDirection::RIGHT:
                now_ObDirection = ObDirection::RIGHT;
                object.get()->setOrigin({text_size.x,text_size.y / 2.f});
                break;
            default:
                break;
        }

        now_ObDirection = oligin;  

    }

    void ob2d::object::ObText2D::setText(const std::string &text){
        if(!object) return;
        object.get()->setString(sf::String::fromUtf8(text.begin(),text.end()));
        setOrigin(now_ObDirection);
    }

    void ob2d::object::ObText2D::setSize(uint size){
        if(!object) return;
        object.get()->setCharacterSize(size);
        setOrigin(now_ObDirection);
    }

    void ob2d::object::ObText2D::setFont(ob2d::object::ObFont *font){
        if(!object) return;
        object.get()->setFont(font->getSfFont());

        setOrigin(now_ObDirection);
    }

    void ob2d::object::ObText2D::setStyle(uint16_t style){
        if(!object) return;
        object.get()->setStyle(sf::Text::Regular);  //まずはリセットから

        if((style & ObTextStyle::default_style) > 0){
            object.get()->setStyle(sf::Text::Regular);
            return;
        }

        uint16_t style_set;
        if((style & ObTextStyle::bold_style) > 0) style_set |= sf::Text::Bold;
        if((style & ObTextStyle::under_line) > 0) style_set |= sf::Text::Underlined;
        if((style & ObTextStyle::italic_style) > 0) style_set |= sf::Text::Italic;
        object.get()->setStyle(style_set);
    }

            //Object2D派生

    ob2d::ObStatus<float> ob2d::object::ObText2D::getStatus(){
        if(!object) return {{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f}};

        auto object_pos = object.get()->getPosition();
        auto object_size = object.get()->getGlobalBounds().size;
        Vector2T<float> obj_size = {object_size.x,object_size.y};

        switch(now_ObDirection){
            case ObDirection::CENTER:
                            
                return {
                    {object_pos.x - obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                    {object_pos.x - obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                    {object_pos.x + obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                    {object_pos.x + obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                    obj_size
                };
            case ObDirection::LEFT:
                return {
                    {object_pos.x,object_pos.y},
                    {object_pos.x,object_pos.y + obj_size.y / 2.f},
                    {object_pos.x + obj_size.x,object_pos.y},
                    {object_pos.x + obj_size.x,object_pos.y + obj_size.y},
                    obj_size
                    };
            case ObDirection::RIGHT:
                return {
                    {object_pos.x - obj_size.x,object_pos.y},
                    {object_pos.x - obj_size.x,object_pos.y + obj_size.y},
                    {object_pos.x  ,object_pos.y},
                    {object_pos.x  ,object_pos.y + obj_size.y},
                    obj_size
                };
            default:
                return {
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0},
                    {0,0}
                };
        }

    }

    void ob2d::object::ObText2D::setPosition(ob2d::Vector2T<float> pos){
        if(!object) return;                    //クラッシュ対策
        Vector2T<float> moved_position;
        auto text_size = object.get()->getGlobalBounds().size;
        switch(now_ObDirection){
            case ObDirection::CENTER:
                object.get()->setPosition({pos.x + text_size.x / 2.f ,pos.y + text_size.y / 2.f});
                break;
            case ObDirection::LEFT:
                object.get()->setPosition({pos.x,pos.y + text_size.y / 2.f});
                break;
            case ObDirection::RIGHT:
                object.get()->setPosition({pos.x + text_size.x,pos.y + text_size.y / 2.f});
                break;
        }
    }

    void ob2d::object::ObText2D::move(ob2d::Vector2T<float> velocity){
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

    void ob2d::object::ObText2D::setColor(ob2d::color::Colors color){
        if(!object) return;
        object.get()->setFillColor(color);
    }

    void ob2d::object::ObText2D::setBorder(ob2d::color::Colors color,float size){
        if(!object) return;
        object.get()->setOutlineThickness(size);
        object.get()->setOutlineColor(color);
    }

    void ob2d::object::ObText2D::setUnVisible(bool flag){
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

    void ob2d::object::ObText2D::setRote(bool flag , float angle){
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
         
    float ob2d::object::ObText2D::getRoteAngle(){
        if(!object) return 0.f;
        return object.get()->getRotation().asDegrees();
    }

    void ob2d::object::ObText2D::setAlpha(uint alpha_body,bool with_border,uint alpha_border){
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
    ob2d::Vector2T<uint> ob2d::object::ObText2D::getAlpha(){
        if(!object) return {0u,0u};
        return {object.get()->getFillColor().a,object.get()->getOutlineColor().a};
    }
                
    ob2d::Vector2T<ob2d::color::Colors> ob2d::object::ObText2D::getColor(){
        if(!object) return {ob2d::color::Colors(255,255,255),ob2d::color::Colors(255,255,255)};

        return {object.get()->getFillColor(),object.get()->getOutlineColor()};
    }

    void ob2d::object::ObText2D::startFadeIn(float prosess_time,uint alpha_body,uint alpha_border){
        if(!object) return;
        if(!(is_fadeing_in || is_fadeing_out) && is_unvisible){
        setUnVisible(false);
        is_fadeing_in = true;
        fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
        fade_delta      = {static_cast<float>(alpha_body) / fade_count ,static_cast<float>(alpha_border) / fade_count};

        }
    }

    void ob2d::object::ObText2D::startFadeOut(float prosess_time) {
        if(!object) return;
        if(!(is_fadeing_in || is_fadeing_out) && !is_unvisible){
            is_fadeing_out = true;
            fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
            fade_color = {getAlpha().x,getAlpha().y};
            fade_delta = {static_cast<float>(fade_color.x) / fade_count ,static_cast<float>(fade_color.y) / fade_count};
        }
    }

    void ob2d::object::ObText2D::dt_update(){
        if(!object) return;


        //アップデート関数の実行
        if((user_update_event != nullptr) && enable_update_function) user_update_event();

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

    void ob2d::object::ObText2D::dt_getMemory(){
         if(!object){
             object = std::make_unique<sf::Text>(init_font->getSfFont());

            //コンストラクター
            setFont(init_font);
            setSize(init_text_size);
            setText(init_text);

            setOrigin(init_direction);
            setColor(init_text_color);
            setPosition(init_postion);
            setBorder(init_border_color,init_border_size);
            setStyle(init_style);

            is_active_object = true;        //オブジェクトを生存状態にする。
        }
    }

    void ob2d::object::ObText2D::dt_removeMemory(){
        if(object){
            object = nullptr;

            is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。
        }
    }

    ob2d::object::ObText2D::ObText2D(ob2d::object::ObFont *font,uint text_size,ob2d::color::Colors text_color,const std::string &text,ob2d::Vector2T<float> position,uint update_direction ,float border_size,ob2d::color::Colors border_color ,uint16_t style){
        is_died = std::make_shared<bool>();
        init_direction = update_direction;
        init_style = style;
        init_font = font;
        init_text_size = text_size;
        init_text_color = text_color;
        init_postion = position;
        init_text = text;
        init_border_size = border_size;
        init_border_color = border_color;

    }

    ob2d::object::ObText2D::~ObText2D(){
        *(is_died.get()) = true;
        if(is_collision_obj) disableCollision();
        dt_removeMemory();
    }

    //当たり判定関連▼
    void ob2d::object::ObText2D::enableCollision_Playable(){
        //当たり判定が無効ならば当たり判定を有効にする
        if(!is_collision_obj){
            is_collision_obj = true;
            is_playable_obj   = true;

            object::detail::collision_playable_object.push_back(this);
        }
    }

    void ob2d::object::ObText2D::enableCollision_Fixed(){
        //当たり判定が無効ならば当たり判定を有効にする
        if(!is_collision_obj){
            is_collision_obj = true;
            is_playable_obj   = false;

            object::detail::collision_object.push_back(this);
        }
    }

}