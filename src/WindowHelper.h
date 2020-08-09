#pragma once

#include "GLFW/glfw3.h"

class WindowHelper
{
public:
	static int width;
	static int height;
	static double aspectRatio;
	static double lastMousePositionX;
	static double lastMousePositionY;
	static double dx;
	static double dy;

	static void Initialize(const int w, const int h);
	static void OnResize(GLFWwindow* window, int nWidth, int nHeight);
	static void MouseMoved(double nX, double nY);
};