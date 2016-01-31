#include "GameState.h"

GameState gameState;
GameState prevGameState;
bool isSoundEnable = true;

GameState getGameState(){
  return gameState;
}

GameState getPrevGameState(){
  return prevGameState;
}

void setGameState(GameState gState){
  prevGameState = gameState;
  gameState = gState;
}

void setSoundState(bool isEnable){
  isSoundEnable = false;
}
void changeSoundState(){
  isSoundEnable = !isSoundEnable;
}
bool getSoundState(){
  return isSoundEnable;
}