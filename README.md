[![Ob2D_logo](https://raw.githubusercontent.com/kaepi2022/Ob2D_web/refs/heads/main/ob2d_logo.png)](https://kaepi2022.github.io/Ob2D)

# Ob2D 🎮 C++専用2Dゲームエンジン

**Ob2D**は、C++で2Dゲームを高速かつ簡単に作れる軽量ゲームエンジンです。  
低レイヤーの操作感覚を保ちつつ、描画・入力・衝突判定・サウンド再生まで一通りサポートします。


もちろんGUIエディタもなく、さらにプログラムのファイル構成に制限はありません全て自分で作れるのです！！

---

## 特徴

- **純C++対応**  
  余計なラッパーなしで、C++そのままの感覚でゲーム開発が可能。

- **高速・軽量**  
  描画や衝突判定も軽快で、小規模〜中規模のゲームに最適。

- **衝突判定サポート**  
  AABBベースの衝突判定を実装済みで、物理表現も簡単。

- **同期/非同期ロード対応**  
  画像や音声をゲームループを止めずにロード可能。

- **拡張性**  
  SFML/OpenGLとの連携APIも用意しているため、既存のオブジェクトをOb2Dに描画可能。

---

## サンプルコードちょい見せ

```C++

#include <iostream>
#include <Ob2d.hpp>

int main(void){
  std::string window_name = "Hello,Ob2D!!";
  ob2d::Vector2T<unsigned int> window_size(500u,400u);

  ob2d::window::initWindow(window_name,window_size);  //500*400のウィンドウ作成
  ob2d::scene::ObScene main_scene;

  ob2d::sounds::ObMusic hello_music("Hello.mp3",true);    //trueは自動再生

  ob2d::object::ObFont   hello_font("hello.ttf");
  ob2d::object::ObText2D sample_text(hello_font,30u,"Hello,Ob2D!");
  
  //レイヤー0にテキストオブジェクトを配置
  ob2d::setScene(sample_text,ob2d::scene::ObLayer::layer_pm0);  

  while(ob2d::window::is_WindowOpen()){
    ob2d::updateSystem();
    
    //いろんな処理

    ob2d::updateRender();
  }

  return 0;
}
```
