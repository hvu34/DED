#pragma once
#include <SDL.h>
#include <vector>
#include "Player.h"
#include "NPCFish.h"
#include "Menu.h"
#include <SDL_ttf.h>
#include <string>
#include <vector>
class Game {
public:
    Game();
    ~Game();
    Game(const Game&) = delete; // Delete copy constructor
    Game& operator=(const Game&) = delete; // Delete assignment operator
    bool Init();
    void Run();
    void Clean();
    bool IsGameOver() const { return gameOver; }
    void Reset();
    SDL_Renderer* GetRenderer() const { return renderer; }
    void HandleEvents();
    void Update();
    void Render();
    void SpawnNPCFish(int numFish);


private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* background;
    std::vector<SDL_Texture*> backgrounds;   // tất cả các background
    int currentBackgroundIndex;             // chỉ số background đang dùng

    Player* player;
    std::vector<NPCFish*> enemies;
    Menu* menu;

    bool isRunning;
    bool inMenu;
    bool inGameOver;

    SDL_Texture* LoadTexture(const char* file);

    const Uint8* keyState;

    Uint32 lastSpawnTime;
    Uint32 spawnInterval; // đơn vị: milliseconds

    int level;
    int score;
    int fishEaten;
    int fishToNextLevel;

    TTF_Font* font;

    void RenderText(const std::string& text, int x, int y);
    int bossNextScore;

    bool gameOver;
    bool CheckGameOver() const;
    int bigFishNextScore = 100;  // spawn NPC to hơn Player 100 điểm

};
