#include "../src/modelTool/ml.h"
#include <iostream>
#include <math.h>
#include <vector>
#define MODEL_SET
#define BINDINGS
#define PI 3.14159265358979323846264

int steps = 3;
int cylinderRes = 3;
float initialRadius = 0.12;
float radiusFactor = 0.65;
float initialLength = 0.7;
float lengthFactor = 0.8;

const vec SOA = vec::up;
const vec SOB = vec::right;
const vec SOC = vec::forward;

const vec DOA = (vec::up + vec::right + vec::forward).Normalized();
const vec DOB = (vec::up - vec::right + vec::forward).Normalized();
const vec DOC = (vec::up + vec::right - vec::forward).Normalized();
const vec DOD = (vec::up - vec::right - vec::forward).Normalized();

struct config
{
	vec p; // position
	bool o; // orientation
};

std::vector<std::vector<config>> configs;

void bindings()
{
	if (ImGui::SliderInt("Steps", &steps, 1, 9))haveToGenerateModel = true;
	if (ImGui::SliderInt("Cylinder resolution", &cylinderRes, 3, 32))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Initial radius", &initialRadius, 0.01, 1.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Radius factor", &radiusFactor, 0.2, 1.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Initial length", &initialLength, 0.01, 3.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Length factor", &lengthFactor, 0.2, 1.0))haveToGenerateModel = true;
}

void Cylinder(float radius, int steps, const vec& posA, const vec& posB, bool cap = true)
{
	using namespace ml;
	vec dir = (posB - posA).Normalized();
	dir.Normalize();
	float test = dir.Dot(vec::up);
	vec localRight = abs(test) == 1.0 ? vec::right : (dir * vec::up).Normalized();
	vec localForward = (dir * localRight).Normalized();

	float angleStep = 2.0 * PI / (double)steps;
	unsigned int quad[4];
	unsigned int* vertices = (unsigned int*)alloca(sizeof(unsigned int) * steps * 2);

	unsigned int* capA = (unsigned int*)alloca(sizeof(unsigned int) * steps);
	unsigned int* capB = (unsigned int*)alloca(sizeof(unsigned int) * steps);

	for (int i = 0; i < steps; i++)
	{
		vertices[i * 2] = capA[i] = vertex(posA + localRight * cos(angleStep * i) * radius + localForward * sin(angleStep * i) * radius);
		vertices[i * 2 + 1] = capB[i] = vertex(posB + localRight * cos(angleStep * i) * radius + localForward * sin(angleStep * i) * radius);

		if (i > 0)
		{
			quad[0] = vertices[i * 2 - 2];
			quad[1] = vertices[i * 2];
			quad[2] = vertices[i * 2 + 1];
			quad[3] = vertices[i * 2 - 1];
			face(quad, 4);
		}
	}
	quad[0] = quad[1];
	quad[3] = quad[2];
	quad[1] = vertices[0];
	quad[2] = vertices[1];
	face(quad, 4);

	if (!cap)
		return;

	face(capA, steps);
	face(capB, steps, true);
}


void generateAt(float r, float l, const config& c)
{
	vec posA, posB;
	config newConfigs;
	newConfigs.o = c.o;//!c.o;
	if (!c.o)
	{
		posA = c.p + SOA * l * 0.5;
		posB = c.p - SOA * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);

		posA = c.p + SOB * l * 0.5;
		posB = c.p - SOB * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);

		posA = c.p + SOC * l * 0.5;
		posB = c.p - SOC * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);
	}
	else
	{
		posA = c.p + DOA * l * 0.5;
		posB = c.p - DOA * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);

		posA = c.p + DOB * l * 0.5;
		posB = c.p - DOB * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);

		posA = c.p + DOC * l * 0.5;
		posB = c.p - DOC * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);

		posA = c.p + DOD * l * 0.5;
		posB = c.p - DOD * l * 0.5;
		Cylinder(r, cylinderRes, posA, posB, false);
		newConfigs.p = posA; configs.back().push_back(newConfigs);
		newConfigs.p = posB; configs.back().push_back(newConfigs);
	}
}

void generateModel()
{
	configs.clear();

	float l = initialLength;
	float r = initialRadius;

	config ic; // initial configuration
	ic.p = vec::zero;
	ic.o = true; // normal orientation

	configs.emplace_back();
	generateAt(r, l, ic);

	r *= radiusFactor;
	l *= lengthFactor;

	configs.emplace_back();

	for (int i = 1; i < steps; i++)
	{
		for (config& c : configs[i - 1])
		{
			generateAt(r, l, c);
		}

		r *= radiusFactor;
		l *= lengthFactor;
		configs.emplace_back();
	}
}