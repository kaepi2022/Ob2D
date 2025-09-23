/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObAnimation.cpp

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
    ob2d::object::ObTexture* ObAnimation::dt_getSpriteSheet(){
        return sprite_sheet;
    }

    std::vector<std::variant<ob2d::object::ObFrameRect,ObTexture*>>* ob2d::object::ObAnimation::dt_getAnimation_data(const std::string &animation_name){
        return &animation_box[animation_name];
    }

    std::vector<std::variant<ob2d::object::ObFrameRect,ob2d::object::ObTexture*>>* ob2d::object::ObAnimation::dt_getFirst_data(){
        return &(animation_box.begin()->second);
    }

    void ob2d::object::ObAnimation::addAnimation(const std::string &animation_name,std::vector<std::variant<ob2d::object::ObFrameRect,ob2d::object::ObTexture*>> animation_frame){
        if(animation_frame.empty()) return;        //アニメーションデータが空なら危険なので

            for(auto &it : animation_frame){
            //もし、FrameRectを持っていてスプライトシートがなければアニメーション追加をw禁止
                if(std::holds_alternative<ob2d::object::ObFrameRect>(it)) if(!has_sprite_sheet) return;
        }

        animation_box[ animation_name ] = animation_frame;
    }

    ob2d::object::ObAnimation::ObAnimation(std::map<std::string, std::vector<std::variant<ob2d::object::ObFrameRect,ob2d::object::ObTexture*>>> animation_data,ObTexture* sprite_sheet_image){
        if(!sprite_sheet){  
            sprite_sheet = sprite_sheet_image;
            has_sprite_sheet = true;
        }

        //アニメーション追加
        for(auto &it : animation_data){
            addAnimation(it.first,it.second);
        }
    }

    bool ob2d::object::ObAnimation::has_SpriteSheet(){
        return has_sprite_sheet;
    }

    bool ob2d::object::ObAnimation::find_animation(const std::string &animation_name){
        return animation_box.find(animation_name) != animation_box.end();
    }
}