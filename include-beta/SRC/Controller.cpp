#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <map>
#include <array>

#include "System.hpp"
#include "hid.hpp"

namespace ob2d::hid::controller{
    namespace detail{
        std::array<bool,8> player_sfml_id_used_status = {false,false,false,false,false,false,false,false};
        std::array<Player*,8> players_list            = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
        Player* wait_player = nullptr;
        bool is_waiting_join_player = false;
    }

    const std::string nulldevice = "nulldevice";
    
    ob2d::hid::controller::Player::Player(){

    }
    ob2d::hid::controller::Player::~Player(){
        disconnectController();
    }

    bool ob2d::hid::controller::Player::is_ActiveController(){
        return SFML_buttonID != ob2d::hid::controller::Player::nullController;
    }

    bool ob2d::hid::controller::Player::disconnectController(){
        if(!is_ActiveController()) return false;

        detail::players_list[SFML_buttonID] = nullptr;
        detail::player_sfml_id_used_status[SFML_buttonID] = false;
        SFML_buttonID = nullController;

        return true;
    }
    
    uint8_t* ob2d::hid::controller::Player::dt_getdata(){
        return &SFML_buttonID;
    }

    constexpr uint8_t deadzone = 5;    //しきい値

    void ob2d::hid::controller::Player::dt_update(){
        if(!is_ActiveController()) return;
        

        if(!sf::Joystick::isConnected(SFML_buttonID)) disconnectController();

        now_button_status = 0;

        stick_L_position = { sf::Joystick::getAxisPosition(SFML_buttonID,stick_status[0].x), sf::Joystick::getAxisPosition(SFML_buttonID,stick_status[0].y) };
        stick_R_position = { sf::Joystick::getAxisPosition(SFML_buttonID,stick_status[1].x), sf::Joystick::getAxisPosition(SFML_buttonID,stick_status[1].y) };

        auto ZL_ = sf::Joystick::getAxisPosition(SFML_buttonID,ZLR_trigger_status[0]);
        ZL_pressure = ((ZL_ < 0 ?  100 - (ZL_ * -1) : ZL_ + 100) / 200) * 100;

        auto ZR_ = sf::Joystick::getAxisPosition(SFML_buttonID,ZLR_trigger_status[1]);
        ZR_pressure = ((ZR_ < 0 ?  100 - (ZR_ * -1) : ZR_ + 100) / 200) * 100;



        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_A]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_A;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_B]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_B;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_X]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_X;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_Y]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_Y;

        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_L]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_L;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_R]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_R;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_ZL]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_ZL;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_ZR]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_ZR;

        if(sf::Joystick::getAxisPosition(SFML_buttonID,sf::Joystick::Axis::PovY) >  deadzone) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_UP;
        if(sf::Joystick::getAxisPosition(SFML_buttonID,sf::Joystick::Axis::PovY) < -deadzone) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_DOWN;
        if(sf::Joystick::getAxisPosition(SFML_buttonID,sf::Joystick::Axis::PovX) < -deadzone) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_LEFT;
        if(sf::Joystick::getAxisPosition(SFML_buttonID,sf::Joystick::Axis::PovX) >  deadzone) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_RIGHT;

        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_START]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_START;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_SERECT]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_SERECT;

        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_STICK_L]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_STICK_L;
        if(sf::Joystick::isButtonPressed(SFML_buttonID,key_map[ob2d::hid::controller::Buttons::BUTTON_STICK_R]) ) now_button_status |= ob2d::hid::controller::Buttons::BUTTON_STICK_R;

        button_trigger = ~bef_button_status & now_button_status;
        button_release =  bef_button_status & ~now_button_status;
        button_hold    =  now_button_status;

        bef_button_status = now_button_status;
    }

    std::map<uint16_t,uint8_t>& ob2d::hid::controller::Player::getKeyMap(){
        return key_map;
    }

    void setPlayer(ob2d::hid::controller::Player &target_player){
        detail::is_waiting_join_player = true;
        detail::wait_player = &target_player;
    }

    std::string ob2d::hid::controller::Player::getControllerDeviceName(){
        if(!is_ActiveController())  return ob2d::hid::controller::nulldevice;
        return sf::Joystick::getIdentification(SFML_buttonID).name;
    }

    void ob2d::hid::controller::Player::setStickTypes(ob2d::Vector2T<ob2d::hid::controller::StickType> L_stick,ob2d::Vector2T<ob2d::hid::controller::StickType> R_stick){
        stick_status[0] = {static_cast<sf::Joystick::Axis>(L_stick.x),static_cast<sf::Joystick::Axis>(L_stick.y)};
        stick_status[1] = {static_cast<sf::Joystick::Axis>(R_stick.x),static_cast<sf::Joystick::Axis>(R_stick.y)};
    }

    void ob2d::hid::controller::Player::setTriggerPressureType(ob2d::hid::controller::StickType BUTTON_ZL,ob2d::hid::controller::StickType BUTTON_ZR){
        ZLR_trigger_status[0] = {static_cast<sf::Joystick::Axis>(BUTTON_ZL)};
        ZLR_trigger_status[1] = {static_cast<sf::Joystick::Axis>(BUTTON_ZR)};
    }
    
}