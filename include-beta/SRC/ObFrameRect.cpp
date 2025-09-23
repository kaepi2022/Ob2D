/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObFrameRect.cpp

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
    ob2d::object::ObFrameRect::ObFrameRect(ob2d::Vector2T<uint> cut_start_position,ob2d::Vector2T<uint> cut_size){
        cutter_texture.position.x = cut_start_position.x;
        cutter_texture.position.y = cut_start_position.y;
        cutter_texture.size.x     = cut_size.x;
        cutter_texture.size.y     = cut_size.y;
    }

    sf::IntRect ob2d::object::ObFrameRect::sf_getIntRect(){
        return cutter_texture;
    }
}