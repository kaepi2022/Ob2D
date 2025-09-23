#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <cstdlib>
#include <map>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

#include "System.hpp"

namespace ob2d{
    using uint = unsigned int;

    namespace scene::detail{

            uint16_t scene_system_manager = 0u;       //システム的にシーンを管理する。
            uint16_t now_scene_id = 0u;               //ゲームで設定されているシーンID
    }

    namespace time::detail{
            float window_fps = 0.f;
    }

    namespace detail{
        bool resized_rendorsize = false;
        bool inited_window      = false;
    
        void errorPrintf(error codes){
            //switch(codes){
            //    case error::WINDOW_EMPTY:
            //        std::cout << "\033[31m Error CODE 01 \033[0m: \n The window has not been initialized!\nInitialize it with ob2d::window::initWindow()!!\n\n\n Window ga syokika sareteimasenn!\nob2d::window::initWindow() wo yobidasite syokika site kudasai !" << std::endl;
            //}
        }

        

    };


    void openURL(const std::string &web_url) {
        if (web_url.find("http", 0) == 0) {
            std::string command = "start \"\" " + web_url;
            std::system(command.c_str());
        }
    }

    namespace sfml_tool{
        sf::String sf_convertString(const std::string &string){
            return sf::String::fromUtf8(string.begin(), string.end());
        }
    }

    namespace color {

        



        Colors HexColor(std::string color_code_hex) {
            if (!(color_code_hex.length() == 7) || color_code_hex.substr(0, 1) != "#")
                return Colors(0, 0, 0);
            return Colors(
                std::stoi(color_code_hex.substr(1, 2), nullptr, 16),
                std::stoi(color_code_hex.substr(3, 2), nullptr, 16),
                std::stoi(color_code_hex.substr(5, 2), nullptr, 16)
            );
        }

    }
};