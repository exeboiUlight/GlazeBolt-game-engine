#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
#include <vector>

class Input {
public:
    static Input& getInstance() {
        static Input instance;
        return instance;
    }

    void init(GLFWwindow* window) {
        m_window = window;
        
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetJoystickCallback(joystickCallback);
    }

    void update() {
        for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
            if (glfwJoystickPresent(i)) {
                updateGamepad(i);
            }
        }
        
        m_keyPressedPrev = m_keyPressed;
        m_mousePressedPrev = m_mousePressed;
        m_gamepadPressedPrev = m_gamepadPressed;
    }

    bool isKeyPressed(int key) const {
        if (key < 0 || key >= 512) return false;
        return m_keyPressed[key];
    }

    bool isKeyJustPressed(int key) const {
        if (key < 0 || key >= 512) return false;
        return m_keyPressed[key] && !m_keyPressedPrev[key];
    }

    bool isKeyJustReleased(int key) const {
        if (key < 0 || key >= 512) return false;
        return !m_keyPressed[key] && m_keyPressedPrev[key];
    }

    bool isMouseButtonPressed(int button) const {
        if (button < 0 || button >= 8) return false;
        return m_mousePressed[button];
    }

    bool isMouseButtonJustPressed(int button) const {
        if (button < 0 || button >= 8) return false;
        return m_mousePressed[button] && !m_mousePressedPrev[button];
    }

    bool isMouseButtonJustReleased(int button) const {
        if (button < 0 || button >= 8) return false;
        return !m_mousePressed[button] && m_mousePressedPrev[button];
    }

    double getMouseX() const { return m_mouseX; }
    double getMouseY() const { return m_mouseY; }
    double getMouseDeltaX() const { return m_mouseDeltaX; }
    double getMouseDeltaY() const { return m_mouseDeltaY; }
    double getScrollX() const { return m_scrollX; }
    double getScrollY() const { return m_scrollY; }

    bool isGamepadConnected(int id) const {
        return id >= 0 && id < 16 && m_gamepadConnected[id];
    }

    bool isGamepadButtonPressed(int id, int button) const {
        if (id < 0 || id >= 16 || button < 0 || button >= 16) return false;
        return m_gamepadPressed[id][button];
    }

    bool isGamepadButtonJustPressed(int id, int button) const {
        if (id < 0 || id >= 16 || button < 0 || button >= 16) return false;
        return m_gamepadPressed[id][button] && !m_gamepadPressedPrev[id][button];
    }

    bool isGamepadButtonJustReleased(int id, int button) const {
        if (id < 0 || id >= 16 || button < 0 || button >= 16) return false;
        return !m_gamepadPressed[id][button] && m_gamepadPressedPrev[id][button];
    }

    float getGamepadAxis(int id, int axis) const {
        if (id < 0 || id >= 16 || axis < 0 || axis >= 8) return 0.0f;
        return m_gamepadAxes[id][axis];
    }

    void resetScroll() {
        m_scrollX = 0.0;
        m_scrollY = 0.0;
    }

    void resetMouseDelta() {
        m_mouseDeltaX = 0.0;
        m_mouseDeltaY = 0.0;
    }

private:
    Input() : m_window(nullptr), m_mouseX(0.0), m_mouseY(0.0), 
              m_mouseDeltaX(0.0), m_mouseDeltaY(0.0), 
              m_scrollX(0.0), m_scrollY(0.0) {
        m_keyPressed.fill(false);
        m_keyPressedPrev.fill(false);
        m_mousePressed.fill(false);
        m_mousePressedPrev.fill(false);
        m_gamepadConnected.fill(false);
        
        for (int i = 0; i < 16; ++i) {
            m_gamepadPressed[i].fill(false);
            m_gamepadPressedPrev[i].fill(false);
            m_gamepadAxes[i].fill(0.0f);
        }
    }

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        Input& input = getInstance();
        if (action == GLFW_PRESS) {
            input.m_keyPressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            input.m_keyPressed[key] = false;
        }
    }

    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        Input& input = getInstance();
        if (action == GLFW_PRESS) {
            input.m_mousePressed[button] = true;
        } else if (action == GLFW_RELEASE) {
            input.m_mousePressed[button] = false;
        }
    }

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        Input& input = getInstance();
        input.m_mouseDeltaX = xpos - input.m_mouseX;
        input.m_mouseDeltaY = ypos - input.m_mouseY;
        input.m_mouseX = xpos;
        input.m_mouseY = ypos;
    }

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        Input& input = getInstance();
        input.m_scrollX = xoffset;
        input.m_scrollY = yoffset;
    }

    static void joystickCallback(int jid, int event) {
        Input& input = getInstance();
        if (event == GLFW_CONNECTED) {
            input.m_gamepadConnected[jid] = true;
        } else if (event == GLFW_DISCONNECTED) {
            input.m_gamepadConnected[jid] = false;
            input.m_gamepadPressed[jid].fill(false);
            input.m_gamepadPressedPrev[jid].fill(false);
            input.m_gamepadAxes[jid].fill(0.0f);
        }
    }

    void updateGamepad(int id) {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(id, &state)) {
            for (int i = 0; i < 16; ++i) {
                m_gamepadPressed[id][i] = state.buttons[i] == GLFW_PRESS;
            }
            for (int i = 0; i < 8; ++i) {
                m_gamepadAxes[id][i] = state.axes[i];
            }
        }
    }

    GLFWwindow* m_window;
    
    std::array<bool, 512> m_keyPressed;
    std::array<bool, 512> m_keyPressedPrev;
    
    std::array<bool, 8> m_mousePressed;
    std::array<bool, 8> m_mousePressedPrev;
    
    double m_mouseX;
    double m_mouseY;
    double m_mouseDeltaX;
    double m_mouseDeltaY;
    double m_scrollX;
    double m_scrollY;
    
    std::array<bool, 16> m_gamepadConnected;
    std::array<std::array<bool, 16>, 16> m_gamepadPressed;
    std::array<std::array<bool, 16>, 16> m_gamepadPressedPrev;
    std::array<std::array<float, 8>, 16> m_gamepadAxes;
};