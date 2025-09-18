int main(void){
  std::string window_name = "Hello,Ob2D!!";
  ob2d::Vector2T<unsigned int> window_size(500u,400u);

  ob2d::window::initWindow(window_name,window_size);  //500*400のウィンドウ作成
  ob2d::scene::ObScene main_scene;

  ob2d::sounds::ObMusic hello_music("Hello.mp3",true);    //trueは自動再生

  ob2d::object::ObFont   hello_font("hello.ttf");
  ob2d::object::ObText2D sample_text(hello_font,30u,"Hello,Ob2D!");
  
  //レイヤー0にテキストオブジェクトを配置
  ob2d::scene::addObject(sample_text,ob2d::scene::ObLayer::layer_pm0);

  ob2d::setScene(main_scene);      //シーンを設定する(描画対象化)

  while(ob2d::window::is_WindowOpen()){
    ob2d::updateSystem();
    
    //いろんな処理

    ob2d::updateRender();
  }

  return 0;
}
