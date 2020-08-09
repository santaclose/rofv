#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "Shader.h"
#include "Texture.h"
#include "WindowHelper.h"
#include "Camera.h"

#include "modelTool/ml.h"
bool haveToGenerateModel = true;

//#include "../assets/models/ripple.hpp"
//#include "../assets/models/spiralStairs.hpp"
#include "../assets/models/maze.hpp"
//#include "../assets/models/uShapedStairs.hpp"
//#include "../assets/models/randomPointCube.hpp"
//#include "../assets/models/lineFractal.hpp"
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

	//glfwSetCursorPosCallback(window, MouseMotion);
	//glfwSetMouseButtonCallback(window, MouseButtonPressed);
	//glfwSetScrollCallback(window, MouseScroll);
	//glfwSetKeyCallback(window, KeyPressed);
	//glfwSetCharCallback(window, CharCallback);
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

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	bool cursorOverImGui = false;
	// ImGui windows
	{
		ImGui::SetNextWindowPos(ImVec2(WindowHelper::width - WindowHelper::width * IMGUI_WINDOWS_WIDTH_RATIO, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(WindowHelper::width * IMGUI_WINDOWS_WIDTH_RATIO, WindowHelper::height));
		ImGui::Begin("Options");

		if (ImGui::Button("Render mode (Z)"))
		{
			renderingMode++;
			if (renderingMode == 3)
				renderingMode = 0;

			if (renderingMode == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else if (renderingMode == 1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else if (renderingMode == 2)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			}
		}

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
		{
			useVertexNormals = !useVertexNormals;
			ml::setUseVertexNormals(useVertexNormals);
			haveToGenerateModel = true;
		}
		ImGui::Text((useVertexNormals ? "Yes" : "No"));

		if (ImGui::Button("Use texture (T)"))
			useTexture = !useTexture;

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
			std::cout << "exporting as obj\n";
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

	if (!ImGui::IsAnyItemHovered() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemActive())
	{
		if (ImGui::GetIO().MouseClicked[0])
			camera.StartOrbiting();
		else if (ImGui::GetIO().MouseReleased[0])
			camera.StopOrbiting();

		camera.OnMouseScroll(ImGui::GetIO().MouseWheel);
		camera.OnMouseMoved(ImGui::GetIO().MouseDelta.x , ImGui::GetIO().MouseDelta.y);
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	// Setup ImGui binding
	ImGui::CreateContext();
	auto f = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/FiraCode/FiraCode-Regular.ttf", 14.0f);
	ImGui::GetIO().FontDefault = f;
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
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
		diffuseShader = new Shader("assets/shaders/diffuse.shader");
		diffuseShader->Bind();

		texture = new Texture("assets/textures/def.png");
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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}