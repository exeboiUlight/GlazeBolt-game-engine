#include <graphics/window.h>
#include <graphics/mesh.h>
#include <subject/camera.h>
#include <graphics/shader.h>

int main() {
    Window window(800, 600, u8"BlezeBolt game engine - движок для инди игр");
    Camera2D camera(window, {0, 0}, {1, 1});

    const char* vertexSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    Shader shader(vertexSource, fragmentSource);
    
    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);

    Mesh2D triange = Mesh2D();

    float cameraSpeed = 0.2f;

    std::vector<Mesh2D::Vertex> vertex = {{1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}};

    triange.setData(vertex, {0, 1, 2});

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

        shader.use();
        triange.draw();

        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}