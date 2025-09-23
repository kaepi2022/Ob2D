/*-----------------------------------------------------------------------------*
    Project: Ob2D - ObCollision.cpp

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


namespace ob2d::object{

    struct ObFrameRect{
        private:
            sf::IntRect cutter_texture;
        public:

    };

   

    struct ObAnimation{
            private :
                std::map<std::string, std::vector<std::variant<ObFrameRect,ObTexture*>>> animation_box;    //アニメーションデータ
                ObTexture*   sprite_sheet = nullptr;                   //スプライトシート
                bool has_sprite_sheet = false;                          //スプライトシートを持っているか

            public:
                ObTexture* dt_getSpriteSheet(){
                    return sprite_sheet;
                }

                std::vector<std::variant<ObFrameRect,ObTexture*>>* dt_getAnimation_data(const std::string &animation_name){
                    return &animation_box[animation_name];
                }

                std::vector<std::variant<ObFrameRect,ObTexture*>>* dt_getFirst_data(){
                    return &(animation_box.begin()->second);
                }

                void addAnimation(const std::string &animation_name,std::vector<std::variant<ObFrameRect,ObTexture*>> animation_frame){
                    if(animation_frame.empty()) return;        //アニメーションデータが空なら危険なので

                    for(auto &it : animation_frame){
                        //もし、FrameRectを持っていてスプライトシートがなければアニメーション追加をw禁止
                        if(std::holds_alternative<ObFrameRect>(it)) if(!has_sprite_sheet) return;
                    }

                    animation_box[ animation_name ] = animation_frame;
                }

                ObAnimation(std::map<std::string, std::vector<std::variant<ObFrameRect,ObTexture*>>> animation_data,ObTexture* sprite_sheet_image = nullptr){
                    if(!sprite_sheet){  
                        sprite_sheet = sprite_sheet_image;
                        has_sprite_sheet = true;
                    }

                    //アニメーション追加
                    for(auto &it : animation_data){
                        addAnimation(it.first,it.second);
                    }
                }

                bool has_SpriteSheet(){
                    return has_sprite_sheet;
                }

                bool find_animation(const std::string &animation_name){
                    return animation_box.find(animation_name) != animation_box.end();
                }
        };


}

namespace ob2d::object{
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
                float OverLap_l = 0.f;       
                float OverLap_r = 0.f;       
                float OverLap_u = 0.f;       
                float OverLap_d = 0.f;     

                uint8_t collision_status = 0;    //ビットフラグ管理

                bool is_collision_obj = false;   //当たり判定オブジェクトであるか
                bool is_playable_obj   = false;   //動的な当たり判定オブジェクトであるか
                bool is_fixed_obj   = false;   //動的な当たり判定オブジェクトであるか
                bool is_GhostColision = false;   //一時的なあたりが判定完全無効状態であるか

                //オブジェクト管理▼
                
                    //カスタムupdate▼
                std::function<void()> user_update_event = nullptr;       //ユーザーが設定できる毎フレームオブジェクトに更新処理を実行&持たせることができる関数   
                bool enable_update_function            = false;         //ユーザーが設定した関数を毎回実行するかどうか

                    //フェードイン・アウト▼
                float fade_count                      = 0;             //透明度調整をあと何回しなければならないかを記録
                float fade_sum_delta                  = 0;             //何フレーム回数分処理を行ったかを保存
                ob2d::Vector2T<float> fade_power      = {0,0};                    
                ob2d::Vector2T<uint> fade_goalAlpha = {255u,255u}; //フェードインをする際の初期透明度 
                ob2d::Vector2T<uint> fade_color = {255u,255u}; //フェードインをする際の初期透明度 
                ob2d::Vector2T<float> fade_delta       = {0,0};             //フェードイン系の増加、減少量
                bool is_fadeing_in = false;     //フェードイン中であるか
                bool is_fadeing_out = false;    //フェードアウト中であるか
                bool is_unvisible = false;          //描画を無効化(当たり判定、アニメーションなどは更新される)
                    
                    //オリジン
                bool seted_custom_origin = false;

            public:
                std::shared_ptr<bool> dt_getDiedFlag(){
                    return is_died;
                }

                void dt_updateCollision(uint8_t collision,float overlap_up,float overlap_down,float overlap_left,float overlap_right){
                    collision_status = collision;       //当たり判定の管理

                    OverLap_u = overlap_up;
                    OverLap_d = overlap_down;
                    OverLap_l = overlap_left;
                    OverLap_r = overlap_right;

                }

                void dt_setSceneIDLayer(uint16_t scene_id,ob2d::scene::ObLayer layer_id){
                    scene_game_layer_id[scene_id] = layer_id;
                }

                void dt_setScene(uint16_t scene_id){
                    now_layer_id = scene_game_layer_id[scene_id];
                }

                ob2d::scene::ObLayer dt_getSceneIDLayer(uint16_t scene_id){
                    return scene_game_layer_id[scene_id];
                }

                ob2d::scene::detail::object_scene_info dt_getObjInfo(){
                    return {now_layer_id,is_active_object};
                }

                bool is_active()   const{ return is_active_object; }        //シーン範囲外で無効化の状態
                bool is_unVisible() const{ return is_unvisible; }           //描画がスキップされるオブジェクトかどうか



                virtual void dt_update()    = 0;      //描画処理などをお任せします。
                virtual void dt_getMemory() = 0;      //シーンに応じてメモリーを取得する。
                virtual void dt_removeMemory() = 0;      //シーンに応じてメモリーを消し飛ばすw。

                //オブジェクト操作関数▼
                virtual ob2d::ObStatus<float> getStatus()                                       = 0;     //座標、オブジェクトサイズの取得をする。
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
                bool has_update_func(){ return user_update_event != nullptr; }

                void setUpdateFunction(std::function<void()> update_event,bool enable_flag = true){
                    enable_update_function = enable_flag;       //毎フレーム実機うするかどうかを代入
                    user_update_event = update_event;         //アップデート関数を設定する。
                }

                //アップデート関数を無効にする。
                void erase_update_func(){
                    if(user_update_event != nullptr){
                        enable_update_function = false;
                        user_update_event = nullptr;
                    }
                }
                //アップデート関数を一時的に無効/有効にする。
                void set_enable_update_func(bool flag){
                    if(user_update_event != nullptr){
                        enable_update_function = flag;
                    }
                }

                bool is_Collision(Object2D& target_object,bool check_layer = true){
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


                ob2d::scene::ObLayer getObLayer(){
                    return scene_game_layer_id[ob2d::scene::detail::now_scene_id];
                }


                //当たり判定関連▼
                virtual void enableCollision_Playable() = 0;     //動的オブジェクト化
                virtual void enableCollision_Fixed()   = 0;     //静的オブジェクト化
                virtual void disableCollision()        = 0;     //当たり判定無効化

                void col_setGhost(bool flag){ is_GhostColision = flag;}         //すべてのオブジェクトの当たり判定を消す

                bool col_is_collision_object() const{ return is_collision_obj; }
                bool col_is_ghost_mode() const{ return is_GhostColision; }

                bool col_isConflicting_fixed(uint8_t ObDirection){
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

                bool col_isConflicting_playable(uint8_t ObDirection){
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



        };
}

namespace ob2d::object{
    class ObRect2D : public ob2d::object::Object2D {
            protected:
                std::unique_ptr<sf::RectangleShape> object = nullptr;  //短形図形

                //初期化用の値
                ob2d::Vector2T<float>   init_pos;       //初期位置の座標
                ob2d::Vector2T<float>   init_size;      //初期サイズ
                ob2d::color::Colors     init_color;     //初期カラー


                //オブジェクト関連
                bool setting_need_def_oligin = false;   //もし原点が変更された場合、もとに戻す必要があることを保存する

                Vector2T<float> now_scale = {1,1};

            public:
                //コンストラクタ - システム関係 ユーザーは使うな

                void disableCollision() override final{
                    if(!is_collision_obj) return;

                    if(is_playable_obj){
                        detail::collision_playable_object.erase(std::find(detail::collision_playable_object.begin(),detail::collision_playable_object.end(),this));

                    }
                    
                    if(is_fixed_obj){
                        detail::collision_object.erase(std::find(detail::collision_object.begin(),detail::collision_object.end(),this));
                    }

                    is_collision_obj = false;
                    is_playable_obj   = false;
                    is_fixed_obj      = false;
                    is_GhostColision = false;
                }
                
                ObRect2D(ob2d::Vector2T<float> size = {0.f,0.f}, ob2d::Vector2T<float> position = {0.f,0.f}, ob2d::color::Colors color = ob2d::color::White){
                    is_died = std::make_shared<bool>(false);
                    init_pos    = position;
                    init_size   = size;
                    init_color  = color;
                };       
                ~ObRect2D(){
                    *(is_died.get()) = true;
                    if(is_collision_obj) disableCollision();
                    dt_removeMemory();

                }

                void dt_getMemory()override{
                    if(!object){
                        object = std::make_unique<sf::RectangleShape>();

                        //コンストラクター
                        setSize(init_size);
                        setPosition(init_pos);
                        object.get()->setFillColor(init_color);


                        is_active_object = true;        //オブジェクトを生存状態にする。

                    }
                }

                void dt_removeMemory() override{
                    if(object){
                        object = nullptr;

                        now_scale = {1,1};              //初期化

                        is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。

                    }
                }

                void dt_update() override{
                    if(!object) return;

                    //アップデート関数の実行
                    if((user_update_event != nullptr) && enable_update_function) user_update_event();

                    //カスタム原点に変更されたなら中心に戻す。
                    if(seted_custom_origin){
                        auto object_size = object.get()->getSize();
                        object.get()->setOrigin({object_size.x/2,object_size.y/2});
                        seted_custom_origin = false;
                    }

                    if(is_collision_obj){
                        if (col_isConflicting_fixed(ObDirection::DOWN)) object.get()->move({0,-(OverLap_d)});
                        if (col_isConflicting_fixed(ObDirection::UP)) object.get()->move({0,(OverLap_u)});
                        if (col_isConflicting_fixed(ObDirection::RIGHT)) object.get()->move({-(OverLap_r),0});
                        if (col_isConflicting_fixed(ObDirection::LEFT)) object.get()->move({(OverLap_l),0});
                    }
                    
                    if(is_fadeing_in){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_in = false;      //フェードイン処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                        }
                    }
                    if(is_fadeing_out){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_out = false;      //フェードアウト処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                            fade_color    = {0,0};
                            setUnVisible(true);         //非表示に 
                        }
                    }

                    //非表示の場合は表示しない。
                    if(!is_unvisible) ob2d::window::sf_getWindow().draw( *object.get() );
                }

                //オブジェクト操作関数▼


                void setScale(ob2d::Vector2T<float> scale){
                     //クラッシュ対策
                    if(!object) return;
                    sf::Vector2f obj_size = object.get()->getSize();
                    
                    obj_size = object.get()->getSize();
                    object.get()->setSize({obj_size.x*scale.x,obj_size.y*scale.y});                           //スケールを設定する。

                    //原点が中央でなければ
                    if(seted_custom_origin){
                        auto size = object.get()->getSize();
                        object.get()->setOrigin({size.x / 2.f,size.x / 2.f});
                    }

                }

                void setSize(ob2d::Vector2T<float> size){
                    if(!object) return;                    //クラッシュ対策

                    object.get()->setSize(size.sf_getVector());
                    object.get()->setOrigin({size.x / 2.f, size.y / 2.f});
                }

                void setPosition(ob2d::Vector2T<float> pos) override final{
                    if(!object) return;                    //クラッシュ対策
                    auto obj_size = object.get()->getSize();
                    object.get()->setPosition({pos.x + obj_size.x / 2.f,pos.y + obj_size.y / 2.f});
                }

                void move(ob2d::Vector2T<float> velocity) override final{
                    if(!object) return;                    //クラッシュ対策
                    if (is_collision_obj) {
                        if (col_isConflicting_fixed(ObDirection::RIGHT) && velocity.x > 0 && OverLap_r <= 0.2 * std::abs(velocity.x)) {
                            object.get()->move({-(OverLap_r), 0});
                            velocity.x = 0;
                        }
                        if (col_isConflicting_fixed(ObDirection::LEFT) && velocity.x < 0 && OverLap_l <= 0.2 * std::abs(velocity.x)) {
                            object.get()->move({(OverLap_l), 0});
                            velocity.x = 0;
                        }
                        if (col_isConflicting_fixed(ObDirection::DOWN) && velocity.y > 0 && OverLap_d <= 0.2 * std::abs(velocity.y)) {
                            object.get()->move({0, -(OverLap_d)});
                            velocity.y = 0;

                        }
                        if (col_isConflicting_fixed(ObDirection::UP) && velocity.y < 0 && OverLap_u <= 0.2 * std::abs(velocity.y)) {
                            object.get()->move({0, (OverLap_u)});
                            velocity.y = 0;
                        }

                    }
                    
                    object.get()->move({velocity.x, velocity.y});
                }

                void setColor(ob2d::color::Colors color) override final{
                    if(!object) return;                    //クラッシュ対策

                    object.get()->setFillColor(color);
                }

                void setBorder(ob2d::color::Colors color,float size) override final{
                    if(!object) return;                    //クラッシュ対策
                    object.get()->setOutlineThickness(size);
                    object.get()->setOutlineColor(color);
                }

                void setOrigin(uint8_t up_or_down, uint8_t left_or_right){
                    //クラッシュ対策
                    if(!object) return;

                    if (up_or_down == ObDirection::UP) {
                        if (left_or_right == ObDirection::LEFT) object.get()->setOrigin({0, 0});
                        else if (left_or_right == ObDirection::RIGHT) object.get()->setOrigin({object.get()->getSize().x, 0});

                        seted_custom_origin = true;        //任意の原点に設定されたので、true
                    } else if (up_or_down == ObDirection::DOWN) {
                        if (left_or_right == ObDirection::LEFT)       object.get()->setOrigin({0, object.get()->getSize().y});
                        else if (left_or_right == ObDirection::RIGHT) object.get()->setOrigin({object.get()->getSize().x, object.get()->getSize().y});

                        seted_custom_origin = true;        //任意の原点に設定されたので、true
                    }
                }

                void setAlpha(uint alpha_body,bool with_border = true,uint alpha_border = 0u){
                    if(!object) return;                    //クラッシュ対策

                    uint level_body = std::clamp(alpha_body,0u,255u);   //0<= x <= 255 　に収める
                    uint level_border = with_border ? std::clamp(alpha_body,0u,255u) : std::clamp(alpha_border,0u,255u);   //0<= x <= 255 　に収める
                    
                    auto color = object.get()->getFillColor();
                    auto b_color = object.get()->getOutlineColor();
                    color.a = level_body;
                    b_color.a = level_border;

                    object.get()->setOutlineColor(b_color);
                    object.get()->setFillColor(color);
                }

                //x = オブジェクトそのものの透明度、y = 縁の透明度
                ob2d::Vector2T<uint> getAlpha() override final{
                    if(!object) return {0u,0u};
                    return {object.get()->getFillColor().a,object.get()->getOutlineColor().a};
                }
                
                ob2d::Vector2T<ob2d::color::Colors> getColor() override{
                    if(!object) return {ob2d::color::Colors(255,255,255),ob2d::color::Colors(255,255,255)};

                    return {object.get()->getFillColor(),object.get()->getOutlineColor()};
                }

                void setUnVisible(bool flag){
                    if(!object) return;

                    is_unvisible = flag;
                    auto obj_color_body     = object.get()->getFillColor();
                    auto obj_color_border   = object.get()->getOutlineColor();

                    switch (flag){
                        case true:
                            object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,0u});
                            object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,0u});
                            break;
                        
                        case false:
                            object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,255u});
                            object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,255u});
                            break;
                    }
                }

                void setRote(bool flag , float angle = 0.f){
                    if(!object) return;
                    if(flag){
                        object.get()->setRotation(sf::degrees(angle));
                        if(is_collision_obj) col_setGhost(true);
                    }
                    else{
                        object.get()->setRotation(sf::degrees(0.f));
                       if(is_collision_obj) col_setGhost(false); 
                    } 
                }

                
                float getRoteAngle(){
                    if(!object) return 0.f;
                    return object.get()->getRotation().asDegrees();
                }

                void startFadeIn(float prosess_time,uint alpha_body = 255u,uint alpha_border = 255u) override final{
                    if(!object) return;
                    if(!(is_fadeing_in || is_fadeing_out) && is_unvisible){
                        setUnVisible(false);
                        is_fadeing_in = true;
                        fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
                        fade_delta      = {static_cast<float>(alpha_body) / fade_count ,static_cast<float>(alpha_border) / fade_count};

                    }
                }

                void startFadeOut(float prosess_time) override final{
                    if(!object) return;
                    if(!(is_fadeing_in || is_fadeing_out) && !is_unvisible){
                        is_fadeing_out = true;
                        fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
                        fade_color = {getAlpha().x,getAlpha().y};
                        fade_delta = {static_cast<float>(fade_color.x) / fade_count ,static_cast<float>(fade_color.y) / fade_count};
                    }
                }

                ob2d::ObStatus<float> getStatus()  override final{
                    if(!object) return {{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f}};

                    auto object_pos = object.get()->getPosition();
                    auto object_size = object.get()->getGlobalBounds().size;
                    Vector2T<float> obj_size = {object_size.x,object_size.y};

                            return {
                                obj_size,
                                {object_pos.x ,object_pos.y},
                                {object_pos.x - obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                                {object_pos.x - obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                                {object_pos.x + obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                                {object_pos.x + obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f}
                                
                            };
                }

                //当たり判定関連▼
                 void enableCollision_Playable() override final{
                    //当たり判定が無効ならば当たり判定を有効にする
                        is_collision_obj = true;
                        is_playable_obj   = true;

                        object::detail::collision_playable_object.push_back(this);
                }

                void enableCollision_Fixed() override final{
                    //当たり判定が無効ならば当たり判定を有効にする
                        is_collision_obj = true;
                        is_playable_obj   = false;

                        object::detail::collision_object.push_back(this);
                }
        };
}

namespace ob2d::object{
    class ObSprite2D : public ob2d::object::ObRect2D{
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
                void setTexture(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr){
                    if(!object) return;     //クラッシュ対策

                    
                    if(std::holds_alternative<ObTexture*>(texture_path_or_texture_ptr)){
                        mode_ObTexture = true;        //ObTextureモード^^!
                        mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;

                        if(!std::get<ObTexture*>(texture_path_or_texture_ptr))  exit(0); //エラコ
                        auto obj_texture = std::get<ObTexture*>(texture_path_or_texture_ptr)->sf_getTexture();
                        
                        object.get()->setTextureRect({{0,0},{static_cast<int>(obj_texture->getSize().x),static_cast<int>(obj_texture->getSize().y)}});
                        object.get()->setTexture(obj_texture);        //テクスチャをセット

                        if(animation_clock != nullptr) animation_clock = nullptr;
                    }
                    else if(std::holds_alternative<ObAnimation*>(texture_path_or_texture_ptr)){
                        mode_ObAnimation = true;        //ObAnimationモード> ~!
                        mode_ObTexture = false; obj_texture = nullptr;

                        if(!std::get<ObAnimation*>(texture_path_or_texture_ptr))  exit(0); //エラコ
                        obj_animation_box = std::get<ObAnimation*>(texture_path_or_texture_ptr);
                        if(obj_animation_box->has_SpriteSheet()) object.get()->setTexture(obj_animation_box->dt_getSpriteSheet()->sf_getTexture());
                        
                        auto init_textutre = *(obj_animation_box->dt_getFirst_data()->begin());

                        if(std::holds_alternative<ObTexture*>(init_textutre)){
                            auto ob_texture = std::get<ObTexture*>(init_textutre)->sf_getTexture();
                            object.get()->setTexture(ob_texture);
                            object.get()->setTextureRect({{0,0},{static_cast<int>(ob_texture->getSize().x),static_cast<int>(ob_texture->getSize().y)}});
                        }else if(std::holds_alternative<ObFrameRect>(init_textutre)){
                            object.get()->setTextureRect(std::get<ObFrameRect>(init_textutre).sf_getIntRect());
                        }

                        if(!animation_clock) animation_clock = std::make_unique<sf::Clock>();
                    }
                    else if(std::holds_alternative<std::nullptr_t>(texture_path_or_texture_ptr)){
                        mode_ObTexture   = false; obj_texture = nullptr;
                        mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;
                        if(animation_clock != nullptr) animation_clock = nullptr;

                        object.get()->setTexture(nullptr);
                    }

                    if(is_fadeing_in){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_in = false;      //フェードイン処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                        }
                    }

                    if(is_fadeing_out){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_out = false;      //フェードアウト処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                            fade_color    = {0,0};
                            setUnVisible(true);         //非表示に 
                        }
                    }


                }

                void dt_getMemory() override{
                    if(!object){
                        object = std::make_unique<sf::RectangleShape>();

                        //コンストラクター
                        setSize(init_size);
                        setPosition(init_pos);
                        object.get()->setFillColor(init_color);
                        setTexture(init_texture_data);

                        is_active_object = true;        //オブジェクトを生存状態にする。
                    }
                }

                void dt_removeMemory() override{
                    if(object){
                        object = nullptr;

                        now_scale = {1,1};              //初期化

                        //テクスチャ系を直々に開放
                        mode_ObTexture   = false; obj_texture = nullptr;
                        mode_ObAnimation = false; obj_animation_box = nullptr; obj_now_animation = nullptr;
                        if(animation_clock) animation_clock = nullptr;  //強制退去

                        is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。
                    }
                }

                void dt_update() override{
                    if(!object) return;

                    //アップデート関数の実行
                    if((user_update_event != nullptr) && enable_update_function) user_update_event();

                    //カスタム原点に変更されたなら中心に戻す。
                    if(seted_custom_origin){
                        auto object_size = object.get()->getSize();
                        object.get()->setOrigin({object_size.x/2,object_size.y/2});
                        seted_custom_origin = false;
                    }

                    if(is_collision_obj){
                        if (col_isConflicting_fixed(ObDirection::DOWN)) object.get()->move({0,-(OverLap_d)});
                        if (col_isConflicting_fixed(ObDirection::UP)) object.get()->move({0,(OverLap_u)});
                        if (col_isConflicting_fixed(ObDirection::RIGHT)) object.get()->move({-(OverLap_r),0});
                        if (col_isConflicting_fixed(ObDirection::LEFT)) object.get()->move({(OverLap_l),0});
                    }

                    if(mode_ObAnimation && animation_clock != nullptr){
                        if(mode_animation_is_playing && !mode_animation_is_stoped){
                            //FPSに応じて何回めの更新かを計算する。

                            int animation_current_frame = animation_clock->getElapsedTime().asMilliseconds() / (1000 / mode_animation_FPS);
                            if(mode_animation_kaisuu_loop) mode_animation_loop_i++;

                            mode_animation_setting_frame =  animation_current_frame % mode_animation_frame_count;
                            auto frame    = (obj_now_animation->begin() + mode_animation_setting_frame);


                            if(std::holds_alternative<ObTexture*>(*frame)){
                                auto texture = std::get<ObTexture*>(*frame)->sf_getTexture();

                                object.get()->setTextureRect({{0,0},{static_cast<int>(texture->getSize().x),static_cast<int>(texture->getSize().y)}});
                                if(object.get()->getTexture() != texture) object.get()->setTexture(std::get<ObTexture*>(*frame)->sf_getTexture());

                            }else if(std::holds_alternative<ObFrameRect>(*frame)){
                                object.get()->setTexture(obj_animation_box->dt_getSpriteSheet()->sf_getTexture());
                                object.get()->setTextureRect(std::get<ObFrameRect>(*frame).sf_getIntRect());
                            }
                            
                            if(mode_animation_setting_frame == 0 && mode_animation_kaisuu_loop && mode_animation_loop_count <= mode_animation_loop_i){
                                mode_animation_is_playing = false;
                                mode_animation_done_play = true;
                                animation_clock.get()->reset();
                                mode_animation_kaisuu_loop = false;
                                mode_animation_loop_i = 0;
                                mode_animation_FPS    = 0;
                            }
                        }
                        
                    }

                    if(is_fadeing_in){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_in = false;      //フェードイン処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                        }
                    }
                    if(is_fadeing_out){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_out = false;      //フェードアウト処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                            fade_color    = {0,0};
                            setUnVisible(true);         //非表示に 
                        }
                    }

                    //非表示の場合は表示しない。
                    if(!is_unvisible) ob2d::window::sf_getWindow().draw( *object.get() );
                }

                ObSprite2D(std::variant<ObTexture*,ObAnimation*,std::nullptr_t> texture_path_or_texture_ptr,ob2d::Vector2T<float> size,ob2d::Vector2T<float> position = {0.f,0.f}, ob2d::color::Colors color = ob2d::color::White) : ObRect2D(size,position,color){
                    init_texture_data = texture_path_or_texture_ptr;
                }

                ~ObSprite2D(){
                    *(is_died.get()) = true;
                    if(is_collision_obj) disableCollision();
                    dt_removeMemory();
                }


                void flipTexture_x(bool flag) {
                    //クラッシュ対策
                    if(!object) return;

                    //原点が中央でなければ
                    if(seted_custom_origin){
                        auto size = object.get()->getSize();
                        object.get()->setOrigin({size.x / 2.f,size.y / 2.f});
                    }

                    if (!is_flip_x && flag) {
                        is_flip_x = true;
                        object.get()->scale({-1, 1});
                    } else if (is_flip_x && !flag) {
                        is_flip_x = false;
                        object.get()->scale({-1, 1});
                    }
                }

                void flipTexture_y(bool flag) {
                    //クラッシュ対策
                    if(!object) return;

                    //原点が中央でなければ
                    if(seted_custom_origin){
                        auto size = object.get()->getSize();
                        object.get()->setOrigin({size.x / 2.f,size.y / 2.f});
                    }

                    if (!is_flip_y && flag) {
                        is_flip_y = true;
                        object.get()->scale({1, -1});
                    } else if (is_flip_y && !flag) {
                        is_flip_y = false;
                        object.get()->scale({1, -1});
                    }
                }

                void anime_setAnimarion(const std::string animation_name){
                    if(!object || !animation_clock || !mode_ObAnimation) return;     //オブジェクトが無効かまたはアニメーションモードではなければ終了。
                    if(!obj_animation_box->find_animation(animation_name)) return; //アニメーションがなければ終了。

                    auto set_animation = obj_animation_box->dt_getAnimation_data(animation_name);
                    obj_now_animation = set_animation;                          //アニメーション装着
                    mode_animation_frame_count = set_animation->size();         //アニメーションの数をカウント
                    animation_clock.get()->reset();
                }

                void anime_playAnimation(uint FPS,bool loop = true,uint loop_count = 256){
                    if(!object || !animation_clock || !mode_ObAnimation) return;     //オブジェクトが無効かまたはアニメーションモードではなければ終了。
                    mode_animation_is_playing = true;                           //再生判定
                    mode_animation_is_stoped  = false;
                    mode_animation_loop       = loop;                           //アニメーションループをするかどうか
                    mode_animation_loop_count = loop ? 256 : std::clamp(loop_count,1u,255u);       //ループは最大255回まで
                    mode_animation_kaisuu_loop = (loop_count <= 255);
                    mode_animation_FPS = static_cast<int>(FPS);
                    animation_clock.get()->start();
                }

                void anime_pauseAnimation(){
                    if(!object || !animation_clock || !mode_ObAnimation) return;
                    mode_animation_is_playing = false;
                    mode_animation_is_stoped  = true;
                    animation_clock.get()->stop();
                }

                bool anime_is_playing_animation(){
                    return mode_animation_is_playing && mode_ObAnimation;
                }

                bool anime_is_pausing_animation(){
                    return mode_animation_is_stoped && mode_ObAnimation;
                }

                bool anime_is_play_done_animation(){
                    return mode_animation_done_play;
                }

                uint anime_getAnimation_frame(){
                    return mode_ObAnimation ? mode_animation_setting_frame : 0u;
                } 

        };
}

namespace ob2d::object{
        class ObText2D : public ob2d::object::Object2D{
            private:
                std::unique_ptr<sf::Text> object     =  nullptr;       //オブジェクト本体

                ObFont*                   init_font  =  nullptr;       //初期化用フォント
                Vector2T<float>           init_postion;
                int                       init_text_size   =  1.f;           
                float                     init_border_size =  1.f;      
                std::string               init_text        =  " ";     //初期化テキスト
                uint8_t                   init_direction;

                uint8_t init_style;

                ob2d::color::Colors       init_text_color;             //初期カラー      
                ob2d::color::Colors       init_border_color;           //初期ボーダーカラー
                ob2d::color::Colors       init_font_style;           //初期スタイル

                uint8_t                     now_ObDirection; 
            public:
            //ObTextオリジナル

                void disableCollision() override final{
                    if(!is_collision_obj) return;

                    if(is_playable_obj){
                        detail::collision_playable_object.erase(std::find(detail::collision_playable_object.begin(),detail::collision_playable_object.end(),this));

                    }
                    if(is_fixed_obj){
                        detail::collision_object.erase(std::find(detail::collision_object.begin(),detail::collision_object.end(),this));
                    }

                    is_collision_obj = false;
                    is_playable_obj   = false;
                    is_GhostColision = false;
                }


                void setOrigin(uint8_t oligin){
                    auto text_size = object.get()->getLocalBounds().size;

                    switch(oligin){
                        case ObDirection::CENTER:
                            now_ObDirection = ObDirection::CENTER;
                            object.get()->setOrigin({text_size.x / 2.f,text_size.y / 2.f});
                            break;
                        case ObDirection::LEFT:
                            now_ObDirection = ObDirection::LEFT;
                            object.get()->setOrigin({0.f,text_size.y / 2.f});
                            break;
                        case ObDirection::RIGHT:
                            now_ObDirection = ObDirection::RIGHT;
                            object.get()->setOrigin({text_size.x,text_size.y / 2.f});
                            break;
                        default:
                            break;
                    }

                    now_ObDirection = oligin;
                    
                }

                void setText(const std::string &text){
                    if(!object) return;
                    object.get()->setString(sf::String::fromUtf8(text.begin(),text.end()));

                    setOrigin(now_ObDirection);
                }

                void setSize(uint size){
                    if(!object) return;
                    object.get()->setCharacterSize(size);

                    setOrigin(now_ObDirection);
                }

                void setFont(ob2d::object::ObFont *font){
                    if(!object) return;
                    object.get()->setFont(font->getSfFont());

                    setOrigin(now_ObDirection);
                }

                void setStyle(uint16_t style){
                    if(!object) return;

                    if((style & ObTextStyle::default_style) > 0){
                        object.get()->setStyle(sf::Text::Regular);
                        return;
                    }

                    uint16_t style_set;
                    if((style & ObTextStyle::bold_style) > 0) style_set |= sf::Text::Bold;
                    if((style & ObTextStyle::under_line) > 0) style_set |= sf::Text::Underlined;
                    if((style & ObTextStyle::italic_style) > 0) style_set |= sf::Text::Italic;
                    object.get()->setStyle(style_set);
                }

            //Object2D派生

                ob2d::ObStatus<float> getStatus()  override final{
                    if(!object) return {{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f},{0.f,0.f}};

                    auto object_pos = object.get()->getPosition();
                    auto object_size = object.get()->getGlobalBounds().size;
                    Vector2T<float> obj_size = {object_size.x,object_size.y};

                    switch(now_ObDirection){
                        case ObDirection::CENTER:
                            
                            return {

                                {object_pos.x - obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                                {object_pos.x - obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                                {object_pos.x + obj_size.x / 2.f ,object_pos.y - obj_size.y / 2.f},
                                {object_pos.x + obj_size.x / 2.f ,object_pos.y + obj_size.y / 2.f},
                                obj_size
                                };
                        case ObDirection::LEFT:

                            return {
                                {object_pos.x,object_pos.y},
                                {object_pos.x,object_pos.y + obj_size.y / 2.f},
                                {object_pos.x + obj_size.x,object_pos.y},
                                {object_pos.x + obj_size.x,object_pos.y + obj_size.y},
                                obj_size
                            };
                        case ObDirection::RIGHT:
                            return {
                                {object_pos.x - obj_size.x,object_pos.y},
                                {object_pos.x - obj_size.x,object_pos.y + obj_size.y},
                                {object_pos.x  ,object_pos.y},
                                {object_pos.x  ,object_pos.y + obj_size.y},
                                obj_size
                            };
                        default:
                            return {
                                {0,0},
                                {0,0},
                                {0,0},
                                {0,0},
                                {0,0}
                            };
                    }
                }

                void setPosition(ob2d::Vector2T<float> pos) override final{
                    if(!object) return;                    //クラッシュ対策
                    Vector2T<float> moved_position;
                    auto text_size = object.get()->getGlobalBounds().size;
                    switch(now_ObDirection){
                        case ObDirection::CENTER:
                            object.get()->setPosition({pos.x + text_size.x / 2.f ,pos.y + text_size.y / 2.f});
                            break;
                        case ObDirection::LEFT:
                            object.get()->setPosition({pos.x,pos.y + text_size.y / 2.f});
                            break;
                        case ObDirection::RIGHT:
                            object.get()->setPosition({pos.x + text_size.x,pos.y + text_size.y / 2.f});
                            break;
                    }
                }

                void move(ob2d::Vector2T<float> velocity) override final{
                    if(!object) return;                    //クラッシュ対策
                    if (is_collision_obj) {

                        if (col_isConflicting_fixed(ObDirection::RIGHT) && velocity.x > 0 && OverLap_r <= 0.2 * std::abs(velocity.x)) {
                            object.get()->move({-(OverLap_r), 0});
                            velocity.x = 0;
                        }
                        if (col_isConflicting_fixed(ObDirection::LEFT) && velocity.x < 0 && OverLap_l <= 0.2 * std::abs(velocity.x)) {
                            object.get()->move({(OverLap_l), 0});
                            velocity.x = 0;
                        }
                        if (col_isConflicting_fixed(ObDirection::DOWN) && velocity.y > 0 && OverLap_d <= 0.2 * std::abs(velocity.y)) {
                            object.get()->move({0, -(OverLap_d)});
                            velocity.y = 0;
                        }
                        if (col_isConflicting_fixed(ObDirection::UP) && velocity.y < 0 && OverLap_u <= 0.2 * std::abs(velocity.y)) {
                            object.get()->move({0, (OverLap_u)});
                            velocity.y = 0;
                        }

                    }
                    
                    object.get()->move({velocity.x, velocity.y});
                }
                void setColor(ob2d::color::Colors color) override final{
                    if(!object) return;
                    object.get()->setFillColor(color);
                }

                void setBorder(ob2d::color::Colors color,float size) override final{
                    if(!object) return;
                    object.get()->setOutlineThickness(size);
                }

                void setUnVisible(bool flag) override final{
                    if(!object) return;

                    is_unvisible = flag;
                    auto obj_color_body     = object.get()->getFillColor();
                    auto obj_color_border   = object.get()->getOutlineColor();

                    switch (flag){
                        case true:
                            object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,0u});
                            object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,0u});
                            break;
                        
                        case false:
                            object.get()->setFillColor({obj_color_body.r,obj_color_body.g,obj_color_body.b,255u});
                            object.get()->setOutlineColor({obj_color_border.r,obj_color_border.g,obj_color_border.b,255u});
                            break;
                    }
                }

                void setRote(bool flag , float angle = 0.f) override final{
                    if(!object) return;
                    if(flag){
                        object.get()->setRotation(sf::degrees(angle));
                        if(is_collision_obj) col_setGhost(true);
                    }
                    else{
                        object.get()->setRotation(sf::degrees(0.f));
                       if(is_collision_obj) col_setGhost(false); 
                    } 
                }
         
                float getRoteAngle() override final{
                    if(!object) return 0.f;
                    return object.get()->getRotation().asDegrees();
                }

                void setAlpha(uint alpha_body,bool with_border = true,uint alpha_border = 0u) override final{
                    if(!object) return;                    //クラッシュ対策

                    uint level_body = std::clamp(alpha_body,0u,255u);   //0<= x <= 255 　に収める
                    uint level_border = with_border ? std::clamp(alpha_body,0u,255u) : std::clamp(alpha_border,0u,255u);   //0<= x <= 255 　に収める
                    
                    auto color = object.get()->getFillColor();
                    auto b_color = object.get()->getOutlineColor();
                    color.a = level_body;
                    b_color.a = level_border;

                    object.get()->setOutlineColor(b_color);
                    object.get()->setFillColor(color);
                }

                //x = オブジェクトそのものの透明度、y = 縁の透明度
                ob2d::Vector2T<uint> getAlpha() override final{
                    if(!object) return {0u,0u};
                    return {object.get()->getFillColor().a,object.get()->getOutlineColor().a};
                }
                
                ob2d::Vector2T<ob2d::color::Colors> getColor() override{
                    if(!object) return {ob2d::color::Colors(255,255,255),ob2d::color::Colors(255,255,255)};

                    return {object.get()->getFillColor(),object.get()->getOutlineColor()};
                }

                void startFadeIn(float prosess_time,uint alpha_body = 255u,uint alpha_border = 255u) override final{
                    if(!object) return;
                    if(!(is_fadeing_in || is_fadeing_out) && is_unvisible){
                        setUnVisible(false);
                        is_fadeing_in = true;
                        fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
                        fade_delta      = {static_cast<float>(alpha_body) / fade_count ,static_cast<float>(alpha_border) / fade_count};

                    }
                }

                void startFadeOut(float prosess_time) override final{
                    if(!object) return;
                    if(!(is_fadeing_in || is_fadeing_out) && !is_unvisible){
                        is_fadeing_out = true;
                        fade_count = prosess_time * ob2d::window::getWindowFPS();  //実行フレームをFPSで計算
                        fade_color = {getAlpha().x,getAlpha().y};
                        fade_delta = {static_cast<float>(fade_color.x) / fade_count ,static_cast<float>(fade_color.y) / fade_count};
                    }
                }

                void dt_update() override{
                    if(!object) return;


                    //アップデート関数の実行
                    if((user_update_event != nullptr) && enable_update_function) user_update_event();

                    if(is_collision_obj){
                        if (col_isConflicting_fixed(ObDirection::DOWN)) object.get()->move({0,-(OverLap_d)});
                        if (col_isConflicting_fixed(ObDirection::UP)) object.get()->move({0,(OverLap_u)});
                        if (col_isConflicting_fixed(ObDirection::RIGHT)) object.get()->move({-(OverLap_r),0});
                        if (col_isConflicting_fixed(ObDirection::LEFT)) object.get()->move({(OverLap_l),0});
                    }
                    
                    if(is_fadeing_in){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_delta.x * fade_sum_delta,false,fade_delta.y * fade_sum_delta);        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_in = false;      //フェードイン処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                        }
                    }
                    if(is_fadeing_out){
                        fade_sum_delta++;       //計算回数を加算
                        setAlpha(fade_color.x - (fade_delta.x * fade_sum_delta) ,false,fade_color.y - (fade_delta.y * fade_sum_delta) );        //透明度をボーダーと分けて設定

                        if(fade_count <= fade_sum_delta){
                            fade_sum_delta = 0;         //初期化の値そのものにする。
                            is_fadeing_out = false;      //フェードアウト処理完了！
                            fade_count    = 0;          //初期化
                            fade_delta    = {0,0};      //増加量の初期化
                            fade_color    = {0,0};
                            setUnVisible(true);         //非表示に 
                        }
                    }

                    //非表示の場合は表示しない。
                    if(!is_unvisible) ob2d::window::sf_getWindow().draw( *object.get() );
                }

                void dt_getMemory() override final{
                    if(!object){
                        object = std::make_unique<sf::Text>(init_font->getSfFont());

                        //コンストラクター
                        setFont(init_font);
                        setSize(init_text_size);
                        setText(init_text);

                        setOrigin(init_direction);
                        setColor(init_text_color);
                        setPosition(init_postion);
                        setBorder(init_border_color,init_border_size);
                        setStyle(init_style);

                        is_active_object = true;        //オブジェクトを生存状態にする。
                    }
                }

                void dt_removeMemory() override final{
                    if(object){
                        object = nullptr;

                        is_active_object = false;       //オブジェクトを瀕死状態にするwwwwwww。
                    }
                }

                ObText2D(ob2d::object::ObFont *font,uint text_size,ob2d::color::Colors text_color,const std::string &text,ob2d::Vector2T<float> position = {0.f,0.f},uint update_direction = ObDirection::LEFT ,float border_size = 0.f,ob2d::color::Colors border_color = ob2d::color::Black,uint16_t style = ob2d::object::ObTextStyle::default_style){
                    is_died = std::make_shared<bool>();
                    init_direction = update_direction;
                    init_style = style;
                    init_font = font;
                    init_text_size = text_size;
                    init_text_color = text_color;
                    init_postion = position;
                    init_text = text;
                    init_border_size = border_size;
                    init_border_color = border_color;

                }
                ~ObText2D(){
                    *(is_died.get()) = true;
                    if(is_collision_obj) disableCollision();
                    dt_removeMemory();
                }

                //当たり判定関連▼
                 void enableCollision_Playable() override final{
                    //当たり判定が無効ならば当たり判定を有効にする
                    if(!is_collision_obj){
                        is_collision_obj = true;
                        is_playable_obj   = true;

                        object::detail::collision_playable_object.push_back(this);
                    }
                }

                void enableCollision_Fixed() override final{
                    //当たり判定が無効ならば当たり判定を有効にする
                    if(!is_collision_obj){
                        is_collision_obj = true;
                        is_playable_obj   = false;

                        object::detail::collision_object.push_back(this);
                    }
                }

        };
}

namespace ob2d::object::detail{
        namespace detail{

            constexpr float OB2D_UD = 0.015f;
            constexpr float OB2D_LR =  0.01f;

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
    
}