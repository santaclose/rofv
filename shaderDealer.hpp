static unsigned int CompileShader(unsigned int type, const std::string& source)
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

static unsigned int CreateShader(const std::string& filepath)//const std::string& vertexShader, const std::string& fragmentShader)
{
	std::ifstream inputStream(filepath);

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
			//std::cout << "CHAR: " << line[r + 7] << std::endl;
			currentShaderType = line[r + 7] == 'v' ? ShaderType::VERTEX : ShaderType::FRAGMENT;
		}
		else
		{
			shaderSources[(int)currentShaderType] << line << '\n';
			//std::cout << "wrote line to " << (currentShaderType == ShaderType::VERTEX ? "vertex" : "fragment") << " shader:\n" << line << '\n';
		}
	}

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, shaderSources[0].str());
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, shaderSources[1].str());

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}