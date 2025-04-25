#include "Game.h"
#include <SDL_image.h>
#include <string>
#include <iostream>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <SDL.h>
#include <cmath>

Mix_Chunk* eatSound = nullptr;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MIN_SPAWN_INTERVAL = 1000;
const int BASE_SPAWN_INTERVAL = 3000;
const int SPAWN_INTERVAL_RANGE = 3000;
const int LEVEL_SPAWN_REDUCTION = 200;

Game::Game() : 
    window(nullptr), 
    renderer(nullptr), 
    background(nullptr), 
    isRunning(false), 
    player(nullptr), 
    menu(nullptr),
    inMenu(true),
    inGameOver(false),
    gameOver(false),
    score(0),
    level(1),
    fishEaten(0),
    fishToNextLevel(5),
    lastSpawnTime(0),
    spawnInterval(0),
    font(nullptr) {}

Game::~Game() {
    Clean();
}

bool Game::Init() {
    srand(static_cast<unsigned int>(time(nullptr)));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    font = TTF_OpenFont("timesbd.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow("Big Fish Eat Small Fish", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // --- Load tất cả background vào vector ---
    std::vector<std::string> bgFiles = {
        "Images/back.jpg",
        "Images/back1.jpg",
        "Images/back2.jpg",
        "Images/back3.jpg",
        "Images/back4.jpg"
    };
    for (auto& path : bgFiles) {
        SDL_Texture* tex = LoadTexture(path.c_str());
        if (!tex) {
            std::cerr << "Failed to load background: " << path << std::endl;
            // có thể exit nếu background thiết yếu
        }
        backgrounds.push_back(tex);
    }
    // Khởi đầu dùng background đầu tiên
    currentBackgroundIndex = 0;
    background = backgrounds[currentBackgroundIndex];


    player = new Player(100, 100, 64, 64, LoadTexture("Images/fish.png"), renderer);
    if (!player) {
        std::cerr << "Failed to create player" << std::endl;
        return false;
    }

    keyState = SDL_GetKeyboardState(NULL);
    lastSpawnTime = SDL_GetTicks();
    spawnInterval = BASE_SPAWN_INTERVAL + rand() % SPAWN_INTERVAL_RANGE;
    isRunning = true;

    menu = new Menu(renderer);
    if (!menu) {
        std::cerr << "Failed to create menu" << std::endl;
        return false;
    }

    inMenu = true;
    return true;
}


SDL_Texture* Game::LoadTexture(const char* file) {
    if (!file) {
        std::cerr << "Invalid file path provided to LoadTexture" << std::endl;
        return nullptr;
    }

    SDL_Surface* temp = IMG_Load(file);
    if (!temp) {
        std::cerr << "Failed to load image " << file << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, temp);
    SDL_FreeSurface(temp);
    
    if (!tex) {
        std::cerr << "Failed to create texture from " << file << ": " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    return tex;
}

void Game::Run() {
    while (isRunning) {
        HandleEvents();
        
        if (inMenu) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            menu->RenderStartMenu();
            SDL_RenderPresent(renderer);
            continue;
        }
        
        if (inGameOver) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            menu->RenderGameOver(score);
            SDL_RenderPresent(renderer);
            continue;
        }

        Update();
        Render();
        
        if (CheckGameOver()) {
            inGameOver = true;
            std::cout << "Game Over! Score: " << score << std::endl;
        }
        
        SDL_Delay(16);
    }
}

void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            return;
        }

        if (inMenu) {
            if (menu->HandleStartMenuEvents(event)) {
                inMenu = false;
                Reset();
            }
            continue;
        }

        if (inGameOver) {
            if (menu->HandleGameOverEvents(event)) {
                inGameOver = false;
                Reset();
            }
            continue;
        }

        keyState = SDL_GetKeyboardState(NULL);
        if (isRunning)
            player->HandleInput(keyState);
    }
}

void Game::Update() {
    if (gameOver) return;

    player->Update();
    int playerPts = player->CalculatePoints();

    // --- NPC ăn nhau ---
    for (size_t i = 0; i < enemies.size(); ++i) {
        NPCFish* npcA = enemies[i];
        if (!npcA) continue;

        for (size_t j = i + 1; j < enemies.size();) {
            NPCFish* npcB = enemies[j];
            if (!npcB) {
                ++j;
                continue;
            }

            if (npcA->IsColliding(npcB)) {
                int pointA = npcA->CalculatePoints();
                int pointB = npcB->CalculatePoints();
                // Nếu cả hai đã hơn Player 50 điểm thì không cho ăn nhau
                if (pointA > playerPts + 50 && pointB > playerPts + 50) {
                    ++j;
                    continue;
                }
                // Nếu một trong hai đã hơn Player 50 điểm thì chỉ bên kia ăn (hoặc bỏ qua)
                if (pointA > playerPts + 50) {
                    ++j;
                    continue;
                }
                if (pointB > playerPts + 50) {
                    ++j;
                    continue;
                }
                if (pointA > pointB) {
                    npcA->Grow(0.5);
                    delete npcB;
                    enemies.erase(enemies.begin() + j);
                    continue;
                }
                else if (pointB > pointA) {
                    npcB->Grow(0.5);
                    delete npcA;
                    enemies.erase(enemies.begin() + i);
                    --i;
                    break;
                }
            }
            ++j;
        }
    }

    // --- Player ăn hoặc bị ăn ---
    for (auto it = enemies.begin(); it != enemies.end();) {
        NPCFish* npc = *it;
        npc->Update();

        if (player->IsColliding(npc)) {
            int playerPts = player->CalculatePoints();
            int npcPts = npc->CalculatePoints();

            if (playerPts > npcPts) {
                player->Grow(npcPts/15);
                delete npc;
                it = enemies.erase(it);
                score += 10;
                fishEaten++;
                if (fishEaten >= fishToNextLevel) {
                    level++;
                    fishEaten = 0;
                    fishToNextLevel += 2;
                    std::cout << "🎉 LEVEL UP! Level: " << level << std::endl;

                    // --- đổi background theo level (vòng lặp qua backgrounds) ---
                    // --- đổi background theo level, loop khi tới cuối ---
                    currentBackgroundIndex = (currentBackgroundIndex + 1) % backgrounds.size();
                    background = backgrounds[currentBackgroundIndex];


                    spawnInterval = std::max(1000u, spawnInterval - 300);
                }
                continue;
            }
            else if (npcPts > playerPts) {
                std::cout << "💀 Bị ăn bởi cá có điểm cao hơn!" << std::endl;
                gameOver = true;
                return;
            }
        }
        ++it;
    }

    // --- Spawn NPC (gọi lại hàm chung) ---
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastSpawnTime >= spawnInterval) {
        int numFishToSpawn = 1 + rand() % 3;
        SpawnNPCFish(numFishToSpawn);
        lastSpawnTime = currentTime;
        spawnInterval = std::max(
            MIN_SPAWN_INTERVAL,
            BASE_SPAWN_INTERVAL + rand() % SPAWN_INTERVAL_RANGE
            - level * LEVEL_SPAWN_REDUCTION
        );
    }

    // --- Spawn “NPC to” khi đạt ngưỡng bigFishNextScore ---
    if (score >= bigFishNextScore) {
        // 1) Tính điểm Player và mục tiêu
        int pPts = player->CalculatePoints();
        int targetPts = pPts + 100;

        // 2) Tính w,h sao cho CalculatePoints() = w*2 = targetPts
        int w = static_cast<int>(std::sqrt(targetPts * 100.0f));    
        int h = w;

        // 3) Vùng né quanh Player
        SDL_Rect pr = player->getRect();
        int margin = std::max(w, h) * 2;
        SDL_Rect safe = { pr.x - margin, pr.y - margin, pr.w + margin * 2, pr.h + margin * 2 };

        // 4) Tìm vị trí spawn ngoài safeZone
        int sx, sy;
        SDL_Rect r;
        do {
            sx = rand() % (SCREEN_WIDTH - w);
            sy = rand() % (SCREEN_HEIGHT - h);
            r = { sx, sy, w, h };
        } while (SDL_HasIntersection(&r, &safe));

        // 5) Tạo NPCFish “to”
        SDL_Texture* tex = LoadTexture("Images/shark real.png");
        NPCFish* bigFish = new NPCFish(sx, sy, w, h, tex, renderer);

        // 6) Gán vận tốc random
        float vx = ((rand() % 2) == 0 ? 1.5f : -1.5f) + (rand() % 3) * 0.5f;
        float vy = ((rand() % 3) - 1) * 0.3f;
        bigFish->SetVelocity(vx, vy);
        enemies.push_back(bigFish);

        // 7) nâng ngưỡng spawn lần sau
        bigFishNextScore += 100;
    }


}


void Game::Render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background, NULL, NULL);

    player->Render();
    for (auto enemy : enemies) enemy->Render();
    RenderText("Level: " + std::to_string(level), 10, 10);
    RenderText("Score: " + std::to_string(score), 10, 40);

    SDL_RenderPresent(renderer);
    std::cout << "Level: " << level << " | Score: " << score << std::endl;
}

void Game::Clean() {
    delete player;
    player = nullptr;

    delete menu;
    menu = nullptr;

    for (auto e : enemies) {
        delete e;
        e = nullptr;
    }
    enemies.clear();

    SDL_DestroyTexture(background);
    background = nullptr;

    // Dọn tất cả background textures
    for (auto tex : backgrounds) {
        if (tex) SDL_DestroyTexture(tex);
    }
    backgrounds.clear();
    background = nullptr;
    
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    
    SDL_DestroyWindow(window);
    window = nullptr;
    
    IMG_Quit();
    SDL_Quit();
}

void Game::RenderText(const std::string& text, int x, int y) {
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool Game::CheckGameOver() const {
    if (!player) return true;
    
    // Kiểm tra nếu player ra khỏi màn hình
    SDL_Rect playerRect = player->getRect();
    if (playerRect.x < 0 || playerRect.x + playerRect.w > SCREEN_WIDTH ||
        playerRect.y < 0 || playerRect.y + playerRect.h > SCREEN_HEIGHT) {
        return true;
    }
    
    return gameOver;
}

void Game::Reset() {
    gameOver = false;
    isRunning = true;
    score = 0;
    level = 1;
    fishEaten = 0;
    fishToNextLevel = 5;

    if (player) player->Reset();

    for (auto& fish : enemies) delete fish;
    enemies.clear();
    SpawnNPCFish(25);  // Sinh ra 25 cá NPC ngẫu nhiên
    bossNextScore = 100;    // khởi lại ngưỡng spawn boss



    lastSpawnTime = SDL_GetTicks();
    spawnInterval = BASE_SPAWN_INTERVAL + rand() % SPAWN_INTERVAL_RANGE;
}

    
void Game::SpawnNPCFish(int numFish) {
    // 1) Vùng né quanh Player
    SDL_Rect pRect = player->getRect();
    int margin = std::max(pRect.w, pRect.h) * 2;
    SDL_Rect safeZone = {
        pRect.x - margin,
        pRect.y - margin,
        pRect.w + margin * 2,
        pRect.h + margin * 2
    };

    for (int i = 0; i < numFish; ++i) {
        // 2) Tính kích thước con cá
        int playerW = pRect.w;
        int smallW = static_cast<int>(playerW * 0.8f);
        int bigExtraMin = std::max(playerW / 4, 10);
        int bigExtraMax = std::max(playerW / 2, 50);
        // giãn 20% con to, 80% con nhỏ
        int bigCount = static_cast<int>(numFish * 0.2f + 0.5f);
        int smallCount = numFish - bigCount;

        // Spawn cá nhỏ trước, rồi cá to, hoặc ngược lại tùy ý
        int w = (i < smallCount ? smallW
            : playerW + (bigExtraMin + rand() % (bigExtraMax - bigExtraMin + 1)));
        int h = w;

        // 3) Chọn spawnX chỉ ở hai bên rìa
        int spawnX, spawnY;
        SDL_Rect rect;
        do {
            int side = rand() % 2;  // 0 = trái, 1 = phải
            spawnX = (side == 0) ? -w : SCREEN_WIDTH;
            // random Y trong màn hình
            spawnY = rand() % (SCREEN_HEIGHT - h);
            rect = { spawnX, spawnY, w, h };
        } while (SDL_HasIntersection(&rect, &safeZone));

        // 4) Chọn texture ngẫu nhiên
        std::string imgPath;
        switch (rand() % 3) {
        case 0: imgPath = "Images/Enemies 1.png"; break;
        case 1: imgPath = "Images/Enemies 2.png"; break;
        default: imgPath = "Images/New Fish.png"; break;
        }
        SDL_Texture* tex = LoadTexture(imgPath.c_str());
        if (!tex) continue;

        // 5) Tạo NPCFish và gán velocity
        NPCFish* fish = new NPCFish(spawnX, spawnY, w, h, tex, renderer);
        float vx = (spawnX < 0 ? 1.5f : -1.5f) + (rand() % 3) * 0.5f;
        float vy = ((rand() % 3) - 1) * 0.3f;
        fish->SetVelocity(vx, vy);
        enemies.push_back(fish);

        // (tuỳ bạn: in log để debug)
        // std::cout << "Spawn NPC at ("<<spawnX<<","<<spawnY<<") size="<<w<<"\n";
    }
}


    




