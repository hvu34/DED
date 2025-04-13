#pragma once
#include "SDL2/SDL.h"

class Fish {
public:
    Fish(int x, int y, int size);

    // Thêm tham số bool isPlayer vào khai báo
    void Update(bool isPlayer);

    void Render(SDL_Renderer* renderer);
    SDL_Rect GetRect() const;

    void Grow(int amount);
    bool IsBiggerThan(const Fish& other) const;
    void UpdateAuto();

private:
    int x, y;
    int size;
    int vx, vy; // Tốc độ di chuyển của cá NPC
};
