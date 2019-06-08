#include "ml.h"
#include <iostream>
#include <math.h>
#define MODEL_SET
using namespace ml;

float size = 5.0;
float step = 0.05;
float height = 0.05;
float frequency = 1.9;
float accent = 2;

float yFunc(float x, float z)
{
	return sin(frequency*(pow(x, accent) + pow(z, accent)));
}

void generateModel()
{
	v cursor = -v::f * size * 0.5 - v::r * size * 0.5;
	int quad[4];

	while (cursor.x < size * 0.5)
	{
		quad[0] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
		cursor += v::r * step;
		quad[1] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
		cursor += v::f * step;
		quad[2] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
		cursor -= v::r * step;
		quad[3] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
		face(quad, 4);

		while (cursor.z > -size * 0.5)
		{
			quad[0] = quad[3];
			quad[1] = quad[2];
			cursor += (v::r + v::f) * step;
			quad[2] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
			cursor -= v::r * step;
			quad[3] = vertex(cursor + v::u * height * yFunc(cursor.x, cursor.z));
			face(quad, 4);
		}
		cursor += v::r * step;
		cursor.z = size * 0.5;
	}
}