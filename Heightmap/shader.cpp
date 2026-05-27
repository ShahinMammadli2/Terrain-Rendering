#include "shader.h"
#include <iostream>

Shader& Shader::Use()
{
    glUseProgram(this->ID);
    return*this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* tessControlSource, const char* tessEvalSource, const char* geometrySource)
{
    unsigned int sVertex = 0, sFragment = 0, gShader = 0, sTCS = 0, sTES = 0;

    // vertex shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // fragment shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    
    // Tessellation control shader
    if (tessControlSource != nullptr)
    {
        sTCS = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(sTCS, 1, &tessControlSource, nullptr);
        glCompileShader(sTCS);
        checkCompileErrors(sTCS, "TESS_CONTROL");
    }

    // Tessellation evaluation shader
    if (tessEvalSource != nullptr)
    {
        sTES = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(sTES, 1, &tessEvalSource, nullptr);
        glCompileShader(sTES);
        checkCompileErrors(sTES, "TESS_EVALUATION");
    }

    // geometry shader
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }

    // shader program
    ID = glCreateProgram();
    glAttachShader(ID, sVertex);
    glAttachShader(ID, sFragment); 
    if (tessControlSource != nullptr) glAttachShader(ID, sTCS);
    if (tessEvalSource != nullptr) glAttachShader(ID, sTES);
    if (geometrySource != nullptr) glAttachShader(ID, gShader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // delete shaders
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (tessControlSource != nullptr) glDeleteShader(sTCS);
    if (tessEvalSource != nullptr) glDeleteShader(sTES);
    if (geometrySource != nullptr) glDeleteShader(gShader);
}

void Shader::setFloat(const char* name, float value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::setInteger(const char* name, int value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}

void Shader::setVec2f(const char* name, float x, float y, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}

void Shader::setVec2f(const char* name, glm::vec2 value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}

void Shader::SetVec3f(const char* name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}

void Shader::SetVec3f(const char* name, glm::vec3& value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}

void Shader::SetVec4f(const char* name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}

void Shader::SetVec4f(const char* name, glm::vec4& value, bool useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const char* name, glm::mat4& matrix, bool useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}

