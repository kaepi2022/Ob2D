#ifndef OBSFML_HPP
#define OBSFML_HPP

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "System.hpp"
#include "Window.hpp"

namespace ob2d::sfml{
    void ob_drawWindow(const sf::Drawable &sfml_shape,ob2d::scene::ObLayer draw_layer, const sf::RenderStates &states = sf::RenderStates::Default){
        ob2d::window::sf_getWindow().clear(ob2d::color::color_usagi::Cocoa);
        ob2d::window::sf_getWindow().draw(sfml_shape,states);
        ob2d::window::sf_getWindow().display();
    }
}


#endif