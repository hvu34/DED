#pragma once
#include "GameObject.h"
#include <SDL_ttf.h>

class Player : public GameObject {
public:
    Player(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer);
    Player(const Player&) = delete; // Delete copy constructor
    Player& operator=(const Player&) = delete; // Delete assignment operator
    void Update() override;
    void HandleInput(const Uint8* keyState);


    void Grow(int delta);
    void Reset();
    float GetSize() const { return rect.w; };
    bool IsColliding(GameObject* other) const;
    int CalculatePoints() const override;
    void Render() override;
    SDL_Rect getRect() const;

private:
    TTF_Font* scoreFont;
    SDL_RendererFlip flip;
    int points;
    int moveSpeed;
    float velX = 0.0f, velY = 0.0f;   // vận tốc
    float accel = 1.5f;               // gia tốc
    float maxSpeed = 5.5f;            // giới hạn tốc độ
    bool movingLeft = false;
    bool movingRight = false;
    bool movingUp = false;
    bool movingDown = false;


};
