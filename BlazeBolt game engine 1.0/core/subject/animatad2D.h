#pragma once

#include <graphics/mesh.h>
#include <graphics/shader.h>
#include <subject/sprite2D.h>
#include <utils/math/vector.h>
#include <gif_load.h>

#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <cstring>

class Animation2D {
public:
    Animation2D() 
        : m_currentFrame(0)
        , m_playing(false)
        , m_looping(true)
        , m_elapsedTime(0)
        , m_texture(0)
        , m_shader(nullptr)
        , m_ownsShader(false)
        , m_visible(true)
        , m_position(0, 0)
        , m_size(1, 1)
        , m_origin(0.5f, 0.5f)
        , m_rotation(0)
        , m_color(1, 1, 1, 1)
        , m_mesh(nullptr)
        , m_useTextureAtlas(false)
        , m_gifLoaded(false)
        , m_speedMultiplier(1.0f)
    {
        initDefaultShader();
        generateQuadMesh();
    }

    Animation2D(Shader* shader) 
        : m_currentFrame(0)
        , m_playing(false)
        , m_looping(true)
        , m_elapsedTime(0)
        , m_texture(0)
        , m_shader(shader)
        , m_ownsShader(false)
        , m_visible(true)
        , m_position(0, 0)
        , m_size(1, 1)
        , m_origin(0.5f, 0.5f)
        , m_rotation(0)
        , m_color(1, 1, 1, 1)
        , m_mesh(nullptr)
        , m_useTextureAtlas(false)
        , m_gifLoaded(false)
        , m_speedMultiplier(1.0f)
    {
        generateQuadMesh();
    }

    ~Animation2D() {
        cleanup();
    }

    bool loadFromGIF(const std::string& filepath) {
        cleanupFrames();
        
        FILE* file = fopen(filepath.c_str(), "rb");
        if (!file) {
            std::cerr << "Failed to open GIF file: " << filepath << std::endl;
            return false;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (fileSize <= 0) {
            std::cerr << "GIF file is empty: " << filepath << std::endl;
            fclose(file);
            return false;
        }

        std::vector<uint8_t> buffer(fileSize);
        size_t readSize = fread(buffer.data(), 1, fileSize, file);
        fclose(file);

        if (readSize != static_cast<size_t>(fileSize)) {
            std::cerr << "Failed to read full GIF file" << std::endl;
            return false;
        }

        m_gifData = buffer;
        
        long result = GIF_Load(m_gifData.data(), m_gifData.size(), 
                               frameWriterCallback, nullptr, this, 0);

        if (result <= 0 || m_frames.empty()) {
            std::cerr << "Failed to load GIF: " << filepath << " (result: " << result << ")" << std::endl;
            return false;
        }

        for (auto& frame : m_frames) {
            if (frame.delayMs < 10) {
                frame.delayMs = 100;
            }
        }

        if (!m_frames.empty()) {
            m_gifLoaded = true;
            m_currentFrame = 0;
            m_elapsedTime = 0;
            updateTextureFromCurrentFrame();
        }

        return !m_frames.empty();
    }

    void loadFromSpriteSheet(const std::string& texturePath, 
                              int frameWidth, int frameHeight,
                              int totalFrames, int framesPerRow,
                              int frameDelayMs) {
        cleanupFrames();

        if (m_texture != 0) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }

        int width, height, channels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
        
        if (!data) {
            std::cerr << "Failed to load sprite sheet: " << texturePath << std::endl;
            return;
        }

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);

        float texWidth = static_cast<float>(width);
        float texHeight = static_cast<float>(height);
        float fw = static_cast<float>(frameWidth) / texWidth;
        float fh = static_cast<float>(frameHeight) / texHeight;

        for (int i = 0; i < totalFrames; ++i) {
            int row = i / framesPerRow;
            int col = i % framesPerRow;
            
            Frame frame;
            frame.width = frameWidth;
            frame.height = frameHeight;
            frame.delayMs = frameDelayMs;
            frame.xOffset = 0;
            frame.yOffset = 0;
            frame.hasTransparency = true;
            frame.transparentIndex = 0;
            frame.pixelData.clear();
            
            frame.texCoords[0] = col * fw;
            frame.texCoords[1] = row * fh;
            frame.texCoords[2] = (col + 1) * fw;
            frame.texCoords[3] = (row + 1) * fh;
            
            m_frames.push_back(frame);
        }

        m_useTextureAtlas = true;
        m_gifLoaded = true;
        m_currentFrame = 0;
        m_elapsedTime = 0;
        
        if (!m_frames.empty()) {
            updateVertexUVs();
        }
    }

    void update(float deltaTime) {
        if (!m_gifLoaded || m_frames.empty()) return;
        if (!m_playing) return;
        
        float currentDelay = static_cast<float>(m_frames[m_currentFrame].delayMs) / m_speedMultiplier;
        if (currentDelay <= 0) currentDelay = 33.0f;
        
        m_elapsedTime += deltaTime * 1000.0f;
        
        int framesSwitched = 0;
        while (m_elapsedTime >= currentDelay && framesSwitched < 10) {
            m_elapsedTime -= currentDelay;
            
            m_currentFrame++;
            if (m_currentFrame >= static_cast<int>(m_frames.size())) {
                if (m_looping) {
                    m_currentFrame = 0;
                } else {
                    m_currentFrame = static_cast<int>(m_frames.size()) - 1;
                    m_playing = false;
                    break;
                }
            }
            
            if (!m_useTextureAtlas) {
                updateTextureFromCurrentFrame();
            } else {
                updateVertexUVs();
            }
            
            framesSwitched++;
            currentDelay = static_cast<float>(m_frames[m_currentFrame].delayMs) / m_speedMultiplier;
        }
    }

    void draw() const {
        if (!m_visible || !m_mesh || !m_shader) return;
        if (!m_gifLoaded || m_frames.empty()) return;
        
        m_shader->use();
        
        const_cast<Animation2D*>(this)->updateTransform();
        
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
        
        m_mesh->draw();
        
        glDisable(GL_BLEND);
        
        if (m_texture != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void play() { 
        m_playing = true; 
    }
    
    void pause() { 
        m_playing = false; 
    }
    
    void stop() { 
        m_currentFrame = 0; 
        m_elapsedTime = 0; 
        m_playing = false; 
        if (m_gifLoaded && !m_useTextureAtlas) updateTextureFromCurrentFrame();
    }
    
    void restart() { 
        m_currentFrame = 0; 
        m_elapsedTime = 0; 
        m_playing = true; 
        if (m_gifLoaded && !m_useTextureAtlas) updateTextureFromCurrentFrame();
    }
    
    void setLooping(bool loop) { m_looping = loop; }
    bool isLooping() const { return m_looping; }
    bool isPlaying() const { return m_playing; }
    
    void setSpeed(float multiplier) { 
        m_speedMultiplier = std::max(0.1f, multiplier); 
    }
    
    float getSpeed() const { return m_speedMultiplier; }
    
    void setFrame(int frame) {
        if (!m_gifLoaded) return;
        if (frame >= 0 && frame < static_cast<int>(m_frames.size())) {
            m_currentFrame = frame;
            m_elapsedTime = 0;
            if (!m_useTextureAtlas) {
                updateTextureFromCurrentFrame();
            } else {
                updateVertexUVs();
            }
        }
    }
    
    int getFrameCount() const { return static_cast<int>(m_frames.size()); }
    int getCurrentFrame() const { return m_currentFrame; }
    
    void setPosition(float x, float y) { m_position.x = x; m_position.y = y; }
    void setPosition(const Vector2& pos) { m_position = pos; }
    Vector2 getPosition() const { return m_position; }
    
    void setSize(float width, float height) { m_size.x = width; m_size.y = height; generateQuadMesh(); }
    void setSize(const Vector2& size) { m_size = size; generateQuadMesh(); }
    Vector2 getSize() const { return m_size; }
    
    void setOrigin(float x, float y) { m_origin.x = x; m_origin.y = y; generateQuadMesh(); }
    void setOrigin(const Vector2& origin) { m_origin = origin; generateQuadMesh(); }
    Vector2 getOrigin() const { return m_origin; }
    
    void setRotation(float degrees) { m_rotation = degrees; }
    float getRotation() const { return m_rotation; }
    
    void setColor(float r, float g, float b, float a = 1.0f) { m_color = Vector4(r, g, b, a); }
    void setColor(const Vector4& color) { m_color = color; }
    Vector4 getColor() const { return m_color; }
    
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    
    void setShader(Shader* shader) {
        if (m_ownsShader && m_shader) delete m_shader;
        m_shader = shader;
        m_ownsShader = false;
    }
    
    int getFrameWidth() const { return m_frames.empty() ? 0 : m_frames[0].width; }
    int getFrameHeight() const { return m_frames.empty() ? 0 : m_frames[0].height; }
    
    bool isLoaded() const { return m_gifLoaded && !m_frames.empty(); }

private:
    struct Frame {
        int width = 0;
        int height = 0;
        int delayMs = 100;
        int xOffset = 0;
        int yOffset = 0;
        bool hasTransparency = false;
        uint8_t transparentIndex = 0;
        std::vector<uint8_t> pixelData;
        float texCoords[4] = {0, 0, 1, 1};
    };

    std::vector<Frame> m_frames;
    std::vector<uint8_t> m_gifData;
    int m_currentFrame;
    bool m_playing;
    bool m_looping;
    float m_elapsedTime;
    
    GLuint m_texture;
    Shader* m_shader;
    bool m_ownsShader;
    bool m_visible;
    
    Vector2 m_position;
    Vector2 m_size;
    Vector2 m_origin;
    float m_rotation;
    Vector4 m_color;
    
    Mesh2D* m_mesh;
    bool m_useTextureAtlas;
    bool m_gifLoaded;
    float m_speedMultiplier;
    
    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;

    void updateTextureFromCurrentFrame() {
        if (m_frames.empty() || m_useTextureAtlas) return;
        if (m_currentFrame < 0 || m_currentFrame >= static_cast<int>(m_frames.size())) return;
        
        const Frame& frame = m_frames[m_currentFrame];
        
        if (frame.pixelData.empty()) return;
        
        if (m_texture == 0) {
            glGenTextures(1, &m_texture);
        }
        
        GLenum format = frame.hasTransparency ? GL_RGBA : GL_RGB;
        
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.width, frame.height, 0, 
                     format, GL_UNSIGNED_BYTE, frame.pixelData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        updateVertexUVs();
    }

    void updateVertexUVs() {
        if (!m_mesh || m_frames.empty()) return;
        
        float u1 = 0, v1 = 0, u2 = 1, v2 = 1;
        
        if (m_useTextureAtlas && m_currentFrame < static_cast<int>(m_frames.size())) {
            const Frame& frame = m_frames[m_currentFrame];
            u1 = frame.texCoords[0];
            v1 = frame.texCoords[1];
            u2 = frame.texCoords[2];
            v2 = frame.texCoords[3];
        }
        
        float left = -m_size.x * m_origin.x;
        float right = m_size.x * (1.0f - m_origin.x);
        float bottom = -m_size.y * m_origin.y;
        float top = m_size.y * (1.0f - m_origin.y);

        std::vector<Mesh2D::Vertex> vertices = {
            {left,  top,    u1, v1},
            {right, top,    u2, v1},
            {right, bottom, u2, v2},
            {left,  bottom, u1, v2}
        };

        std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

        m_mesh->setData(vertices, indices);
    }

    void generateQuadMesh() {
        if (m_mesh) {
            delete m_mesh;
        }
        m_mesh = new Mesh2D();
        updateVertexUVs();
    }

    void updateTransform() {
        if (!m_shader) return;
        
        Matrix3x3 transform = Matrix3x3::identity();
        transform = Matrix3x3::translation(m_position.x, m_position.y) * transform;
        transform = Matrix3x3::rotation(m_rotation) * transform;
        transform = Matrix3x3::scale(m_size.x, m_size.y) * transform;
        
        float transformArray[9];
        transform.toFloatArray(transformArray);
        m_shader->setMat3("uTransform", transformArray);
    }

    void initDefaultShader() {
        m_shader = new Shader(vertexShaderSource, fragmentShaderSource);
        m_ownsShader = true;
    }

    void cleanup() {
        if (m_mesh) {
            delete m_mesh;
            m_mesh = nullptr;
        }
        if (m_texture != 0) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }
        if (m_ownsShader && m_shader) {
            delete m_shader;
            m_shader = nullptr;
        }
    }

    void cleanupFrames() {
        m_frames.clear();
        m_gifData.clear();
        m_useTextureAtlas = false;
        m_gifLoaded = false;
        m_currentFrame = 0;
        m_elapsedTime = 0;
    }

    static void frameWriterCallback(void* anim, struct GIF_WHDR* whdr) {
        Animation2D* self = static_cast<Animation2D*>(anim);
        if (!self || !whdr) return;
        
        if (whdr->frxd <= 0 || whdr->fryd <= 0) return;
        
        Frame frame;
        frame.width = whdr->frxd;
        frame.height = whdr->fryd;
        frame.delayMs = whdr->time * 10;
        frame.xOffset = whdr->frxo;
        frame.yOffset = whdr->fryo;
        frame.hasTransparency = (whdr->tran >= 0);
        frame.transparentIndex = static_cast<uint8_t>(whdr->tran);
        
        if (frame.delayMs < 10) {
            frame.delayMs = 100;
        }
        
        int pixelCount = frame.width * frame.height;
        if (pixelCount <= 0) return;
        
        int bytesPerPixel = frame.hasTransparency ? 4 : 3;
        
        frame.pixelData.resize(pixelCount * bytesPerPixel);
        
        if (whdr->bptr && whdr->cpal) {
            if (frame.hasTransparency) {
                for (int i = 0; i < pixelCount; ++i) {
                    uint8_t idx = whdr->bptr[i];
                    if (idx == frame.transparentIndex) {
                        frame.pixelData[i * 4 + 0] = 0;
                        frame.pixelData[i * 4 + 1] = 0;
                        frame.pixelData[i * 4 + 2] = 0;
                        frame.pixelData[i * 4 + 3] = 0;
                    } else if (idx < 256) {
                        frame.pixelData[i * 4 + 0] = whdr->cpal[idx].R;
                        frame.pixelData[i * 4 + 1] = whdr->cpal[idx].G;
                        frame.pixelData[i * 4 + 2] = whdr->cpal[idx].B;
                        frame.pixelData[i * 4 + 3] = 255;
                    } else {
                        frame.pixelData[i * 4 + 0] = 0;
                        frame.pixelData[i * 4 + 1] = 0;
                        frame.pixelData[i * 4 + 2] = 0;
                        frame.pixelData[i * 4 + 3] = 255;
                    }
                }
            } else {
                for (int i = 0; i < pixelCount; ++i) {
                    uint8_t idx = whdr->bptr[i];
                    if (idx < 256) {
                        frame.pixelData[i * 3 + 0] = whdr->cpal[idx].R;
                        frame.pixelData[i * 3 + 1] = whdr->cpal[idx].G;
                        frame.pixelData[i * 3 + 2] = whdr->cpal[idx].B;
                    } else {
                        frame.pixelData[i * 3 + 0] = 0;
                        frame.pixelData[i * 3 + 1] = 0;
                        frame.pixelData[i * 3 + 2] = 0;
                    }
                }
            }
        }
        
        self->m_frames.push_back(frame);
    }
};

const char* Animation2D::vertexShaderSource = R"(
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

const char* Animation2D::fragmentShaderSource = R"(
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