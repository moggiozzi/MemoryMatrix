#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif

enum GameState{
  GS_MENU,
  GS_INGAME,
  GS_GAMEOVER,
  GS_PAUSE,
  GS_EXIT
};

GameState getGameState();
GameState getPrevGameState();
void setGameState(GameState gState);

void setSoundState(bool isEnable);
void changeSoundState();
bool getSoundState();

#endif