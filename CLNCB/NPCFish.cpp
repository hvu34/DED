// NPCFish.h
#pragma once
#include "GameObject.h"
#include <SDL.h>
#include <SDL_ttf.h> // để dùng TTF_Font
#include <iostream>
#include <string>
#include "NPCFish.h"

// NPCFish.cpp
#include "NPCFish.h"
#include <random>
#include <algorithm>
#include <cmath>

NPCFish::NPCFish(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer)
    : GameObject(x, y, w, h, tex, renderer),
    velocityX(0), velocityY(0),
    maxSpeed(2.0f),
    wanderAngle(0),
    angleChangeSpeed(0.1f),
    frameCounter(0),
    randomMoveInterval(60),
    actualAngle(0),
    flip(SDL_FLIP_NONE)
{
    scoreFont = TTF_OpenFont("timesbd.ttf", 16);
    if (!scoreFont) {
        std::cerr << "Failed to load NPCFish font: " << TTF_GetError() << std::endl;
    }
}

NPCFish::~NPCFish() {
}

void NPCFish::Update() {
    frameCounter++;
    
    // Thay đổi góc di chuyển ngẫu nhiên sau mỗi khoảng thời gian
    if (frameCounter % randomMoveInterval == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-M_PI, M_PI);
        wanderAngle = dis(gen);
    }

    // Dần dần xoay về góc mong muốn
    float angleDiff = wanderAngle - actualAngle;
    while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
    while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
    actualAngle += angleDiff * angleChangeSpeed;

    // Cập nhật vận tốc dựa trên góc
    velocityX = cos(actualAngle) * maxSpeed;
    velocityY = sin(actualAngle) * maxSpeed;

    // Cập nhật vị trí
    rect.x += velocityX;
    rect.y += velocityY;

    // Kiểm tra va chạm với biên
    if (rect.x < 0) {
        rect.x = 0;
        wanderAngle = M_PI - wanderAngle;
    }
    if (rect.x > 800 - rect.w) {
        rect.x = 800 - rect.w;
        wanderAngle = M_PI - wanderAngle;
    }
    if (rect.y < 0) {
        rect.y = 0;
        wanderAngle = -wanderAngle;
    }
    if (rect.y > 600 - rect.h) {
        rect.y = 600 - rect.h;
        wanderAngle = -wanderAngle;
    }

    // Cập nhật hướng quay
    flip = velocityX > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

void NPCFish::Render() {
    // Render con cá với hướng quay phù hợp
    SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, flip);
    if (scoreFont) {
        SDL_Color color = { 255, 255, 255 }; // trắng
        std::string pointText = std::to_string(CalculatePoints());

        SDL_Surface* surface = TTF_RenderText_Blended(scoreFont, pointText.c_str(), color);
        if (surface) {
            SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, surface);
            if (textTex) {
                SDL_Rect textRect = {
                    rect.x + rect.w / 2 - surface->w / 2,
                    rect.y - 20, // phía trên đầu cá
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(renderer, textTex, NULL, &textRect);
                SDL_DestroyTexture(textTex);
            }
            SDL_FreeSurface(surface);
        }
    }

}

void NPCFish::Grow(int delta) {
    rect.w = std::max(20, std::min(200, rect.w + delta));
    rect.h = std::max(20, std::min(200, rect.h + delta));
    points = CalculatePoints();
}

int NPCFish::CalculatePoints() const {
    // Điểm dựa trên diện tích (w*h) chia 100, tối thiểu 5, tối đa 500
    int pts = (rect.w * rect.h) / 100;
    return std::clamp(pts, 5, 500);
}


void NPCFish::SetVelocity(float vx, float vy) {
    velocityX = vx;
    velocityY = vy;
}

int NPCFish::GetPoint() const {
    return CalculatePoints();
}