// Window.h
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
private:
    int _width, _height;
    GLFWwindow* window;
    const char* _title;

public:
    Window(int width, int height, const char* title) 
        : _width(width), _height(height), _title(title), window(nullptr) {
        
        if (!glfwInit()) {
            return;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }
        
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            return;
        }

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
            glViewport(0, 0, width, height);
        });

        glfwSwapInterval(0);
    }
    
    ~Window() {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
    
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    Window(Window&& other) noexcept 
        : _width(other._width), _height(other._height), 
          _title(other._title), window(other.window) {
        other.window = nullptr;
    }
    
    Window& operator=(Window&& other) noexcept {
        if (this != &other) {
            if (window) glfwDestroyWindow(window);
            _width = other._width;
            _height = other._height;
            _title = other._title;
            window = other.window;
            other.window = nullptr;
        }
        return *this;
    }
    
    bool shouldClose() const {
        return glfwWindowShouldClose(window);
    }
    
    void pollEvents() {
        glfwPollEvents();
    }
    
    void swapBuffers() {
        glfwSwapBuffers(window);
    }
    
    void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
    
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    GLFWwindow* getGLFWwindow() const { return window; }
    
    void setTitle(const char* title) {
        _title = title;
        glfwSetWindowTitle(window, title);
    }
    
    void setSize(int width, int height) {
        _width = width;
        _height = height;
        glfwSetWindowSize(window, width, height);
    }
    
    void setShouldClose(bool flag) {
        glfwSetWindowShouldClose(window, flag);
    }
};