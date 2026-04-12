#pragma once

#include <utils/math/vector.h>
#include <graphics/window.h>

class Camera2D {
private:
    Vector2 m_position;
    Vector2 m_scale;
    Window* m_window;
    float m_targetAspectRatio;
    
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Camera2D* camera = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
        if (camera) {
            camera->updateViewport();
        }
    }
    
public:
    Camera2D(Window& window, Vector2 position, Vector2 scale, float targetAspectRatio = 16.0f / 9.0f) 
        : m_position(position), m_scale(scale), m_window(&window), m_targetAspectRatio(targetAspectRatio) {
        
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
    
    void setTargetAspectRatio(float aspectRatio) {
        m_targetAspectRatio = aspectRatio;
        updateViewport();
    }
    
    float getTargetAspectRatio() const {
        return m_targetAspectRatio;
    }
    
private:
    void updateViewport() {
        if (m_window && m_window->GetWindow()) {
            int windowWidth, windowHeight;
            glfwGetFramebufferSize(m_window->GetWindow(), &windowWidth, &windowHeight);
            
            float windowAspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
            
            int viewportWidth, viewportHeight;
            int offsetX, offsetY;
            
            if (windowAspectRatio > m_targetAspectRatio) {
                viewportHeight = windowHeight;
                viewportWidth = static_cast<int>(windowHeight * m_targetAspectRatio);
                offsetX = (windowWidth - viewportWidth) / 2;
                offsetY = 0;
            } else {
                viewportWidth = windowWidth;
                viewportHeight = static_cast<int>(windowWidth / m_targetAspectRatio);
                offsetX = 0;
                offsetY = (windowHeight - viewportHeight) / 2;
            }
            
            glViewport(
                offsetX + static_cast<int>(-m_position.x * (static_cast<float>(viewportWidth) / windowWidth)),
                offsetY + static_cast<int>(-m_position.y * (static_cast<float>(viewportHeight) / windowHeight)),
                static_cast<int>(viewportWidth * m_scale.x),
                static_cast<int>(viewportHeight * m_scale.y)
            );
        }
    }
};