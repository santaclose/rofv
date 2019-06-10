#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vl.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace ml
{
	struct vertexS
	{
		::v pos;
		::v normal;
		float u;
		float v;

		vertexS(float x, float y, float z) { this->pos = ::v(x, y, z); }
		vertexS(::v p) { this->pos = p; }
		vertexS(float px, float py, float pz, float nx, float ny, float nz) { this->pos = ::v(px, py, pz); this->normal = ::v(nx, ny, nz); }
	};
	struct faceS
	{
		std::vector<unsigned int> verts;
		v normal;
	};

	void setExporting();
	void setUseVertexNormals(bool useVertexNormals);
	int getLastDrawVertexCount();

	void drawModel();
	void clearModel();
	void generateFace(faceS& theFace);
	inline v calcNormal(faceS& theFace);
	int vertex(float x, float y, float z);
	int vertex(v pos);
	void face(unsigned int* ids, int length);
	void face(unsigned int* ids, int length, bool invert);
	void face(unsigned int* ids, int length, int start);
	void face(unsigned int* ids, int length, int start, bool invert);
	void faceSeq(unsigned int* ids, int count, int vertsPerFace);
}