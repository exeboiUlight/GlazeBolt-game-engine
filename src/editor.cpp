#define STB_IMAGE_IMPLEMENTATION

// base graphics
#include <graphics/window.h>
#include <graphics/mesh.h>
#include <graphics/shader.h>

// subjects
#include <subject/sprite2D.h>
#include <subject/text.h>

// utils
#include <utils/input/input.h>

int main() {
    Window window(1200, 600, u8"BlezeBolt game engine - движок для инди игр");

    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);

    Sprite2D sprite;
    sprite.setPosition(0, 0);
    sprite.setSize(1.0f, 1.0f);
    sprite.setTexture("engine/assets/textures/wall.jpg");
    sprite.setColor(0.3, 0.14, 0.15);

    Text text("./engine/assets/arial.ttf", 48);
    text.setText("Hello, BlazeBolt");
    text.setColor(1, 1, 1, 1);
    text.setPosition(100, 100);  // Пиксельные координаты
    text.setScale(1.0f);
    text.setScreenSize(window.getWidth(), window.getHeight());  // Важно: установить размер экрана

    while (!window.shouldClose()) {
        window.clear();
        
        // sprite.draw();
        text.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}