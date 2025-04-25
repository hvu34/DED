#include <SDL.h>
#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    // Khởi tạo game
    if (!game.Init()) {
        return -1;
    }

    // Bắt đầu vòng lặp game (có menu bên trong)
    game.Run();

    // Dọn tài nguyên
    game.Clean();

    return 0;
}
