#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

std::string Shader::readFile(const char * path)
{
	std::string fileData;
	try
	{
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(path);
		std::stringstream dataStream;
		dataStream << file.rdbuf();
		fileData = dataStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "ERROR::READING_FILE::\"" << path << "\"::" << e.what() << std::endl;
	}
	return fileData;
}

Shader::Shader(const char * vertex, const char * fragment, bool shouldCompile)
{
	_ID = glCreateProgram();
	if (_ID)
		throw std::exception("Couldn't create shader");
	if (!vertex)
	{
		throw std::exception("You must have a vertex shader");
	}
	_shaders = new unsigned int[5];
	_shaders[0] = 1;
	addShader(vertex, GL_VERTEX_SHADER);
	if (fragment)
		addShader(fragment, GL_FRAGMENT_SHADER);
	if (shouldCompile)
		compile();
}

Shader::Shader(const char * vertex, const char * fragment) : Shader(vertex, fragment, true)
{

}

Shader::Shader(const char* vertex) : Shader(vertex, nullptr, false)
{

}



Shader::~Shader()
{
	if (_shaders)
	{
		for (int i = 1, len = _shaders[0]; i < len; _shaders[i] ? glDeleteShader(_shaders[i++]) : i++);
		delete _shaders;
		_shaders = nullptr;
	}
	if (_used)
		glUseProgram(0);
	glDeleteProgram(_ID);
}

void Shader::compile()
{
	if (!_shaders)
	{
		std::cerr << "You can't recompile the shader program" << std::endl;
		return;
	}
	if (_shaders[0] - 1 <= 0)
	{
		std::cerr << "You can't compile the shader program without any shaders" << std::endl;
		return;
	}
	//Attaching everything
	for (int i = 1, len = _shaders[0]; i < len; _shaders[i] ? glAttachShader(_ID, _shaders[i++]) : i++);
	glLinkProgram(_ID);

	//Checking that everything is fine
	GLint result = 0;
	GLchar eLog[1024] = { 0 };
	glGetProgramiv(_ID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_ID, sizeof(eLog), NULL, eLog);
		std::cerr << "ERROR::LINKING:: '" << eLog << '\'' << std::endl;
		return;
	}

	glValidateProgram(_ID);
	glGetProgramiv(_ID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(_ID, sizeof(eLog), NULL, eLog);
		std::cerr << "ERROR::VAILDATING:: '" << eLog << '\'' << std::endl;
		return;
	}

	//Cleaning up
	for (int i = 1, len = _shaders[0]; i < len; _shaders[i] ? glDeleteShader(_shaders[i++]) : i++);
	delete _shaders;
	_shaders = nullptr;
}

void Shader::addShader(const char * path, GLenum shaderType)
{
	if (!_shaders)
	{
		std::cerr << "The shader was already compiled, you can't add shaders to it" << std::endl;
		return;
	}
	if (_shaders[0] >= 5)
	{
		std::cerr << "You can not add more than 4 shaders" << std::endl;
		return;
	}
	std::string shaderData = readFile(path);
	if (shaderData.empty())
		throw std::exception("There was a problem reading the shader file");
	unsigned int shader = glCreateShader(shaderType);
	const char* data = shaderData.c_str();
	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(shader);
		return;
	};
	_shaders[_shaders[0]++] = shader;
}

void Shader::use()
{
	if (!compiled())
	{
		std::cerr << "You can't use the shader until you compile it" << std::endl;
		return;
	}
	if (_used)
	{
		std::cout << "You are already using the program" << std::endl;
		return;
	}
	glUseProgram(_ID);
	_used = true;
}

void Shader::unuse()
{
	if (!_used)
	{
		std::cout << "You are not using the program" << std::endl;
		return;
	}
	glUseProgram(0);
	_used = false;
}

const bool Shader::compiled() const
{
	return !_shaders;
}

const bool Shader::isUsed() const
{
	return _used;
}

const unsigned int Shader::getID() const
{
	return _ID;
}

Shader::operator const unsigned int()
{
	return getID();
}
