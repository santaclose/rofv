#include "../ml.h"
#include <iostream>
#include <math.h>
#define MODEL_SET
#define BINDINGS

float size = 5.0;
int vertexCount = 150;
float height = 0.05;
float frequency = 1.9;
float accent = 2;

float step;

float yFunc(float x, float z)
{
	return sin(frequency*(pow(x, accent) + pow(z, accent)));
}

void bindings()
{
	if (ImGui::SliderFloat("Size", &size, 0.01f, 8.0f))haveToGenerateModel = true;
	if (ImGui::SliderInt("Vertices by dim", &vertexCount, 2, 200))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Height", &height, 0.01f, 5.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Frequency", &frequency, 0.01f, 20.0f))haveToGenerateModel = true;
}

void generateModel()
{
	using namespace ml;

	step = size / vertexCount;

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
	}
}