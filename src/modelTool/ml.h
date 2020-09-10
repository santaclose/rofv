#pragma once

#include <glad/glad.h>
#include "vl.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>

#define BIND(imguiType, name, var, min, max) \
	if (ImGui::imguiType(name, var, min, max))haveToGenerateModel = true
#define BINDCHECKBOX(imguiType, name, var) \
	if (ImGui::imguiType(name, var))haveToGenerateModel = true

namespace ml
{
	struct vertexS
	{
		vec pos;
		vec normal;
		float u;
		float v;

		vertexS(float x, float y, float z) { pos = vec(x, y, z); u = 0.0; v = 0.0; }
		vertexS(vec p) { pos = p; u = 0.0; v = 0.0; }
	};
	struct faceS
	{
		std::vector<unsigned int> verts;
		vec normal;
	};

	void destroyEverything();
	void setExporting();
	void setUseVertexNormals(bool useVertexNormals);
	int getLastDrawVertexCount();

	void drawModel();
	void clearModel();
	void afterGenerate();

	void generateFace(faceS& theFace);
	inline vec calcNormal(faceS& theFace);

	unsigned int vertex(float x, float y, float z, float u = 0.0f, float v = 0.0f);
	unsigned int vertex(const vec& pos, float u = 0.0f, float v = 0.0f);
	const vec& getVertexPosition(unsigned int v);

	void concaveFace(unsigned int* ids, int length, bool invert = false);
	void face(unsigned int* ids, int length);
	void face(unsigned int* ids, int length, bool invert);
	void face(unsigned int* ids, int length, int start);
	void face(unsigned int* ids, int length, int start, bool invert);
	void faceSeq(unsigned int* ids, int count, int vertsPerFace);
	void faceSeq(std::vector<unsigned int>& vertices, int vertsPerFace, bool invert = false);

	void setMaterial(const std::string& name);
}