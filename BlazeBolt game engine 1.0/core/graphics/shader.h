#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
    unsigned int m_RendererID;
    
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
    
    std::string readShaderFile(const std::string& filepath) {
        std::string content;
        std::ifstream fileStream(filepath, std::ios::in);
        
        if (!fileStream.is_open()) {
            std::cout << "ERROR::SHADER: Failed to read file: " << filepath << std::endl;
            return "";
        }
        
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        content = buffer.str();
        fileStream.close();
        
        return content;
    }
    
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexCode = readShaderFile(vertexPath);
        std::string fragmentCode = readShaderFile(fragmentPath);
        
        if (vertexCode.empty() || fragmentCode.empty()) {
            m_RendererID = 0;
            return;
        }
        
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        unsigned int vertex, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertex);
        glAttachShader(m_RendererID, fragment);
        glLinkProgram(m_RendererID);
        checkCompileErrors(m_RendererID, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    Shader(const char* vertexSource, const char* fragmentSource) {
        unsigned int vertex, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vertex);
        glAttachShader(m_RendererID, fragment);
        glLinkProgram(m_RendererID);
        checkCompileErrors(m_RendererID, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    ~Shader() {
        if (m_RendererID != 0) {
            glDeleteProgram(m_RendererID);
        }
    }
    
    void use() const {
        glUseProgram(m_RendererID);
    }
    
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), (int)value);
    }
    
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }
    
    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), x, y);
    }
    
    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), x, y, z);
    }
    
    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), x, y, z, w);
    }
    
    void setMat2(const std::string& name, float* value) const {
        glUniformMatrix2fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, value);
    }
    
    void setMat3(const std::string& name, float* value) const {
        glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, value);
    }
    
    void setMat4(const std::string& name, float* value) const {
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, value);
    }
    
    unsigned int getRendererID() const {
        return m_RendererID;
    }
};