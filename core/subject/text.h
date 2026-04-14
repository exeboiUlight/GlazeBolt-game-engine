#pragma once

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <utils/math/vector.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

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
    std::vector<Mesh2D*> m_glyphMeshes;
    Vector2 m_position;
    float m_scale;
    Vector4 m_color;
    std::string m_text;
    bool m_visible;
    bool m_ownsShader;
    int m_screenWidth;
    int m_screenHeight;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    
    float toNDCX(float pixelX) const {
        return (pixelX / m_screenWidth) * 2.0f - 1.0f;
    }
    
    float toNDCY(float pixelY) const {
        return 1.0f - (pixelY / m_screenHeight) * 2.0f;
    }
    
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
    }
    
    void regenerateMeshes() {
        for (auto mesh : m_glyphMeshes) {
            delete mesh;
        }
        m_glyphMeshes.clear();

        if (m_screenWidth == 0 || m_screenHeight == 0) return;

        float x = m_position.x;
        float y = m_position.y;

        for (size_t i = 0; i < m_text.length(); i++) {
            char c = m_text[i];
            if (m_characters.find(c) == m_characters.end()) continue;

            Character ch = m_characters[c];

            float xpos = x + ch.Bearing.x * m_scale;

            float ypos = y - ch.Bearing.y * m_scale;

            float w = ch.Size.x * m_scale;
            float h = ch.Size.y * m_scale;

            float xpos_ndc = toNDCX(xpos);
            float ypos_ndc = toNDCY(ypos);
            float xpos_w_ndc = toNDCX(xpos + w);
            float ypos_h_ndc = toNDCY(ypos + h);

            std::vector<Mesh2D::Vertex> vertices = {
                {xpos_ndc,     ypos_ndc,     0.0f, 0.0f},
                {xpos_w_ndc,   ypos_ndc,     1.0f, 0.0f},
                {xpos_w_ndc,   ypos_h_ndc,   1.0f, 1.0f},
                {xpos_ndc,     ypos_h_ndc,   0.0f, 1.0f}
            };

            std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

            Mesh2D* mesh = new Mesh2D();
            mesh->setData(vertices, indices);
            m_glyphMeshes.push_back(mesh);

            x += (ch.Advance >> 6) * m_scale;
        }
    }

    void initDefaultShader() {
        m_shader = new Shader(vertexShaderSource, fragmentShaderSource);
        m_ownsShader = true;
    }
    
public:
    Text(const std::string& fontPath, unsigned int fontSize = 48)
        : m_shader(nullptr), m_position(0, 0), 
          m_scale(1.0f), m_color(1, 1, 1, 1), m_visible(true), 
          m_ownsShader(false), m_screenWidth(1200), m_screenHeight(600) {
        initDefaultShader();
        initFreeType(fontPath, fontSize);
    }
    
    Text(Shader* shader, const std::string& fontPath, unsigned int fontSize = 48)
        : m_shader(shader), m_position(0, 0),
          m_scale(1.0f), m_color(1, 1, 1, 1), m_visible(true), 
          m_ownsShader(false), m_screenWidth(1200), m_screenHeight(600) {
        initFreeType(fontPath, fontSize);
    }
    
    ~Text() {
        for (auto mesh : m_glyphMeshes) {
            delete mesh;
        }
        for (auto& pair : m_characters) {
            glDeleteTextures(1, &pair.second.TextureID);
        }
        if (m_ownsShader && m_shader) delete m_shader;
    }
    
    void setScreenSize(int width, int height) {
        m_screenWidth = width;
        m_screenHeight = height;
        regenerateMeshes();
    }
    
    void setText(const std::string& text) {
        m_text = text;
        regenerateMeshes();
    }
    
    std::string getText() const { return m_text; }
    
    void setPosition(float x, float y) {
        m_position.x = x;
        m_position.y = y;
        regenerateMeshes();
    }
    
    void setPosition(const Vector2& position) {
        m_position = position;
        regenerateMeshes();
    }
    
    Vector2 getPosition() const { return m_position; }
    
    void setScale(float scale) {
        m_scale = scale;
        regenerateMeshes();
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
        if (!m_visible || !m_shader || m_glyphMeshes.empty()) return;
        
        m_shader->use();
        m_shader->setVec4("uColor", m_color.x, m_color.y, m_color.z, m_color.w);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (size_t i = 0; i < m_text.length() && i < m_glyphMeshes.size(); i++) {
            char c = m_text[i];
            if (m_characters.find(c) != m_characters.end()) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_characters.at(c).TextureID);
                m_shader->setInt("uTextTexture", 0);
                
                m_glyphMeshes[i]->draw();
            }
        }
        
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

const char* Text::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
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