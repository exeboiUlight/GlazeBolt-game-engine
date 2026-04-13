#define STB_IMAGE_IMPLEMENTATION

// base graphics
#include <graphics/window.h>
#include <graphics/mesh.h>
#include <graphics/shader.h>

// subjects
#include <subject/sprite2D.h>

// utils
#include <utils/input/input.h>

int main() {
    Window window(800, 600, u8"BlezeBolt game engine - движок для инди игр");

    // window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);
    window.setClearColor(0, 0, 0, 0);

    Sprite2D sprite;
    sprite.setPosition(0, 0);
    sprite.setSize(1.0f, 1.0f);
    sprite.setTexture("engine/assets/textures/wall.jpg");
    sprite.setColor(0.3, 0.14, 0.15);

    while (!window.shouldClose()) {
        window.clear();
        
        sprite.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}