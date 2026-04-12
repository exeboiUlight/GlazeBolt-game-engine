#pragma once

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <utils/math/vector.h>
#include <stb_image/stb_image.h>
#include <iostream>

class Sprite2D {
private:
    struct TexturedVertex {
        float x, y;
        float u, v;
    };
    
    Mesh2D* m_mesh;
    Shader* m_shader;
    GLuint m_texture;
    GLuint m_VBO, m_VAO, m_EBO;
    Vector2 m_position;
    Vector2 m_size;
    Vector2 m_origin;
    float m_rotation;
    Vector4 m_color;
    bool m_visible;
    std::string m_texturePath;
    bool m_ownsShader;
    GLsizei m_indexCount;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    
    void generateTexturedQuad() {
        float halfWidth = m_size.x * 0.5f;
        float halfHeight = m_size.y * 0.5f;
        
        float left = -halfWidth;
        float right = halfWidth;
        float bottom = -halfHeight;
        float top = halfHeight;
        
        float originX = m_origin.x * m_size.x;
        float originY = m_origin.y * m_size.y;
        
        left += originX;
        right += originX;
        bottom += originY;
        top += originY;
        
        std::vector<float> vertices = {
            left,  top,      0.0f, 1.0f,
            right, top,      1.0f, 1.0f,
            right, bottom,   1.0f, 0.0f,
            left,  bottom,   0.0f, 0.0f
        };
        
        std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
        };
        
        m_indexCount = indices.size();
        
        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);
        }
        
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        
        glBindVertexArray(m_VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
    }
    
    void updateTransform() {
        if (!m_shader) return;
        
        Matrix3x3 transform = Matrix3x3::identity();
        
        transform = Matrix3x3::translation(m_position.x, m_position.y) * transform;
        
        transform = Matrix3x3::rotation(m_rotation) * transform;
        
        transform = Matrix3x3::scale(m_size.x * 0.5f, m_size.y) * transform;
        
        float transformArray[9];
        transform.toFloatArray(transformArray);
        
        m_shader->setMat3("uTransform", transformArray);
    }
    
    void loadTexture(const std::string& filepath) {
        if (m_texture != 0) {
            glDeleteTextures(1, &m_texture);
        }
        
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
        
        if (data) {
            GLenum internalFormat, format;
            
            if (nrChannels == 1) {
                internalFormat = GL_RED;
                format = GL_RED;
            } else if (nrChannels == 3) {
                internalFormat = GL_RGB;
                format = GL_RGB;
            } else if (nrChannels == 4) {
                internalFormat = GL_RGBA;
                format = GL_RGBA;
            } else {
                internalFormat = GL_RGB;
                format = GL_RGB;
            }
            
            glGenTextures(1, &m_texture);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            stbi_image_free(data);
            
            m_texturePath = filepath;
            std::cout << "Texture loaded successfully: " << filepath << " (" << width << "x" << height << ", channels: " << nrChannels << ")" << std::endl;
        } else {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
            std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
            m_texture = 0;
        }
    }
    
    void initDefaultShader() {
        m_shader = new Shader(vertexShaderSource, fragmentShaderSource);
        m_ownsShader = true;
    }
    
public:
    Sprite2D() 
        : m_mesh(nullptr), m_shader(nullptr), m_texture(0), 
          m_VBO(0), m_VAO(0), m_EBO(0),
          m_position(0, 0), m_size(1, 1), m_origin(0.5f, 0.5f),
          m_rotation(0), m_color(1, 1, 1, 1), m_visible(true), 
          m_ownsShader(false), m_indexCount(0) {
        initDefaultShader();
        generateTexturedQuad();
    }
    
    Sprite2D(Shader* shader) 
        : m_mesh(nullptr), m_shader(shader), m_texture(0),
          m_VBO(0), m_VAO(0), m_EBO(0),
          m_position(0, 0), m_size(1, 1), m_origin(0.5f, 0.5f),
          m_rotation(0), m_color(1, 1, 1, 1), m_visible(true), 
          m_ownsShader(false), m_indexCount(0) {
        generateTexturedQuad();
    }
    
    ~Sprite2D() {
        if (m_VAO != 0) {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);
        }
        if (m_texture != 0) {
            glDeleteTextures(1, &m_texture);
        }
        if (m_ownsShader && m_shader) {
            delete m_shader;
        }
        if (m_mesh) {
            delete m_mesh;
        }
    }
    
    void setTexture(const std::string& filepath) {
        loadTexture(filepath);
    }
    
    void setShader(Shader* shader) {
        if (m_ownsShader && m_shader) {
            delete m_shader;
        }
        m_shader = shader;
        m_ownsShader = false;
    }
    
    void setPosition(float x, float y) {
        m_position.x = x;
        m_position.y = y;
    }
    
    void setPosition(const Vector2& position) {
        m_position = position;
    }
    
    Vector2 getPosition() const {
        return m_position;
    }
    
    void setSize(float width, float height) {
        m_size.x = width;
        m_size.y = height;
        generateTexturedQuad();
    }
    
    void setSize(const Vector2& size) {
        m_size = size;
        generateTexturedQuad();
    }
    
    Vector2 getSize() const {
        return m_size;
    }
    
    void setOrigin(float x, float y) {
        m_origin.x = x;
        m_origin.y = y;
        generateTexturedQuad();
    }
    
    void setOrigin(const Vector2& origin) {
        m_origin = origin;
        generateTexturedQuad();
    }
    
    Vector2 getOrigin() const {
        return m_origin;
    }
    
    void setRotation(float degrees) {
        m_rotation = degrees;
    }
    
    float getRotation() const {
        return m_rotation;
    }
    
    void setColor(float r, float g, float b, float a = 1.0f) {
        m_color.x = r;
        m_color.y = g;
        m_color.z = b;
        m_color.w = a;
    }
    
    void setColor(const Vector4& color) {
        m_color = color;
    }
    
    Vector4 getColor() const {
        return m_color;
    }
    
    void setVisible(bool visible) {
        m_visible = visible;
    }
    
    bool isVisible() const {
        return m_visible;
    }
    
    void draw() const {
        if (!m_visible || m_indexCount == 0 || !m_shader) return;
        
        m_shader->use();
        
        const_cast<Sprite2D*>(this)->updateTransform();
        
        m_shader->setVec4("uColor", m_color.x, m_color.y, m_color.z, m_color.w);
        
        if (m_texture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            m_shader->setInt("uTexture", 0);
            m_shader->setBool("uHasTexture", true);
        } else {
            m_shader->setBool("uHasTexture", false);
        }
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glDisable(GL_BLEND);
        
        if (m_texture != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    GLuint getTextureID() const {
        return m_texture;
    }
    
    bool hasTexture() const {
        return m_texture != 0;
    }
    
    std::string getTexturePath() const {
        return m_texturePath;
    }
    
    Shader* getShader() const {
        return m_shader;
    }
};

const char* Sprite2D::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat3 uTransform;

out vec2 TexCoord;

void main() {
    vec3 transformed = uTransform * vec3(aPos, 1.0);
    gl_Position = vec4(transformed.xy, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* Sprite2D::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec4 uColor;
uniform bool uHasTexture;
uniform sampler2D uTexture;

in vec2 TexCoord;

void main() {
    if (uHasTexture) {
        vec4 texColor = texture(uTexture, TexCoord);
        FragColor = texColor * uColor;
    } else {
        FragColor = uColor;
    }
}
)";