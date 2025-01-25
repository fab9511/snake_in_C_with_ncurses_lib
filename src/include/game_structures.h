#pragma once

#define WIDTH 45
#define HEIGHT 45
#define MAX_LENGTH 100
#define MAX_SCORES 10

typedef struct {
    char name[50];
    int score;
} HighScore;

typedef struct {
    int x, y;  // Pozycja
} Segment;

typedef struct {
    Segment segments[MAX_LENGTH]; // Segmenty snake'a
    int length;                   // Aktualna dlugosc 
    char direction;               // Kierunek: 'W', 'A', 'S', 'D'
} Snake;

//rodzaje owocow
typedef enum {
    NORMAL, 
    SPECIAL,
    POISON
} FoodType;

typedef struct {
    int x, y;  // Pozycja jedzenia
    FoodType type;
    bool active;
    time_t lastSpawnTime; 
} Food;

//poziom trudnosci
typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;

typedef struct {
    char board[HEIGHT][WIDTH]; // Plansza gry
    Snake snake;               // gracz
    Food food[3];                 // Jedzenie  0-normal 1-special 2-poison  
    int score;                 // Wynik
    HighScore scores[MAX_SCORES];
    Difficulty difficulty; // Poziom trudnosci
    int scoreCount;     //wynik gracza
    int speed;          //predkosc gry
    int obstacles;      
    time_t startTime;       
    bool gameOver;             // Czy koniec gry??
} GameState;
