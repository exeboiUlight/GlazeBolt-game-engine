#define STB_IMAGE_IMPLEMENTATION

// base graphics
#include <graphics/window.h>
#include <graphics/mesh.h>
#include <graphics/shader.h>

// subjects
#include <subject/camera.h>
#include <subject/sprite2D.h>

int main() {
    Window window(800, 600, u8"BlezeBolt game engine - движок для инди игр");
    Camera2D camera(window, {0, 0}, {1, 1});

    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);

    Sprite2D sprite;
    sprite.setPosition(0, 0);
    sprite.setSize(1, 1);
    sprite.setTexture("engine/assets/textures/bongo.png");

    float cameraSpeed = 0.2f;

    while (!window.shouldClose()) {
        window.clear();

        GLFWwindow* glfwWindow = window.GetWindow();
        
        Vector2 currentPos = camera.getPosition();
        
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            camera.setPosition({currentPos.x, currentPos.y + cameraSpeed});
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            camera.setPosition({currentPos.x, currentPos.y - cameraSpeed});
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            camera.setPosition({currentPos.x - cameraSpeed, currentPos.y});
        }
        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            camera.setPosition({currentPos.x + cameraSpeed, currentPos.y});
        }

        sprite.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}