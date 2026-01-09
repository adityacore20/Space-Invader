#include <raylib.h>
#include "game.h"
#include <string>

std::string FormatWithLeadingZeros(int number, int width) {
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    return std::string(leadingZeros, '0') + numberText;
}

int main() {
    Color grey = {29, 29, 27, 255};
    Color yellow = {243, 216, 63, 255};
    int offset = 50;
    int windowWidth = 750;
    int windowHeight = 700;

    InitWindow(windowWidth + offset, windowHeight + 2*offset, "C++ Space Invaders");
    InitAudioDevice();

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");

    SetTargetFPS(60);
    Game game;

    while(!WindowShouldClose()) {
        game.HandleInput();
        game.Update();

        BeginDrawing();
        ClearBackground(grey);

        DrawRectangleRoundedLinesEx({10,10,780,780}, 0.18f, 20, 2, yellow);
        DrawLineEx({25,730}, {775,730}, 3, yellow);

        // Draw UI (score, level, lives)
        if(game.state == GameState::PLAYING || game.state == GameState::LEVEL_CLEARED || game.state == GameState::GAME_OVER) {
            std::string levelText = "LEVEL " + FormatWithLeadingZeros(game.wave, 2);
            DrawTextEx(font, levelText.c_str(), {570,740}, 34, 2, yellow);

            float x = 50.0f;
            for(int i=0;i<game.lives;i++) {
                DrawTextureV(spaceshipImage, {x,745}, WHITE);
                x += 50;
            }

            DrawTextEx(font, "SCORE", {50,15}, 34, 2, yellow);
            std::string scoreText = FormatWithLeadingZeros(game.score,5);
            DrawTextEx(font, scoreText.c_str(), {50,40}, 34, 2, yellow);

            DrawTextEx(font, "HIGH-SCORE", {570,15}, 34, 2, yellow);
            std::string highscoreText = FormatWithLeadingZeros(game.highscore,5);
            DrawTextEx(font, highscoreText.c_str(), {655,40}, 34, 2, yellow);
        }

        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    CloseAudioDevice();
}
