#define STB_IMAGE_IMPLEMENTATION

#include <graphics/window.h>
#include <graphics/mesh.h>
#include <graphics/shader.h>

#include <subject/sprite2D.h>
#include <subject/text.h>
#include <subject/audio.h>
#include <subject/animatad2D.h>

#include <utils/input/input.h>
#include <chrono>
#include <iostream>
#include <libtcc.h>

int main() {
    Window window(1200, 600, u8"BlazeBolt game engine");
    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);

    Audio audio;
    audio.init();

    // int bgMusic = audio.loadSound("engine/assets/sounds/music_1.wav", "background", true);
    // if (bgMusic >= 0) {
    //     audio.setVolume(bgMusic, 0.5f);
    //     audio.play(bgMusic);
    // }

    Animation2D anim;
    if (anim.loadFromGIF("engine/assets/textures/logo.gif")) {
        std::cout << "GIF загружен успешно!" << std::endl;
        std::cout << "Кадров: " << anim.getFrameCount() << std::endl;
        
        anim.setSize(0.8, 1);
        anim.setPosition(0.0f, 0.0f);
        anim.setOrigin(0.5f, 0.5f);
        anim.setSpeed(1);
        
        anim.play();
    }

    Sprite2D sprite;
    sprite.setPosition(0, 0);
    sprite.setSize(1.0f, 1.0f);
    sprite.setTexture("engine/assets/textures/wall.jpg");
    sprite.setColor(0.3, 0.14, 0.15);

    Text text("./engine/assets/arial.ttf", 48);
    text.setText("Hello, BlazeBolt");
    text.setColor(1, 1, 1, 1);
    text.setPosition(100, 100);
    text.setScale(1.0f);
    text.setScreenSize(window.getWidth(), window.getHeight());

    auto lastTime = std::chrono::steady_clock::now();
    float elapsedTime = 0;

    while (!window.shouldClose()) {
        auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        
        if (deltaTime > 0.033f) deltaTime = 0.033f;
        
        elapsedTime += deltaTime;
        
        static float lastPrint = 0;
        if (elapsedTime - lastPrint > 0.3f) {
            lastPrint = elapsedTime;
        }

        audio.update();
        
        anim.update(deltaTime);
        
        window.clear();
        
        sprite.draw();
        anim.draw();
        text.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}