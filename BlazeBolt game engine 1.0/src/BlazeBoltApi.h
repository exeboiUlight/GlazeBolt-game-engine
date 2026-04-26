// BlazeBoltApi.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Для MinGW/gcc экспорт функций
#ifdef BLAZEBOLT_EXPORTS
    #ifdef _MSC_VER
        #define BLAZEBOLT_API __declspec(dllexport)
    #else
        #define BLAZEBOLT_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define BLAZEBOLT_API __declspec(dllimport)
    #else
        #define BLAZEBOLT_API
    #endif
#endif

// ==================== Структуры для передачи данных ====================

typedef struct {
    float x, y;
} BBE_Vec2;

typedef struct {
    float x, y, z, w;
} BBE_Vec4;

typedef struct {
    int width;
    int height;
    const char* title;
} BBE_WindowConfig;

// ==================== Инициализация и управление движком ====================

BLAZEBOLT_API int BBE_Init(void);
BLAZEBOLT_API void BBE_Shutdown(void);
BLAZEBOLT_API int BBE_CreateWindow(int width, int height, const char* title);
BLAZEBOLT_API void BBE_DestroyWindow(void);
BLAZEBOLT_API int BBE_IsWindowRunning(void);
BLAZEBOLT_API void BBE_BeginFrame(void);
BLAZEBOLT_API void BBE_EndFrame(void);
BLAZEBOLT_API void BBE_SetClearColor(float r, float g, float b, float a);
BLAZEBOLT_API void BBE_PollEvents(void);

// ==================== Спрайты ====================

BLAZEBOLT_API int BBE_Sprite_Create(void);
BLAZEBOLT_API void BBE_Sprite_Destroy(int spriteId);
BLAZEBOLT_API void BBE_Sprite_SetTexture(int spriteId, const char* texturePath);
BLAZEBOLT_API void BBE_Sprite_SetPosition(int spriteId, float x, float y);
BLAZEBOLT_API void BBE_Sprite_SetSize(int spriteId, float width, float height);
BLAZEBOLT_API void BBE_Sprite_SetColor(int spriteId, float r, float g, float b, float a);
BLAZEBOLT_API void BBE_Sprite_SetVisible(int spriteId, int visible);
BLAZEBOLT_API void BBE_Sprite_Draw(int spriteId);

// ==================== Анимации (GIF) ====================

BLAZEBOLT_API int BBE_Anim_Create(void);
BLAZEBOLT_API void BBE_Anim_Destroy(int animId);
BLAZEBOLT_API int BBE_Anim_LoadGIF(int animId, const char* filepath);
BLAZEBOLT_API void BBE_Anim_SetPosition(int animId, float x, float y);
BLAZEBOLT_API void BBE_Anim_SetSize(int animId, float width, float height);
BLAZEBOLT_API void BBE_Anim_Play(int animId);
BLAZEBOLT_API void BBE_Anim_Pause(int animId);
BLAZEBOLT_API void BBE_Anim_Stop(int animId);
BLAZEBOLT_API void BBE_Anim_SetLooping(int animId, int loop);
BLAZEBOLT_API void BBE_Anim_SetSpeed(int animId, float speed);
BLAZEBOLT_API void BBE_Anim_Update(int animId, float deltaTime);
BLAZEBOLT_API void BBE_Anim_Draw(int animId);
BLAZEBOLT_API int BBE_Anim_GetFrameCount(int animId);
BLAZEBOLT_API void BBE_Anim_SetFrame(int animId, int frame);

// ==================== Текст ====================

BLAZEBOLT_API int BBE_Text_Create(const char* fontPath, unsigned int fontSize);
BLAZEBOLT_API void BBE_Text_Destroy(int textId);
BLAZEBOLT_API void BBE_Text_SetString(int textId, const char* text);
BLAZEBOLT_API void BBE_Text_SetPosition(int textId, float x, float y);
BLAZEBOLT_API void BBE_Text_SetScale(int textId, float scale);
BLAZEBOLT_API void BBE_Text_SetColor(int textId, float r, float g, float b, float a);
BLAZEBOLT_API void BBE_Text_SetScreenSize(int textId, int width, int height);
BLAZEBOLT_API void BBE_Text_Draw(int textId);

// ==================== Аудио ====================

BLAZEBOLT_API int BBE_Audio_Init(void);
BLAZEBOLT_API void BBE_Audio_Shutdown(void);
BLAZEBOLT_API int BBE_Audio_LoadSound(const char* filename, const char* soundName, int loop);
BLAZEBOLT_API void BBE_Audio_Play(const char* soundName);
BLAZEBOLT_API void BBE_Audio_Stop(const char* soundName);
BLAZEBOLT_API void BBE_Audio_SetVolume(const char* soundName, float volume);
BLAZEBOLT_API void BBE_Audio_SetPitch(const char* soundName, float pitch);
BLAZEBOLT_API void BBE_Audio_Update(void);

// ==================== Ввод с клавиатуры ====================

BLAZEBOLT_API int BBE_Input_IsKeyPressed(int key);
BLAZEBOLT_API int BBE_Input_IsKeyJustPressed(int key);
BLAZEBOLT_API int BBE_Input_IsKeyJustReleased(int key);

// ==================== Ввод с мыши ====================

BLAZEBOLT_API int BBE_Input_IsMousePressed(int button);
BLAZEBOLT_API int BBE_Input_IsMouseJustPressed(int button);
BLAZEBOLT_API void BBE_Input_GetMousePosition(float* x, float* y);
BLAZEBOLT_API void BBE_Input_GetMouseDelta(float* x, float* y);
BLAZEBOLT_API void BBE_Input_GetScroll(float* x, float* y);

// ==================== Геймпад ====================

BLAZEBOLT_API int BBE_Input_IsGamepadConnected(int id);
BLAZEBOLT_API int BBE_Input_IsGamepadButtonPressed(int id, int button);
BLAZEBOLT_API int BBE_Input_IsGamepadButtonJustPressed(int id, int button);
BLAZEBOLT_API float BBE_Input_GetGamepadAxis(int id, int axis);

// ==================== Утилиты ====================

BLAZEBOLT_API float BBE_GetDeltaTime(void);
BLAZEBOLT_API int BBE_GetWindowWidth(void);
BLAZEBOLT_API int BBE_GetWindowHeight(void);
BLAZEBOLT_API void BBE_Sleep(int milliseconds);

#ifdef __cplusplus
}
#endif