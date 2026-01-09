#include "game.h"
#include <iostream>
#include <fstream>
#include "raylib.h"

Game::Game() {
    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    music = LoadMusicStream("Sounds/music.ogg");
    explosionSound = LoadSound("Sounds/explosion.ogg");
    levelClearedSound = LoadSound("Sounds/level_cleared.ogg");
    gameovermusic= LoadMusicStream("Sounds/game_over.ogg");
    restartButton = {(float)GetScreenWidth()/2 - 120, (float)GetScreenHeight()/2 + 50, 240, 70};

    PlayMusicStream(music);

    state = GameState::MENU;
    alienspeed = 1.0f;
    aliensDirection = 1;
    wave = 1;
    score = 0;
    lives = 3;
    highscore = loadHighscoreFromFile();
    levelCleared = false;
}

Game::~Game() {
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(explosionSound);
    UnloadSound(levelClearedSound);
}

void Game::InitGame() {
    wave = 1;
    alienspeed = 1.0f;
    aliensDirection = 1;
    score = 0;
    lives = 3;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    levelCleared = false;

    obstacles = CreateObstacles();
    aliens = CreateAliens();
    alienLasers.clear();
    spaceship.Reset();
   // mysteryship.Reset();
   StopMusicStream(gameovermusic);  // stop game over music if it was playing
    PlayMusicStream(music);          // start main music
    gameOverMusicPlayed = false;     // reset flag

    state = GameState::PLAYING;
}



void Game::GameOver() {
    state = GameState::GAME_OVER;
    StopMusicStream(music);
    if(!gameOverMusicPlayed) {
        PlayMusicStream(gameovermusic); // start game over music
        gameOverMusicPlayed = true;     // prevent replaying every frame
    }
}


    
    

void Game::Update() {
    
   

    
   
       if(state == GameState::PLAYING || state == GameState::LEVEL_CLEARED) 
       {
        UpdateMusicStream(music);
      }

// Only update game over music if game over
if(state == GameState::GAME_OVER) {
    UpdateMusicStream(gameovermusic);
 }

        if(state==GameState::MENU || state==GameState::GAME_OVER) {
            return;
        }
     


    // LEVEL CLEARED
    if(levelCleared) {
        if(GetTime() - levelClearedTime >= levelClearedDelay) {
            wave++;
            Reset();
            obstacles = CreateObstacles();
            aliens = CreateAliens();
            aliensDirection = 1;
            alienspeed += 0.5f;
            levelCleared = false;
            state = GameState::PLAYING;
        }
        return;
    }

    // Gameplay updates
    double currentTime = GetTime();
    if(currentTime - timeLastSpawn > mysteryShipSpawnInterval) {
        mysteryship.Spawn();
        timeLastSpawn = currentTime;
        mysteryShipSpawnInterval = GetRandomValue(10, 20);
    }

    for(auto& laser : spaceship.lasers) laser.Update();
    MoveAliens();
    AlienShootLaser();
    for(auto& laser : alienLasers) laser.Update();

    DeleteInactiveLasers();
    mysteryship.Update();
    CheckForCollisions();

   // if(aliens.empty()) {
       // levelCleared = true;
       // levelClearedTime = GetTime();
        

     //   PlaySound(levelClearedSound);
        // restart music for next level
      //  state = GameState::LEVEL_CLEARED;
    //}
    if(aliens.empty() && !levelCleared) {
    levelCleared = true;
    levelClearedTime = GetTime(); // mark start time
    StopMusicStream(music);       // pause game music
    PlaySound(levelClearedSound); // play "level cleared" sound
    state = GameState::LEVEL_CLEARED;
    levelClearedSoundPlayed = true;
}
}


// Handle LEVEL_CLEARED state





void Game::HandleInput() {
    if(state == GameState::MENU) {
        Rectangle playButton = {(float)GetScreenWidth()/2 - 120, 350, 240, 70};

        if(IsKeyPressed(KEY_ENTER) ||
           (CheckCollisionPointRec(GetMousePosition(), playButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
            InitGame();
        }
        return;
    }
    if(state == GameState::GAME_OVER) {
        if(IsKeyPressed(KEY_ENTER) ||
           (CheckCollisionPointRec(GetMousePosition(), restartButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
            InitGame(); // restart the game
        }
        return;
    }


    if(state == GameState::PLAYING) {
        if(IsKeyDown(KEY_LEFT)) spaceship.MoveLeft();
        if(IsKeyDown(KEY_RIGHT)) spaceship.MoveRight();
        if(IsKeyDown(KEY_SPACE)) spaceship.FireLaser();
    }
    if(state == GameState::LEVEL_CLEARED)
     {
      // UpdateSound(levelClearedSound); // optional if using streaming sound
    double elapsed = GetTime() - levelClearedTime;
    if(elapsed >= levelClearedDelay) {
        wave++;
        Reset();
        obstacles = CreateObstacles();
        aliens = CreateAliens();
        aliensDirection = 1;
        alienspeed += 0.5f;
        levelCleared = false;
        state = GameState::PLAYING;

        PlayMusicStream(music); // resume game music
    }
    return; // skip other gameplay updates while showing LEVEL_CLEARED
}







}

void Game::Draw() {
    // Draw gameplay elements even if the game is over
    if(state == GameState::PLAYING || state == GameState::LEVEL_CLEARED || state == GameState::GAME_OVER) {
        spaceship.Draw();
        for(auto& laser: spaceship.lasers) laser.Draw();
        for(auto& obstacle: obstacles) obstacle.Draw();
        for(auto& alien: aliens) alien.Draw();
        for(auto& laser: alienLasers) laser.Draw();
        mysteryship.Draw();
    }

    // LEVEL CLEARED
    if(levelCleared) {
        const char* text = "LEVEL CLEARED";
        int fontSize = 40;
        Vector2 textSize = MeasureTextEx(font, text, fontSize, 2);
        Vector2 textPos = {GetScreenWidth()/2 - textSize.x/2, GetScreenHeight()/2 - textSize.y/2};
        DrawTextEx(font, text, textPos, fontSize, 2, YELLOW);
    }

    // GAME OVER
    if(state == GameState::GAME_OVER) {
        const char* text = "GAME OVER";
        int fontSize = 60;
        Vector2 textSize = MeasureTextEx(font, text, fontSize, 4);
        Vector2 textPos = {GetScreenWidth()/2 - textSize.x/2, GetScreenHeight()/2 - textSize.y/2};
        DrawTextEx(font, text, textPos, fontSize, 4, RED);
        DrawRectangleRounded(restartButton, 0.3f, 20, {80, 80, 80, 255});
    DrawRectangleRoundedLines(restartButton, 0.3f, 20, YELLOW);

    const char* restartText = "PLAY AGAIN";
    Vector2 restartTextSize = MeasureTextEx(font, restartText, 40, 2);
    DrawTextEx(font, restartText, {restartButton.x + restartButton.width/2 - restartTextSize.x/2,
                                   restartButton.y + restartButton.height/2 - restartTextSize.y/2},
               40, 2, YELLOW);

    }

    // MENU
    if(state == GameState::MENU) {
        const char* title = "SPACE INVADERS";
        const char* playText = "PLAY";

        float titleSize = 60;
        float playSize = 40;

        Vector2 titleDim = MeasureTextEx(font, title, titleSize, 4);
        Vector2 playDim = MeasureTextEx(font, playText, playSize, 2);
        Vector2 titlePos = {GetScreenWidth()/2 - titleDim.x/2, 200};

        Rectangle playButton = {(float)GetScreenWidth()/2 - 120, 350, 240, 70};

        DrawTextEx(font, title, titlePos, titleSize, 4, YELLOW);
        DrawRectangleRounded(playButton, 0.3f, 20, {80, 80, 80, 255});
        DrawRectangleRoundedLines(playButton, 0.3f, 20, YELLOW);
        DrawTextEx(font, playText, {playButton.x + playButton.width/2 - playDim.x/2,
                                    playButton.y + playButton.height/2 - playDim.y/2}, playSize, 2, YELLOW);
    }
}

void Game::DeleteInactiveLasers()
{
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();){
        if(!it -> active) {
            it = spaceship.lasers.erase(it);
        } else {
            ++ it;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();){
        if(!it -> active) {
            it = alienLasers.erase(it);
        } else {
            ++ it;
        }
    }
}

std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth))/5;

    for(int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }
    return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
    std::vector<Alien> aliens;
    for(int row = 0; row < 5; row++) {
        for(int column = 0; column < 11; column++) {

            int alienType;
            if(row == 0) {
                alienType = 3;
            } else if (row == 1 || row == 2) {
                alienType = 2;
            } else {
                alienType = 1;
            }

            float x = 75 + column * 55;
            float y = 110 + row * 55;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    for(auto& alien: aliens) {
        if(alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if(alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection*alienspeed);
    }
}

void Game::MoveDownAliens(int distance)
{
    for(auto& alien: aliens) {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if(currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type -1].width/2, 
                                    alien.position.y + alien.alienImages[alien.type - 1].height}, 6));
        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions()
{
    //Spaceship Lasers

    for(auto& laser: spaceship.lasers) {
        auto it = aliens.begin();
        while(it != aliens.end()){
            if(CheckCollisionRecs(it -> getRect(), laser.getRect()))
            {
                PlaySound(explosionSound);
                if(it -> type == 1) {
                    score += 100;
                } else if (it -> type == 2) {
                    score += 200;
                } else if(it -> type == 3) {
                    score += 300;
                }
                checkForHighscore();

                it = aliens.erase(it);
                laser.active = false;
            } else {
                ++it;
            }
        }

        for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }

        if(CheckCollisionRecs(mysteryship.getRect(), laser.getRect())){
            mysteryship.alive = false;
            laser.active = false;
            score += 500;
            checkForHighscore();
            PlaySound(explosionSound);
        }
    }

    //Alien Lasers

    for(auto& laser: alienLasers) {
        if(CheckCollisionRecs(laser.getRect(), spaceship.getRect())){
            laser.active = false;
            lives --;
            if(lives == 0) {
                GameOver();
            }
        }

          for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }
    }

    //Alien Collision with Obstacle
    
    for(auto& alien: aliens) {
        for(auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()) {
                if(CheckCollisionRecs(it -> getRect(), alien.getRect())) {
                    it = obstacle.blocks.erase(it);
                } else {
                    it ++;
                }
            }
        }

        if(CheckCollisionRecs(alien.getRect(), spaceship.getRect())) {
            GameOver();
        }
    }
}

/*void Game::GameOver()
{
    run = false;
}

//void Game::InitGame()
//{
 //   obstacles = CreateObstacles();
 //   aliens = CreateAliens();
  //  aliensDirection = 1;
  //  timeLastAlienFired = 0.0;
  //  timeLastSpawn = 0.0;
  //  lives = 3;
 
  score = 0;
    highscore = loadHighscoreFromFile();
    run = true;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
}
 */  

void Game::checkForHighscore()
{
    if(score > highscore) {
        highscore = score;
        saveHighscoreToFile(highscore);
    }
}

void Game::saveHighscoreToFile(int highscore)
{
    std::ofstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile << highscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to save highscore to file" << std::endl;
    }
}

int Game::loadHighscoreFromFile() {
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to load highscore from file." << std::endl;
    }
    return loadedHighscore;
}

void Game::Reset() {
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
     //alienspeed = 1.0f;
    //aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
    levelCleared = false;

}
   