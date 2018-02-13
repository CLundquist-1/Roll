#version 330 core
out vec4 FragColor;
in vec3 vColor;
uniform vec4 ourColor;
void main()
{
   FragColor = vec4(vColor, 1.0f);				//predefined output of the fragment shader
};