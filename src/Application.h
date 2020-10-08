#pragma once
#include <imgui.h>

#define APPLICATION_NAME "rofv"
#define APPLICATION_WIDTH 1280
#define APPLICATION_HEIGHT 720
#define APPLICATION_DARK_STYLE

namespace Application
{
	void Init();
	void OnUpdate(const ImGuiIO& io);
	void Terminate();
}