/*-----------------------------------------------------------------------------*
    Project: Ob2D - Scene_ObScene.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gr
    atitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <array>
#include <functional>
#include <memory>
#include <atomic>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "Window.hpp"

#include "Object.hpp"
#include "Scene.hpp"


namespace ob2d::scene{
    ob2d::scene::ObScene::ObScene(){
        ob2d::scene::detail::scene_system_manager++;
        this_scene_id = ob2d::scene::detail::scene_system_manager;
    }

    void ob2d::scene::ObScene::dt_end_scene_prosess(){
        if(close_event[0] != nullptr) close_event[0]();            
        for(int i = 0; i < layer_array.size(); i++){
            for(std::vector<ob2d::scene::detail::ObManeger>::iterator it =  object_layer_list[static_cast<ObLayer>(i)].begin(); it != object_layer_list[static_cast<ObLayer>(i)].end();){
                if(*(it->die_flag.get())){
                    it = object_layer_list[static_cast<ObLayer>(i)].erase(it);
                }
                else{
                    it->object->dt_removeMemory();
                    it++;
                }
            }
        }

        if(close_event[1] != nullptr)  close_event[1]();
    }

    void ob2d::scene::ObScene::dt_start_scene_prosess(){
        if(start_event[0] != nullptr) start_event[0]();


        for(int i = 0; i < layer_array.size(); i++){
            for(std::vector<ob2d::scene::detail::ObManeger>::iterator it =  object_layer_list[static_cast<ObLayer>(i)].begin(); it != object_layer_list[static_cast<ObLayer>(i)].end();){
                if(*(it->die_flag.get())){
                    it = object_layer_list[static_cast<ObLayer>(i)].erase(it);
                }
                else{
                    it->object->dt_getMemory();
                    it->object->dt_setScene(this_scene_id);
                    it++;
                }
            }
        }

        if(start_event[1] != nullptr) start_event[1]();
    }

    void ob2d::scene::ObScene::dt_update(ob2d::scene::ObLayer layer_id){
        for(std::vector<ob2d::scene::detail::ObManeger>::iterator it =  object_layer_list[layer_id].begin(); it != object_layer_list[layer_id].end();){                                                                      
            if(*(it->die_flag.get())){  
                it = object_layer_list[layer_id].erase(it);
            }
            else{
                it->object->dt_update();
                it++;
            }
        }
    }

    void ob2d::scene::ObScene::dt_setActiveScene(bool flag){
        is_active_scene = flag;
        if(is_set_ready_scene.load()) is_set_ready_scene = !flag;
    }

    uint16_t ob2d::scene::ObScene::dt_get_SceneSystemID(){
        return this_scene_id;
    }

    void ob2d::scene::ObScene::loadScene(){
        if(!is_active_scene) dt_start_scene_prosess();  //アクティブシーンでないときにする。
        is_set_ready_scene = true;
    }
            
    bool ob2d::scene::ObScene::is_SetReadyScene(){
        return is_set_ready_scene.load();
    }

    void ob2d::scene::ObScene::setStartEvent(std::function<void()> event_A,std::function<void()> event_B){
        start_event[0] = event_A;
        start_event[1] = event_B;
    }
    void ob2d::scene::ObScene::setCloseEvent(std::function<void()> event_A,std::function<void()> event_B){
        close_event[0] = event_A;
        close_event[1] = event_B;
    }

    bool ob2d::scene::ObScene::is_ActiveScene(){
        return is_active_scene;
    }

    void ob2d::scene::ObScene::addObject(ob2d::object::Object2D *obj_ptr,ob2d::scene::ObLayer layer_id){
        if(!obj_ptr) return;     //不正なアドレスなら終了
        obj_ptr->dt_setSceneIDLayer(this_scene_id,layer_id);
        obj_ptr->dt_setScene(this_scene_id);
        object_layer_list[layer_id].push_back(detail::ObManeger(obj_ptr->dt_getDiedFlag(),obj_ptr));
        if(is_active_scene) obj_ptr->dt_getMemory();
    }

    void ob2d::scene::ObScene::moveLayer(ob2d::object::Object2D *obj_ptr,ob2d::scene::ObLayer moved_layer){
        if(!obj_ptr) return;     //不正なアドレスなら終了

        auto now_layer = obj_ptr->dt_getSceneIDLayer(this_scene_id);
        if(now_layer == moved_layer) return ;

        auto it = std::find(object_layer_list[now_layer].begin(),object_layer_list[now_layer].end(),detail::ObManeger(obj_ptr->dt_getDiedFlag(),obj_ptr));

        if(it != object_layer_list[now_layer].end()){
            object_layer_list[moved_layer].push_back(detail::ObManeger(obj_ptr->dt_getDiedFlag(),obj_ptr));
            object_layer_list[now_layer].erase(it);
            obj_ptr->dt_setSceneIDLayer(this_scene_id,moved_layer);

            if(is_active_scene) obj_ptr->dt_setScene(this_scene_id);
        }
    }

    void ob2d::scene::ObScene::deleteFromScene(ob2d::object::Object2D *obj_ptr){
        if(!obj_ptr) return;     //不正なアドレスなら終了
        auto now_layer = obj_ptr->getObLayer();
        auto it = std::find(object_layer_list[now_layer ].begin(),object_layer_list[now_layer ].end(),detail::ObManeger(obj_ptr->dt_getDiedFlag(),obj_ptr));
        if(it != object_layer_list[now_layer ].end()){
            object_layer_list[now_layer ].erase(it);
            if(is_active_scene) obj_ptr->dt_removeMemory();
        }

    }

}