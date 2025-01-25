#pragma once

#include <stdio.h>  //funckje z ktorych korzystaja pozostale pliki
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#include "food.h"          // Funkcje związane z owocami (spawnFood, drawFood, checkEatingFruit itd.)
#include "gameplay.h"      // Funkcje związane z rozgrywką (initializeGame, updateGame, drawBoard itd.)
#include "game_structures.h" // Struktury danych (GameState, FoodType, HighScore itd.)
#include "highscores.h"    // Operacje na wynikach (loadHighScores, saveHighScores, sortHighScores itd.)
#include "menu.h"          // Funkcje obsługujące menu (displayMenu, chooseDifficulty itd.)
#include "snake.h"         // Funkcje związane z wężem (growSnake, obsługa ruchów itd.)
#include "snakeLib.h"      // Główna biblioteka funkcji wspólnych (może zawierać nagłówki do wszystkich funkcji)
#include "utility.h"       // Funkcje pomocnicze (drawTimer, inne pomocne narzędzia)

