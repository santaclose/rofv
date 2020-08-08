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

#include "Shader.h"
#include "Texture.h"
//#include "shaderDealer.hpp"
#include "WindowHelper.h"
#include "Camera.h"

#include "u/ml.h"
bool haveToGenerateModel = true;

//#include "u/models/ripple.hpp"
//#include "u/models/spiralStairs.hpp"
#include "u/models/maze.hpp"
//#include "u/models/uShapedStairs.hpp"
//#include "u/models/randomPointCube.hpp"
//#include "u/models/lineFractal.hpp"
#ifndef MODEL_SET
	#include "defaultModel.hpp"
#endif // !MODEL_SET


#define VIEWPORT_WIDTH 1280
#define VIEWPORT_HEIGHT 800
#define IMGUI_WINDOWS_WIDTH_RATIO 0.16

#define VIEWPORT_3D_MSAA_COUNT 8
#define VIEWPORT_UV_MSAA_COUNT 8
#define BACKGROUND_COLOR 0.6

// OPENGL BUFFERS
unsigned int modelVertexBuffer;
unsigned int indexBuffer;

//////////////// OPTIONS

int renderingMode = 0;
bool useVertexNormals = true;
bool useTexture = false;
bool saveUvsOption = true;

// LIGHT
glm::vec4 lightDirection = { 0.2, -0.3, -1.0, 0.0 };
glm::vec4 finalLightDirection = { 0.2, -0.3, -0.1, 0.0 };

//////////////// DIFFUSE LIGHTING SHADER

Texture* texture;
Shader* diffuseShader;
//unsigned int diffuseShader;

//////////////// RENDERING

Camera camera;

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	WindowHelper::OnResize(window, width, height);
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
	WindowHelper::MouseMoved(xpos, ypos);

	if (!ImGui::IsAnyItemHovered() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemActive())
	{
		camera.OnMouseMoved(WindowHelper::dx, WindowHelper::dy);
	}
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

		renderingMode++;
		if (renderingMode == 3)
			renderingMode = 0;

		if (renderingMode == 0)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_LIGHT0);
		}
		else if (renderingMode == 1)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_LIGHT0);
		}
		else if (renderingMode == 2)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDisable(GL_LIGHT0);
		}
		break;
	case GLFW_KEY_S:
		if (action == GLFW_RELEASE)
			break;
		useVertexNormals = !useVertexNormals;
		ml::setUseVertexNormals(useVertexNormals);
		haveToGenerateModel = true;
		break;
	case GLFW_KEY_T:
		if (action == GLFW_RELEASE)
			break;
		useTexture = !useTexture;
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

	WindowHelper::Initialize(resX, resY);

	return window;
}


inline void viewportTick(GLFWwindow* window)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set Projection, Model and View matrices
	camera.UpdateTransformationMatrices(WindowHelper::aspectRatio);
	
	// Update light direction
	finalLightDirection = glm::inverse(camera.modelMatrix) * lightDirection;

	// send mvp matrices to the gpu
	diffuseShader->SetUniformMatrix4fv("projectionMat", &camera.projectionMatrix[0][0]);
	diffuseShader->SetUniformMatrix4fv("viewMat", &camera.viewMatrix[0][0]);
	diffuseShader->SetUniformMatrix4fv("modelMat", &camera.modelMatrix[0][0]);
	diffuseShader->SetUniform3fv("lightDir", &finalLightDirection.x);
	diffuseShader->SetUniform1i("noLighting", renderingMode);
	diffuseShader->SetUniform1i("useTexture", useTexture);

	// Draw model

	if (haveToGenerateModel)
	{
		ml::clearModel();
#ifndef MODEL_SET
		defaultModel::generateModel();
#else
		generateModel();
#endif
		haveToGenerateModel = false;
		ml::afterGenerate();
	}

	ml::drawModel();
	
	// Check for any input, or window movement
	glfwPollEvents();

	ImGui_ImplGlfwGL3_NewFrame();

	// ImGui windows
	{
		ImGui::SetNextWindowPos(ImVec2(WindowHelper::width - WindowHelper::width * IMGUI_WINDOWS_WIDTH_RATIO, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(WindowHelper::width * IMGUI_WINDOWS_WIDTH_RATIO, WindowHelper::height));
		ImGui::Begin("Options");

		//ImGui::SliderFloat("TEST", &testValue, 0.1f, 2.0f);

		if (ImGui::Button("Render mode (Z)"))
			KeyPressed(window, GLFW_KEY_Z, 0, GLFW_PRESS, 0);
		if (renderingMode == 0)
		{
			ImGui::Text("Fill");
		}
		else if (renderingMode == 1)
		{
			ImGui::Text("Line");
		}
		else
		{
			ImGui::Text("Point");
		}

		if (ImGui::Button("Smooth shading (S)"))
			KeyPressed(window, GLFW_KEY_S, 0, GLFW_PRESS, 0);
		ImGui::Text((useVertexNormals ? "Yes" : "No"));

		if (ImGui::Button("Use texture (T)"))
			KeyPressed(window, GLFW_KEY_T, 0, GLFW_PRESS, 0);
		ImGui::Text((useTexture ? "Yes" : "No"));

		ImGui::SliderFloat("light x", &lightDirection.x, -1.0, 1.0);
		ImGui::SliderFloat("light y", &lightDirection.y, -1.0, 1.0);
		ImGui::SliderFloat("light z", &lightDirection.z, -1.0, 1.0);

		std::stringstream ss;
		ss << "Vertex count: " << ml::getLastDrawVertexCount() << '\n';
		ImGui::Text(ss.str().c_str());

		ImGui::Checkbox("Save UVs", &saveUvsOption);
		if (ImGui::Button("Save"))
		{
			haveToGenerateModel = true;
			ml::setExporting();
		}
		
#ifndef MODEL_SET
		ImGui::Text("No model code detected");
#endif

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(WindowHelper::width * IMGUI_WINDOWS_WIDTH_RATIO, WindowHelper::height));
		ImGui::Begin("Parameters");

#ifdef MODEL_SET
#ifdef BINDINGS
		bindings();
#endif // BINDINGS
#else // MODEL_SET
		defaultModel::bindings();
#endif // MODEL_SET

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
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ml::vertexS), 0);						  // vertex positions
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ml::vertexS), (void*)(sizeof(float) * 3)); // vertex normals
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ml::vertexS), (void*)(sizeof(float) * 6)); // vertex uvs
		// TODO vertex uvs

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(unsigned int), defaultModelIndexBuffer, GL_STATIC_DRAW);

		//diffuseShader = CreateShader("shaders/diffuse.shader");
		//glUseProgram(diffuseShader);
		diffuseShader = new Shader("shaders/diffuse.shader");
		diffuseShader->Bind();

		texture = new Texture("textures/def.png");
		texture->Bind();

		diffuseShader->SetUniform1i("tex", 0);
			   
		while (!glfwWindowShouldClose(window)/* && !glfwWindowShouldClose(pWindow)*/)
		{
			viewportTick(window);
			//viewportParameterTick(pWindow);
		}
		delete diffuseShader;
		delete texture;
		glfwDestroyWindow(window);
	}

	ml::destroyEverything();
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}