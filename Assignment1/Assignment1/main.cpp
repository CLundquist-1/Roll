#include<GL/glew.h>
#include<GLFW\glfw3.h>
#include<GL\freeglut.h>

#include<iostream>

using std::cout;
using std::endl;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void initDrawing();

// settings
const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

bool wire = false;


//Triangle vertices
const float vertices[] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};

const unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

//Defining the shader programs
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"			//predefined output of the vertex shader
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"				//predefined output of the fragment shader
"}\n\0";


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

	initDrawing();

	/////////////////////////////Render Loop///////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		// Draw nothing, see you in tutorial 2 !

		processInput(window);

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//What layout location we are setting, size of the attribute, type of data, if we want numbers not from 0 to 1 to be normalized (mapped ot them), stride, offset to beginning
	glEnableVertexAttribArray(0);	//This turns layout location 0
}

/////////////////////////////////////Compiling Shaders/////////////////////////////////////////////
unsigned int compileShader(const char* source, const int shaderType) {
	unsigned int shader;							//Just another id to keep track of our shader
	shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &source, NULL);
	//first - shader ID, second - number of strings (in string array), third - source code, fourth - array of string lengths

	glCompileShader(shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	//Gets the shaders status, second - type of status, third - store success value

	//Check for successful shader compilation
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

unsigned int linkShaders(unsigned int vertexShader, unsigned int fragmentShader) {
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int  success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//We don't need the shader objects once we have the program (I guess they are copied or something)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

/////////////////////////////////////////Lets put all the drawing initialization in one place////////////////////////////////////
void initDrawing() {
	initTriangle();
	unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
	unsigned int shaderProgram = linkShaders(vertexShader, fragmentShader);
	glUseProgram(shaderProgram);
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