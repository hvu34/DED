#include "GameLoop.h"
#include "SDL_image.h"

int main(int argc, char* argv[]) {
    GameLoop game;

    if (game.Initialize("Cá lớn nuốt cá bé", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false)) {
        while (game.IsRunning()) {
            game.HandleEvents();
            game.Update();
            game.Render();
            SDL_Delay(16); // ~60 FPS
        }
    }

    game.Clean();
    return 0;
}
