#include "Shader.h"
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW\glfw3.h>
#include<GL\freeglut.h>

#include<iostream>

using std::cout;
using std::endl;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
Shader initDrawing();

// settings
const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

bool wire = false;


//Triangle vertices
const float vertices[] = {
	// positions          // colors           // texture coords
	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

const unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

const float texCoords[] = {
	0.0f, 0.0f,  // lower-left corner  
	1.0f, 0.0f,  // lower-right corner
	0.5f, 1.0f   // top-center corner
};

//Defining the shader programs
const char *vertexShaderSource = "vertexShader.vs";
const char *fragmentShaderSource = "fragmentShader.fs";


int main() {
	if (!glfwInit()) {
		cout << "Failed to initialize GLFW\n";
		return -1;
	}
	//////////////////////////////Change this later, it is the creation of our context window////////////////////////////////////////////////
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow(scr_width, scr_height, "Tutorial 01", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	Shader shader = initDrawing();

	/////////////////////////////Render Loop///////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		// Draw nothing, see you in tutorial 2 !

		processInput(window);

		/*float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);					//sets the uniform of the currently active shader program*/

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);			//There is also a depth buffer bit and stencil buffer bit

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//What primitive to use, starting index in vertex buffer, number of VERTICES to draw

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//What primitive to use, number of VERTICES to draw, type of elements, starting index in vertex buffer (offset)

		//Swap buffers
		glfwSwapBuffers(window);

		//Poll input events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


/////////////////////////////////////////Generate and pass our triangle vertex data to the GPU///////////////////////////////////
void initTriangle()
{
	//The VAO will store enabled vertex attributes, attribute configurations, and vertex buffer objects associated with vertex attributes
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create vertex buffer object that will contain all the triangles vertex data. It makes it easier to pass the GPU large portions of data at a time
	unsigned int VBO;
	
	//Generate the buffer in GPU memory and pass the id of the object back into the vertex buffer object
	glGenBuffers(1, &VBO); //We pass it the address so it can manipulate the VBO contents directly

	//Now we associate the id to an Array Buffer. Remember we can bind to several different buffers at once as long as they are a different buffer type
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //From this point on any buffer calls we make will be to the currently bound buffer VBO

	//Now lets push our triangles vertex data to the buffers memory on the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Remember we can be bound to different types of buffers at once so we have to specify which buffer type we want to add data to
	//The second parameter passes the size of data in bytes we want to pass, the third is the actual data (remember vertices is just a pointer to data, which explains the sizeof)
	//Static - data won't change much, Dynamic - Will change a lot, Stream - Will change every time. This is for optimization on the GPU's end (faster reads vs faster writes)

	//An Element Array Buffer will allow us to select the "order" in which we want to use the vertexes in the Array Buffer and thus reuse vertices
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//The data is now in GPU memory

	//Define the data we just pushed to GPU memory

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//What layout location we are setting, size of the attribute, type of data, if we want numbers not from 0 to 1 to be normalized (mapped ot them), stride, offset to beginning
	glEnableVertexAttribArray(0);	//This turns layout location 0

	//Since we now have color at the later half of each set of vector we now need an offset
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//We now have texture point data we need to pass along
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	

	////////////////////////////////////////////Texture Stuff//////////////////////////////////////////
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

	unsigned int texture, texture2;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);				//This is default anyways, but allows for multiple texture unit to be made and switched between with relative ease
	glBindTexture(GL_TEXTURE_2D, texture);

	//s,t,r, = x,y,z | REPEAT, MIRROR_REPEAT, CLAMP-TO-EDGE, CLAMP-TO-BORDER
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


	/*float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);*/

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				//Cannot use mipmaps for magnification

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//Texture target, mipmap level we want to create if you want to set it manually, format to store texture, width, height, should be 0 (legacy), format of data, datatype, data array
		glGenerateMipmap(GL_TEXTURE_2D);	//Automatically generate required mipmaps
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data); //Free up the memory we used to store the image
}

/////////////////////////////////////////Lets put all the drawing initialization in one place////////////////////////////////////
Shader initDrawing() {
	initTriangle();
	Shader shader(vertexShaderSource, fragmentShaderSource);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

	return shader;
}


/////////////////////////////////////////Change viewport to match a change in window size////////////////////////////////////////
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

////////////////////////////////////Check for polled input and process it/////////////////////////////////////////////////////
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//Enter default mode
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//Enter wireframe mode
			
		wire = !wire;
	}
}