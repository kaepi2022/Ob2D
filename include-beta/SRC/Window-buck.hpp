#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "System.hpp"
#include "timer.hpp"

namespace ob2d{


    /**
    * @brief ウィンドウ管理に関する機能を提供する名前空間。
    * ウィンドウの生成・操作・イベント管理などを扱います。
    */
    namespace window{

        namespace SFML{
            /**
             * @brief SFMLのRenderWindowインスタンス（上級者向け直接操作用）                 *
             * これはOb2Dのラッパー機能の下にある
             * 生のSFMLのレンダリングウィンドウです。
             * より細かい制御が必要な場合は直接このwindowを操作することも可能です。
             * ただし、通常はOb2DのAPIを使うことを推奨します。
            */
            std::shared_ptr<sf::RenderWindow> window_ptr = nullptr;
            //sf::RenderWindow window(sf::VideoMode({0,0}),"Window");
        };

        namespace detail{

            std::function<void()> close_event = nullptr;  /** @brief ウィンドウが閉じられたときに呼ばれるコールバック関数 */
            uint window_FPS = 0;
            bool is_window_open = true;

            float mouse_wheel_delta = 0.f;

            Vector2T<float> game_screen_ofset;
            Vector2T<float> scaled_gamesize;
            float scale = 1.f;      //inf対策


            //ウィンドウモードの定義
            enum class WindowMode{
                Windowed,
                FullScreen
            };

            uint16_t window_style = sf::Style::Default;
            WindowMode window_mode = WindowMode::Windowed;      //デフォルトのウィンドウ設定
            std::string window_name = "";                       //ウィンドウ名を保存
            ob2d::Vector2T<uint> game_size = {400,400};                   //ウィンドウのサイズを保存

            bool is_window_fixed_type = false;

            void applyWindowConfig(){
                if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                SFML::window_ptr.get()->create(sf::VideoMode({game_size.x,game_size.y}),sf::String::fromUtf8(window_name.begin(),window_name.end()),window_style,static_cast<sf::State>(window_mode));
                SFML::window_ptr.get()->setFramerateLimit(window_FPS);
            }
        }
        
        enum WindowStyle{
            default_style    = (1 << 0),
            title_bar        = (1 << 1),
            resize_button    = (1 << 2),
            close_button     = (1 << 3)
        };

        /**
         * @brief ウィンドウが閉じられた際に呼び出すコールバック関数の登録・解除を管理します。
         *
         * ウィンドウの閉じる操作を検知し、登録された関数を自動で呼び出します。
         */
        namespace closeEvent{

            /**
             * @brief ウィンドウが閉じられたときに呼び出す関数を設定します。
             *
             * 例えば「ゲームデータの保存」などを自動で実行したいときに使えます。
             * 
             * @param func 閉じたときに呼ばれる関数（引数も戻り値も void の関数）
             */
            void setCloseEvent(std::function<void()> close_func){
                if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                detail::close_event = close_func;
            }

            /**
             * @brief ウィンドウを閉じるときのイベントを無効にします。
             *
             * すでに設定してあるコールバック関数を解除します。
             */
            void disableCloseEvent(){
                if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
                detail::close_event = nullptr;
            }
        };
        


        constexpr unsigned int WINDOW_FPS_DEFAULT = 60;         /** @brief windowのデフォルトFPSです。 60FPSを指します */



        /**
         * @brief 現在のウィンドウサイズ（幅と高さ）を返します。
         *
         * 幅 (x) と 高さ (y) の 2つの整数を1つの構造体にまとめて返します。
         * 
         * @return 現在のウィンドウサイズ（単位はピクセル）
         */
        ob2d::Vector2T<float> getWindowSize(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            auto window_size = SFML::window_ptr.get()->getSize();
            return {static_cast<float>(window_size.x),static_cast<float>(window_size.y)};
        }

        ob2d::Vector2T<float> getGameSize(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            auto camera_size = SFML::window_ptr.get()->getView().getSize();
            return {camera_size.x,camera_size.y};
        }

        /**
         * @brief 現在のウィンドウのフレームレート（FPS）を返します。
         * 
         * initialize_Window関数で指定されたFPS値を取得できます。
         * 
         * @return ウィンドウのフレームレート（単位：frames per second）
         */
        ob2d::uint getWindowFPSLimit(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            return detail::window_FPS;
        }
        
        float getWindowFPS(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            return (1.f / time::detail::delta_time_sec);
        }
        
        /**
         * @brief ウィンドウのタイトルバーの名前を変更します。
         *
         * UTF-8対応しているので日本語や記号も使用可能です。
         * 
         * @param window_name 新しいウィンドウの名前
         */
        void setWindowTitle(const std::string &window_name){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            detail::window_name = window_name;
            SFML::window_ptr.get()->setTitle(sf::String::fromUtf8(window_name.begin(), window_name.end()));
        }

        void setWindowFPS(ob2d::uint FPS){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            SFML::window_ptr.get()->setFramerateLimit(FPS);
            detail::window_FPS = FPS;
        }

        void setWindowPosition(ob2d::Vector2T<float> position){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            SFML::window_ptr.get()->setPosition({static_cast<int>(position.x),static_cast<int>(position.y)});
        }

        ob2d::Vector2T<float> getWindowPosition(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            auto window_pos = SFML::window_ptr.get()->getPosition();
            return {static_cast<float>(window_pos.x),static_cast<float>(window_pos.y)};
        }   

        /**
         * @brief ウィンドウの左上に表示されるアイコン画像を変更します。
         *
         * 64×64の画像サイズを推奨します。画像形式はPNGやJPGが使えます。
         * 
         * @param icon_path アイコン画像ファイルのパス
         */
        ob2d::logs::LogCode setWindowIcon(const std::string &icon_path){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            sf::Image icon;
            if(icon.loadFromFile(icon_path)){
                SFML::window_ptr.get()->setIcon({64,64},icon.getPixelsPtr());
                return ob2d::logs::codes::OB2D_SUCCESS;
            }else{
                return ob2d::logs::codes::OB2D_ICON_LOAD_FAILED;
            }
        }

        /**
         * @brief ウィンドウが現在ユーザーに選択されているかを返します。
         *
         * 他のアプリにフォーカスがある場合などは false になります。
         * 
         * @return 選択状態なら true、そうでなければ false
         */
        bool isWindowSelected(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            return SFML::window_ptr.get()->hasFocus();
        }

        /**
         * @brief ウィンドウが閉じられていないかどうかを返します。
         *
         * ゲームループを継続する条件として使うことが多いです。
         * 
         * @return ウィンドウが開いていれば true、閉じていれば false
         */
        bool isWindowOpen(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            return detail::is_window_open;
        }



        /**
         * @brief ゲームを強制的に閉じます。
         *
         * 閉じるイベントが設定されていれば、それも同時に呼ばれます。
         */
        void closeGame(){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);
            if(ob2d::window::detail::close_event) detail::close_event();
            detail::is_window_open = false;
            SFML::window_ptr.get()->close();
        }

        void setVSyncEnabled(bool enabled){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            SFML::window_ptr.get()->setVerticalSyncEnabled(enabled);
        }
        


        void setWindowStyle(uint8_t window_style){
            if(!SFML::window_ptr) ob2d::detail::errorPrintf(ob2d::detail::error::WINDOW_EMPTY);

            if((window_style & WindowStyle::default_style) > 0){
                detail::window_style = sf::Style::Default;
                detail::applyWindowConfig();
                return;
            }

            if((window_style & WindowStyle::title_bar) > 0)     detail::window_style |= sf::Style::Titlebar;
            if((window_style & WindowStyle::close_button) > 0)  detail::window_style |= sf::Style::Close;
            if((window_style & WindowStyle::resize_button) > 0) detail::window_style |= sf::Style::Resize;
            detail::applyWindowConfig();
        }




        };

    };


#endif