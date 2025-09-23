#include <map>
#include <vector>
#include <variant>
#include <memory>

#include <SFML/Graphics.hpp>    

#include "System.hpp"
#include "Object.hpp"

namespace ob2d::object{
    void ob2d::object::ObTexture::setPixels(const std::vector<std::vector<ob2d::color::Colors>>& pixel_data){
        uint width = 0u,height = 0u;
        height = pixel_data.size();     //高さを取得
        for(auto& it : pixel_data) width = std::max(width,static_cast<uint>(it.size()));    //横の長さを取得
                    
        //テクスチャ初期化
        texture_base = std::make_unique<sf::Image>();
        texture_base.get()->resize({width,height});

        int width_count = 0,height_count = 0;

        for(auto& it : pixel_data){
            width_count = 0;

            for(auto &pixel : it){
                texture_base.get()->setPixel({width_count,height_count},pixel);
                ++width_count;
            }

            if(width_count < width){
                for(int i = 0; i < (width - width_count); i++){
                    texture_base.get()->setPixel({static_cast<uint>(width_count + i),static_cast<uint>(height_count)},ob2d::color::Colors(0u,0u,0u,0u));
                }
            }
            ++height_count;
            }

            texture.loadFromImage(*texture_base.get());
            texture_base = nullptr;   //おさらばだー

        }

    std::vector<std::vector<ob2d::color::Colors>> ob2d::object::ObTexture::getPixels(){

        std::vector<std::vector<ob2d::color::Colors>> pixel_info;

        const uint8_t * pixels = texture.copyToImage().getPixelsPtr();
        auto texture_size = texture.getSize();


        for(int i = 0; i < texture_size.y; i++){
            pixel_info.push_back({});       //カラーを保存するベクトルを作成
            for(int j = 0; j < texture_size.x; j+4){    //RGBAのカラーを4つ取得する。
                auto pixel_color = ob2d::color::Colors(pixels[i],pixels[i+1],pixels[i+2],pixels[i+3]);
                (pixel_info.begin() + i)->push_back(pixel_color);
            }
        }

        return pixel_info;
    }

    ob2d::object::ObTexture::ObTexture(std::variant<std::string,std::vector<std::vector<ob2d::color::Colors>>> texture_data){
        if(std::holds_alternative<std::string>(texture_data)){
            bool test = texture.loadFromFile(std::get<std::string>(texture_data));
        }
        else if(std::holds_alternative<std::vector<std::vector<ob2d::color::Colors>>>(texture_data)){
            setPixels(std::get<std::vector<std::vector<ob2d::color::Colors>>>(texture_data));
        }
    }

    bool ob2d::object::ObTexture::loadTexture(const std::string &texture_path){
        if(!texture.loadFromFile(texture_path)){
            return false;
        }

        return true;
    }

    ob2d::Vector2T<uint> ob2d::object::ObTexture::getSize(){
        auto texture_size = texture.getSize();
        return ob2d::Vector2T<uint>(texture_size.x,texture_size.y);
    }

    void ob2d::object::ObTexture::setRepeated(bool flag){
        texture.setRepeated(flag);
    }

    void ob2d::object::ObTexture::setSmooth(bool flag){
        texture.setSmooth(flag);
    }

    bool ob2d::object::ObTexture::saveToFile(const std::string &file_path){
        return texture.copyToImage().saveToFile(file_path);
    }

    bool ob2d::object::ObTexture::is_Repeated(){ return texture.isRepeated(); }
    bool ob2d::object::ObTexture::is_Smooth(){ return texture.isSmooth(); }

    sf::Texture* ob2d::object::ObTexture::sf_getTexture(){
        return &texture;
    }
}