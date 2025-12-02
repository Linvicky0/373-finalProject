#ifndef GAME_H
#define GAME_H

#include <stdint.h>

typedef enum {
    ROCK,
    PAPER,
    SCISSORS,
	NOCHOICE
} Gesture;

typedef enum {
    USER_WINS,
    ROBOT_WINS,
    TIED,
	NOGAME
} GameResult;

struct GameInfo {
	GameResult res;
	Gesture userChoice;
	Gesture robotChoice;
};

#endif /* GAME_H */
