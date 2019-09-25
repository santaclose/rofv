#include "Shader.h"

#include <iostream>
#include <sstream>
#include <fstream>

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

Shader::Shader() : id(-1) {}
Shader::Shader(const std::string& filePath)
{
	std::ifstream inputStream(filePath);

	enum class ShaderType
	{
		VERTEX = 0, FRAGMENT = 1
	};

	ShaderType currentShaderType = ShaderType::VERTEX;
	std::stringstream shaderSources[2];

	std::string line;
	while (getline(inputStream, line))
	{
		if (int r = line.find("#shader") != std::string::npos)
		{
			currentShaderType = line[r + 7] == 'v' ? ShaderType::VERTEX : ShaderType::FRAGMENT;
		}
		else
		{
			shaderSources[(int)currentShaderType] << line << '\n';
		}
	}

	id = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, shaderSources[0].str());
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, shaderSources[1].str());

	glAttachShader(id, vs);
	glAttachShader(id, fs);
	glLinkProgram(id);
	glValidateProgram(id);

	glDeleteShader(vs);
	glDeleteShader(fs);
}


Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::Bind() const
{
	glUseProgram(id);
	std::cout << "Shader " << id << " bound\n";
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (uniformLocationCache.find(name) != uniformLocationCache.end())
		return uniformLocationCache[name];

	int location = glGetUniformLocation(id, name.c_str());
	if (location == -1)
		std::cout << "Could not get uniform location for " << name << std::endl;

	uniformLocationCache[name] = location;
	return location;
}

void Shader::SetUniformMatrix4fv(const std::string& name, const float* pointer)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, pointer);
}
void Shader::SetUniform3fv(const std::string& name, const float* pointer)
{
	glUniform3fv(GetUniformLocation(name), 1, pointer);
}
void Shader::SetUniform1i(const std::string& name, const int value)
{
	glUniform1i(GetUniformLocation(name), value);
}