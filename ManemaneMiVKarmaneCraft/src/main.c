#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// ============== Шейдер для рендеринга мира через raymarching ==============
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec2 UV;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    UV = (aPos + 1.0) / 2.0;\n"
    "}\n";

const char* fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 UV;\n"
    "uniform float u_time;\n"
    "uniform vec2 u_resolution;\n"
    "uniform vec3 u_cameraPos;\n"
    "uniform vec3 u_cameraTarget;\n"
    "\n"
    "// Типы блоков\n"
    "#define TYPE_AIR 0\n"
    "#define TYPE_STONE 1\n"
    "#define TYPE_DIRT 2\n"
    "#define TYPE_GRASS 3\n"
    "#define TYPE_SAND 4\n"
    "#define TYPE_WOOD 5\n"
    "#define TYPE_LEAVES 6\n"
    "#define TYPE_WATER 7\n"
    "#define TYPE_BRICK 8\n"
    "\n"
    "// Безопасная функция получения блока\n"
    "int getBlockType(vec3 pos) {\n"
    "    int x = int(floor(pos.x + 0.5));\n"
    "    int y = int(floor(pos.y + 0.5));\n"
    "    int z = int(floor(pos.z + 0.5));\n"
    "    \n"
    "    // Границы мира\n"
    "    if(y < 0 || y > 128) return TYPE_AIR;\n"
    "    if(x < -32 || x > 48) return TYPE_AIR;\n"
    "    if(z < -32 || z > 48) return TYPE_AIR;\n"
    "    \n"
    "    // Простой тестовый мир\n"
    "    if(y == 60) {\n"
    "        // Пол земли из травы\n"
    "        return TYPE_GRASS;\n"
    "    }\n"
    "    if(y < 60 && y > 55) {\n"
    "        return TYPE_DIRT;\n"
    "    }\n"
    "    if(y <= 55 && y > 0) {\n"
    "        return TYPE_STONE;\n"
    "    }\n"
    "    if(y == 0) {\n"
    "        return TYPE_BRICK;\n"
    "    }\n"
    "    \n"
    "    // Тестовый столб\n"
    "    if(x >= 7 && x <= 9 && z >= 7 && z <= 9) {\n"
    "        if(y > 60 && y < 70) {\n"
    "            return TYPE_WOOD;\n"
    "        }\n"
    "        if(y == 70) {\n"
    "            return TYPE_LEAVES;\n"
    "        }\n"
    "    }\n"
    "    \n"
    "    // Стена для теста\n"
    "    if(x == 15 && y >= 60 && y <= 65 && z >= 5 && z <= 15) {\n"
    "        return TYPE_STONE;\n"
    "    }\n"
    "    \n"
    "    return TYPE_AIR;\n"
    "}\n"
    "\n"
    "// Получение цвета блока\n"
    "vec3 getBlockColor(int type) {\n"
    "    if(type == TYPE_STONE) return vec3(0.5, 0.5, 0.5);\n"
    "    if(type == TYPE_DIRT) return vec3(0.6, 0.4, 0.2);\n"
    "    if(type == TYPE_GRASS) return vec3(0.2, 0.8, 0.2);\n"
    "    if(type == TYPE_SAND) return vec3(0.9, 0.8, 0.5);\n"
    "    if(type == TYPE_WOOD) return vec3(0.6, 0.4, 0.1);\n"
    "    if(type == TYPE_LEAVES) return vec3(0.2, 0.6, 0.1);\n"
    "    if(type == TYPE_WATER) return vec3(0.2, 0.5, 0.8);\n"
    "    if(type == TYPE_BRICK) return vec3(0.4, 0.3, 0.3);\n"
    "    return vec3(1.0, 0.0, 1.0);\n"
    "}\n"
    "\n"
    "// Raymarching с фиксированным шагом\n"
    "bool rayMarch(vec3 origin, vec3 direction, float maxDist, out vec3 hitPoint, out vec3 hitNormal, out int hitType) {\n"
    "    float dist = 0.0;\n"
    "    \n"
    "    for(int i = 0; i < 150; i++) {\n"
    "        vec3 point = origin + direction * dist;\n"
    "        \n"
    "        if(dist > maxDist) {\n"
    "            return false;\n"
    "        }\n"
    "        \n"
    "        int type = getBlockType(point);\n"
    "        \n"
    "        if(type != TYPE_AIR) {\n"
    "            hitPoint = point;\n"
    "            hitType = type;\n"
    "            \n"
    "            // Упрощенное вычисление нормали\n"
    "            vec3 eps = vec3(0.1, 0.0, 0.0);\n"
    "            vec3 normal;\n"
    "            normal.x = float(getBlockType(point + eps.xyy)) - float(getBlockType(point - eps.xyy));\n"
    "            normal.y = float(getBlockType(point + eps.yxy)) - float(getBlockType(point - eps.yxy));\n"
    "            normal.z = float(getBlockType(point + eps.yyx)) - float(getBlockType(point - eps.yyx));\n"
    "            \n"
    "            if(length(normal) < 0.01) {\n"
    "                hitNormal = vec3(0.0, 1.0, 0.0);\n"
    "            } else {\n"
    "                hitNormal = normalize(normal);\n"
    "            }\n"
    "            \n"
    "            return true;\n"
    "        }\n"
    "        \n"
    "        dist += 0.2;\n"
    "    }\n"
    "    \n"
    "    return false;\n"
    "}\n"
    "\n"
    "// Функция для неба\n"
    "vec3 getSkyColor(vec3 direction) {\n"
    "    float t = 0.5 + 0.5 * direction.y;\n"
    "    vec3 color = mix(vec3(0.1, 0.2, 0.4), vec3(0.4, 0.6, 0.9), t);\n"
    "    return color;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec3 cameraPos = u_cameraPos;\n"
    "    vec3 cameraTarget = u_cameraTarget;\n"
    "    \n"
    "    vec3 forward = normalize(cameraTarget - cameraPos);\n"
    "    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));\n"
    "    vec3 up = cross(right, forward);\n"
    "    \n"
    "    float aspect = u_resolution.x / u_resolution.y;\n"
    "    float fov = 70.0 * 3.14159 / 180.0;\n"
    "    float scale = tan(fov * 0.5);\n"
    "    \n"
    "    vec2 uv = UV * 2.0 - 1.0;\n"
    "    uv.x *= aspect;\n"
    "    \n"
    "    vec3 rayDir = normalize(forward + right * uv.x * scale + up * uv.y * scale);\n"
    "    \n"
    "    vec3 hitPoint, hitNormal;\n"
    "    int hitType;\n"
    "    \n"
    "    vec3 color;\n"
    "    \n"
    "    if(rayMarch(cameraPos, rayDir, 300.0, hitPoint, hitNormal, hitType)) {\n"
    "        vec3 blockColor = getBlockColor(hitType);\n"
    "        \n"
    "        // Простое освещение\n"
    "        vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));\n"
    "        float ambient = 0.4;\n"
    "        float diffuse = max(0.1, dot(hitNormal, lightDir));\n"
    "        \n"
    "        float lighting = ambient + diffuse * 0.6;\n"
    "        color = blockColor * lighting;\n"
    "    } else {\n"
    "        color = getSkyColor(rayDir);\n"
    "    }\n"
    "    \n"
    "    FragColor = vec4(color, 1.0);\n"
    "}\n";

// ============== Инициализация OpenGL ==============
typedef struct {
    GLFWwindow* window;
    int width;
    int height;
    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    float lastTime;
    float cameraYaw;
    float cameraPitch;
    float cameraX, cameraY, cameraZ;
    bool firstMouse;
    double lastX, lastY;
} Engine;

float fullscreenVertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
    if(engine) {
        engine->width = width;
        engine->height = height;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = (Engine*)glfwGetWindowUserPointer(window);
    if(!engine) return;
    
    if(engine->firstMouse) {
        engine->lastX = xpos;
        engine->lastY = ypos;
        engine->firstMouse = false;
    }
    
    float xoffset = xpos - engine->lastX;
    float yoffset = engine->lastY - ypos;
    engine->lastX = xpos;
    engine->lastY = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    engine->cameraYaw += xoffset;
    engine->cameraPitch += yoffset;
    
    if(engine->cameraPitch > 89.0f) engine->cameraPitch = 89.0f;
    if(engine->cameraPitch < -89.0f) engine->cameraPitch = -89.0f;
}

unsigned int createShader(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader error: %s\n", infoLog);
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader error: %s\n", infoLog);
    }
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Program link error: %s\n", infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

Engine createEngine(int width, int height, const char* title) {
    Engine engine;
    engine.width = width;
    engine.height = height;
    engine.cameraX = 8.0f;
    engine.cameraY = 65.0f;
    engine.cameraZ = 8.0f;
    engine.cameraYaw = -90.0f;
    engine.cameraPitch = -10.0f;
    engine.firstMouse = true;
    engine.lastTime = 0;
    
    if(!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        engine.window = NULL;
        return engine;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    engine.window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!engine.window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return engine;
    }
    
    glfwMakeContextCurrent(engine.window);
    glfwSetFramebufferSizeCallback(engine.window, framebuffer_size_callback);
    glfwSetCursorPosCallback(engine.window, mouse_callback);
    glfwSetInputMode(engine.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(engine.window, &engine);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to init GLAD\n");
        return engine;
    }
    
    printf("OpenGL: %s\n", glGetString(GL_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    
    engine.shaderProgram = createShader(vertexShaderSource, fragmentShaderSource);
    
    glGenVertexArrays(1, &engine.VAO);
    glGenBuffers(1, &engine.VBO);
    
    glBindVertexArray(engine.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, engine.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullscreenVertices), fullscreenVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    return engine;
}

int main() {
    Engine engine = createEngine(1024, 768, "Minecraft - Raymarching Edition");
    if(!engine.window) {
        return -1;
    }
    
    glUseProgram(engine.shaderProgram);
    int timeLoc = glGetUniformLocation(engine.shaderProgram, "u_time");
    int resLoc = glGetUniformLocation(engine.shaderProgram, "u_resolution");
    int cameraPosLoc = glGetUniformLocation(engine.shaderProgram, "u_cameraPos");
    int cameraTargetLoc = glGetUniformLocation(engine.shaderProgram, "u_cameraTarget");
    
    printf("\n=== MINECRAFT RAYMARCHING ===\n");
    printf("Camera position: (%.1f, %.1f, %.1f)\n", engine.cameraX, engine.cameraY, engine.cameraZ);
    printf("Controls: WASD - Move, Mouse - Look, Space/Shift - Up/Down, ESC - Exit\n\n");
    
    engine.lastTime = glfwGetTime();
    int frameCount = 0;
    
    while(!glfwWindowShouldClose(engine.window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - engine.lastTime;
        
        if(deltaTime > 0.1) deltaTime = 0.1;
        
        engine.lastTime = currentTime;
        
        float speed = 20.0f * deltaTime;
        
        float radYaw = engine.cameraYaw * 3.14159f / 180.0f;
        float radPitch = engine.cameraPitch * 3.14159f / 180.0f;
        
        float forwardX = cos(radYaw) * cos(radPitch);
        float forwardY = sin(radPitch);
        float forwardZ = sin(radYaw) * cos(radPitch);
        
        float rightX = cos(radYaw + 3.14159f/2.0f);
        float rightZ = sin(radYaw + 3.14159f/2.0f);
        
        if(glfwGetKey(engine.window, GLFW_KEY_W) == GLFW_PRESS) {
            engine.cameraX += forwardX * speed;
            engine.cameraZ += forwardZ * speed;
        }
        if(glfwGetKey(engine.window, GLFW_KEY_S) == GLFW_PRESS) {
            engine.cameraX -= forwardX * speed;
            engine.cameraZ -= forwardZ * speed;
        }
        if(glfwGetKey(engine.window, GLFW_KEY_A) == GLFW_PRESS) {
            engine.cameraX -= rightX * speed;
            engine.cameraZ -= rightZ * speed;
        }
        if(glfwGetKey(engine.window, GLFW_KEY_D) == GLFW_PRESS) {
            engine.cameraX += rightX * speed;
            engine.cameraZ += rightZ * speed;
        }
        if(glfwGetKey(engine.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            engine.cameraY += speed;
        }
        if(glfwGetKey(engine.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            engine.cameraY -= speed;
        }
        
        float targetX = engine.cameraX + forwardX;
        float targetY = engine.cameraY + forwardY;
        float targetZ = engine.cameraZ + forwardZ;
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUniform1f(timeLoc, currentTime);
        glUniform2f(resLoc, (float)engine.width, (float)engine.height);
        glUniform3f(cameraPosLoc, engine.cameraX, engine.cameraY, engine.cameraZ);
        glUniform3f(cameraTargetLoc, targetX, targetY, targetZ);
        
        glBindVertexArray(engine.VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glfwSwapBuffers(engine.window);
        glfwPollEvents();
        
        frameCount++;
        if(currentTime - engine.lastTime >= 1.0f) {
            // printf("FPS: %d | Pos: (%.1f, %.1f, %.1f)\n",
            //        frameCount, engine.cameraX, engine.cameraY, engine.cameraZ);
            frameCount = 0;
        }
        
        if(glfwGetKey(engine.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(engine.window, 1);
        }
    }
    
    glDeleteVertexArrays(1, &engine.VAO);
    glDeleteBuffers(1, &engine.VBO);
    glDeleteProgram(engine.shaderProgram);
    glfwTerminate();
    
    return 0;
}