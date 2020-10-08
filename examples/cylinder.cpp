#include <Model.h>
#include <modelTool/ml.h>
#include <imgui.h>

float radius = 1.0f;
float height = 1.0f;
int steps = 8;
bool cap = true;

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

	face(capA, steps, true);
	face(capB, steps, false);
}

void Model::Bindings(bool& haveToGenerateModel)
{
	BIND(SliderFloat, "Radius", &radius, 0.01f, 3.0f);
	BIND(SliderFloat, "Height", &height, 0.05f, 2.0f);
	BIND(SliderInt, "Steps", &steps, 3, 20);
	BINDCHECKBOX(Checkbox, "Cap", &cap);
}
void Model::GenerateModel()
{
	Cylinder(radius, steps, -vec::up * 0.5f * height, vec::up * 0.5f * height, cap);
}