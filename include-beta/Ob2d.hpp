#ifndef OB2D_HPP
#define OB2D_HPP

#include <iostream>
#include <cmath>
#include <random>
#include <ctime>
#include <map>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <functional>

// OB2D内部の各種ヘッダまとめw
#include "SRC/System.hpp"
#include "SRC/Window.hpp"
#include "SRC/Math.hpp"
#include "SRC/Hid.hpp"
#include "SRC/Object.hpp"
#include "SRC/scene.hpp"
#include "SRC/sounds.hpp"
#include "SRC/Time.hpp"

namespace ob2d {

    namespace window {
            namespace detail{
                ob2d::scene::ObScene* now_scene = nullptr;

                std::function<void(bool)> render_syori = [](bool resize){
                        sf::FloatRect view_port = ob2d::window::detail::dt_correctAspect();
                        for(auto &layer : ob2d::scene::layer_array){
                            if(resize){
                                auto size = ob2d::window::getRenderSize();
                                layer->sf_getView().setSize({static_cast<float>(size.x),static_cast<float>(size.y)});
                                auto win_size = ob2d::window::getWindowStatus().object_size.sf_getVector();
                                layer->sf_getView().setCenter({render_size.x / 2.f,render_size.y / 2.f});
                            }
                            layer->sf_getView().setViewport(view_port);

                            ob2d::window::sf_getWindow().setView(layer->sf_getView());
                        }
                };

                void dt_updateWindow(){
                    if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                    
                    while(const std::optional events = ob2d::window::sf_getWindow().pollEvent()){
                        if(events->getIf<sf::Event::Closed>()){
                            if(ob2d::window::detail::close_event[0] != nullptr) ob2d::window::detail::close_event[0]();
                            ob2d::window::sf_getWindow().close();
                            if(ob2d::window::detail::close_event[1] != nullptr) ob2d::window::detail::close_event[1]();

                            exit(0);    //安全に終了させる。
                        }

                        if(const auto* mouseWheelScrolled = events->getIf<sf::Event::MouseWheelScrolled>()){
                            ob2d::hid::mouse::detail::mouse_wheel_delta_now.x = (mouseWheelScrolled->position.x - ob2d::hid::mouse::detail::mouse_wheel_pos_bef.x);
                            ob2d::hid::mouse::detail::mouse_wheel_delta_now.y = (mouseWheelScrolled->position.y - ob2d::hid::mouse::detail::mouse_wheel_pos_bef.y);
                            ob2d::hid::mouse::detail::mouse_wheel_pos_bef = {mouseWheelScrolled->position.x,mouseWheelScrolled->position.y};

                        }else{
                            ob2d::hid::mouse::detail::mouse_wheel_delta_now = {0.f,0.f};
                        }

                        // catch the resize events
                        if (const auto* resized = events->getIf<sf::Event::Resized>()) {
                            render_syori(false);
                        }
                    }

                    if(ob2d::window::detail::resized_render){
                        render_syori(true);
                        ob2d::window::detail::resized_render = false;
                    }

                    


                }
            }

    }; // namespace window


    void updateRender() {

        if(!ob2d::window::is_WindowEnable()) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

        if (!ob2d::object::detail::collision_playable_object.empty()){
            ob2d::object::detail::updateCollision();
        }

        ob2d::window::sf_getWindow().clear(ob2d::window::detail::LetterBox);  //描画画面外の部分を塗りつぶす

        int update_count = 0;       //ループ回数をカウント初期化
        for(auto &layer : ob2d::scene::layer_array){
            if(update_count == 0) ob2d::window::sf_getWindow().draw(ob2d::window::detail::RenderArea);//後ろの描画エリアを一番初めに描画する
            if(ob2d::window::detail::now_scene != nullptr) ob2d::window::detail::now_scene->dt_update(static_cast<ob2d::scene::ObLayer>(update_count));
            
            ob2d::window::sf_getWindow().setView(layer->sf_getView());
            ++update_count;
        }
            
            // バッファの内容を画面に反映
        ob2d::window::sf_getWindow().display();
    }


    /**
     * @brief ゲームループ先頭で呼ぶべき、各システムの状態を更新する関数
     *
     * - 入力デバイスの状態更新（キーボード、マウス、コントローラ）
     * - 衝突判定オブジェクトの更新
     * - ウィンドウ状態とゲームカメラの更新
     */
    void updateSystem() {
        ob2d::time::detail::dt_updateDeltatime();

        if(ob2d::detail::inited_window){
            auto window_size = ob2d::window::getWindowStatus().object_size;
            for(auto &layer : ob2d::scene::layer_array){
                layer->sf_getView().setCenter({window_size.x / 2.f,window_size.y / 2.f});
                layer->sf_getView().setSize({window_size.x,window_size.y});
                ob2d::window::sf_getWindow().setView(layer->sf_getView());
            }

            ob2d::detail::inited_window = false;
        }
        sf::Joystick::update();
        if(ob2d::hid::controller::detail::is_waiting_join_player){

            for(uint i = 0; i < 8; i++){
                for(uint j = 0; j < 32; j++){
                    if(ob2d::hid::controller::detail::player_sfml_id_used_status[i]) continue;

                    if(sf::Joystick::isButtonPressed(i,j)){
                        *(ob2d::hid::controller::detail::wait_player->dt_getdata()) = i;

                        ob2d::hid::controller::detail::players_list[i] = ob2d::hid::controller::detail::wait_player;
                        ob2d::hid::controller::detail::player_sfml_id_used_status[i] = true;

                        ob2d::hid::controller::detail::is_waiting_join_player = false;
                        ob2d::hid::controller::detail::wait_player = nullptr;   
                        std::cout << "sux";
                    }

                    if(!ob2d::hid::controller::detail::is_waiting_join_player) break;
                }
            }

        }


        for(int i = 0; i < 8; i ++) if(ob2d::hid::controller::detail::players_list[i] != nullptr) ob2d::hid::controller::detail::players_list[i]->dt_update();
        ob2d::hid::detail::UpdateKeyStatus();
        ob2d::hid::detail::UpdateMouseStatus();
        // 移動可能な衝突オブジェクトが存在するときのみ更新

        for(auto &layer : ob2d::scene::layer_array){
            layer->dt_update();
        }

        if(ob2d::window::is_WindowEnable()) ob2d::window::detail::dt_updateWindow();

    }

    namespace scene{
        
        ObScene &getNowScene(){
            return *ob2d::window::detail::now_scene;
        }
        
        void setScene(ob2d::scene::ObScene* scene){

            if(ob2d::window::detail::now_scene){
                if(ob2d::window::detail::now_scene == scene) return;
                ob2d::window::detail::now_scene->dt_setActiveScene(false);
                ob2d::window::detail::now_scene->dt_end_scene_prosess();
            }

            scene->dt_setActiveScene(true);
            ob2d::scene::detail::now_scene_id = scene->dt_get_SceneSystemID();
            scene->dt_start_scene_prosess();

            for(auto &layer : ob2d::scene::layer_array){
                layer->setPosition({0.f,0.f});
            }

            ob2d::window::detail::now_scene = scene;
        }

        void changeScene(ob2d::scene::ObScene* scene){
            if(!scene->is_SetReadyScene()) return;                           //シーンの準備を確認

            if(ob2d::window::detail::now_scene){
                if(ob2d::window::detail::now_scene == scene) return;        //同じシーンだったら終了
                ob2d::window::detail::now_scene->dt_setActiveScene(false);
                ob2d::window::detail::now_scene->dt_end_scene_prosess();
            }

            scene->dt_setActiveScene(true);
            ob2d::scene::detail::now_scene_id = scene->dt_get_SceneSystemID();

            for(auto &layer : ob2d::scene::layer_array){
                layer->setPosition({0.f,0.f});
            }


            ob2d::window::detail::now_scene = scene;
        }
    };

    namespace hid::mouse{

        bool isSelectedCursor(ob2d::object::Object2D& obj,ob2d::scene::ObLayer layer_id){
            if(!obj.is_active()) return false;

                auto mc_pos = ob2d::hid::mouse::getCursorPosition(layer_id);
                auto obj_pos = obj.getStatus();

                return (
                    mc_pos.x >= obj_pos.left_up_position.x && mc_pos.x <= obj_pos.right_up_position.x
                 && mc_pos.y >= obj_pos.left_up_position.y && mc_pos.y <= obj_pos.left_down_position.y
                    );
            }
    };


} // namespace ob2d

#endif