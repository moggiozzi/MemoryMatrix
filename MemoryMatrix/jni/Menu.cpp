#include "Menu.h"
#include "ResourceManager.h"
#include "GameState.h"
#include "Settings.h"

enum MenuEntry{ 
  ME_TITLE1,
  ME_TITLE2, 
  ME_PLAY, 
  ME_EXIT,

  ME_PAUSE,
  ME_RESUME,
  ME_MENU,

  ME_GAMEOVER,
  ME_TRYAGAIN,

  ME_INPUT_METHOD,
  ME_DISPLAY_ORIENTATION,

  ME_ENTRY_COUNT
};

bool Menu::init(){
  return true;
}

void Menu::draw(){
  //GLHelper::setColor(1.f,1.f,1.f);
  //GLHelper::drawText(50,50,"Hello");
  //GameState gs = getGameState();
  //switch (gs)
  //{
  //case GS_MENU:
  //  break;
  //case GS_PAUSE:
  //  break;
  //case GS_GAMEOVER:
  //  break;
  //default:
  //  break;
  //}
}

void Menu::touch(int x, int y){
}

bool Menu::keyDown(uint keyCode){
  return true;
}