#pragma once

#include <utils/math/vector.h>
#include <graphics/window.h>

class Camera2D {
private:
    Vector2 m_position;
    Vector2 m_scale;
    Window* m_window;
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Camera2D* camera = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
        if (camera) {
            glViewport(
                static_cast<int>(-camera->m_position.x),
                static_cast<int>(-camera->m_position.y),
                static_cast<int>(width * camera->m_scale.x),
                static_cast<int>(height * camera->m_scale.y)
            );
        }
    }
    
public:
    Camera2D(Window& window, Vector2 position, Vector2 scale) 
        : m_position(position), m_scale(scale), m_window(&window) {
        
        GLFWwindow* glfwWindow = window.GetWindow();
        glfwSetWindowUserPointer(glfwWindow, this);
        glfwSetFramebufferSizeCallback(glfwWindow, FramebufferSizeCallback);
        
        updateViewport();
    }
    
    Vector2 getPosition() const { return m_position; }
    
    void setPosition(Vector2 position) { 
        m_position = position;
        updateViewport();
    }
    
    Vector2 getScale() const { return m_scale; }
    
    void setScale(Vector2 scale) { 
        m_scale = scale;
        updateViewport();
    }
    
    void move(Vector2 delta) {
        m_position.x += delta.x;
        m_position.y += delta.y;
        updateViewport();
    }
    
    void moveCamera(Vector2 delta) {
        m_position.x += delta.x;
        m_position.y += delta.y;
        updateViewport();
    }
    
private:
    void updateViewport() {
        if (m_window && m_window->GetWindow()) {
            int width, height;
            glfwGetFramebufferSize(m_window->GetWindow(), &width, &height);
            glViewport(
                static_cast<int>(-m_position.x),
                static_cast<int>(-m_position.y),
                static_cast<int>(width * m_scale.x),
                static_cast<int>(height * m_scale.y)
            );
        }
    }
};