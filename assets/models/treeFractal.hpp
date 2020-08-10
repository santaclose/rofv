#include "../src/modelTool/ml.h"
#include <iostream>
#include <math.h>
#include <vector>
#define MODEL_SET
#define BINDINGS
#define PI 3.14159265358979323846264

int steps = 3;
int cylinderRes = 6;
float initialRadius = 0.12;
float radiusFactor = 0.65;
float initialLength = 0.7;
float lengthFactor = 0.8;
float verticalAngle = PI / 8;

struct config
{
	vec p; // position
	vec o; // orientation
};

std::vector<std::vector<config>> configs;

void bindings()
{
	if (ImGui::SliderInt("Steps", &steps, 1, 20))haveToGenerateModel = true;
	if (ImGui::SliderInt("Cylinder resolution", &cylinderRes, 3, 32))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Initial radius", &initialRadius, 0.01, 1.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Radius factor", &radiusFactor, 0.2, 1.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Initial length", &initialLength, 0.01, 3.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Length factor", &lengthFactor, 0.2, 1.0))haveToGenerateModel = true;
	if (ImGui::SliderFloat("Vertical angle", &verticalAngle, 0.001, PI))haveToGenerateModel = true;
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
	vec posB = c.p + c.o.Normalized() * l;
	Cylinder(r, cylinderRes, c.p, posB, false);
}

void generateModel()
{
	configs.clear();

	float l = initialLength;
	float r = initialRadius;

	config ica, icb, icc; // initial configurations
	ica.p = icb.p = icc.p = vec::zero - vec::up;

	ica.o = vec(cos(verticalAngle), sin(verticalAngle), 0.0);

	vec h;
	h = vec(cos(2 * PI / 3.0), 0.0, sin(2 * PI / 3.0));
	icb.o = vec(0.0, sin(verticalAngle), 0.0) + h.Normalized() * cos(verticalAngle);

	h = vec(cos(4 * PI / 3.0), 0.0, sin(4 * PI / 3.0));
	icc.o = vec(0.0, sin(verticalAngle), 0.0) + h.Normalized() * cos(verticalAngle);

	generateAt(r, l, ica);
	generateAt(r, l, icb);
	generateAt(r, l, icc);

	configs.emplace_back();
	ica.p += ica.o.Normalized() * l;
	icb.p += icb.o.Normalized() * l;
	icc.p += icc.o.Normalized() * l;
	configs.back().push_back(ica);
	configs.back().push_back(icb);
	configs.back().push_back(icc);

	r *= radiusFactor;
	l *= lengthFactor;

	configs.emplace_back();

	for (int i = 1; i < steps; i++)
	{
		for (config& c : configs[i - 1])
		{
			//std::cout << "alasdfjalsd\n";
			config ca, cb, cc;
			ca.p = cb.p = cc.p = c.p;

			// cross products
			vec xRef = (c.o * vec::up).Normalized();
			vec yRef = c.o.Normalized();
			vec zRef = (c.o * xRef).Normalized();

			ca.o = xRef * cos(verticalAngle) + yRef * sin(verticalAngle);

			vec h;
			h = xRef * cos(2 * PI / 3.0) + zRef * sin(2 * PI / 3.0);
			cb.o = yRef * sin(verticalAngle) + h.Normalized() * cos(verticalAngle);

			h = xRef * cos(4 * PI / 3.0) + zRef * sin(4 * PI / 3.0);
			cc.o = yRef * sin(verticalAngle) + h.Normalized() * cos(verticalAngle);

			generateAt(r, l, ca);
			generateAt(r, l, cb);
			generateAt(r, l, cc);

			ca.p += ca.o.Normalized() * l;
			cb.p += cb.o.Normalized() * l;
			cc.p += cc.o.Normalized() * l;
			configs.back().push_back(ca);
			configs.back().push_back(cb);
			configs.back().push_back(cc);
		}

		r *= radiusFactor;
		l *= lengthFactor;
		configs.emplace_back();
	}
}