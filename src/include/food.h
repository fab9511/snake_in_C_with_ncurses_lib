#pragma once
#include "game_structures.h"

void checkEatingFruit(GameState *state);
void spawnFood(GameState *state, FoodType type);
void changePositionFood(GameState *state);
void drawFood(GameState *state);
