#include "Menu.h"
#include "ResourceManager.h"
#include "GameState.h"
#include "Settings.h"

enum MenuEntry{ 
  ME_TITLE,

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

Rect<int> entryRects [ ME_ENTRY_COUNT ];

bool Menu::init(){
  return true;
}

void Menu::draw(){
  GLHelper::setColor(Settings::colorText);
  GameState gs = getGameState();
  int strHeight = GLHelper::getHeight() / 5;
  int symSize = std::min( strHeight, GLHelper::getWidth() / 13 );
  switch (gs)
  {
  case GS_MENU:
    GLHelper::drawText( (GLHelper::getWidth() - 13*symSize)/2, strHeight*1, "MEMORY MATRIX", symSize );
    GLHelper::drawText( (GLHelper::getWidth() - 5*symSize)/2, strHeight*2, "Start", symSize );
    GLHelper::drawText( (GLHelper::getWidth() - 4*symSize)/2, strHeight*3, "Exit", symSize );
  break;
  case GS_PAUSE:
    GLHelper::drawText( (GLHelper::getWidth() - 5*symSize)/2, strHeight*1, "PAUSE", symSize );
    GLHelper::drawText( (GLHelper::getWidth() - 8*symSize)/2, strHeight*2, "Continue", symSize );
    GLHelper::drawText( (GLHelper::getWidth() - 4*symSize)/2, strHeight*3, "Exit", symSize );
  break;
  case GS_GAMEOVER:
    break;
  default:
    break;
  }
}

void Menu::touch(int x, int y){
  int strHeight = GLHelper::getHeight() / 5;
  int row = y / strHeight;
  GameState gs = getGameState();
  switch(gs){
  case GS_MENU:
    switch(row){
      case 2: setGameState( GS_INGAME ); break;
      case 3: exit(0); break;
    }
  break;
  case GS_PAUSE:
    switch(row){
      case 2: setGameState( GS_INGAME ); break;
      case 3: setGameState( GS_MENU ); break;
    }
  break;
  }
}