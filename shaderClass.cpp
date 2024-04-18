#include"Shader.h"
Shader::Shader()
{
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	GLuint vertexShader = CreateShader(vertexFile, "VERTEX", GL_VERTEX_SHADER);
	GLuint fragmentShader = CreateShader(fragmentFile, "FRAGMENT", GL_FRAGMENT_SHADER);

	ID = glCreateProgram();
	{
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		compileErrors(ID, "PROGRAM");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// vector version of Shader(const char* vertexFile, const char* fragmentFile)
Shader::Shader(std::vector<shaderInput> shadersTypes)
{
	std::vector<GLuint> shaders;

	ID = glCreateProgram();
	for (int i = 0; i < shadersTypes.size(); i++)
	{
		auto currentShaderInfo = shadersTypes.at(i);

		auto shader = CreateShader(currentShaderInfo.shaderPath, currentShaderInfo.shaderName, currentShaderInfo.shaderType);
		
		shaders.push_back(shader);
		glAttachShader(ID, shader);
	}
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	for (int i = 0; i < shaders.size(); i++)
	{
		glDeleteShader(shaders.at(i));
	}
}


GLuint Shader::CreateShader(const char* ShaderPath, const char* shaderName, GLenum ShaderType)
{
	std::string code = get_file_contents(ShaderPath);
	const char* source = code.c_str();

	GLuint geometryShader = glCreateShader(ShaderType);
	glShaderSource(geometryShader, 1, &source, NULL);
	glCompileShader(geometryShader);
	compileErrors(geometryShader, shaderName);

	return geometryShader;
}

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}



void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}