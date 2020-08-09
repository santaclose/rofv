#pragma once

#include <string>
#include <glad/glad.h>
#include <unordered_map>

class Shader
{
private:
	std::unordered_map<std::string, int> uniformLocationCache;
public:
	unsigned int id;
private:
	static unsigned int CompileShader(unsigned int type, const std::string& source);
	int GetUniformLocation(const std::string& name);
public:
	Shader(const std::string& filePath);
	Shader();
	~Shader();
	void Bind() const;
	void SetUniformMatrix4fv(const std::string& name, const float* pointer);
	void SetUniform3fv(const std::string& name, const float* pointer);
	void SetUniform1i(const std::string& name, const int value);
};