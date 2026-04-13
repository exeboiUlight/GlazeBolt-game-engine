// text.h - обновленная версия
#pragma once

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <utils/math/vector.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <memory>

struct Character {
    GLuint TextureID;
    Vector2 Size;
    Vector2 Bearing;
    unsigned int Advance;
};

class Text {
private:
    std::map<char, Character> m_characters;
    Shader* m_shader;
    Mesh2D* m_mesh;
    Vector2 m_position;
    float m_scale;
    Vector4 m_color;
    std::string m_text;
    bool m_visible;
    bool m_ownsShader;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    
    void initFreeType(const std::string& fontPath, unsigned int fontSize) {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }
        
        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            std::cerr << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
            FT_Done_FreeType(ft);
            return;
        }
        
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cerr << "ERROR::FREETYPE: Failed to load Glyph: " << c << std::endl;
                continue;
            }
            
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            Character character = {
                texture,
                Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            m_characters.insert(std::pair<char, Character>(c, character));
        }
        
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        
        generateQuadMesh();
    }
    
    void generateQuadMesh() {
        std::vector<Mesh2D::Vertex> vertices(6 * m_text.length());
        std::vector<GLuint> indices;
        
        float x = m_position.x;
        float y = m_position.y;
        
        for (size_t i = 0; i < m_text.length(); i++) {
            char c = m_text[i];
            if (m_characters.find(c) == m_characters.end()) continue;
            
            Character ch = m_characters[c];
            
            float xpos = x + ch.Bearing.x * m_scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * m_scale;
            float w = ch.Size.x * m_scale;
            float h = ch.Size.y * m_scale;
            
            size_t idx = i * 6;
            vertices[idx] = {xpos, ypos + h, 0.0f, 0.0f};
            vertices[idx + 1] = {xpos + w, ypos + h, 1.0f, 0.0f};
            vertices[idx + 2] = {xpos + w, ypos, 1.0f, 1.0f};
            vertices[idx + 3] = {xpos, ypos, 0.0f, 1.0f};
            vertices[idx + 4] = {xpos, ypos + h, 0.0f, 0.0f};
            vertices[idx + 5] = {xpos + w, ypos, 1.0f, 1.0f};
            
            for (int j = 0; j < 6; j++) {
                indices.push_back(idx + j);
            }
            
            x += (ch.Advance >> 6) * m_scale;
        }
        
        if (m_mesh) delete m_mesh;
        m_mesh = new Mesh2D();
        m_mesh->setData(vertices, indices);
    }
    
    void initDefaultShader() {
        m_shader = new Shader(vertexShaderSource, fragmentShaderSource);
        m_ownsShader = true;
    }
    
public:
    Text(const std::string& fontPath, unsigned int fontSize = 48)
        : m_shader(nullptr), m_mesh(nullptr), m_position(0, 0), 
          m_scale(1.0f), m_color(1, 1, 1, 1), m_visible(true), m_ownsShader(false) {
        initDefaultShader();
        initFreeType(fontPath, fontSize);
    }
    
    Text(Shader* shader, const std::string& fontPath, unsigned int fontSize = 48)
        : m_shader(shader), m_mesh(nullptr), m_position(0, 0),
          m_scale(1.0f), m_color(1, 1, 1, 1), m_visible(true), m_ownsShader(false) {
        initFreeType(fontPath, fontSize);
    }
    
    ~Text() {
        if (m_mesh) delete m_mesh;
        for (auto& pair : m_characters) {
            glDeleteTextures(1, &pair.second.TextureID);
        }
        if (m_ownsShader && m_shader) delete m_shader;
    }
    
    void setText(const std::string& text) {
        m_text = text;
        generateQuadMesh();
    }
    
    std::string getText() const { return m_text; }
    
    void setPosition(float x, float y) {
        m_position.x = x;
        m_position.y = y;
        generateQuadMesh();
    }
    
    void setPosition(const Vector2& position) {
        m_position = position;
        generateQuadMesh();
    }
    
    Vector2 getPosition() const { return m_position; }
    
    void setScale(float scale) {
        m_scale = scale;
        generateQuadMesh();
    }
    
    float getScale() const { return m_scale; }
    
    void setColor(float r, float g, float b, float a = 1.0f) {
        m_color = Vector4(r, g, b, a);
    }
    
    void setColor(const Vector4& color) {
        m_color = color;
    }
    
    Vector4 getColor() const { return m_color; }
    
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    
    void draw() const {
        if (!m_visible || !m_mesh || !m_shader) return;
        
        m_shader->use();
        
        glActiveTexture(GL_TEXTURE0);
        m_shader->setInt("uTextTexture", 0);
        m_shader->setVec4("uColor", m_color.x, m_color.y, m_color.z, m_color.w);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (size_t i = 0; i < m_text.length(); i++) {
            char c = m_text[i];
            if (m_characters.find(c) != m_characters.end()) {
                glBindTexture(GL_TEXTURE_2D, m_characters.at(c).TextureID);
            }
        }
        
        m_mesh->draw();
        
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

const char* Text::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat3 uTransform;

void main() {
    vec3 transformed = uTransform * vec3(aPos, 1.0);
    gl_Position = vec4(transformed.xy, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* Text::fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uTextTexture;
uniform vec4 uColor;

void main() {
    float alpha = texture(uTextTexture, TexCoord).r;
    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
)";