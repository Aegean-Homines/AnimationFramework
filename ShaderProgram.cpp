#include "ShaderProgram.h"
#include "Util.h"
#include "glew.h"

#include <iostream>

// Asks OpenGL to create an empty shader program.
void ShaderProgram::CreateProgram()
{
	program = glCreateProgram();
}

// Use a shader program
void ShaderProgram::Use()
{
	glUseProgram(program);
}

// Done using a shader program
void ShaderProgram::Unuse()
{
	glUseProgram(0);
}

// Read, send to OpenGL, and compile a single file into a shader program.
void ShaderProgram::CreateShader(const char* fileName, int type)
{
	// Read the source from the named file
	char* src = Util::ReadFile(fileName);
	const char* psrc[1] = { src };

	// Create a shader and attach, hand it the source, and compile it.
	int shader = glCreateShader(type);
	glAttachShader(program, shader);
	glShaderSource(shader, 1, psrc, NULL);
	glCompileShader(shader);
	delete src;

	// Get the compilation status
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	// If compilation status is not OK, get and print the log message.
	if (status != 1) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* buffer = new char[length];
		glGetShaderInfoLog(shader, length, NULL, buffer);
		printf("Compile log for %s:\n%s\n", fileName, buffer);
		getchar();
		delete buffer;
		exit(-1);
	}
}

void ShaderProgram::LinkProgram()
{
	// Link program and check the status
	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	// If link failed, get and print log
	if (status != 1) {
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* buffer = new char[length];
		glGetProgramInfoLog(program, length, NULL, buffer);
		printf("Link log:\n%s\n", buffer);
		getchar();
		delete buffer;
		exit(-1);
	}
}
