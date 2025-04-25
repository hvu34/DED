#pragma once
#include "NPCFish.h"
#include "Player.h"

class SharkBoss : public NPCFish {
public:
    SharkBoss(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* renderer, Player* player);
    SharkBoss(const SharkBoss&) = delete;
    SharkBoss& operator=(const SharkBoss&) = delete;
    void Update() override;
    float posX, posY;
private:
    Player* target;
    float speed;
    
};
