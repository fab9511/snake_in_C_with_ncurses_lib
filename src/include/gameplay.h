#pragma once
#include "game_structures.h"

void initializeGame(GameState *state);
void applyDifficultySettings(GameState *state);
void spawnObstacles(GameState *state);
bool checkCollision(GameState *state);
void handleInput(GameState *state);
void updateGame(GameState *state);
void drawBoard(GameState *state);
