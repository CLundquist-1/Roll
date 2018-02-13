#version 330 core
layout (location = 0) in vec3 aPos;							//In for position
layout (location = 1) in vec3 aColor;						//In for color
layout (location = 2) in vec2 aTexCoord;

out vec3 vColor;											//Out for color
out vec2 TexCoord;											//Out for texture

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
   gl_Position = project * view * model * vec4(aPos, 1.0);			//predefined output of the vertex shader
	vColor = aColor;										//Set the out color for the fragment shader
	TexCoord = aTexCoord;
};