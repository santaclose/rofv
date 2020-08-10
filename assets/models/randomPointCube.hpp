#include "../src/modelTool/ml.h"
#include <iostream>
#include <math.h>
#define MODEL_SET
#define BINDINGS

float cubeSideLength = 5.0;
int pointCount = 150;
int seed = 0;

float Random(float min, float max)
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void bindings()
{
	if (ImGui::SliderFloat("Side length", &cubeSideLength, 0.01f, 30.0f))haveToGenerateModel = true;
	if (ImGui::SliderInt("Point count", &pointCount, 1, 2160000))haveToGenerateModel = true;
	if (ImGui::SliderInt("Seed", &seed, 1, 10000))haveToGenerateModel = true;
}

void generateModel()
{
	using namespace ml;
	srand(seed);

	for (int i = 0; i < pointCount; i++)
	{
		vertex(vec(Random(cubeSideLength / -2.0, cubeSideLength / 2.0), Random(cubeSideLength / -2.0, cubeSideLength / 2.0), Random(cubeSideLength / -2.0, cubeSideLength / 2.0)));
	}

	/*step = size / vertexCount;

	vec cursor = -vec::forward * size * 0.5 - vec::right * size * 0.5;

	unsigned int quad[4];
	unsigned int* vertexMat = (unsigned int*)alloca(sizeof(unsigned int) * vertexCount * vertexCount);//[vertexCount][vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		for (int j = 0; j < vertexCount; j++)
		{
			vertexMat[vertexCount * i + j] = vertex(cursor + vec::up * height * yFunc(cursor.x, cursor.z), cursor.x * 0.1, cursor.z * 0.1);
			cursor += vec::forward * step;

			if (i > 0 && j > 0)
			{
				quad[0] = vertexMat[vertexCount * (i - 1) + j - 1];
				quad[1] = vertexMat[vertexCount * i + j - 1];
				quad[2] = vertexMat[vertexCount * i + j];
				quad[3] = vertexMat[vertexCount * (i - 1) + j];
				face(quad, 4);
			}
		}
		cursor += vec::right * step;
		cursor.z = size * 0.5;
	}*/
}