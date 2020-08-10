#include "../src/modelTool/ml.h"
#define MODEL_SET
#define BINDINGS
#define pi 3.14159265358979323846264

int stepCount = 50;					//	[1,150]

float stepHeight = 0.07;			//	0.001
float stepAngle = 2 * pi / 30;		//	0.001
float thickness = 0.2;				//	0.001
float innerRadius = 1;
float outerRadius = 2.8;

void bindings()
{
	if (ImGui::SliderInt("stepCount", &stepCount, 2, 200))haveToGenerateModel = true;
	if (ImGui::SliderFloat("stepHeight", &stepHeight, 0.01f, 8.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("stepAngle", &stepAngle, 0.001f, 5.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("innerRadius", &innerRadius, 0.01f, 20.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("outerRadius", &outerRadius, 0.01f, 20.0f))haveToGenerateModel = true;
}

void spiral(vec origin)
{
	using namespace ml;
	float currentAngle = 0;

	unsigned int outerSide[7]; //tris
	unsigned int innerSide[7]; //tris
	unsigned int bottom[4]; // quads
	unsigned int vertices[4]; //quads

	vec pos(cos(currentAngle) * innerRadius,
		0,
		sin(currentAngle) * innerRadius);

	vertices[0] = vertex(origin + pos);
	innerSide[1] = innerSide[3] = innerSide[5] = vertices[0];
	bottom[0] = vertex(origin + pos - vec::up * thickness);
	innerSide[0] = bottom[0];
	pos.x = cos(currentAngle) * outerRadius;
	pos.z = sin(currentAngle) * outerRadius;
	vertices[1] = vertex(origin + pos);
	outerSide[6] = outerSide[2] = vertices[1];
	bottom[3] = vertex(origin + pos - vec::up * thickness);
	outerSide[0] = bottom[3];

	for (int i = 0; i < stepCount; i++)
	{
		pos += vec::up * stepHeight;
		vertices[2] = vertex(origin + pos);
		outerSide[5] = vertices[2];
		pos.x = cos(currentAngle) * innerRadius;
		pos.z = sin(currentAngle) * innerRadius;
		vertices[3] = vertex(origin + pos);
		innerSide[6] = vertices[3];
		face(vertices, 4);

		currentAngle -= stepAngle;
		vertices[0] = vertices[3];
		vertices[1] = vertices[2];
		pos.x = cos(currentAngle) * outerRadius;
		pos.z = sin(currentAngle) * outerRadius;
		vertices[2] = vertex(origin + pos);
		bottom[2] = vertex(origin + pos - vec::up * thickness);
		outerSide[1] = outerSide[3] = bottom[2];
		outerSide[4] = vertices[2];
		pos.x = cos(currentAngle) * innerRadius;
		pos.z = sin(currentAngle) * innerRadius;
		vertices[3] = vertex(origin + pos);
		innerSide[4] = vertices[3];
		bottom[1] = vertex(origin + pos - vec::up * thickness);
		innerSide[2] = bottom[1];
		face(vertices, 4);
		face(bottom, 4);
		face(outerSide, 3);
		face(outerSide, 3, 2);
		face(outerSide, 3, 4);
		face(innerSide, 3);
		face(innerSide, 3, 2);
		face(innerSide, 3, 4);

		vertices[0] = vertices[3];
		vertices[1] = vertices[2];

		bottom[0] = bottom[1];
		bottom[3] = bottom[2];

		outerSide[6] = outerSide[2] = outerSide[4];
		outerSide[0] = outerSide[1];

		innerSide[0] = innerSide[2];
		innerSide[1] = innerSide[3] = innerSide[5] = innerSide[4];

		pos.x = cos(currentAngle) * outerRadius;
		pos.z = sin(currentAngle) * outerRadius;
	}
}


void generateModel()
{
	spiral(vec::zero);
}