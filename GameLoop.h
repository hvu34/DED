#pragma once
#include "SDL2/SDL.h"
#include "Fish.h"
#include <iostream>
#include <vector>

class GameLoop {
public:
    GameLoop();
    ~GameLoop();

    bool Initialize(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void HandleEvents();
    void Update();
    void Render();
    void Clean();
    bool IsRunning() const;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    SDL_Texture* LoadTexture(const char* filePath); // Khai báo LoadTexture
    SDL_Texture* backgroundTexture;  // Texture cho background
    SDL_Texture* fishTexture;        // Texture cho cá người chơi
    SDL_Texture* npcFishTexture;     // Texture cho cá NPC

    Fish playerFish;                  // 🎮 Cá do người chơi điều khiển
    std::vector<Fish> npcFishes;      // 🐟 Các cá còn lại
};
bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);

