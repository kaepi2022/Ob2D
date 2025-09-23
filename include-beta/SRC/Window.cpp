#include <iostream>
#include <array>
#include <string>
#include <memory>
#include <functional>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "Window.hpp"

namespace ob2d::window{
    namespace detail{
        std::shared_ptr<sf::RenderWindow>   ObWindow      = nullptr;   //ウィンドウ
        std::unique_ptr<sf::Image>          window_icon   = nullptr;

        bool    is_FullScreen   = false;
        bool    is_WindowScreen = false;

    }

    void initWindow(const std::string& window_name,ob2d::Vector2T<uint> window_size,uint8_t window_style_value){
        detail::ObWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(window_size.x,window_size.y)),"test");
        setRenderSize({window_size.x,window_size.y});
        setWindowSize({window_size.x,window_size.y});
        ob2d::detail::inited_window = true;
    }

    sf::RenderWindow& sf_getWindow(){
        if(!detail::ObWindow) exit(0);
        return *detail::ObWindow.get();
    }

    void setWindowFPSLimit(uint window_fps){
        if(!is_WindowEnable()) return;
        sf_getWindow().setFramerateLimit(window_fps);
    } 

    float getWindowFPS(){
        return ob2d::time::detail::window_fps;
    }

    void setWindowSize(ob2d::Vector2T<uint> window_size){
        if(!is_WindowEnable()) return;
        sf_getWindow().setSize(window_size.sf_getVector());
    }

    void setWindowPosition(ob2d::Vector2T<uint> window_position){
        if(!is_WindowEnable()) return;

        sf_getWindow().setPosition({static_cast<uint>(window_position.x),static_cast<uint>(window_position.y)});
    }

    ob2d::ObStatus<uint> getWindowStatus(){
        if(!is_WindowEnable()) return {0u,0u};
        auto position = sf_getWindow().getPosition();
        auto size     = sf_getWindow().getSize();

        return {
            ob2d::Vector2T<uint>(size.x,size.y),
            ob2d::Vector2T<uint>(position.x + size.x / 2u,position.y + size.y  / 2u),
            ob2d::Vector2T<uint>(position.x,position.y),
            ob2d::Vector2T<uint>(position.x,position.y + size.y),
            ob2d::Vector2T<uint>(position.x + size.x,position.y),
            ob2d::Vector2T<uint>(position.x + size.x,position.y + size.y)
        };
    }

    void setWindowName(const std::string &window_name){
        if(!is_WindowEnable()) return;
        sf_getWindow().setTitle(ob2d::sfml_tool::sf_convertString(window_name));
    }

    void setWindowIcon(const std::string &image_path){
        if(detail::window_icon == nullptr) detail::window_icon = std::make_unique<sf::Image>();

        if(!detail::window_icon->loadFromFile(image_path)){
            return;
        }

        sf_getWindow().setIcon(*detail::window_icon.get());
    }

    void moveWindow(ob2d::Vector2T<int> velocity){
        if(is_FullScreen || !is_WindowEnable()) return;

        auto window_pos = sf_getWindow().getPosition();
        sf_getWindow().setPosition({window_pos.x + velocity.x,window_pos.y + velocity.y});
    }
}

namespace ob2d::window{
    namespace detail{
        sf::RectangleShape                  RenderArea;
        ob2d::color::Colors                 LetterBox;
        bool                                is_VisibleRenderArea = true;
        ob2d::Vector2T<uint>                render_size = {1,1};

        ob2d::Vector2T<float>               view_offset;
        ob2d::Vector2T<float>               view_viewport;
        float                               view_render_scale = 1.f;

        bool resized_render = false;

        sf::FloatRect dt_correctAspect(){
            auto window_size = sf_getWindow().getSize();
            float scaleX = static_cast<float>(window_size.x) / static_cast<float>(render_size.x);
            float scaleY = static_cast<float>(window_size.y) / static_cast<float>(render_size.y);
            view_render_scale = std::min(scaleX, scaleY);

            // 中央寄せ (黒帯対応)
            view_viewport.x = ((float)render_size.x * view_render_scale) / (float)window_size.x;
            view_viewport.y = ((float)render_size.y * view_render_scale) / (float)window_size.y;
            view_offset.x = (1.f - view_viewport.x) / 2.f;
            view_offset.y = (1.f - view_viewport.y) / 2.f;
            return {{view_offset.x,view_offset.y},{view_viewport.x,view_viewport.y}};
        }
    }

    void setRenderSize(ob2d::Vector2T<uint> render_size){
        if(!is_WindowEnable()) return;
        detail::render_size = render_size;
        auto size = render_size.sf_getVector();
        detail::RenderArea.setSize({static_cast<int>(size.x),static_cast<int>(size.y)});
        detail::resized_render = true;
    }
    
    //window_render.cppの関数
    void setLetterBoxColor(ob2d::color::Colors fill_color){
        if(!is_WindowEnable()) return;
        detail::LetterBox = fill_color;
    }

    void setRenderAreaColor(ob2d::color::Colors fill_color){
        if(!is_WindowEnable()) return;
        detail::RenderArea.setFillColor(fill_color);
    }

    void setRenderAreaVisible(bool flag){
        detail::is_VisibleRenderArea = flag;
    }

    ob2d::Vector2T<uint> getRenderSize(){
        return detail::render_size;
    }

    bool is_RenderAreaVisible(){ return detail::is_VisibleRenderArea; };
}

namespace ob2d::window{
    namespace detail{
        std::array<std::function<void()>,2> close_event   = {nullptr,nullptr};
    }

    void setCloseFunction(std::function<void()> event_A,std::function<void()> event_B = nullptr){
        if(event_A != nullptr) detail::close_event[0] = event_A;
        if(event_B != nullptr) detail::close_event[1] = event_B;
    }

    bool has_CloseFunction(){ return (detail::close_event[0] != nullptr) || (detail::close_event[1] != nullptr); }

}

namespace ob2d::window{

    bool is_WindowOpen(){ return sf_getWindow().isOpen(); }
    bool is_WindowEnable(){ return detail::ObWindow != nullptr; }
    bool has_WindowFocus(){ return sf_getWindow().hasFocus(); }
    bool is_FullScreen(){ return detail::is_FullScreen; }
    bool is_WindowScreen(){ return detail::is_WindowScreen; }

}