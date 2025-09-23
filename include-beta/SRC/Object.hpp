#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <variant>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <algorithm>

#include "System.hpp"
#include "Window.hpp"
#include "hid.hpp"

namespace ob2d{


    namespace object{
        namespace detail{
            using ObTextStyle = int;

            enum collision_info{
                FIXED_U     = (1<<0),
                FIXED_D     = (1<<1),
                FIXED_L     = (1<<2),
                FIXED_R     = (1<<3),
                PLAYABLE_U  = (1<<4),
                PLAYABLE_D  = (1<<5),
                PLAYABLE_L  = (1<<6),
                PLAYABLE_R  = (1<<7),
            };
        }



        namespace ObDirection{
            constexpr uint8_t RIGHT =  0;
            constexpr uint8_t LEFT  =  1;
            constexpr uint8_t UP    =  2;
            constexpr uint8_t DOWN  =  3;
            constexpr uint8_t CENTER = 4;
        };

        struct ObFrameRect{
            private:
                sf::IntRect cutter_texture;
            public:
                ObFrameRect(Vector2T<uint> cut_start_position,Vector2T<uint> cut_size);

                sf::IntRect sf_getIntRect();
        };

        struct ObFont{
            private:
                bool test;
                sf::Font font_file;
            public:
                ObFont(const std::string &font_path){
                    test = font_file.openFromFile(font_path);
                }
                const sf::Font &getSfFont() const{
                    return font_file;
                }
        };

        namespace ObTextStyle{
            constexpr uint8_t default_style = (1 << 0);
            constexpr uint8_t italic_style  = (1 << 1);
            constexpr uint8_t bold_style    = (1 << 2);
            constexpr uint8_t under_line    = (1 << 3);
        };

        class ObTexture{
            private:
                sf::Texture texture;
                std::unique_ptr<sf::Image> texture_base;

            public:
                void setPixels(const std::vector<std::vector<ob2d::color::Colors>>& pixel_data);
                std::vector<std::vector<ob2d::color::Colors>> getPixels();
                ObTexture(std::variant<std::string,std::vector<std::vector<ob2d::color::Colors>>> texture_data);
                bool loadTexture(const std::string &texture_path);
                ob2d::Vector2T<uint> getSize();
                void setRepeated(bool flag);
                void setSmooth(bool flag);
                bool saveToFile(const std::string &file_path);
                bool is_Repeated();
                bool is_Smooth();
                sf::Texture* sf_getTexture();
        };
        
        struct ObAnimation{
            private :
                std::map<std::string, std::vector<std::variant<ObFrameRect,ObTexture*>>> animation_box;    //アニメーションデータ
                ObTexture*   sprite_sheet;
                bool has_sprite_sheet;

            public:
                ObTexture* dt_getSpriteSheet();

                std::vector<std::variant<ObFrameRect,ObTexture*>>* dt_getAnimation_data(const std::string &animation_name);
                

                std::vector<std::variant<ObFrameRect,ObTexture*>>* dt_getFirst_data();

                void addAnimation(const std::string &animation_name,std::vector<std::variant<ObFrameRect,ObTexture*>> animation_frame);
                ObAnimation(std::map<std::string, std::vector<std::variant<ObFrameRect,ObTexture*>>> animation_data,ObTexture* sprite_sheet_image = nullptr);

                bool has_SpriteSheet();

                bool find_animation(const std::string &animation_name);
        };

        class Object2D{
            protected:

                //シーン管理関連
                std::shared_ptr<bool>                           is_died = nullptr;                        //オブジェクトは行きているかを管理するフラグ
                bool                                            is_active_object = false;   //シーンによりオブジェクトの生存確認をする。
                std::map<uint16_t,ob2d::scene::ObLayer>   scene_game_layer_id;        //そのシーンIDに応じたレイヤー
                uint16_t                                        now_scene_system_id;        //オブジェクトがいるシーン
                ob2d::scene::ObLayer                      now_layer_id;               //オブジェクトが現在いるレイヤー
                ob2d::scene::detail::object_scene_info    obj_scene_info;    //当たり判定測定時に使う便利な道具箱☆

                //当たり判定関連

                //特定の面がどれだけ滑り込んでいるか
                float OverLap_l; 
                float OverLap_r;   
                float OverLap_u;   
                float OverLap_d;   

                uint8_t collision_status;

                bool is_collision_obj;
                bool is_playable_obj;
                bool is_fixed_obj;
                bool is_GhostColision;

                //オブジェクト管理▼
                
                    //カスタムupdate▼
                std::function<void()> user_update_event;
                bool enable_update_function;

                    //フェードイン・アウト▼
                float fade_count;
                float fade_sum_delta;
                ob2d::Vector2T<float> fade_power;               
                ob2d::Vector2T<uint> fade_goalAlpha;
                ob2d::Vector2T<uint> fade_color;
                ob2d::Vector2T<float> fade_delta;
                bool is_fadeing_in = false;
                bool is_fadeing_out = false;
                bool is_unvisible = false;
                    
                    //オリジン
                bool seted_custom_origin;

            public:
                std::shared_ptr<bool> dt_getDiedFlag();

                void dt_updateCollision(uint8_t collision,float overlap_up,float overlap_down,float overlap_left,float overlap_right);

                void dt_setSceneIDLayer(uint16_t scene_id,ob2d::scene::ObLayer layer_id);

                void dt_setScene(uint16_t scene_id);

                ob2d::scene::ObLayer dt_getSceneIDLayer(uint16_t scene_id);

                ob2d::scene::detail::object_scene_info dt_getObjInfo();

                bool is_active();
                bool is_unVisible();



                virtual void dt_update()    = 0;      //描画処理などをお任せします。
                virtual void dt_getMemory() = 0;      //シーンに応じてメモリーを取得する。
                virtual void dt_removeMemory() = 0;      //シーンに応じてメモリーを消し飛ばすw。

                //オブジェクト操作関数▼
                virtual ob2d::ObStatus<float> getStatus()                        = 0;     //座標、オブジェクトサイズの取得をする。
                virtual Vector2T<ob2d::color::Colors> getColor()                 = 0;
                virtual void   setColor(ob2d::color::Colors color)               = 0;     //オブジェクトの色を変更する。
                virtual void   setBorder(ob2d::color::Colors color,float size)   = 0;     //オブジェクトのボーダーをを追加する。
                virtual void   setAlpha(uint alpha_body,bool with_border = true,uint alpha_border = 0u)     = 0;     //オブジェクトの透明度を変更する縁も透明化可能。
                virtual void   setUnVisible(bool flag)                          = 0;     //見えなくする。
                virtual void   setRote(bool flag,float angle = 0.f)                = 0;     //回転する。falseで0°に戻す
                virtual float  getRoteAngle()                                   = 0;     //角度入手
                virtual void   startFadeIn(float prosess_time,uint alpha_body = 255u,uint alpha_border = 255u)                       = 0;     //オブジェクトを徐々に可視状態にします。
                virtual void   startFadeOut(float prosess_time)                  = 0;     //オブジェクトを徐々に不可視状態にします。
                virtual void   setPosition(ob2d::Vector2T<float> pos)            = 0;     //オブジェクトの位置を左上を原点に変更する。
                virtual void   move(ob2d::Vector2T<float> velocity)              = 0;     //当たり判定状況に沿って動かす
                virtual ob2d::Vector2T<uint> getAlpha()                          = 0;     //透明度を取得

                //アップデート関数がしっかり設定されているか
                bool has_update_func();

                void setUpdateFunction(std::function<void()> update_function,bool enable_flag = true);

                //アップデート関数を無効にする。
                void erase_update_func();

                //アップデート関数を一時的に無効/有効にする。
                void set_enable_update_func(bool flag);

                bool is_Collision(Object2D& target_object,bool check_layer = true);


                ob2d::scene::ObLayer getObLayer();


                //当たり判定関連▼
                virtual void enableCollision_Playable() = 0;     //動的オブジェクト化
                virtual void enableCollision_Fixed()   = 0;     //静的オブジェクト化
                virtual void disableCollision()        = 0;     //当たり判定無効化

                void col_setGhost(bool flag);
                bool col_is_collision_object();
                bool col_is_ghost_mode();

                bool col_isConflicting_fixed(uint8_t ObDirection);

                bool col_isConflicting_playable(uint8_t ObDirection);



        };


        namespace detail{
            extern std::vector<Object2D*> collision_playable_object;
            extern std::vector<Object2D*> collision_object;

            void updateCollision();
        }

        class ObRect2D : public Object2D {
            protected:
                std::unique_ptr<sf::RectangleShape> object;

                //初期化用の値
                ob2d::Vector2T<float>   init_pos;       //初期位置の座標
                ob2d::Vector2T<float>   init_size;      //初期サイズ
                ob2d::color::Colors     init_color;     //初期カラー


                //オブジェクト関連
                bool setting_need_def_oligin;

                Vector2T<float> now_scale;

            public:
                //コンストラクタ - システム関係 ユーザーは使うな

                void disableCollision() override final;
                
                ObRect2D(ob2d::Vector2T<float> size = {0.f,0.f}, ob2d::Vector2T<float> position = {0.f,0.f}, ob2d::color::Colors color = ob2d::color::White);
                ~ObRect2D();

                void dt_getMemory()override;

                void dt_removeMemory() override;

                void dt_update() override;

                //オブジェクト操作関数▼


                void setScale(ob2d::Vector2T<float> scale);

                void setSize(ob2d::Vector2T<float> size);

                void setPosition(ob2d::Vector2T<float> pos) override final;

                void move(ob2d::Vector2T<float> velocity) override final;

                void setColor(ob2d::color::Colors color) override final;

                void setBorder(ob2d::color::Colors color,float size) override final;

                void setOrigin(uint8_t up_or_down, uint8_t left_or_right);

                void setAlpha(uint alpha_body,bool with_border = true,uint alpha_border = 0u);

                //x = オブジェクトそのものの透明度、y = 縁の透明度
                ob2d::Vector2T<uint> getAlpha() override final;
                
                ob2d::Vector2T<ob2d::color::Colors> getColor() override;

                void setUnVisible(bool flag);

                void setRote(bool flag , float angle = 0.f);

                
                float getRoteAngle();

                void startFadeIn(float prosess_time,uint alpha_body = 255u,uint alpha_border = 255u) override final;

                void startFadeOut(float prosess_time) override final;

                ob2d::ObStatus<float> getStatus()  override final;

                //当たり判定関連▼
                 void enableCollision_Playable() override final;

                void enableCollision_Fixed() override final;
        };

        class ObSprite2D : public ObRect2D{
            private:
                //反転しているかまとめ
                bool is_flip_x  = false;        
                bool is_flip_y  = false;

                //テクスチャ別モード

                bool mode_ObTexture   = false;
                bool mode_ObAnimation = false;

                int  mode_animation_frame_count = 0;            //フレーム数
                int  mode_animation_setting_frame = 0;          //現在装着中のフレーム
                int  mode_animation_FPS = 0;          //アニメーションの再生FPS
                int  mode_animation_loop_count    = 0;          //アニメーションのループカウント(最大255回)
                int  mode_animation_loop_i        = 0;          //ループした回数
                bool mode_animation_kaisuu_loop   = false;      //回数ループは有効であるか
                bool mode_animation_done_play      = false;     //アニメーションは再生が終わったか

                bool mode_animation_loop = false;               //アニメーションのループ
                bool mode_animation_is_playing = false;         //アニメーションは再生中であるか
                bool mode_animation_is_stoped = false;          //アニメーションは停止されている


                //アニメーション
                std::unique_ptr<sf::Clock> animation_clock = nullptr;  ///< アニメーション時間管理勝手に始まらないために、newで管理
            
                //ポインタテクスチャ
                ObTexture*   obj_texture                            = nullptr;
                ObAnimation* obj_animation_box                      = nullptr;

                std::vector<std::variant<ObFrameRect,ObTexture*>>* obj_now_animation = nullptr;
                
                std::variant<ObTexture*,ObAnimation*,std::nullptr_t>   init_texture_data;


            public:
                bool setTexture(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr);

                void dt_getMemory() override;

                void dt_removeMemory() override;

                void dt_update() override;

                ObSprite2D(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr,ob2d::Vector2T<float> size,ob2d::Vector2T<float> position = {0.f,0.f}, ob2d::color::Colors color = ob2d::color::White);

                ~ObSprite2D();


                void flipTexture_x(bool flag);

                void flipTexture_y(bool flag);

                void anime_setAnimarion(const std::string animation_name);

                void anime_playAnimation(uint FPS,bool loop = true,uint loop_count = 256);

                void anime_pauseAnimation();

                bool anime_is_playing_animation();

                bool anime_is_pausing_animation();

                bool anime_is_play_done_animation();
                

                uint anime_getAnimation_frame();
        };

        class ObText2D : public Object2D{
            private:
                std::unique_ptr<sf::Text> object;

                ObFont*                   init_font;
                Vector2T<float>           init_postion;
                int                       init_text_size;       
                float                     init_border_size;
                std::string               init_text;
                uint8_t                   init_direction;

                uint8_t init_style;

                ob2d::color::Colors       init_text_color;             //初期カラー      
                ob2d::color::Colors       init_border_color;           //初期ボーダーカラー
                ob2d::color::Colors       init_font_style;           //初期スタイル

                uint8_t                     now_ObDirection; 
            public:
            //ObTextオリジナル

                void disableCollision() override final;


                void setOrigin(uint8_t oligin);

                void setText(const std::string &text);

                void setSize(uint size);

                void setFont(ob2d::object::ObFont *font);

                void setStyle(uint16_t style);

            //Object2D派生

                ob2d::ObStatus<float> getStatus()  override final;

                void setPosition(ob2d::Vector2T<float> pos) override final;

                void move(ob2d::Vector2T<float> velocity) override final;

                void setColor(ob2d::color::Colors color) override final;

                void setBorder(ob2d::color::Colors color,float size) override final;

                void setUnVisible(bool flag) override final;

                void setRote(bool flag , float angle = 0.f) override final;
                
         
                float getRoteAngle() override final;

                void setAlpha(uint alpha_body,bool with_border = true,uint alpha_border = 0u) override final;

                //x = オブジェクトそのものの透明度、y = 縁の透明度
                ob2d::Vector2T<uint> getAlpha() override final;
                
                ob2d::Vector2T<ob2d::color::Colors> getColor() override;

                void startFadeIn(float prosess_time,uint alpha_body = 255u,uint alpha_border = 255u) override final;

                void startFadeOut(float prosess_time) override final;

                void dt_update() override;

                void dt_getMemory() override final;

                void dt_removeMemory() override final;

                ObText2D(ob2d::object::ObFont *font,uint text_size,ob2d::color::Colors text_color,const std::string &text,ob2d::Vector2T<float> position = {0.f,0.f},uint update_direction = ObDirection::LEFT ,float border_size = 0.f,ob2d::color::Colors border_color = ob2d::color::Black,uint16_t style = ob2d::object::ObTextStyle::default_style);
                ~ObText2D();

                //当たり判定関連▼
                void enableCollision_Playable() override final;
                void enableCollision_Fixed() override final;

        };
    };
};

#endif