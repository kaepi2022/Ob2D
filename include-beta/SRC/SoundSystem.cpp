#include <iostream>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <memory>
#include <cmath>
#include "Time.hpp"
#include "Window.hpp"
#include "Object.hpp"

#include "Sounds.hpp"

namespace ob2d::sounds{
            //マイクの録音をサポートしているか

    ob2d::Vector3T<float> getListenerPosition(){
        auto pos = sf::Listener::getPosition();
        return {pos.x,pos.y,pos.z};
    }

    void setListenerPosition(ob2d::Vector3T<float> position){
        sf::Listener::setPosition({position.x,position.y,position.z});
    }

    bool is_SupportMic(){
        return sf::SoundBufferRecorder::isAvailable();
    }

    void setGrobalSoundVolume(float volume){
        sf::Listener::setGlobalVolume(volume);
    }

    float getGrobalSoundVolume(){
        return sf::Listener::getGlobalVolume();
    }
}