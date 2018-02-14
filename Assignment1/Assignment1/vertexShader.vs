#version 330 core
layout (location = 0) in vec3 aPos;							//In for position

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
   gl_Position = project * view * model * vec4(aPos, 1.0);			//predefined output of the vertex shader
};