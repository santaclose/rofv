#include <glad/glad.h>
#include "WindowHelper.h"

int WindowHelper::width;
int WindowHelper::height;
double WindowHelper::aspectRatio;
double WindowHelper::lastMousePositionX;
double WindowHelper::lastMousePositionY;
double WindowHelper::dx;
double WindowHelper::dy;

void WindowHelper::Initialize(const int w, const int h)
{
	width = w;
	height = h;
	aspectRatio = (double)width / (double)height;
}

void WindowHelper::OnResize(GLFWwindow* window, int nWidth, int nHeight)
{
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	aspectRatio = (double)width / (double)height;
}
void WindowHelper::MouseMoved(double nX, double nY)
{
	dx = nX - lastMousePositionX;
	dy = nY - lastMousePositionY;

	lastMousePositionX = nX;
	lastMousePositionY = nY;
}