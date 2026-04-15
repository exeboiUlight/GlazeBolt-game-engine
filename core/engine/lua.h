// lua.h
#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <graphics/window.h>

#include <subject/sprite2D.h>
#include <subject/text.h>
#include <subject/audio.h>
#include <subject/animatad2D.h>
#include <input/input.h>

#include <string>
#include <unordered_map>

class luaEngine {
private:
    lua_State* L;
    Window* m_window;
    std::unordered_map<std::string, Sprite2D*> m_sprites;
    std::unordered_map<std::string, Animation2D*> m_animations;
    std::unordered_map<std::string, Text*> m_texts;
    std::unordered_map<std::string, Audio*> m_audios;
    std::unordered_map<std::string, int> m_soundIds;

    static int lua_SetClearColor(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        float r = luaL_checknumber(L, 1);
        float g = luaL_checknumber(L, 2);
        float b = luaL_checknumber(L, 3);
        float a = luaL_optnumber(L, 4, 1.0f);
        engine->m_window->setClearColor(r, g, b, a);
        return 0;
    }

    static int lua_Clear(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        engine->m_window->clear();
        return 0;
    }

    static int lua_SwapBuffers(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        engine->m_window->swapBuffers();
        return 0;
    }

    static int lua_PollEvents(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        engine->m_window->pollEvents();
        Input::getInstance().update();
        return 0;
    }

    static int lua_ShouldClose(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        lua_pushboolean(L, engine->m_window->shouldClose());
        return 1;
    }

    static int lua_GetDeltaTime(lua_State* L) {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        lua_pushnumber(L, deltaTime);
        return 1;
    }

    static int lua_SpriteNew(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        if (engine->m_sprites.find(name) != engine->m_sprites.end()) {
            lua_pushboolean(L, false);
            return 1;
        }
        Sprite2D* sprite = new Sprite2D();
        engine->m_sprites[name] = sprite;
        lua_pushboolean(L, true);
        return 1;
    }

    static int lua_SpriteSetTexture(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        const char* path = luaL_checkstring(L, 2);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setTexture(path);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetPosition(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setPosition(x, y);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetSize(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float w = luaL_checknumber(L, 2);
        float h = luaL_checknumber(L, 3);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setSize(w, h);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetOrigin(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setOrigin(x, y);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetRotation(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float rot = luaL_checknumber(L, 2);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setRotation(rot);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetColor(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float r = luaL_checknumber(L, 2);
        float g = luaL_checknumber(L, 3);
        float b = luaL_checknumber(L, 4);
        float a = luaL_optnumber(L, 5, 1.0f);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setColor(r, g, b, a);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteSetVisible(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        bool visible = lua_toboolean(L, 2);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->setVisible(visible);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_SpriteDraw(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_sprites.find(name);
        if (it != engine->m_sprites.end()) {
            it->second->draw();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationNew(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        if (engine->m_animations.find(name) != engine->m_animations.end()) {
            lua_pushboolean(L, false);
            return 1;
        }
        Animation2D* anim = new Animation2D();
        engine->m_animations[name] = anim;
        lua_pushboolean(L, true);
        return 1;
    }

    static int lua_AnimationLoadGIF(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        const char* path = luaL_checkstring(L, 2);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            bool result = it->second->loadFromGIF(path);
            lua_pushboolean(L, result);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationLoadSpriteSheet(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        const char* path = luaL_checkstring(L, 2);
        int frameW = luaL_checkinteger(L, 3);
        int frameH = luaL_checkinteger(L, 4);
        int totalFrames = luaL_checkinteger(L, 5);
        int framesPerRow = luaL_checkinteger(L, 6);
        int delayMs = luaL_checkinteger(L, 7);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->loadFromSpriteSheet(path, frameW, frameH, totalFrames, framesPerRow, delayMs);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationUpdate(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float dt = luaL_checknumber(L, 2);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->update(dt);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationPlay(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->play();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationPause(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->pause();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationStop(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->stop();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationSetLooping(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        bool loop = lua_toboolean(L, 2);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->setLooping(loop);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationSetSpeed(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float speed = luaL_checknumber(L, 2);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->setSpeed(speed);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationSetPosition(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->setPosition(x, y);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationSetSize(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float w = luaL_checknumber(L, 2);
        float h = luaL_checknumber(L, 3);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->setSize(w, h);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AnimationDraw(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_animations.find(name);
        if (it != engine->m_animations.end()) {
            it->second->draw();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_TextNew(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        const char* fontPath = luaL_checkstring(L, 2);
        int fontSize = luaL_optinteger(L, 3, 48);
        if (engine->m_texts.find(name) != engine->m_texts.end()) {
            lua_pushboolean(L, false);
            return 1;
        }
        Text* text = new Text(fontPath, fontSize);
        text->setScreenSize(engine->m_window->getWidth(), engine->m_window->getHeight());
        engine->m_texts[name] = text;
        lua_pushboolean(L, true);
        return 1;
    }

    static int lua_TextSetString(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        const char* str = luaL_checkstring(L, 2);
        auto it = engine->m_texts.find(name);
        if (it != engine->m_texts.end()) {
            it->second->setText(str);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_TextSetPosition(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float x = luaL_checknumber(L, 2);
        float y = luaL_checknumber(L, 3);
        auto it = engine->m_texts.find(name);
        if (it != engine->m_texts.end()) {
            it->second->setPosition(x, y);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_TextSetScale(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float scale = luaL_checknumber(L, 2);
        auto it = engine->m_texts.find(name);
        if (it != engine->m_texts.end()) {
            it->second->setScale(scale);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_TextSetColor(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        float r = luaL_checknumber(L, 2);
        float g = luaL_checknumber(L, 3);
        float b = luaL_checknumber(L, 4);
        float a = luaL_optnumber(L, 5, 1.0f);
        auto it = engine->m_texts.find(name);
        if (it != engine->m_texts.end()) {
            it->second->setColor(r, g, b, a);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_TextDraw(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        auto it = engine->m_texts.find(name);
        if (it != engine->m_texts.end()) {
            it->second->draw();
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AudioInit(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* name = luaL_checkstring(L, 1);
        if (engine->m_audios.find(name) != engine->m_audios.end()) {
            lua_pushboolean(L, false);
            return 1;
        }
        Audio* audio = new Audio();
        bool result = audio->init();
        if (result) {
            engine->m_audios[name] = audio;
        } else {
            delete audio;
        }
        lua_pushboolean(L, result);
        return 1;
    }

    static int lua_AudioLoadSound(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* audioName = luaL_checkstring(L, 1);
        const char* filePath = luaL_checkstring(L, 2);
        const char* soundName = luaL_checkstring(L, 3);
        bool loop = lua_toboolean(L, 4);
        auto it = engine->m_audios.find(audioName);
        if (it != engine->m_audios.end()) {
            int id = it->second->loadSound(filePath, soundName, loop);
            if (id >= 0) {
                engine->m_soundIds[soundName] = id;
            }
            lua_pushinteger(L, id);
        } else {
            lua_pushinteger(L, -1);
        }
        return 1;
    }

    static int lua_AudioPlay(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* audioName = luaL_checkstring(L, 1);
        const char* soundName = luaL_checkstring(L, 2);
        auto it = engine->m_audios.find(audioName);
        if (it != engine->m_audios.end()) {
            it->second->play(soundName);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AudioStop(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* audioName = luaL_checkstring(L, 1);
        const char* soundName = luaL_checkstring(L, 2);
        auto it = engine->m_audios.find(audioName);
        if (it != engine->m_audios.end()) {
            it->second->stop(soundName);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_AudioSetVolume(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        const char* audioName = luaL_checkstring(L, 1);
        const char* soundName = luaL_checkstring(L, 2);
        float volume = luaL_checknumber(L, 3);
        auto it = engine->m_audios.find(audioName);
        if (it != engine->m_audios.end()) {
            it->second->setVolume(soundName, volume);
            lua_pushboolean(L, true);
        } else {
            lua_pushboolean(L, false);
        }
        return 1;
    }

    static int lua_IsKeyPressed(lua_State* L) {
        int key = luaL_checkinteger(L, 1);
        lua_pushboolean(L, Input::getInstance().isKeyPressed(key));
        return 1;
    }

    static int lua_IsKeyJustPressed(lua_State* L) {
        int key = luaL_checkinteger(L, 1);
        lua_pushboolean(L, Input::getInstance().isKeyJustPressed(key));
        return 1;
    }

    static int lua_GetMouseX(lua_State* L) {
        lua_pushnumber(L, Input::getInstance().getMouseX());
        return 1;
    }

    static int lua_GetMouseY(lua_State* L) {
        lua_pushnumber(L, Input::getInstance().getMouseY());
        return 1;
    }

    static int lua_IsMousePressed(lua_State* L) {
        int button = luaL_checkinteger(L, 1);
        lua_pushboolean(L, Input::getInstance().isMouseButtonPressed(button));
        return 1;
    }

    static int lua_GetWindowWidth(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        lua_pushinteger(L, engine->m_window->getWidth());
        return 1;
    }

    static int lua_GetWindowHeight(lua_State* L) {
        luaEngine* engine = static_cast<luaEngine*>(lua_touserdata(L, lua_upvalueindex(1)));
        lua_pushinteger(L, engine->m_window->getHeight());
        return 1;
    }

    static int lua_Print(lua_State* L) {
        int n = lua_gettop(L);
        for (int i = 1; i <= n; i++) {
            const char* str = luaL_tolstring(L, i, nullptr);
            std::cout << str;
            lua_pop(L, 1);
            if (i < n) std::cout << " ";
        }
        std::cout << std::endl;
        return 0;
    }

public:
    luaEngine(Window& window) : L(luaL_newstate()), m_window(&window) {
        luaL_openlibs(L);

        lua_pushlightuserdata(L, this);

        lua_pushcclosure(L, lua_SetClearColor, 1);
        lua_setglobal(L, "SetClearColor");

        lua_pushcclosure(L, lua_Clear, 1);
        lua_setglobal(L, "Clear");

        lua_pushcclosure(L, lua_SwapBuffers, 1);
        lua_setglobal(L, "SwapBuffers");

        lua_pushcclosure(L, lua_PollEvents, 1);
        lua_setglobal(L, "PollEvents");

        lua_pushcclosure(L, lua_ShouldClose, 1);
        lua_setglobal(L, "ShouldClose");

        lua_pushcclosure(L, lua_GetDeltaTime, 1);
        lua_setglobal(L, "GetDeltaTime");

        lua_pushcclosure(L, lua_SpriteNew, 1);
        lua_setglobal(L, "SpriteNew");

        lua_pushcclosure(L, lua_SpriteSetTexture, 1);
        lua_setglobal(L, "SpriteSetTexture");

        lua_pushcclosure(L, lua_SpriteSetPosition, 1);
        lua_setglobal(L, "SpriteSetPosition");

        lua_pushcclosure(L, lua_SpriteSetSize, 1);
        lua_setglobal(L, "SpriteSetSize");

        lua_pushcclosure(L, lua_SpriteSetOrigin, 1);
        lua_setglobal(L, "SpriteSetOrigin");

        lua_pushcclosure(L, lua_SpriteSetRotation, 1);
        lua_setglobal(L, "SpriteSetRotation");

        lua_pushcclosure(L, lua_SpriteSetColor, 1);
        lua_setglobal(L, "SpriteSetColor");

        lua_pushcclosure(L, lua_SpriteSetVisible, 1);
        lua_setglobal(L, "SpriteSetVisible");

        lua_pushcclosure(L, lua_SpriteDraw, 1);
        lua_setglobal(L, "SpriteDraw");

        lua_pushcclosure(L, lua_AnimationNew, 1);
        lua_setglobal(L, "AnimationNew");

        lua_pushcclosure(L, lua_AnimationLoadGIF, 1);
        lua_setglobal(L, "AnimationLoadGIF");

        lua_pushcclosure(L, lua_AnimationLoadSpriteSheet, 1);
        lua_setglobal(L, "AnimationLoadSpriteSheet");

        lua_pushcclosure(L, lua_AnimationUpdate, 1);
        lua_setglobal(L, "AnimationUpdate");

        lua_pushcclosure(L, lua_AnimationPlay, 1);
        lua_setglobal(L, "AnimationPlay");

        lua_pushcclosure(L, lua_AnimationPause, 1);
        lua_setglobal(L, "AnimationPause");

        lua_pushcclosure(L, lua_AnimationStop, 1);
        lua_setglobal(L, "AnimationStop");

        lua_pushcclosure(L, lua_AnimationSetLooping, 1);
        lua_setglobal(L, "AnimationSetLooping");

        lua_pushcclosure(L, lua_AnimationSetSpeed, 1);
        lua_setglobal(L, "AnimationSetSpeed");

        lua_pushcclosure(L, lua_AnimationSetPosition, 1);
        lua_setglobal(L, "AnimationSetPosition");

        lua_pushcclosure(L, lua_AnimationSetSize, 1);
        lua_setglobal(L, "AnimationSetSize");

        lua_pushcclosure(L, lua_AnimationDraw, 1);
        lua_setglobal(L, "AnimationDraw");

        lua_pushcclosure(L, lua_TextNew, 1);
        lua_setglobal(L, "TextNew");

        lua_pushcclosure(L, lua_TextSetString, 1);
        lua_setglobal(L, "TextSetString");

        lua_pushcclosure(L, lua_TextSetPosition, 1);
        lua_setglobal(L, "TextSetPosition");

        lua_pushcclosure(L, lua_TextSetScale, 1);
        lua_setglobal(L, "TextSetScale");

        lua_pushcclosure(L, lua_TextSetColor, 1);
        lua_setglobal(L, "TextSetColor");

        lua_pushcclosure(L, lua_TextDraw, 1);
        lua_setglobal(L, "TextDraw");

        lua_pushcclosure(L, lua_AudioInit, 1);
        lua_setglobal(L, "AudioInit");

        lua_pushcclosure(L, lua_AudioLoadSound, 1);
        lua_setglobal(L, "AudioLoadSound");

        lua_pushcclosure(L, lua_AudioPlay, 1);
        lua_setglobal(L, "AudioPlay");

        lua_pushcclosure(L, lua_AudioStop, 1);
        lua_setglobal(L, "AudioStop");

        lua_pushcclosure(L, lua_AudioSetVolume, 1);
        lua_setglobal(L, "AudioSetVolume");

        lua_pushcclosure(L, lua_IsKeyPressed, 1);
        lua_setglobal(L, "IsKeyPressed");

        lua_pushcclosure(L, lua_IsKeyJustPressed, 1);
        lua_setglobal(L, "IsKeyJustPressed");

        lua_pushcclosure(L, lua_GetMouseX, 1);
        lua_setglobal(L, "GetMouseX");

        lua_pushcclosure(L, lua_GetMouseY, 1);
        lua_setglobal(L, "GetMouseY");

        lua_pushcclosure(L, lua_IsMousePressed, 1);
        lua_setglobal(L, "IsMousePressed");

        lua_pushcclosure(L, lua_GetWindowWidth, 1);
        lua_setglobal(L, "GetWindowWidth");

        lua_pushcclosure(L, lua_GetWindowHeight, 1);
        lua_setglobal(L, "GetWindowHeight");

        lua_pushcclosure(L, lua_Print, 1);
        lua_setglobal(L, "Print");

        Input::getInstance().init(window.GetWindow());
    }

    ~luaEngine() {
        for (auto& pair : m_sprites) delete pair.second;
        for (auto& pair : m_animations) delete pair.second;
        for (auto& pair : m_texts) delete pair.second;
        for (auto& pair : m_audios) delete pair.second;
        if (L) lua_close(L);
    }

    bool doFile(const std::string& filename) {
        if (luaL_dofile(L, filename.c_str()) != LUA_OK) {
            std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            return false;
        }
        return true;
    }

    bool doString(const std::string& code) {
        if (luaL_dostring(L, code.c_str()) != LUA_OK) {
            std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            return false;
        }
        return true;
    }

    void onWindowResize(int width, int height) {
        for (auto& pair : m_texts) {
            pair.second->setScreenSize(width, height);
        }
    }
};