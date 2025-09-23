#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <atomic>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "System.hpp"
#include "Window.hpp"

#include "Scene.hpp"


namespace ob2d::scene::layer{

    void ob2d::scene::layer::ObLayer_camera::zoom(float lens) {
        if(ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
        scene.zoom(1.f / lens);
    }

    void ob2d::scene::layer::ObLayer_camera::moveCenter(Vector2T<float> velocity) {
        if(ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
        scene.move({velocity.x,velocity.y});
    }

    void ob2d::scene::layer::ObLayer_camera::setAngle(float angle) {
        if(ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
        scene.setRotation(sf::degrees(angle));
    }

    void ob2d::scene::layer::ObLayer_camera::setPosition(ob2d::Vector2T<float> position){
        if(ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
        auto render_size = ob2d::window::getRenderSize();
        scene.setCenter({position.x + render_size.x / 2.f,position.y + render_size.y / 2.f});
    }


    ob2d::ObStatus<float> ob2d::scene::layer::ObLayer_camera::getCameraStatus() {
        if(ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
        auto camera_pos = scene.getCenter();
        auto camera_size = ob2d::window::sf_getWindow().getSize();

        return {
            { camera_size.x, camera_size.y },
            { camera_pos.x , camera_pos.y }, // 左上
            { camera_pos.x - camera_size.x / 2.f, camera_pos.y - camera_size.y / 2.f }, // 左上
            { camera_pos.x - camera_size.x / 2.f, camera_pos.y + camera_size.y / 2.f }, // 左下
            { camera_pos.x + camera_size.x / 2.f, camera_pos.y - camera_size.y / 2.f }, // 右上
            { camera_pos.x + camera_size.x / 2.f, camera_pos.y + camera_size.y / 2.f }, // 右下
        };
    }


    void ob2d::scene::layer::ObLayer_camera::dt_update(){
        //view_size = window::getGameSize();
        if(ob2d::detail::resized_rendorsize){
            scene.setSize({ob2d::window::getRenderSize().x,ob2d::window::getRenderSize().y});
            scene.setCenter({0,0});
            ob2d::window::sf_getWindow().setView(scene);
     
        }
    }

    sf::View& ob2d::scene::layer::ObLayer_camera::sf_getView(){
        return scene;
    }
    
}

namespace ob2d::scene{
    layer::ObLayer_camera layer_m3;
    layer::ObLayer_camera layer_m2;
    layer::ObLayer_camera layer_m1;
    layer::ObLayer_camera layer_pm0;
    layer::ObLayer_camera layer_p1;
    layer::ObLayer_camera layer_p2;
    layer::ObLayer_camera layer_p3;

    std::vector<ob2d::scene::layer::ObLayer_camera*> layer_array = {
        &ob2d::scene::layer_m3,        //レイヤー-3
        &ob2d::scene::layer_m2,        //レイヤー-2
        &ob2d::scene::layer_m1,        //レイヤー-1
        &ob2d::scene::layer_pm0,       //レイヤー±0
        &ob2d::scene::layer_p1,        //レイヤー 1
        &ob2d::scene::layer_p2,        //レイヤー 2
        &ob2d::scene::layer_p3         //レイヤー 3
    };
}

