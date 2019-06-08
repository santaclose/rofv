#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include <iostream>

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw_gl3.h"

#include "perspectiveGLFunction.inc"
//#include "shaderDealer.inc"
#include "Camera.h"
#include "ParametersHandler.h"
#include "u/model.hpp"

#define VIEWPORT_WIDTH 1280
#define VIEWPORT_HEIGHT 800
#define IMGUI_WINDOWS_WIDTH_RATIO 0.16

#define VIEWPORT_3D_MSAA_COUNT 8
#define VIEWPORT_UV_MSAA_COUNT 8
#define BACKGROUND_COLOR 0.8

//#define CAMERA_DISTANCE -5

bool recalculateLighting = false;

bool wireframeMode = false;
bool cullBackFaces = false;
bool saveUvsOption = true;

int windowWidth = VIEWPORT_WIDTH, windowHeight = VIEWPORT_HEIGHT;
double aspectRatio = (double)VIEWPORT_WIDTH / (double)VIEWPORT_HEIGHT;

double lastPosition[2];
Camera camera;

// lighting
float light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	/* White diffuse light. */
float light_position[] = { -0.5, -0.5, -1, 0 };
float ambient_light[] = { .8, .8, .8, 1 };


/*inline void updateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION_MATRIX);
	glLoadIdentity();
	perspectiveGL(60.0, aspectRatio, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW_MATRIX);
}
inline void updateModelviewMatrix()
{
	//glLoadIdentity();
	// move forward
	glTranslatef(0, 0, camera.distance);
	// rotations
	glRotated(camera.phi, 1, 0, 0);
	glRotated(camera.theta, 0, 1, 0);
}*/

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	aspectRatio = (double)windowWidth / (double)windowHeight;
	//updateProjectionMatrix();
}

void CharCallback(GLFWwindow* window, unsigned int c)
{
	//IMGUI
	ImGui_ImplGlfw_CharCallback(window, c);
}

void MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.OnMouseScroll(yoffset);
	//updateModelviewMatrix();

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
	double dx = xpos - lastPosition[0];
	double dy = ypos - lastPosition[1];

	if (!ImGui::IsAnyItemHovered() && !ImGui::IsMouseHoveringAnyWindow() && !ImGui::IsAnyItemActive())
	{
		camera.OnMouseMoved(dx, dy);
		//updateModelviewMatrix();
	}

	lastPosition[0] = xpos;
	lastPosition[1] = ypos;
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
	case GLFW_KEY_C:
		if (action == GLFW_RELEASE)
			break;
		if (cullBackFaces)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
		cullBackFaces = !cullBackFaces;
		break;
	}

	//IMGUI
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}


void SaveModel()
{
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

inline void SetAllGLMatrices()
{
	glMatrixMode(GL_PROJECTION_MATRIX);

		glLoadIdentity();
		perspectiveGL(60.0, aspectRatio, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW_MATRIX);
	
		// move forward
		glTranslatef(0, 0, camera.distance);
		// rotations
		glRotated(camera.phi, 1, 0, 0);
		glRotated(camera.theta, 0, 1, 0);
}

inline void viewportTick(GLFWwindow* window)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set Projection and ModelView matrices
	SetAllGLMatrices();

	// Draw model
#ifndef MODEL_SET

	// default model
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, -1.0);
	glVertex3f(0.0, 1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(-1.0, 0.0, 0.0);
	glVertex3f(-1.0, -1.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
#else

	generateModel();
	clearModel();

#endif // !MODEL_SET
	
	// Check for any input, or window movement
	glfwPollEvents();

	ImGui_ImplGlfwGL3_NewFrame();

	// ImGui windows
	{
		ImGui::SetNextWindowPos(ImVec2(windowWidth - windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, windowHeight));
		ImGui::Begin("Options");

		if (ImGui::Button("Wireframe (Z)"))
			KeyPressed(window, GLFW_KEY_Z, 0, GLFW_PRESS, 0);
		if (ImGui::Button("Cull back faces (C)"))
			KeyPressed(window, GLFW_KEY_C, 0, GLFW_PRESS, 0);

		/*ImGui::Checkbox("Recalculate lights", &recalculateLighting);
		ImGui::SliderFloat("Light diffuse R:", &light_diffuse[0], 0.0f, 5.0f);
		ImGui::SliderFloat("Light diffuse G:", &light_diffuse[1], 0.0f, 5.0f);
		ImGui::SliderFloat("Light diffuse B:", &light_diffuse[2], 0.0f, 5.0f);
		ImGui::SliderFloat("Light diffuse A:", &light_diffuse[3], 0.0f, 1.0f);

		ImGui::DragFloat("Light position X:", &light_position[0]);
		ImGui::DragFloat("Light position Y:", &light_position[1]);
		ImGui::DragFloat("Light position Z:", &light_position[2]);
		ImGui::SliderFloat("Light position W:", &light_position[3], 0.0f, 1.0f);*/

		ImGui::Checkbox("Save UVs", &saveUvsOption);
		if (ImGui::Button("Save"))
			SaveModel();

		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(windowWidth * IMGUI_WINDOWS_WIDTH_RATIO, windowHeight));
		ImGui::Begin("Parameters");

		ImGui::SliderFloat("radius", &radius, 0.1f, 5.0f);
		ImGui::SliderFloat("height", &height, 1.0f, 8.0f);
		ImGui::SliderInt("hResolution", &hResolution, 5, 300);
		ImGui::SliderInt("vResolution", &vResolution, 5, 300);
		ImGui::SliderInt("sides", &sides, -20, 20);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

	// Update Screen
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

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glClearColor(BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR, 1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST); // Depth Testing
	//updateProjectionMatrix();

	/*GLFWwindow* pWindow = initParameterWindow(200, 800);*/
	if (/*pWindow != NULL && */window != NULL)
	{
		while (!glfwWindowShouldClose(window)/* && !glfwWindowShouldClose(pWindow)*/)
		{
			viewportTick(window);
			//viewportParameterTick(pWindow);
		}
	}

	glfwDestroyWindow(window);

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}