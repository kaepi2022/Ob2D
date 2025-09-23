/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObCollision.cpp

    Copyright (C)2025 MineKura(KAEPI) All rights reserved.
    These coded instructions, statements, and computer programs are protected 
    by copyright.They may be freely used subject to license restrictions.

    This project partially uses the OpenGL and SFML APIs.
    I express my deepest gratitude and respect to the developers of OpenGL/SFML.

    C++ Standard: C++17 (GCC 14.2.0)
*---------------------------------------------------------------------------*/

#include <vector>

#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "Object.hpp"


namespace ob2d::object::detail{
    std::vector<ob2d::object::Object2D*> collision_playable_object;     //すり抜けるが壁などには当たり判定があるオブジェクトを格納するベクトル配列
    std::vector<ob2d::object::Object2D*> collision_object;              //床の当たり判定などを格納するベクトル

    void updateCollision() { 
        for (auto& it : collision_playable_object) {                     
            auto obj_pos = it->getStatus();

            uint8_t collision_status = 0;                    // 初期化（毎フレームリセット）
            float overlap_left = 0, overlap_right = 0, overlap_up = 0, overlap_down = 0;
            float overlap_leftp = 0, overlap_rightp = 0, overlap_upp = 0, overlap_downp = 0;

            // movable vs movable
            for (auto& target : collision_playable_object) {
                if(!target->is_active())    continue;   //シーン外のためスキップ
                if(target == it)            continue;   //自分同士でやるのを防ぐ。
                if(it->col_is_ghost_mode() || target->col_is_ghost_mode()) continue;   //Ghostモードの場合スキップ
                if(it->dt_getObjInfo() != target->dt_getObjInfo()) continue;

                auto tar_pos = target->getStatus();
                //ド・モルガン使うの、あ～いいっすね
                bool x_overlap = !(obj_pos.right_up_position.x < tar_pos.left_up_position.x || obj_pos.left_up_position.x > tar_pos.right_up_position.x);
                bool y_overlap = !(obj_pos.left_down_position.y < tar_pos.left_up_position.y || obj_pos.left_up_position.y > tar_pos.left_down_position.y);
                if (x_overlap && y_overlap) {
                    float l =  tar_pos.right_up_position.x  - obj_pos.left_up_position.x;
                    float r = obj_pos.right_up_position.x  - tar_pos.left_up_position.x;
                    float u = tar_pos.left_down_position.y - obj_pos.left_up_position.y;
                    float d = obj_pos.left_down_position.y - tar_pos.left_up_position.y;

                            // 左方向衝突判定
                    if (l > 0 && l < r && l < u && l < d) {
                                collision_status |= ob2d::object::detail::collision_info::PLAYABLE_L;
                            }
                            // 右方向衝突判定
                            else if (r > 0 && r < l && r < u && r < d) {
                                collision_status |= ob2d::object::detail::collision_info::PLAYABLE_R;
                            }
                            // 上方向衝突判定
                            if (u > 0 && u < l && u < r && u < d) {
                                collision_status |= ob2d::object::detail::collision_info::PLAYABLE_U;
                            }
                            // 下方向衝突判定
                            else if (d > 0 && d < l && d < r && d < u) {
                                collision_status |= ob2d::object::detail::collision_info::PLAYABLE_D;
                            }
                        
                        }

                    
                    }
                    //静的当たり判定オブジェクトがない場合は終了。
                    // movable vs fixed
                    for (auto& fixed : collision_object) {

                        if(!fixed->is_active()) continue;   //シーン外のため終了。
                        if(it->col_is_ghost_mode() || fixed->col_is_ghost_mode()) continue;
                        if(it->dt_getObjInfo() != fixed->dt_getObjInfo()) continue;
                        auto fix_pos = fixed->getStatus();

                        bool x_overlap = !(obj_pos.right_up_position.x < fix_pos.left_up_position.x || obj_pos.left_up_position.x > fix_pos.right_up_position.x);
                        bool y_overlap = !(obj_pos.left_down_position.y < fix_pos.left_up_position.y || obj_pos.left_up_position.y > fix_pos.left_down_position.y);
                        if (x_overlap && y_overlap) {
                            float l_r_ex =  fix_pos.object_size.x * 0.05f;
                            float d_u_ex = fix_pos.object_size.y * 0.075f;
                            float l =  fix_pos.right_up_position.x  - obj_pos.left_up_position.x;
                            float r = obj_pos.right_up_position.x  - fix_pos.left_up_position.x;
                            float u = fix_pos.left_down_position.y - obj_pos.left_up_position.y;
                            float d = obj_pos.left_down_position.y - fix_pos.left_up_position.y;

                            // 左方向衝突判定
                            if (l <= fix_pos.object_size.x &&  l < u && l < d && l < l_r_ex) {  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_L;
                                    overlap_left = l;  
                                
                            }  
                            // 右方向衝突判定  
                            else if (r <= fix_pos.object_size.x && r < l && r < u && r < d && r < l_r_ex) {  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_R;
                                    overlap_right = r;  
                            }
                            // 上方向衝突判定  
                           else if (u <= fix_pos.object_size.y && u < l && u < r && u < d) {  

                                if(!(r <= l_r_ex) && !(d < d_u_ex)){  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_U;
                                    overlap_up = u;  
                                }else if(!(l <= l_r_ex) && !(u < d_u_ex)){  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_U;
                                    overlap_up = u;  
                                }  

                            }  
                            // 下方向衝突判定  
                            else if (d <= fix_pos.object_size.y && d < l && d < r && d < u) {  
                                if(!(r <= l_r_ex) && !(d < d_u_ex)){  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_D;
                                    overlap_down = d;  
                                }else if(!(l <= l_r_ex) && !(u < d_u_ex)){  
                                    collision_status |= ob2d::object::detail::collision_info::FIXED_D;
                                    overlap_down = d;  
                                }  

                            }
                }
            }
                
            it->dt_updateCollision(collision_status,overlap_up,overlap_down,overlap_left,overlap_right);
        }
    }
            
}