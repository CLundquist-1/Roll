#include "Shader.h"
#include "stb_image.h"

#include<GL/glew.h>
#include<GLFW\glfw3.h>
#include<GL\freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<math.h>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::getchar;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
Shader initDrawing();
void matMan(Shader);
void window_focus_callback(GLFWwindow* window, int focused);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float CalcDistance(glm::vec3 position, int index, float time);
float Lift(float time);
glm::mat4 CalcModel(glm::vec3 position, int index);

void CalcPosition(float distance);

vector<float> getTrack(string filename, char delim = ',');

// settings
constexpr unsigned int scr_width = 800;
constexpr unsigned int scr_height = 600;
unsigned int trackVAO;
unsigned int cartVAO;

bool wire = false;

bool processed = false;

constexpr float rectH = 0.1;
constexpr float rectL = 0.2;
constexpr float rectW = 0.1;
const float cartVerts[] = {
	-rectL, -rectH, -rectW,  0.0f, 0.0f,
	rectL, -rectH, -rectW,  1.0f, 0.0f,
	rectL,  rectH, -rectW,  1.0f, 1.0f,
	rectL,  rectH, -rectW,  1.0f, 1.0f,
	-rectL,  rectH, -rectW,  0.0f, 1.0f,
	-rectL, -rectH, -rectW,  0.0f, 0.0f,

	-rectL, -rectH,  rectW,  0.0f, 0.0f,
	rectL, -rectH,  rectW,  1.0f, 0.0f,
	rectL,  rectH,  rectW,  1.0f, 1.0f,
	rectL,  rectH,  rectW,  1.0f, 1.0f,
	-rectL,  rectH,  rectW,  0.0f, 1.0f,
	-rectL, -rectH,  rectW,  0.0f, 0.0f,

	-rectL,  rectH,  rectW,  1.0f, 0.0f,
	-rectL,  rectH, -rectW,  1.0f, 1.0f,
	-rectL, -rectH, -rectW,  0.0f, 1.0f,
	-rectL, -rectH, -rectW,  0.0f, 1.0f,
	-rectL, -rectH,  rectW,  0.0f, 0.0f,
	-rectL,  rectH,  rectW,  1.0f, 0.0f,

	rectL,  rectH,  rectW,  1.0f, 0.0f,
	rectL,  rectH, -rectW,  1.0f, 1.0f,
	rectL, -rectH, -rectW,  0.0f, 1.0f,
	rectL, -rectH, -rectW,  0.0f, 1.0f,
	rectL, -rectH,  rectW,  0.0f, 0.0f,
	rectL,  rectH,  rectW,  1.0f, 0.0f,

	-rectL, -rectH, -rectW,  0.0f, 1.0f,
	rectL, -rectH, -rectW,  1.0f, 1.0f,
	rectL, -rectH,  rectW,  1.0f, 0.0f,
	rectL, -rectH,  rectW,  1.0f, 0.0f,
	-rectL, -rectH,  rectW,  0.0f, 0.0f,
	-rectL, -rectH, -rectW,  0.0f, 1.0f,

	-rectL,  rectH, -rectW,  0.0f, 1.0f,
	rectL,  rectH, -rectW,  1.0f, 1.0f,
	rectL,  rectH,  rectW,  1.0f, 0.0f,
	rectL,  rectH,  rectW,  1.0f, 0.0f,
	-rectL,  rectH,  rectW,  0.0f, 0.0f,
	-rectL,  rectH, -rectW,  0.0f, 1.0f
};

vector<float> track;
vector<glm::vec3> trackVec;
vector<glm::vec3> trackNorms;
vector<glm::vec3> trackTans;
vector<glm::vec3> trackBNs;

//Defining the shader programs
const char *vertexShaderSource = "vertexShader.vs";
const char *fragmentShaderSource = "fragmentShader.fs";

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float lastX = scr_width * 0.5, lastY = scr_height * 0.5;

float yaw = -90.0f, pitch = 0.0f, fov = 45.0f; //a yaw of 0 results in x=1 (a direction looking right) so we offset this initial value

bool firstMouse = true;

const string inputFile = "track.txt";
const string outputFile = "pTrack.txt";

constexpr int nDivides = 10;
constexpr int nChases = 2;

constexpr int numSeg = 1000;

constexpr float grav = 9.81;

constexpr float liftSpeed = 2.0f;

float segLength = 0.0f;

int maxHeightIndex;

float maxHeight;

constexpr int decIndex = numSeg * 0.9;

float decLength;

int currIndex;

glm::vec3 currPosition;

float currDistance;

///////////////////////////Camera Stuff///////////////////////////////////

//The FPS way
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 25.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);




////////////////////////Other Matrices/////////////////////////////////////
glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
//glm::mat4 proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
//field of view, aspect ratio, near distance, far distance

glm::mat4 model = glm::mat4(1.0f);

int main() {
	if (!glfwInit()) {
		cout << "Failed to initialize GLFW\n";
		return -1;
	}
	//////////////////////////////Creation of our context window////////////////////////////////////////////////
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

	//When focus is given to the window
	glfwSetWindowFocusCallback(window, window_focus_callback);
	//Hide and capture mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//Direct mouse input
	glfwSetCursorPosCallback(window, mouse_callback);

	//Scrolling
	glfwSetScrollCallback(window, scroll_callback);

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

	//We do this outside the main loop because the projection matrix does not change
	unsigned int projLoc = glGetUniformLocation(shader.ID, "project");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	//Lets take depth into account for drawing
	glEnable(GL_DEPTH_TEST);

	glPointSize(1);
	/////////////////////////////Render Loop///////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		glPointSize(1);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//cout << deltaTime << endl;

		/*if (currIndex < maxHeightIndex) {
			CalcPosition(Lift(deltaTime));
		}
		else if(currIndex < decIndex) {
			CalcPosition(CalcDistance(currPosition, deltaTime));
		}
		else {
			CalcPosition(Dec(deltaTime));
		}*/

		CalcPosition(CalcDistance(currPosition, currIndex, deltaTime));

		processInput(window);


		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);				//This is a different way of passing the matrix, giving it the address of the first element


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			//There is also a depth buffer bit and stencil buffer bit
		
		glBindVertexArray(trackVAO);
		glDrawArrays(GL_TRIANGLES, 0, track.size()/3);
		glPointSize(5);
		glDrawArrays(GL_POINTS, maxHeightIndex, 1);

		//glm::mat4 temp = glm::translate(model, currPosition);

		glm::mat4 temp = CalcModel(currPosition, currIndex);

		//cout << currIndex << endl;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(temp));
		glBindVertexArray(cartVAO);
		//glDrawArrays(GL_TRIANGLES, 0, (sizeof(cartVerts) / sizeof(float))/3);


		//glPointSize(10);
		//glDrawArrays(GL_POINTS, 0, 1);
		//Swap buffers
		glfwSwapBuffers(window);

		//Poll input events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

vector<float> getTrack(string filename, char delim) {
	vector<float> trackVectors;
	string line;
	ifstream myfile(filename);
	if (myfile.is_open())
	{
		getline(myfile, line);
		if (line[0] == 'P')
			processed = true;
		else {
			std::stringstream ss(line);
			std::string token;
			while (std::getline(ss, token, delim)) {
				trackVectors.push_back(std::strtof(token.c_str(), 0));
			}
		}
		while (getline(myfile, line))
		{
			if (line[0] == '/')
				continue;
			std::stringstream ss(line);
			std::string token;
			while (std::getline(ss, token, delim)) {
				trackVectors.push_back(std::strtof(token.c_str(), 0));
			}
			//cout << line << '\n';
		}
		myfile.close();
	}

	else cout << "Unable to open file";
	return trackVectors;
}

bool saveTrack(vector<float> track) {
	ofstream myfile;
	myfile.open(outputFile);
	myfile << "Processed\n";
	for (int i = 0; i < track.size()-2; i+=3) {
		myfile << track[i] << "," << track[i + 1] << "," << track[i + 2] << "\n";
	}
	myfile.close();
	return true;
}

vector<glm::vec3> floatsToVec3s(vector<float> curve) {
	vector<glm::vec3> vecs;
	for (int i = 0; i < curve.size() - 2; i += 3) {
		vecs.push_back(glm::vec3(curve[i], curve[i + 1], curve[i + 2]));
	}
	return vecs;
}

vector<float> vec3sToFloats(vector<glm::vec3> curve) {
	vector<float> vecs;
	for (int i = 0; i < curve.size()-1; i ++) {
		glm::vec3 v = curve[i];
		vecs.push_back(v.x);
		vecs.push_back(v.y);
		vecs.push_back(v.z);
	}
	return vecs;
}

vector<glm::vec3> smoothCurve(vector<float> c, int numDivides = 1, int numChases = 1) {
	vector<glm::vec3> newCurve;
	vector<glm::vec3> curve = floatsToVec3s(c);
	for (int i = 0; i < numDivides; i++) {
		newCurve.clear();
		glm::vec3 veryFirst = curve[0];
		for (int j = 0; j < curve.size() - 1; j++) {
			glm::vec3 first = curve[j];
			glm::vec3 second = curve[j+1];
			//float mid = (second - first) / 2 + first;
			glm::vec3 mid = (second + first) * 0.5f;
			first = (first + mid) * 0.5f;
			mid = (mid + second) * 0.5f;
			newCurve.push_back(first);
			newCurve.push_back(mid);
			//newCurve.push_back(curve[j + 1]);
		}
		//Account for the last vertex and a closed circuit
		glm::vec3 last = curve[curve.size() - 1];
		glm::vec3 mid = (last + veryFirst) * 0.5f;
		last = (last + mid) * 0.5f;
		mid = (mid + veryFirst) * 0.5f;
		newCurve.push_back(last);
		newCurve.push_back(mid);

		//Clear the curve we pulled everything from
		curve.clear();

		for (int k = 0; k < numChases-1; k++) {
			glm::vec3 first = newCurve[0];
			for (int j = 0; j < newCurve.size() - 1; j ++) {
				glm::vec3 first = newCurve[j];
				glm::vec3 second = newCurve[j + 1];
				first = (first + second) * 0.5f;
				curve.push_back(first);
			}
			glm::vec3 last = newCurve[newCurve.size() - 1];
			last = (last + first) * 0.5f;
			curve.push_back(last);
			newCurve = curve;
			curve.clear();
		}
		if (i < numDivides)
			curve = newCurve;
	}
	return newCurve;
}

float CalcDistance(const vector<glm::vec3>& track) {
	float distance = 0.0f;
	for (int i = 0; i < track.size() - 1; i++) {
		glm::vec3 diff = track[i + 1] - track[i];
		distance += glm::length(diff);
	}
	return distance;
}

int FindMaxHeightIndex(const vector<glm::vec3>& track) {
	maxHeight = -1.0f;
	maxHeightIndex = -1;
	for (int i = 0; i < track.size(); i++) {
		if (track[i].y > maxHeight) {
			maxHeight = track[i].y;
			maxHeightIndex = i;
		}
	}
	return maxHeightIndex;
}

vector<glm::vec3> ParameterizeCurve(const vector<glm::vec3>& track, const float seglength) {
	vector<glm::vec3> paramTrack;
	float distance = 0.0f;
	paramTrack.push_back(track[0]);
	glm::vec3 diff;
	for (int i = 0; i < track.size() - 1; i++) {
		diff = track[i + 1] - track[i];
		distance += glm::length(diff);
		if (seglength <= (distance)) {
			distance -= seglength;
			paramTrack.push_back(track[i + 1]);
		}
	}
	diff = paramTrack[0] - paramTrack[paramTrack.size() - 1];
	distance += glm::length(diff);
	if (seglength <= distance) {
		float percent = seglength / distance;
		paramTrack.push_back(paramTrack[0]*percent + paramTrack[paramTrack.size() - 1]*(1-percent));
	}
	return paramTrack;
}

/////////////////////////////////////////Camera Rotation///////////////////////////////////////////////////////////////////
void window_focus_callback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		// The window gained input focus
		//glfwGetCursorPos(window, &lastX, &lastY);
	}
	else
	{
		//firstMouse = true;
		// The window lost input focus
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//Calculate change in mouse position
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	//Adjust for camera speed
	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	//Constrain the pitch so there is no reversal of direction
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//Change the front facing portion of the camera based on the pitch and yaw
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

/*float CalcSpeed(glm::vec3 position) {
	return sqrt(2 * grav*(maxHeight - position.y));
}*/

float Lift(float time) {
	return liftSpeed * time;
}

float Dec(glm::vec3 position) {
	return sqrt(2 * grav*(maxHeight - position.y))*(glm::length((trackVec[trackVec.size()-1] - position))/decLength);
}

float CalcSpeed(glm::vec3 position, float index) {
	if (index < maxHeightIndex)
		return liftSpeed;
	else if (index < decIndex)
		return sqrt(2 * grav*(maxHeight - currPosition.y));
	else
		return Dec(position);
}

float CalcDistance(glm::vec3 position, int index,  float time) {
	return CalcSpeed(position, index) * time;
}

void CalcPosition(float distance) {
	currDistance += distance;
	float temp = currDistance / segLength;
	int t = temp;
	if (t != 0) {
		float ratio = (currDistance - t * segLength) / segLength;
		currIndex += t;
		if (currIndex < trackVec.size() - 1) {
			currPosition = trackVec[currIndex] * ratio + trackVec[currIndex + 1] * (1 - ratio);
			currDistance -= t * segLength;
		}
	}
}

void CalcTrackNormals() {
	//float v = CalcSpeed(trackVec[0], 0);
	glm::vec3 n = trackVec[1] - 2.0f*trackVec[0] + trackVec[trackVec.size()-1];
	float x = 0.5f * n.length();
	float c = 0.5f * (glm::length(trackVec[1] - trackVec[trackVec.size() - 1]));
	float k = 1 / (x*x + c * c);
	n = glm::normalize(n);
	n = k * n;
	n = n - glm::vec3(0.0f, grav, 0.0f);
	n = glm::normalize(n);
	trackNorms.push_back(n);
	for (int i = 1; i < trackVec.size()-1; i++) {
		//v = CalcSpeed(trackVec[i], i);
		n = trackVec[i + 1] - 2.0f*trackVec[i] + trackVec[i - 1];
		x = 0.5f * n.length();
		c = 0.5f * (glm::length(trackVec[i+1] - trackVec[i - 1]));
		k = 1 / (x*x + c * c);
		n = glm::normalize(n);
		n = k * n;
		n = n - glm::vec3(0.0f, grav, 0.0f);
		n = glm::normalize(n);
		trackNorms.push_back(n);
	}
	n = trackVec[0] - 2.0f*trackVec[trackVec.size()-1] + trackVec[trackVec.size() - 2];
	x = 0.5f * n.length();
	c = 0.5f * (glm::length(trackVec[0] - trackVec[trackVec.size() - 2]));
	k = 1 / (x*x + c * c);
	n = glm::normalize(n);
	n = k * n;
	n = n - glm::vec3(0.0f, grav, 0.0f);
	n = glm::normalize(n);
	trackNorms.push_back(n);
}

void CalcTrackTangents() {
	glm::vec3 t;
	for (int i = 0; i < trackVec.size() - 1; i++) {
		t = trackVec[i + 1] - trackVec[i];
		t = glm::normalize(t);
		trackTans.push_back(t);
	}
	t = trackVec[trackVec.size()-1] - trackVec[0];
	t = glm::normalize(t);
	trackTans.push_back(t);
}

void CalcTrackBiNormals() {
	glm::vec3 b;
	for (int i = 0; i < trackVec.size(); i++) {
		b = glm::cross(trackTans[i], trackNorms[i]);
		b = glm::normalize(b);
		trackBNs.push_back(b);
	}
}

void CalcCorrectTrackTans() {
	glm::vec3 t;
	trackTans.clear();
	for (int i = 0; i < trackVec.size(); i++) {
		t = glm::cross(trackNorms[i], trackBNs[i]);
		t = glm::normalize(t);
		trackTans.push_back(t);
	}
}

glm::mat4 CalcModel(glm::vec3 position, int index) {
	index = index % trackVec.size();
	glm::mat4 model = glm::mat4(glm::vec4(trackBNs[index],0.0f), glm::vec4(trackNorms[index],0.0f), glm::vec4(trackTans[index],0.0f), glm::vec4(position,1.0f));
	return model;
}

void triangleTrack() {
	//vector<float> t;
	float trackWidth = 0.1f;
	track.clear();
	glm::vec3 first = trackWidth*trackBNs[0] + trackVec[0];
	glm::vec3 second = (-trackWidth) * trackBNs[0] + trackVec[0];
	glm::vec3 third;
	for (int i = 1; i < trackVec.size(); i++) {
		track.push_back(first.x);
		track.push_back(first.y);
		track.push_back(first.z);
		track.push_back(second.x);
		track.push_back(second.y);
		track.push_back(second.z);
		if(i % 2 == 0)
			third = trackWidth * trackBNs[i] + trackVec[i];
		else
			third = (-trackWidth) * trackBNs[i] + trackVec[i];
		track.push_back(third.x);
		track.push_back(third.y);
		track.push_back(third.z);
		first = second;
		second = third;
	}
	/*track.push_back(first.x);
	track.push_back(first.y);
	track.push_back(first.z);
	track.push_back(second.x);
	track.push_back(second.y);
	track.push_back(second.z);
	if (trackBNs.size() % 2 == 0)
		third = trackWidth * trackBNs[0] + trackVec[0];
	else
		third = (-trackWidth) * trackBNs[0] + trackVec[0];
	track.push_back(third.x);
	track.push_back(third.y);
	track.push_back(third.z);*/
}

/////////////////////////////////////////Generate and pass our triangle vertex data to the GPU///////////////////////////////////
void initTrack()
{
	/*ifstream myfile(outputFile);
	if (myfile.is_open())
	{
		track = getTrack(outputFile);
	}
	else
	{
		track = getTrack(inputFile);
	}
	//track = getTrack(inputFile);
	if (!processed) {
		track = smoothCurve(track, 15, 2);
		saveTrack(track);
	}*/

	track = getTrack(inputFile);
	trackVec = smoothCurve(track, nDivides, nChases);
	float curveLength = CalcDistance(trackVec);

	segLength = curveLength / numSeg;
	trackVec = ParameterizeCurve(trackVec, segLength);

	
	//End variables
	maxHeightIndex = FindMaxHeightIndex(trackVec);
	currPosition = trackVec[0];
	decLength = (trackVec.size() - decIndex)*segLength;

	CalcTrackNormals();
	CalcTrackTangents();
	CalcTrackBiNormals();
	CalcCorrectTrackTans();

	triangleTrack();
	//track = vec3sToFloats(trackVec);

	//The VAO will store enabled vertex attributes, attribute configurations, and vertex buffer objects associated with vertex attributes
	glGenVertexArrays(1, &trackVAO);
	glBindVertexArray(trackVAO);

	//Create vertex buffer object that will contain all the triangles vertex data. It makes it easier to pass the GPU large portions of data at a time
	unsigned int VBO;

	//Generate the buffer in GPU memory and pass the id of the object back into the vertex buffer object
	glGenBuffers(1, &VBO); //We pass it the address so it can manipulate the VBO contents directly

						   //Now we associate the id to an Array Buffer. Remember we can bind to several different buffers at once as long as they are a different buffer type
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //From this point on any buffer calls we make will be to the currently bound buffer VBO

	//Now lets push our triangles vertex data to the buffers memory on the GPU
	glBufferData(GL_ARRAY_BUFFER, track.size()*sizeof(float), &track[0], GL_STATIC_DRAW);


	//The data is now in GPU memory

	//Define the data we just pushed to GPU memory

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//What layout location we are setting, size of the attribute, type of data, if we want numbers not from 0 to 1 to be normalized (mapped ot them), stride, offset to beginning
	glEnableVertexAttribArray(0);	//This turns layout location 0

	//////////////////////////////////////////////////Cart Stuff/////////////////////////////////////////////////////////////////
	glGenVertexArrays(1, &cartVAO);
	glBindVertexArray(cartVAO);

	//Create vertex buffer object that will contain all the triangles vertex data. It makes it easier to pass the GPU large portions of data at a time
	VBO;

	//Generate the buffer in GPU memory and pass the id of the object back into the vertex buffer object
	glGenBuffers(1, &VBO); //We pass it the address so it can manipulate the VBO contents directly

						   //Now we associate the id to an Array Buffer. Remember we can bind to several different buffers at once as long as they are a different buffer type
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //From this point on any buffer calls we make will be to the currently bound buffer VBO

										//Now lets push our triangles vertex data to the buffers memory on the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(cartVerts), &cartVerts[0], GL_STATIC_DRAW);


	//The data is now in GPU memory

	//Define the data we just pushed to GPU memory

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//What layout location we are setting, size of the attribute, type of data, if we want numbers not from 0 to 1 to be normalized (mapped ot them), stride, offset to beginning
	glEnableVertexAttribArray(0);	//This turns layout location 0
}

/////////////////////////////////////////Lets put all the drawing initialization in one place////////////////////////////////////
Shader initDrawing() {
	initTrack();
	Shader shader(vertexShaderSource, fragmentShaderSource);
	shader.use();

	return shader;
}


/////////////////////////////////////////Change viewport to match a change in window size////////////////////////////////////////
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

////////////////////////////////////Check for polled input and process it/////////////////////////////////////////////////////
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//Enter default mode
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//Enter wireframe mode
		wire = !wire;
	}
	//Camera movement
	float cameraSpeed = 4.0f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += glm::vec3(0.0f,1.0f,0.0f) * cameraSpeed;
}