#pragma once

#include <glad/glad.h>
#include "vl.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace ml
{
	struct vertexS
	{
		::vec pos;
		::vec normal;
		float u;
		float v;

		vertexS(float x, float y, float z) { pos = ::vec(x, y, z); }
		vertexS(::vec p) { pos = p; }
		//vertexS(float px, float py, float pz, float nx, float ny, float nz) { pos = ::v(px, py, pz); normal = ::v(nx, ny, nz); }
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
	inline ::vec calcNormal(faceS& theFace);
	unsigned int vertex(float x, float y, float z, float u = 0.0f, float v = 0.0f);
	unsigned int vertex(const ::vec& pos, float u = 0.0f, float v = 0.0f);
	//unsigned int vertexCp(::v pos, float u = 0.0f, float v = 0.0f);
	//unsigned int vertex(::vec&& pos, float u = 0.0f, float v = 0.0f);
	void face(unsigned int* ids, int length);
	void face(unsigned int* ids, int length, bool invert);
	void face(unsigned int* ids, int length, int start);
	void face(unsigned int* ids, int length, int start, bool invert);
	void faceSeq(unsigned int* ids, int count, int vertsPerFace);
}