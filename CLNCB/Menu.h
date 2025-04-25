#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();

    void RenderStartMenu();
    void RenderGameOver(int score);
    bool HandleStartMenuEvents(SDL_Event& event);
    bool HandleGameOverEvents(SDL_Event& event);
    

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* playButton;
    SDL_Texture* playAgainButton;
    SDL_Texture* quitButton;

    void RenderButton(SDL_Texture* button, int x, int y, int w, int h);
    bool IsMouseOverButton(int mouseX, int mouseY, int buttonX, int buttonY, int buttonW, int buttonH);
    SDL_Texture* CreateTextTexture(const std::string& text, SDL_Color color);
}; 