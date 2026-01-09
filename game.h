#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include "alien.h"
#include "spaceship.h"
#include "obstacle.h"
#include "mysteryship.h"
#include "laser.h"

enum class GameState { MENU, PLAYING, LEVEL_CLEARED, GAME_OVER };

class Game {
public:
    Game();
    ~Game();

    void Update();
    void Draw();
    void HandleInput();

    void InitGame();
    void Reset();
    void GameOver();

    void DeleteInactiveLasers();
    void MoveAliens();
    void MoveDownAliens(int distance);
    void AlienShootLaser();
    void CheckForCollisions();
    std::vector<Obstacle> CreateObstacles();
    std::vector<Alien> CreateAliens();

    void checkForHighscore();
    void saveHighscoreToFile(int highscore);
    int loadHighscoreFromFile();

    // Gameplay variables
    Spaceship spaceship;
    std::vector<Obstacle> obstacles;
    std::vector<Alien> aliens;
    std::vector<Laser> alienLasers;
    MysteryShip mysteryship;

    Music music;
    Sound explosionSound;
    Sound levelClearedSound;
    Font font;

    float alienspeed;
    int aliensDirection;
    int wave;
    int score;
    int highscore;
    int lives;

    double timeLastAlienFired;
    double alienLaserShootInterval = 1.0;
    double timeLastSpawn;
    double mysteryShipSpawnInterval;

    double levelClearedTime;
    double levelClearedDelay = 2.0;

    bool levelCleared;
    bool run;
    Rectangle restartButton;
    Music gameovermusic;
    bool gameOverMusicPlayed = false;
    bool levelClearedSoundPlayed = false;

    // State
    GameState state;
};
