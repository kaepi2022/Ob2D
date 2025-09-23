/*-----------------------------------------------------------------------------*
    Project: Ob2D - Object2D.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <vector>
#include <memory>

#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "Object.hpp"


namespace ob2d::object{
    std::shared_ptr<bool> Object2D::dt_getDiedFlag(){
        return is_died;
    }

    void Object2D::dt_updateCollision(uint8_t collision,float overlap_up,float overlap_down,float overlap_left,float overlap_right){
        collision_status = collision;       //当たり判定の管理
        
        OverLap_u = overlap_up;
        OverLap_d = overlap_down;
        OverLap_l = overlap_left;
        OverLap_r = overlap_right;

    }

    void Object2D::dt_setSceneIDLayer(uint16_t scene_id,ob2d::scene::ObLayer layer_id){
        scene_game_layer_id[scene_id] = layer_id;
    }

    void Object2D::dt_setScene(uint16_t scene_id){
        now_layer_id = scene_game_layer_id[scene_id];
    }

    ob2d::scene::ObLayer Object2D::dt_getSceneIDLayer(uint16_t scene_id){
        return scene_game_layer_id[scene_id];
    }

    ob2d::scene::detail::object_scene_info Object2D::dt_getObjInfo(){
        return {now_layer_id,is_active_object};
    }

    bool Object2D::is_active()   { return is_active_object; }        //シーン範囲外で無効化の状態
    bool Object2D::is_unVisible(){ return is_unvisible; }           //描画がスキップされるオブジェクトかどうか




        //アップデート関数がしっかり設定されているか
    bool Object2D::has_update_func(){ return user_update_event != nullptr; }

    void Object2D::setUpdateFunction(std::function<void()> update_event,bool enable_flag){
        enable_update_function = enable_flag;       //毎フレーム実機うするかどうかを代入
        user_update_event = update_event;         //アップデート関数を設定する。
    }

                //アップデート関数を無効にする。
    void Object2D::erase_update_func(){
        if(user_update_event != nullptr){
            enable_update_function = false;
            user_update_event = nullptr;
        }
    }
                //アップデート関数を一時的に無効/有効にする。
    void Object2D::set_enable_update_func(bool flag){
        if(user_update_event != nullptr){
            enable_update_function = flag;
        }
    }

    bool Object2D::is_Collision(ob2d::object::Object2D& target_object,bool check_layer){
        if(!is_active_object || !target_object.is_active()) return false;
            auto tar_pos = target_object.getStatus();
            auto obj_pos = getStatus();

            //レイヤーもチェックする場合ならレイヤーが同じか比較し、しなければ、レイヤーを無視する。
            bool layer = check_layer ? target_object.getObLayer() == getObLayer() : true;

        return (
            obj_pos.right_up_position.x >= tar_pos.left_up_position.x && obj_pos.left_up_position.x <= tar_pos.right_up_position.x
             && obj_pos.left_down_position.y >= tar_pos.left_up_position.y && obj_pos.left_up_position.y <= tar_pos.right_down_position.y
             && layer       //レイヤーも確認
        );
    }


    ob2d::scene::ObLayer Object2D::getObLayer(){
        return scene_game_layer_id[ob2d::scene::detail::now_scene_id];
    }



    void Object2D::col_setGhost(bool flag){ is_GhostColision = flag;}         //すべてのオブジェクトの当たり判定を消す

    bool Object2D::col_is_collision_object() { return is_collision_obj; }
    bool Object2D::col_is_ghost_mode() { return is_GhostColision; }

    bool Object2D::col_isConflicting_fixed(uint8_t ObDirection){
        if(!is_collision_obj) return false; // 当たり判定が無効な場合はfalseを返す

            switch (ObDirection){
                case ObDirection::UP:
                    return (collision_status & detail::collision_info::FIXED_U) > 0;
                case ObDirection::DOWN:
                    return (collision_status & detail::collision_info::FIXED_D) > 0;
                case ObDirection::LEFT:
                    return (collision_status & detail::collision_info::FIXED_L) > 0;
                case ObDirection::RIGHT:
                    return (collision_status & detail::collision_info::FIXED_R) > 0;
                default:
                    return false;
            }
    }

    bool Object2D::col_isConflicting_playable(uint8_t ObDirection){
        if(!is_collision_obj || !is_playable_obj) return false; // 当たり判定が無効な場合はfalseを返す

        switch (ObDirection){
            case ObDirection::UP:
                return (collision_status & detail::collision_info::PLAYABLE_U) > 0;
            case ObDirection::DOWN:
                return (collision_status & detail::collision_info::PLAYABLE_D) > 0;
            case ObDirection::LEFT:
                return (collision_status & detail::collision_info::PLAYABLE_L) > 0;
            case ObDirection::RIGHT:
                return (collision_status & detail::collision_info::PLAYABLE_R) > 0;
            default:
                return false;
            }
    }

}//end namespace ob2d::object