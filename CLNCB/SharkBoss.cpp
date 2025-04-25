
#include "SharkBoss.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Nếu chưa có chung GAME_WIDTH/HEIGHT, định nghĩa tạm
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

SharkBoss::SharkBoss(int x, int y, int w, int h,
    SDL_Texture* tex,
    SDL_Renderer* renderer,
    Player* player)
    : NPCFish(x, y, w, h, tex, renderer),
    target(player),
    speed(2.0f),              // tăng speed lên chút cho trơn tru
    posX(static_cast<float>(x)),
    posY(static_cast<float>(y))
{
    if (!target) {
        throw std::invalid_argument("Player target cannot be null");
    }
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
}

void SharkBoss::Update() {
    if (!target) return;

    // --- 1) Tính tọa độ tâm Boss & tâm Player ---
    // dùng rect để render, và posX/posY để tính float
    float bossCX = posX + rect.w * 0.5f;
    float bossCY = posY + rect.h * 0.5f;
    SDL_Rect pr = target->getRect();
    float playerCX = pr.x + pr.w * 0.5f;
    float playerCY = pr.y + pr.h * 0.5f;

    // --- 2) Vector hướng tới Player ---
    float dx = playerCX - bossCX;
    float dy = playerCY - bossCY;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1e-3f) return;      // đã tới gần

    // --- 3) Cập nhật posX/posY theo vector chuẩn hóa * speed ---
    posX += dx / dist * speed;
    posY += dy / dist * speed;

    // --- 4) Giữ trong màn hình ---
    posX = std::clamp(posX, 0.0f, float(SCREEN_WIDTH - rect.w));
    posY = std::clamp(posY, 0.0f, float(SCREEN_HEIGHT - rect.h));

    // --- 5) Đồng bộ lại rect để Render vẽ đúng ---
    rect.x = static_cast<int>(posX);
    rect.y = static_cast<int>(posY);

    // --- 6) Đồng bộ x,y (nếu logic khác đọc x,y) ---
    x = rect.x;
    y = rect.y;

    // --- 7) Flip sprite theo hướng di chuyển ngang ---
    flip = (dx < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}
