#pragma once
#include "game_structures.h"

int loadHighScores(HighScore scores[]);
void sortHighScores(HighScore scores[], int count);
void saveHighScores(HighScore scores[], int count);
void addHighScore(HighScore scores[], int *count, char *name, int score);
