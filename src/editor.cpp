#define STB_IMAGE_IMPLEMENTATION

// base graphics
#include <graphics/window.h>
#include <graphics/mesh.h>
#include <graphics/shader.h>

// subjects
#include <subject/camera.h>
#include <subject/sprite2D.h>

// utils
#include <utils/input/input.h>

int main() {
    Window window(800, 600, u8"BlezeBolt game engine - движок для инди игр");
    Camera2D camera(window, {0, 0}, {1, 1});

    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);

    Sprite2D sprite;
    sprite.setPosition(0, 0);
    sprite.setSize(1.0f, 1.0f);
    sprite.setTexture("engine/assets/textures/wall.jpg");

    while (!window.shouldClose()) {
        window.clear();
        
        sprite.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}