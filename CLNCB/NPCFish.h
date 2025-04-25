#pragma once
#include "GameObject.h"
#include <SDL.h> // Thường cần include SDL liên quan đến Texture và Renderer
#include <SDL_ttf.h>

class NPCFish : public GameObject {
public:
    NPCFish(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer);
    ~NPCFish();
    NPCFish(const NPCFish&) = delete;
    NPCFish& operator=(const NPCFish&) = delete;
    void Update() override;
    void Render() override;
    void Grow(int delta);
    int GetPoint() const;
    int CalculatePoints() const;
    SDL_RendererFlip flip;
    void SetVelocity(float vx, float vy);


private:
    float velocityX;
    float velocityY;
    float maxSpeed;
    float wanderAngle;
    float angleChangeSpeed;
    int frameCounter;        
    int randomMoveInterval;
    float actualAngle; // góc hiện tại đang di chuyển (xoay dần về wanderAngle)
    
    TTF_Font* scoreFont;
};