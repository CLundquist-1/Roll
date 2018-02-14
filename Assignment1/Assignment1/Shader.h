#pragma once
#include<GL\glew.h>
//#include <GL\freeglut.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
private:
	void checkCompileErrors(unsigned int, std::string type);
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	/*void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;*/
};