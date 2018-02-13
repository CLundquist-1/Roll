#version 330 core
layout (location = 0) in vec3 aPos;							//In for position
layout (location = 1) in vec3 aColor;						//In for color
out vec3 vColor;												//Out for color
void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);			//predefined output of the vertex shader
	vColor = aColor;											//Set the out color for the fragment shader
};