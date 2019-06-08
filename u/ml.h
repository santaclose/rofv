#pragma once

#include <GLFW/glfw3.h>
#include "vl.h"
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace ml
{
	struct vertexS;
	struct faceS;

	void clearModel();
	void drawFace(faceS& theFace);
	inline v calcNormal(faceS& theFace);
	int vertex(float x, float y, float z);
	int vertex(v pos);
	void face(int* ids, int length);
	void face(int* ids, int length, bool invert);
	void face(int* ids, int length, int start);
	void face(int* ids, int length, int start, bool invert);
	void faceSeq(int* ids, int count, int vertsPerFace);
}