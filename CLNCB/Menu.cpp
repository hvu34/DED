#include "Menu.h"
#include <iostream>

Menu::Menu(SDL_Renderer* renderer) : renderer(renderer) {
    font = TTF_OpenFont("timesbd.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font in Menu: " << TTF_GetError() << std::endl;
    }

    // Tạo các texture cho nút
    SDL_Color white = { 255, 255, 255, 255 };
    playButton = CreateTextTexture("PLAY", white);
    playAgainButton = CreateTextTexture("PLAY AGAIN", white);
    quitButton = CreateTextTexture("QUIT", white);
}

Menu::~Menu() {
    if (playButton) SDL_DestroyTexture(playButton);
    if (playAgainButton) SDL_DestroyTexture(playAgainButton);
    if (quitButton) SDL_DestroyTexture(quitButton);
    if (font) TTF_CloseFont(font);
}

void Menu::RenderStartMenu() {
    std::cout << "[MENU] RenderStartMenu được gọi" << std::endl;
    SDL_Color white = { 255, 255, 255, 255 };

    // Vẽ tiêu đề
    SDL_Texture* title = CreateTextTexture("BIG FISH EAT SMALL FISH", white);
    int titleW, titleH;
    SDL_QueryTexture(title, NULL, NULL, &titleW, &titleH);
    SDL_Rect titleRect = { (800 - titleW) / 2, 100, titleW, titleH };
    SDL_RenderCopy(renderer, title, NULL, &titleRect);
    SDL_DestroyTexture(title);

    // Vẽ nút Play
    RenderButton(playButton, 350, 300, 100, 50);
    

}

void Menu::RenderGameOver(int score) {
    // Vẽ thông báo Game Over
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Texture* gameOver = CreateTextTexture("GAME OVER", white);
    int gameOverW, gameOverH;
    SDL_QueryTexture(gameOver, NULL, NULL, &gameOverW, &gameOverH);
    SDL_Rect gameOverRect = { (800 - gameOverW) / 2, 100, gameOverW, gameOverH };
    SDL_RenderCopy(renderer, gameOver, NULL, &gameOverRect);
    SDL_DestroyTexture(gameOver);

    // Vẽ điểm số
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Texture* scoreTexture = CreateTextTexture(scoreText, white);
    int scoreW, scoreH;
    SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreW, &scoreH);
    SDL_Rect scoreRect = { (800 - scoreW) / 2, 200, scoreW, scoreH };
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTexture);

    // Vẽ các nút
    RenderButton(playAgainButton, 250, 300, 150, 50);
    RenderButton(quitButton, 450, 300, 100, 50);
};

bool Menu::HandleStartMenuEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Kiểm tra nút Play
        if (IsMouseOverButton(mouseX, mouseY, 350, 300, 100, 50)) {
            return true;
        }
    }
    return false;
}

bool Menu::HandleGameOverEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Kiểm tra nút Play Again
        if (IsMouseOverButton(mouseX, mouseY, 250, 300, 150, 50)) {
            return true;
        }
        // Kiểm tra nút Quit
        else if (IsMouseOverButton(mouseX, mouseY, 450, 300, 100, 50)) {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
    }
    return false;
}

void Menu::RenderButton(SDL_Texture* button, int x, int y, int w, int h) {
    SDL_Rect buttonRect = { x, y, w, h };
    
    // Vẽ viền nút
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &buttonRect);
    
    // Vẽ texture
    SDL_RenderCopy(renderer, button, NULL, &buttonRect);
}

bool Menu::IsMouseOverButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonW, int buttonH) {
    return (mouseX >= buttonX && mouseX <= buttonX + buttonW &&
            mouseY >= buttonY && mouseY <= buttonY + buttonH);
}

SDL_Texture* Menu::CreateTextTexture(const std::string& text, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    return texture;
} 
