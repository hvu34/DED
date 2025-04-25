#include <cmath>  // Để tính toán độ dài vector
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "Player.h"

Player::Player(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer)
    : GameObject(x, y, w, h, tex, renderer),
    scoreFont(nullptr),
    points(0),
    flip(SDL_FLIP_NONE)
{
    // Khởi tạo font cho hiển thị điểm
    scoreFont = TTF_OpenFont("timesbd.ttf", 16);

    if (!scoreFont) {
        std::cerr << "Failed to load score font: " << TTF_GetError() << std::endl;
    }
    // Tính điểm ban đầu
    points = CalculatePoints();
    moveSpeed = 5;
}

SDL_Rect Player::getRect() const {
    return { x, y, width, height };
}

int Player::CalculatePoints() const {
    // Điểm cơ bản dựa trên kích thước
    int basePoints = rect.w * rect.h / 100;
    return basePoints;
}

void Player::HandleInput(const Uint8* keyState) {
    movingLeft = keyState[SDL_SCANCODE_LEFT];
    movingRight = keyState[SDL_SCANCODE_RIGHT];
    movingUp = keyState[SDL_SCANCODE_UP];
    movingDown = keyState[SDL_SCANCODE_DOWN];

}

void Player::Update() {
    points = CalculatePoints(); // cập nhật điểm

    // Gia tốc nếu đang nhấn
    if (movingLeft)  velX -= accel;
    if (movingRight) velX += accel;
    if (movingUp)    velY -= accel;
    if (movingDown)  velY += accel;

    // Giảm tốc dần nếu không nhấn
    velX *= 0.9f;
    velY *= 0.9f;

    // Giới hạn tốc độ tối đa
    velX = std::clamp(velX, -maxSpeed, maxSpeed);
    velY = std::clamp(velY, -maxSpeed, maxSpeed);

    // Cập nhật vị trí
    rect.x += static_cast<int>(velX);
    rect.y += static_cast<int>(velY);

    // Cập nhật hướng lật ảnh theo hướng di chuyển
    if (velX < 0) {
        flip = SDL_FLIP_HORIZONTAL;
    }
    else if (velX > 0) {
        flip = SDL_FLIP_NONE;
    }


    // Giữ trong màn hình
    rect.x = std::max(0, std::min(rect.x, 800 - rect.w));
    rect.y = std::max(0, std::min(rect.y, 600 - rect.h));
}

void Player::Grow(int delta) {
    rect.w += delta;
    rect.h += delta;
    points = CalculatePoints(); // Cập nhật điểm khi lớn lên
}

void Player::Render() {
    // Vẽ cá
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, flip);

    // Vẽ điểm số
    if (scoreFont) {
        SDL_Color color = { 255, 255, 255 }; // Màu trắng
        std::stringstream ss;
        ss << points;
        std::string scoreText = ss.str();

        SDL_Surface* surface = TTF_RenderText_Blended(scoreFont, scoreText.c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect textRect = { rect.x + rect.w/2 - surface->w/2, rect.y - 20, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void Player::Reset() {
    // Reset về vị trí ban đầu
    rect.x = 100;
    rect.y = 100;
    // Reset về kích thước ban đầu
    rect.w = 64;
    rect.h = 64;
    // Reset điểm
    points = CalculatePoints();
    // Reset hướng
    flip = SDL_FLIP_NONE;
}

bool Player::IsColliding(GameObject* other) const {
    if (!other) return false;
    
    SDL_Rect otherRect = other->getRect();
    return (rect.x < otherRect.x + otherRect.w &&
            rect.x + rect.w > otherRect.x &&
            rect.y < otherRect.y + otherRect.h &&
            rect.y + rect.h > otherRect.y);
}
