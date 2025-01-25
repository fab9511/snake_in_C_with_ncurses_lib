#include "snakeLib.h"

int loadHighScores(HighScore scores[]) {
    FILE *file = fopen("highscores.txt", "r");
    if (file == NULL) 
        return 0; // Plik nie istnieje, brak wyników

    int count = 0;
    while (count < MAX_SCORES && fscanf(file, "%s %d", scores[count].name, &scores[count].score) == 2) {
        count++;
    }

    fclose(file);
    return count; // ile wyników
}

void sortHighScores(HighScore scores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (scores[j].score < scores[j + 1].score) {
                HighScore temp = scores[j];
                scores[j] = scores[j + 1];
            }
        }
    }
}

void saveHighScores(HighScore scores[], int count) {
    FILE *file = fopen("highscores.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < count && i < MAX_SCORES; i++) {
            fprintf(file, "%s %d\n", scores[i].name, scores[i].score);
        }
        fclose(file);
    }
}

void addHighScore(HighScore scores[], int *count, char *name, int score) {
    if (*count < MAX_SCORES) {
        strcpy(scores[*count].name, name);
        scores[*count].score = score;
        (*count)++;
    } 
    else if (score > scores[*count - 1].score) { 
        strcpy(scores[*count - 1].name, name);
        scores[*count - 1].score = score;
    }

    sortHighScores(scores, *count);
}

void getGameScore(GameState *state) {
    if (state->score > 0) {
        char playerName[50];
        mvprintw(HEIGHT / 2 + 3, WIDTH / 2 - 5, "Enter your name: ");
        refresh();
        echo();
        mvgetstr(HEIGHT / 2 + 4, WIDTH / 2 - 5, playerName);
        noecho();

        addHighScore(state->scores, &state->scoreCount, playerName, state->score);
        saveHighScores(state->scores, state->scoreCount);
    }
}
