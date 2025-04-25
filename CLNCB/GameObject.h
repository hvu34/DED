#pragma once
#include <SDL.h>

class GameObject {
public:
    GameObject(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer);
    virtual ~GameObject() = default;

    virtual void Update() = 0;
    virtual void Render();

    float GetSize() const;
    bool IsColliding(const GameObject* other) const;
    virtual int CalculatePoints() const = 0;
    int GetPoints() const { return points; }

    int GetX() const { return x; }
    int GetY() const { return y; }

    SDL_Rect getRect() const { return rect; }

protected:
    int x, y, width, height;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    float angle;  // Góc xoay hiện tại (tính bằng độ)
    SDL_RendererFlip flip;
    int points = 0;
    SDL_Rect rect;
    

};
