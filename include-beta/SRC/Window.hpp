#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "System.hpp"

namespace ob2d::window{
    namespace detail{
        extern std::shared_ptr<sf::RenderWindow>   ObWindow;
        extern std::array<std::function<void()>,2> close_event;
        extern std::unique_ptr<sf::Image>          window_icon;

        extern sf::RectangleShape                  RenderArea;
        extern ob2d::color::Colors                 LetterBox;
        extern bool                                is_VisibleRenderArea;
        extern ob2d::Vector2T<uint>                render_size;

        extern ob2d::Vector2T<float>               view_offset;
        extern ob2d::Vector2T<float>               view_viewport;
        extern float                               view_render_scale;

        extern bool    is_FullScreen   ;
        extern bool    is_WindowScreen ;

        extern bool resized_render;

        sf::FloatRect  dt_correctAspect();             //アスペクト比を修正する関数
    }

    namespace WindowStyle{
        constexpr uint8_t defalut  = (1 << 0); //<<デフォルトスタイル
        constexpr uint8_t none     = (1 << 1); //<<タイトルバー
        constexpr uint8_t close_bt = (1 << 2); //<<閉じるボタン
        constexpr uint8_t min_bt  = (1 << 3); //<<最小化ボタン
        constexpr uint8_t max_br  = (1 << 4); //<<最大化ボタン
        constexpr uint8_t title_br = (1 << 5); //<<タイトルバー
    }

    //Window_base.cppの関数
    void                 initWindow(const std::string& window_name,ob2d::Vector2T<uint> window_size,uint8_t window_style_value );
    sf::RenderWindow&    sf_getWindow();                     //参照返し☆
    void                 setFullScreen();                    //フルスクリーン
    void                 setWindowScreen();                  //ウィンドウモード
    void                 setWindowFPSLimit(uint window_fps); //ウィンドウのFPS設定
    float                 getWindowFPS();                     //ウィンドウのFPSを取得
    void                 setWindowSize(ob2d::Vector2T<uint> window_size);
    void                 setWindowPosition(ob2d::Vector2T<uint> window_position);
    ob2d::ObStatus<uint> getWindowStatus();
    void                 moveWindow(ob2d::Vector2T<int> velocity);
    
    void              setWindowName(const std::string &window_name);           //ウィンドウ名
    void              setWindowIcon(const std::string &image_path);
    void              setWindowStyle(uint8_t style_value);
    
    //window_bool.cppの関数
    bool              is_WindowOpen();
    bool              has_WindowFocus();
    bool              is_WindowEnable();
    bool              is_FullScreen();
    bool              is_WindowScreen();

    //window_render.cppの関数
    void setRenderSize(ob2d::Vector2T<uint> render_size);
    void setLetterBoxColor(ob2d::color::Colors fill_color);
    void setRenderAreaColor(ob2d::color::Colors fill_color);
    void setRenderAreaVisible(bool flag);
    bool is_RenderAreaVisible();
    ob2d::Vector2T<uint> getRenderSize();

    //window_close_event.cppの関数
    void setCloseFunction(std::function<void()> event_A,std::function<void()> event_B);
    bool has_CloseFunction();
}

#endif