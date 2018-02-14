#version 330 core
out vec4 FragColor;

//in vec3 vColor;									//Passed in by the vertex shader
//in vec2 TexCoord;

//uniform vec4 ourColor;
//uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
	//texture is a built in function that takes in a sampler and texture coordinates
	//FragColor = texture(ourTexture, TexCoord)*vec4(vColor,1.0);			//predefined output of the fragment shader
	FragColor = vec4(1.0f,0.0f,0.0f,1.0f);
	//FragColor = texture(texture2, TexCoord);
	//First color, second color, how much of the second color you want to mix with the first
};