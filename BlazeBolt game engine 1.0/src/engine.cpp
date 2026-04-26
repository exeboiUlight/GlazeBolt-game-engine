#define BLAZEBOLT_EXPORTS
#include "BlazeBoltApi.h"

#include <graphics/window.h>
#include <graphics/shader.h>
#include <graphics/mesh.h>
#include <subject/sprite2D.h>
#include <subject/animatad2D.h>
#include <subject/text.h>
#include <subject/audio.h>
#include <utils/input/input.h>

#include <chrono>
#include <unordered_map>
#include <memory>
#include <thread>
#include <iostream>

// Глобальные объекты движка
static Window* g_window = nullptr;
static Audio* g_audio = nullptr;

// Хранилища для объектов
static std::unordered_map<int, std::unique_ptr<Sprite2D>> g_sprites;
static std::unordered_map<int, std::unique_ptr<Animation2D>> g_animations;
static std::unordered_map<int, std::unique_ptr<Text>> g_texts;
static int g_nextSpriteId = 1;
static int g_nextAnimId = 1;
static int g_nextTextId = 1;

// Тайминг
static std::chrono::steady_clock::time_point g_lastTime;
static float g_deltaTime = 0.0f;

// Вспомогательная функция для обновления дельта времени
static void UpdateDeltaTime() {
    auto now = std::chrono::steady_clock::now();
    g_deltaTime = std::chrono::duration<float>(now - g_lastTime).count();
    g_lastTime = now;
    if (g_deltaTime > 0.033f) g_deltaTime = 0.033f;
}

// ==================== Инициализация движка ====================

extern "C" {

BLAZEBOLT_API int BBE_Init(void) {
    g_lastTime = std::chrono::steady_clock::now();
    return 1;
}

BLAZEBOLT_API void BBE_Shutdown(void) {
    g_sprites.clear();
    g_animations.clear();
    g_texts.clear();
    
    if (g_window) {
        delete g_window;
        g_window = nullptr;
    }
    
    if (g_audio) {
        g_audio->shutdown();
        delete g_audio;
        g_audio = nullptr;
    }
}

BLAZEBOLT_API int BBE_CreateWindow(int width, int height, const char* title) {
    if (g_window) {
        BBE_DestroyWindow();
    }
    
    try {
        g_window = new Window(width, height, title);
        if (!g_window->getGLFWwindow()) {
            delete g_window;
            g_window = nullptr;
            return 0;
        }
        
        Input::getInstance().init(g_window->getGLFWwindow());
        return 1;
    } catch (...) {
        return 0;
    }
}

BLAZEBOLT_API void BBE_DestroyWindow(void) {
    if (g_window) {
        delete g_window;
        g_window = nullptr;
    }
}

BLAZEBOLT_API int BBE_IsWindowRunning(void) {
    if (!g_window) return 0;
    return !g_window->shouldClose();
}

BLAZEBOLT_API void BBE_BeginFrame(void) {
    if (!g_window) return;
    UpdateDeltaTime();
    Input::getInstance().update();
    g_window->clear();
}

BLAZEBOLT_API void BBE_EndFrame(void) {
    if (!g_window) return;
    g_window->swapBuffers();
}

BLAZEBOLT_API void BBE_SetClearColor(float r, float g, float b, float a) {
    if (g_window) {
        g_window->setClearColor(r, g, b, a);
    }
}

BLAZEBOLT_API void BBE_PollEvents(void) {
    if (g_window) {
        g_window->pollEvents();
    }
}

// ==================== Спрайты ====================

BLAZEBOLT_API int BBE_Sprite_Create(void) {
    int id = g_nextSpriteId++;
    g_sprites[id] = std::make_unique<Sprite2D>();
    return id;
}

BLAZEBOLT_API void BBE_Sprite_Destroy(int spriteId) {
    g_sprites.erase(spriteId);
}

BLAZEBOLT_API void BBE_Sprite_SetTexture(int spriteId, const char* texturePath) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->setTexture(texturePath);
    }
}

BLAZEBOLT_API void BBE_Sprite_SetPosition(int spriteId, float x, float y) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->setPosition(x, y);
    }
}

BLAZEBOLT_API void BBE_Sprite_SetSize(int spriteId, float width, float height) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->setSize(width, height);
    }
}

BLAZEBOLT_API void BBE_Sprite_SetColor(int spriteId, float r, float g, float b, float a) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->setColor(r, g, b, a);
    }
}

BLAZEBOLT_API void BBE_Sprite_SetVisible(int spriteId, int visible) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->setVisible(visible != 0);
    }
}

BLAZEBOLT_API void BBE_Sprite_Draw(int spriteId) {
    auto it = g_sprites.find(spriteId);
    if (it != g_sprites.end()) {
        it->second->draw();
    }
}

// ==================== Анимации ====================

BLAZEBOLT_API int BBE_Anim_Create(void) {
    int id = g_nextAnimId++;
    g_animations[id] = std::make_unique<Animation2D>();
    return id;
}

BLAZEBOLT_API void BBE_Anim_Destroy(int animId) {
    g_animations.erase(animId);
}

BLAZEBOLT_API int BBE_Anim_LoadGIF(int animId, const char* filepath) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        return it->second->loadFromGIF(filepath) ? 1 : 0;
    }
    return 0;
}

BLAZEBOLT_API void BBE_Anim_SetPosition(int animId, float x, float y) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->setPosition(x, y);
    }
}

BLAZEBOLT_API void BBE_Anim_SetSize(int animId, float width, float height) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->setSize(width, height);
    }
}

BLAZEBOLT_API void BBE_Anim_Play(int animId) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->play();
    }
}

BLAZEBOLT_API void BBE_Anim_Pause(int animId) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->pause();
    }
}

BLAZEBOLT_API void BBE_Anim_Stop(int animId) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->stop();
    }
}

BLAZEBOLT_API void BBE_Anim_SetLooping(int animId, int loop) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->setLooping(loop != 0);
    }
}

BLAZEBOLT_API void BBE_Anim_SetSpeed(int animId, float speed) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->setSpeed(speed);
    }
}

BLAZEBOLT_API void BBE_Anim_Update(int animId, float deltaTime) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->update(deltaTime);
    }
}

BLAZEBOLT_API void BBE_Anim_Draw(int animId) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->draw();
    }
}

BLAZEBOLT_API int BBE_Anim_GetFrameCount(int animId) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        return it->second->getFrameCount();
    }
    return 0;
}

BLAZEBOLT_API void BBE_Anim_SetFrame(int animId, int frame) {
    auto it = g_animations.find(animId);
    if (it != g_animations.end()) {
        it->second->setFrame(frame);
    }
}

// ==================== Текст ====================

BLAZEBOLT_API int BBE_Text_Create(const char* fontPath, unsigned int fontSize) {
    int id = g_nextTextId++;
    g_texts[id] = std::make_unique<Text>(fontPath, fontSize);
    if (g_window) {
        g_texts[id]->setScreenSize(g_window->getWidth(), g_window->getHeight());
    }
    return id;
}

BLAZEBOLT_API void BBE_Text_Destroy(int textId) {
    g_texts.erase(textId);
}

BLAZEBOLT_API void BBE_Text_SetString(int textId, const char* text) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->setText(text);
    }
}

BLAZEBOLT_API void BBE_Text_SetPosition(int textId, float x, float y) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->setPosition(x, y);
    }
}

BLAZEBOLT_API void BBE_Text_SetScale(int textId, float scale) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->setScale(scale);
    }
}

BLAZEBOLT_API void BBE_Text_SetColor(int textId, float r, float g, float b, float a) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->setColor(r, g, b, a);
    }
}

BLAZEBOLT_API void BBE_Text_SetScreenSize(int textId, int width, int height) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->setScreenSize(width, height);
    }
}

BLAZEBOLT_API void BBE_Text_Draw(int textId) {
    auto it = g_texts.find(textId);
    if (it != g_texts.end()) {
        it->second->draw();
    }
}

// ==================== Аудио ====================

BLAZEBOLT_API int BBE_Audio_Init(void) {
    if (!g_audio) {
        g_audio = new Audio();
    }
    return g_audio->init() ? 1 : 0;
}

BLAZEBOLT_API void BBE_Audio_Shutdown(void) {
    if (g_audio) {
        g_audio->shutdown();
        delete g_audio;
        g_audio = nullptr;
    }
}

BLAZEBOLT_API int BBE_Audio_LoadSound(const char* filename, const char* soundName, int loop) {
    if (!g_audio) return -1;
    return g_audio->loadSound(filename, soundName, loop != 0);
}

BLAZEBOLT_API void BBE_Audio_Play(const char* soundName) {
    if (g_audio) {
        g_audio->play(soundName);
    }
}

BLAZEBOLT_API void BBE_Audio_Stop(const char* soundName) {
    if (g_audio) {
        g_audio->stop(soundName);
    }
}

BLAZEBOLT_API void BBE_Audio_SetVolume(const char* soundName, float volume) {
    if (g_audio) {
        g_audio->setVolume(soundName, volume);
    }
}

BLAZEBOLT_API void BBE_Audio_SetPitch(const char* soundName, float pitch) {
    if (g_audio) {
        g_audio->setPitch(g_audio->getSourceIndex(soundName), pitch);
    }
}

BLAZEBOLT_API void BBE_Audio_Update(void) {
    if (g_audio) {
        g_audio->update();
    }
}

// ==================== Ввод с клавиатуры ====================

BLAZEBOLT_API int BBE_Input_IsKeyPressed(int key) {
    return Input::getInstance().isKeyPressed(key) ? 1 : 0;
}

BLAZEBOLT_API int BBE_Input_IsKeyJustPressed(int key) {
    return Input::getInstance().isKeyJustPressed(key) ? 1 : 0;
}

BLAZEBOLT_API int BBE_Input_IsKeyJustReleased(int key) {
    return Input::getInstance().isKeyJustReleased(key) ? 1 : 0;
}

// ==================== Ввод с мыши ====================

BLAZEBOLT_API int BBE_Input_IsMousePressed(int button) {
    return Input::getInstance().isMouseButtonPressed(button) ? 1 : 0;
}

BLAZEBOLT_API int BBE_Input_IsMouseJustPressed(int button) {
    return Input::getInstance().isMouseButtonJustPressed(button) ? 1 : 0;
}

BLAZEBOLT_API void BBE_Input_GetMousePosition(float* x, float* y) {
    *x = static_cast<float>(Input::getInstance().getMouseX());
    *y = static_cast<float>(Input::getInstance().getMouseY());
}

BLAZEBOLT_API void BBE_Input_GetMouseDelta(float* x, float* y) {
    *x = static_cast<float>(Input::getInstance().getMouseDeltaX());
    *y = static_cast<float>(Input::getInstance().getMouseDeltaY());
}

BLAZEBOLT_API void BBE_Input_GetScroll(float* x, float* y) {
    *x = static_cast<float>(Input::getInstance().getScrollX());
    *y = static_cast<float>(Input::getInstance().getScrollY());
}

// ==================== Геймпад ====================

BLAZEBOLT_API int BBE_Input_IsGamepadConnected(int id) {
    return Input::getInstance().isGamepadConnected(id) ? 1 : 0;
}

BLAZEBOLT_API int BBE_Input_IsGamepadButtonPressed(int id, int button) {
    return Input::getInstance().isGamepadButtonPressed(id, button) ? 1 : 0;
}

BLAZEBOLT_API int BBE_Input_IsGamepadButtonJustPressed(int id, int button) {
    return Input::getInstance().isGamepadButtonJustPressed(id, button) ? 1 : 0;
}

BLAZEBOLT_API float BBE_Input_GetGamepadAxis(int id, int axis) {
    return Input::getInstance().getGamepadAxis(id, axis);
}

// ==================== Утилиты ====================

BLAZEBOLT_API float BBE_GetDeltaTime(void) {
    return g_deltaTime;
}

BLAZEBOLT_API int BBE_GetWindowWidth(void) {
    return g_window ? g_window->getWidth() : 0;
}

BLAZEBOLT_API int BBE_GetWindowHeight(void) {
    return g_window ? g_window->getHeight() : 0;
}

BLAZEBOLT_API void BBE_Sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

} // extern "C"