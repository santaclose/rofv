#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "libs/glm/glm.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw_gl3.h"

#include "shaderDealer.hpp"
#include "Camera.h"
#include "u/ml.h"
#include "u/model.hpp"
#ifndef MODEL_SET
	#include "defaultModel.hpp"
#endif // !MODEL_SET


#define VIEWPORT_WIDTH 1280
#define VIEWPORT_HEIGHT 800
#define IMGUI_WINDOWS_WIDTH_RATIO 0.16

#define VIEWPORT_3D_MSAA_COUNT 8
#define VIEWPORT_UV_MSAA_COUNT 8
#define BACKGROUND_COLOR 0.8

// OPENGL BUFFERS
unsigned int modelVertexBuffer;
unsigned int indexBuffer;

// LIGHT
glm::vec4 lightDirection = { 0.2, -0.3, -1.0, 0.0 };

//////////////// TRANSFORMATIONS

glm::mat4 projectionMatrix = glm::mat4();
glm::mat4 viewMatrix = glm::mat4();
glm::mat4 modelMatrix = glm::mat4();

////////////////

unsigned int shader;

bool wireframeMode = false;
bool useVertexNormals = true;
bool saveUvsOption = true;

int windowWidth = VIEWPORT_WIDTH, windowHeight = VIEWPORT_HEIGHT;
double aspectRatio = (double)VIEWPORT_WIDTH / (double)VIEWPORT_HEIGHT;

double lastMousePosition[2];
Camera camera;

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	aspectRatio = (double)windowWidth / (double)windowHeight;
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
	//IMGUI
	ImGui_ImplGlfw_CharCallback(window, c);
}

void MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.OnMouseScroll(yoffset);

	//IMGUI
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void MouseButtonPressed(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS)
			camera.StartOrbiting();
		else
			camera.StopOrbiting();
		break;
	}

	// IMGUI
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void MouseMotion(GLFWwindow* window, double xpos, double ypos)
{
	double dx = xpos - lastMousePosition[0];
	double dy = ypos - lastMousePosition[1];

	if (!ImGui::IsAnyItemHovered() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemActive())
	{
		camera.OnMouseMoved(dx, dy);
	}

	lastMousePosition[0] = xpos;
	lastMousePosition[1] = ypos;
}

void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_Z:
		if (action == GLFW_RELEASE)
			break;
		if (wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHT0);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_LIGHT0);
		}
		wireframeMode = !wireframeMode;
		break;
	case GLFW_KEY_N:
		if (action == GLFW_RELEASE)
			break;
		if (useVertexNormals)
			ml::setUseVertexNormals(false);
		else
			ml::setUseVertexNormals(true);
		useVertexNormals = !useVertexNormals;
		break;
	}

	//IMGUI
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

GLFWwindow* initViewport(const int resX, const int resY)
{
	glfwWindowHint(GLFW_SAMPLES, VIEWPORT_3D_MSAA_COUNT);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(resX, resY, "viewport", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to open GLFW window.\n";
		glfwTerminate();
		return NULL;
	}

	glfwSetCursorPosCallback(window, MouseMotion);
	glfwSetMouseButtonCallback(window, MouseButtonPressed);
	glfwSetScrollCallback(window, MouseScroll);
	glfwSetKeyCallback(window, KeyPressed);
	glfwSetCharCallback(window, CharCallback);
	glfwSetFramebufferSizeCallback(window, WindowResizeCallback);

	return window;
}

inline void updateMvpMatrices()
{
	projectionMatrix = glm::perspective(camera.fov, aspectRatio, 0.1, 100.0);

	/*modelMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, camera.distance));

	modelMatrix = glm::rotate(modelMatrix, camera.phi, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, camera.theta, glm::vec3(0.0f, 1.0f, 0.0f));*/

	viewMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, camera.distance));

	modelMatrix = glm::rotate(glm::mat4(), camera.phi, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, camera.theta, glm::vec3(0.0f, 1.0f, 0.0f));
}

inline void viewportTick(GLFWwindow* window)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set Projection, Model and View matrices
	updateMvpMatrices();
	
	// Update light direction
	glm::vec4 finalLightDirection = glm::inverse(modelMatrix) * lightDirection;

	// send mvp matrices to the gpu
	int uLoc = glGetUniformLocation(shader, "projectionMat");
	glUniformMatrix4fv(uLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
	uLoc = glGetUniformLocation(shader, "viewMat");
	glUniformMatrix4fv(uLoc, 1, GL_FALSE, &viewMatrix[0][0]);
	uLoc = glGetUniformLocation(shader, "modelMat");
	glUniformMatrix4fv(uLoc, 1, GL_FALSE, &modelMatrix[0][0]);

	uLoc = glGetUniformLocation(shader, "lightDir");
	glUniform3fv(uLoc, 1, &finalLightDirection.x);

	uLoc = glGetUniformLocation(shader, "noLighting");
	glUniform1i(uLoc, wireframeMode);

	// Draw model

#ifndef MODEL_SET
	defaultModel::generateDefaultModel();
#else
	generateModel();
#endif

	ml::drawModel();
	ml::clearModel();
	
	// Check for any input, or window movement
	glfwPollEvents();

	ImGui_ImplGlfwGL3_NewFrame();

	// ImGui windows
	{
		ImGui::SetNextWindowPos(ImVec2(windowWidth - windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, windowHeight));
		ImGui::Begin("Options");

		//ImGui::SliderFloat("TEST", &testValue, 0.1f, 2.0f);

		if (ImGui::Button("Wireframe (Z)"))
			KeyPressed(window, GLFW_KEY_Z, 0, GLFW_PRESS, 0);
		ImGui::Text((wireframeMode ? "Yes" : "No"));

		if (ImGui::Button("Use vertex normals approximation (N)"))
			KeyPressed(window, GLFW_KEY_N, 0, GLFW_PRESS, 0);
		ImGui::Text((useVertexNormals ? "Yes" : "No"));

		ImGui::SliderFloat("light x", &lightDirection.x, -1.0, 1.0);
		ImGui::SliderFloat("light y", &lightDirection.y, -1.0, 1.0);
		ImGui::SliderFloat("light z", &lightDirection.z, -1.0, 1.0);


		std::stringstream ss;
		ss << "Vertex count: " << ml::getLastDrawVertexCount() << '\n';
		ImGui::Text(ss.str().c_str());

		ImGui::Checkbox("Save UVs", &saveUvsOption);
		if (ImGui::Button("Save"))
			ml::setExporting();
		
#ifndef MODEL_SET
		ImGui::Text("No model code detected");
#endif

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, windowHeight));
		ImGui::Begin("Parameters");

		/*ImGui::SliderFloat("radius", &radius, 0.1f, 5.0f);
		ImGui::SliderFloat("height", &height, 1.0f, 8.0f);
		ImGui::SliderInt("hResolution", &hResolution, 5, 300);
		ImGui::SliderInt("vResolution", &vResolution, 5, 300);
		ImGui::SliderInt("sides", &sides, -20, 20);*/

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap front and back buffers
	glfwSwapBuffers(window);
}

int main(int argc, char** argv)
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return -1;
	}
	GLFWwindow* window = initViewport(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {

		std::cout << "GLEW error\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, false);
	ImGui::StyleColorsLight();

	// Get GPU info and supported OpenGL version
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;

	glClearColor(BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (window != NULL)
	{
		glGenBuffers(1, &modelVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelVertexBuffer);
		//glBufferData(GL_ARRAY_BUFFER, defaultModelVertices.size() * sizeof(ml::vertexS), &defaultModelVertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ml::vertexS), 0);						// vertex positions
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ml::vertexS), (void*)(sizeof(float)*3)); // vertex normals
		// TODO vertex uvs

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(unsigned int), defaultModelIndexBuffer, GL_STATIC_DRAW);

		shader = CreateShader("shaders/diffuse.shader");
		glUseProgram(shader);
			   
		while (!glfwWindowShouldClose(window)/* && !glfwWindowShouldClose(pWindow)*/)
		{
			viewportTick(window);
			//viewportParameterTick(pWindow);
		}
		glDeleteProgram(shader);
		glfwDestroyWindow(window);
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}