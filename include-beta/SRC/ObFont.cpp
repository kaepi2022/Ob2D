/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObFont.cpp

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
#include "Object.hpp"

namespace ob2d::object{

    ob2d::object::ObFont::ObFont(const std::string &font_path){
        test = font_file.openFromFile(font_path);
    }

    const sf::Font &ob2d::object::ObFont::getSfFont() const{
        return font_file;
    }
}