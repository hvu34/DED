#include "GameLoop.h"

bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b);
GameLoop::GameLoop()
    : window(nullptr), renderer(nullptr), isRunning(false), playerFish(100, 100, 30) {}

GameLoop::~GameLoop() {}

bool GameLoop::Initialize(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    // Trong GameLoop.cpp, thêm vào hàm Initialize



    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    playerFish = Fish(100, 100, 30);  // Cá người chơi
    npcFishes.push_back(Fish(300, 300, 20));  // Cá NPC đầu tiên
    npcFishes.push_back(Fish(500, 200, 10));  // Cá NPC thứ hai

    isRunning = true;
    return true;
}


void GameLoop::HandleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        isRunning = false;
    }
}

void GameLoop::Update() {
    // Cập nhật cá người chơi
    playerFish.Update(true);

    // Kiểm tra va chạm giữa cá người chơi và cá NPC
    for (size_t i = 0; i < npcFishes.size(); ++i) {
        SDL_Rect playerRect = playerFish.GetRect();
        SDL_Rect npcRect = npcFishes[i].GetRect();

        if (SDL_HasIntersection(&playerRect, &npcRect)) {
            if (playerFish.IsBiggerThan(npcFishes[i])) {
                // Cá người chơi ăn cá NPC
                playerFish.Grow(5); // Tăng kích thước cá người chơi
                npcFishes.erase(npcFishes.begin() + i); // Xóa cá NPC
                --i; // Giảm i vì đã xóa phần tử
            }
        }
    }

    // Cập nhật cá NPC
    for (auto& fish : npcFishes) {
        fish.Update(false);  // isPlayer = false cho cá NPC
    }

    // Kiểm tra va chạm giữa các cá NPC (nếu có)
    for (size_t i = 0; i < npcFishes.size(); ++i) {
        for (size_t j = i + 1; j < npcFishes.size(); ++j) {
            SDL_Rect rect1 = npcFishes[i].GetRect();
            SDL_Rect rect2 = npcFishes[j].GetRect();

            if (SDL_HasIntersection(&rect1, &rect2)) {
                if (npcFishes[i].IsBiggerThan(npcFishes[j])) {
                    npcFishes[i].Grow(5);
                    npcFishes.erase(npcFishes.begin() + j);
                    --j; // Giảm j để tránh bỏ qua phần tử tiếp theo
                } else if (npcFishes[j].IsBiggerThan(npcFishes[i])) {
                    npcFishes[j].Grow(5);
                    npcFishes.erase(npcFishes.begin() + i);
                    --i; // Giảm i vì đã xóa phần tử
                    break; // Thoát vòng lặp j vì i không còn hợp lệ
                }
            }
        }
    }
    for (auto it = npcFishes.begin(); it != npcFishes.end(); ) {
    if (CheckCollision(playerFish.GetRect(), it->GetRect())) {
        if (playerFish.IsBiggerThan(*it)) {
            playerFish.Grow(it->GetRect().w / 4); // Tăng kích thước cá người chơi
            it = npcFishes.erase(it);           // Xóa cá NPC bị ăn
            continue;
        } else {
            // Game Over nếu cá nhỏ hơn chạm cá to hơn
            isRunning = false; // hoặc gọi hàm GameOver()
            break;
        }
    }
    ++it;
}
}



void GameLoop::Render() {
    SDL_RenderClear(renderer);

    // Đặt màu vẽ cho background (màu đen)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, nullptr); // Vẽ một hình chữ nhật bao phủ toàn bộ màn hình

    // Render cá người chơi (màu xanh lá)
    SDL_Rect playerRect = playerFish.GetRect();
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh lá
    SDL_RenderFillRect(renderer, &playerRect);

    // Render cá NPC (màu xanh lá)
    for (const Fish& npcFish : npcFishes) {
        SDL_Rect npcRect = npcFish.GetRect();
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh lá
        SDL_RenderFillRect(renderer, &npcRect);
    }

    SDL_RenderPresent(renderer);
}


void GameLoop::Clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool GameLoop::IsRunning() const {
    return isRunning;
}
bool CheckCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}
