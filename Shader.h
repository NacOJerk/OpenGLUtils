#pragma once

#include <string>
#include <glad/glad.h>

#define SHADER_TYPES 4

class Shader
{
	unsigned int _ID;
	bool _used;
	unsigned int* _shaders;
	static std::string readFile(const char*);
public:
	Shader(const char* vertex, const char* fragment, bool shouldCompile);
	Shader(const char* vertex, const char* fragment);
	Shader(const char* vertex);
	~Shader();
	void compile();
	void addShader(const char* path, GLenum shaderType);
	void use();
	void unuse();
	const bool compiled() const;
	const bool isUsed() const;
	const unsigned int getID() const;
	operator const unsigned int();
};

