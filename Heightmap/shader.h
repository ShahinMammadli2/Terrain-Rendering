#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	// state 
	unsigned int ID;
	// sets the current shader active
	Shader& Use();
	// compiles shader from given source code
	void Compile(const char* vertexSource,
		const char* fragmentSource,
		const char* tessControlSource = nullptr,
		const char* tessEvalSource = nullptr,
		const char* geometrySource = nullptr);
	// utility
	void setFloat(const char* name, float value, bool useShader = false);
	void setInteger(const char* name, int value, bool useShader = false);
	void setVec2f(const char* name, float x, float y, bool useShader = false);
	void setVec2f(const char* name, glm::vec2 value, bool useShader = false);
	void SetVec3f(const char* name, float x, float y, float z, bool useShader = false);
	void SetVec3f(const char* name, glm::vec3& value, bool useShader = false);
	void SetVec4f(const char* name, float x, float y, float z, float w, bool useShader = false);
	void SetVec4f(const char* name, glm::vec4& value, bool useShader = false);
	void SetMat4(const char* name, glm::mat4& matrix, bool useShader = false);
private:
	void checkCompileErrors(unsigned int object, std::string type);
};
#endif // !SHADER_H
