#ifndef SCENE_HPP
#define SCENE_HPP

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
#include "Object.hpp"

namespace ob2d::scene {
    namespace detail {

        struct ObManeger{
            public:
                std::shared_ptr<bool>   die_flag;
                ob2d::object::Object2D* object;

                ObManeger(std::shared_ptr<bool> flag,ob2d::object::Object2D* obj_ptr){
                    die_flag = flag;
                    object = obj_ptr;
                }

                bool operator==(const ObManeger &tar){
                    return this->die_flag == tar.die_flag && this->object == tar.object;
                }

            };
        };

        namespace layer{
            class ObLayer_camera{
                private:
                    sf::View scene;
                    ob2d::Vector2T<float> view_size;
                public:
                    void zoom(float lens);
                    void moveCenter(Vector2T<float> vector);
                    void setAngle(float angle);
                    void setPosition(ob2d::Vector2T<float> position);
                    ob2d::ObStatus<float> getCameraStatus();
                    void dt_update();
                    sf::View& sf_getView();
            };
        };

        extern layer::ObLayer_camera layer_m3;
        extern layer::ObLayer_camera layer_m2;
        extern layer::ObLayer_camera layer_m1;
        extern layer::ObLayer_camera layer_pm0;
        extern layer::ObLayer_camera layer_p1;
        extern layer::ObLayer_camera layer_p2;
        extern layer::ObLayer_camera layer_p3;

    extern std::vector<ob2d::scene::layer::ObLayer_camera*> layer_array ;
        //イキスギィ！


                    
        struct ObScene{
            private:
                uint16_t this_scene_id;       //固有のシーンID　unsigned int
                std::map<ob2d::scene::ObLayer,std::vector<ob2d::scene::detail::ObManeger>> object_layer_list;

                //レイヤーの表示関係
                std::map<ob2d::scene::ObLayer,bool> is_visible_layer;

                std::array<std::function<void()>,2> start_event;
                std::array<std::function<void()>,2> close_event;

                std::atomic<bool> is_set_ready_scene;

                bool has_start_function;
                bool has_end_function;

                bool is_active_scene;
            public:
                ObScene();
                void dt_end_scene_prosess();
                void dt_start_scene_prosess();
                void dt_update(ob2d::scene::ObLayer layer_id);
                void dt_setActiveScene(bool flag);
                uint16_t dt_get_SceneSystemID();
                void loadScene();
                bool is_SetReadyScene();
                void setStartEvent(std::function<void()> event_A,std::function<void()> event_B = nullptr);
                void setCloseEvent(std::function<void()> event_A,std::function<void()> event_B = nullptr);
                bool is_ActiveScene();
                void addObject(ob2d::object::Object2D *obj_ptr,ob2d::scene::ObLayer layer_id);
                void moveLayer(ob2d::object::Object2D *obj_ptr,ob2d::scene::ObLayer moved_layer);
                void deleteFromScene(ob2d::object::Object2D *obj_ptr);

        };

}


#endif