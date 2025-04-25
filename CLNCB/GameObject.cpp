#include "GameObject.h"
#include <cmath>  // Để tính toán khoảng cách giữa các tâm và căn bậc hai

GameObject::GameObject(int x, int y, int w, int h, SDL_Texture* tex, SDL_Renderer* rend)
    : x(x), y(y), width(w), height(h), texture(tex), renderer(rend)
{
    // Thêm:
    rect = { x, y, w, h };
}


void GameObject::Render() {
    SDL_Rect dst = { x, y, width, height };
    SDL_RenderCopyEx(renderer, texture, NULL, &dst, 0, NULL, flip);
}

float GameObject::GetSize() const {
    return width * height;  // Nếu vẫn muốn giữ kích thước diện tích như cũ
}

// Hàm phát hiện va chạm sử dụng Bounding Circle Collision
bool GameObject::IsColliding(const GameObject* other) const {
    SDL_Rect r1 = this->getRect();
    SDL_Rect r2 = other->getRect();

    // Tâm của mỗi hình tròn
    float cx1 = r1.x + r1.w / 2.0f;
    float cy1 = r1.y + r1.h / 2.0f;
    float cx2 = r2.x + r2.w / 2.0f;
    float cy2 = r2.y + r2.h / 2.0f;

    float dx = cx1 - cx2;
    float dy = cy1 - cy2;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Bán kính = nửa đường chéo
    float ra = std::sqrt(r1.w * r1.w + r1.h * r1.h) / 2.0f;
    float rb = std::sqrt(r2.w * r2.w + r2.h * r2.h) / 2.0f;

    return distance < (ra + rb);
}
