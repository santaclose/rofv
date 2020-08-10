#include "../src/modelTool/ml.h"
#define MODEL_SET
#define BINDINGS

int stepCount = 50;					//	[1,150]
float stairsWidth = 1;				//	0.001
float midSize = 1;					//	0.001
int stepsPerSegment = 15;

float stepSize = 0.15;				//	0.001
float stepHeight = 0.07;			//	0.001
float thickness = 0.2;				//	0.001

bool createRailings = true;
bool createSideCap = true;

float railingHOffset = 0.06;		//	0.001
float railingVOffset = -0.03;		//	0.001

float railingWidth = 0.05;			//	0.001
float railingThickness = 0.05;		//	0.001

int railingPipeEachXSteps = 3;		// [1,200]

float railingPipeHeight = 0.7;		//	0.001
int railingPipeRes = 1;				//	[1,50]
float railingPipeRadius = 0.01;		//	0.0001
float railingPipeTwistAngle = 0;
int railingPipePolygonSides = 32;

void bindings()
{
/*	if (ImGui::SliderInt("stepCount", &stepCount, 2, 200))haveToGenerateModel = true;
	if (ImGui::SliderFloat("stepHeight", &stepHeight, 0.01f, 8.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("stepAngle", &stepAngle, 0.001f, 5.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("innerRadius", &innerRadius, 0.01f, 20.0f))haveToGenerateModel = true;
	if (ImGui::SliderFloat("outerRadius", &outerRadius, 0.01f, 20.0f))haveToGenerateModel = true;*/
}

void createRailingSeg(unsigned int* array, bool invert)
{
	using namespace ml;
	unsigned int quad[4];
	if (invert)
	{
		quad[0] = array[4];
		quad[1] = array[7];
		quad[2] = array[3];
		quad[3] = array[0];
		face(quad, 4);

		quad[0] = array[6];
		quad[1] = array[2];
		quad[2] = array[3];
		quad[3] = array[7];
		face(quad, 4);

		quad[0] = array[2];
		quad[1] = array[6];
		quad[2] = array[5];
		quad[3] = array[1];
		face(quad, 4);

		quad[0] = array[4];
		quad[1] = array[0];
		quad[2] = array[1];
		quad[3] = array[5];
		face(quad, 4);
		return;
	}
	quad[0] = array[4];
	quad[1] = array[0];
	quad[2] = array[3];
	quad[3] = array[7];
	face(quad, 4);
	quad[0] = array[3];
	quad[1] = array[2];
	quad[2] = array[6];
	quad[3] = array[7];
	face(quad, 4);
	quad[0] = array[1];
	quad[1] = array[5];
	quad[2] = array[6];
	quad[3] = array[2];
	face(quad, 4);
	quad[0] = array[0];
	quad[1] = array[4];
	quad[2] = array[5];
	quad[3] = array[1];
	face(quad, 4);
}

void createTwistedRailingPipe(vec origin)
{
	/*float yStep = railingPipeHeight / railingPipeRes;

	int vertices[railingPipePolygonSides][railingPipeRes + 1];
	int quad[4];

	float angle = 0;

	for (int i = 0; i < railingPipePolygonSides; i++)
	{
		float currentAngle = angle;
		float currentHeight = 0;
		float angleStep = railingPipeTwistAngle / railingPipeRes;

		for (int j = 0; j < railingPipeRes + 1; j++)
		{
			vertices[i][j] = vertex(origin + r * cos(currentAngle) * railingPipeRadius - b * sin(currentAngle) * railingPipeRadius + vec::up * currentHeight);

			if (i > 0 && j > 0)
			{
				quad[0] = vertices[i - 1][j - 1];
				quad[1] = vertices[i][j - 1];
				quad[2] = vertices[i][j];
				quad[3] = vertices[i - 1][j];
				face(quad, 4);
			}

			currentAngle += angleStep;
			currentHeight += yStep;
		}


		angle += pi * 2 / railingPipePolygonSides;
	}

	for (int j = 0; j < railingPipeRes + 1; j++)
	{
		if (j == 0)
			continue;

		quad[0] = vertices[railingPipePolygonSides - 1][j - 1];
		quad[1] = vertices[0][j - 1];
		quad[2] = vertices[0][j];
		quad[3] = vertices[railingPipePolygonSides - 1][j];
		face(quad, 4);
	}*/
}

void createUshapeBasis(vec origin, vec dir, int steps, float stairsWidth, float midSize, int stepsPerSegment)
{
	using namespace ml;
	dir.y = 0;
	vec localRight = dir * vec::up;
	dir.Normalize();
	localRight.Normalize();

	unsigned int quad[4];
	unsigned int thQuad[4];
	unsigned int sideTri[3];

	quad[0] = vertex(origin);
	quad[1] = vertex(origin + localRight * stairsWidth);

	thQuad[0] = vertex(origin - vec::up * thickness);
	thQuad[1] = vertex(origin + localRight * stairsWidth - vec::up * thickness);

	for (int i = 1; i <= steps; i++)
	{
		origin += vec::up * stepHeight;
		quad[2] = vertex(origin + localRight * stairsWidth);
		quad[3] = vertex(origin);

		sideTri[2] = quad[1];
		int leftSideTemp = quad[0];

		face(quad, 4);

		quad[0] = quad[3];
		quad[1] = quad[2];

		origin += dir * stepSize;
		quad[2] = vertex(origin + localRight * stairsWidth);
		quad[3] = vertex(origin);

		thQuad[2] = vertex(origin + localRight * stairsWidth - vec::up * thickness);
		thQuad[3] = vertex(origin - vec::up * thickness);

		face(quad, 4);
		face(thQuad, 4, true);

		//right side

		sideTri[0] = thQuad[1];
		sideTri[1] = thQuad[2];
		face(sideTri, 3);

		sideTri[0] = sideTri[1];
		sideTri[1] = quad[2];
		face(sideTri, 3);

		sideTri[0] = sideTri[1];
		sideTri[1] = quad[1];
		face(sideTri, 3);

		//left side

		if (createSideCap)
		{
			sideTri[2] = leftSideTemp;
			sideTri[0] = thQuad[0];
			sideTri[1] = thQuad[3];
			face(sideTri, 3, true);

			sideTri[0] = sideTri[1];
			sideTri[1] = quad[3];
			face(sideTri, 3, true);

			sideTri[0] = sideTri[1];
			sideTri[1] = quad[0];
			face(sideTri, 3, true);
		}

		if (i % stepsPerSegment == 0 && i != steps) // mid
		{
			unsigned int midVertices[18];
			midVertices[1] = thQuad[3];
			midVertices[13] = quad[3];

			midVertices[10] = midVertices[12] = vertex(origin + dir * midSize);
			midVertices[2] = midVertices[6] = vertex(origin + dir * midSize - vec::up * thickness);

			midVertices[14] = midVertices[11] = midVertices[16] = quad[2];
			midVertices[0] = midVertices[5] = midVertices[8] = thQuad[2];

			origin += localRight * stairsWidth * 2;
			localRight = -localRight;
			dir = -dir;
			quad[0] = vertex(origin);
			quad[1] = quad[2]; //vertex(origin + localRight * stairsWidth);
			thQuad[0] = vertex(origin - vec::up * thickness);
			thQuad[1] = thQuad[2]; //vertex(origin + localRight * stairsWidth - vec::up * thickness);

			midVertices[17] = quad[0];
			midVertices[4] = thQuad[0];

			midVertices[15] = midVertices[9] = vertex(origin - dir * midSize);
			midVertices[7] = midVertices[3] = vertex(origin - dir * midSize - vec::up * thickness);

			faceSeq(midVertices, 18, 3);

			if (createSideCap)
			{
				unsigned int midCapVertices[12];

				midCapVertices[0] = midVertices[1];
				midCapVertices[1] = midVertices[13];
				midCapVertices[3] = midCapVertices[6] = midVertices[2];
				midCapVertices[2] = midCapVertices[7] = midVertices[10];

				midCapVertices[8] = midVertices[17];
				midCapVertices[9] = midVertices[4];
				midCapVertices[4] = midCapVertices[11] = midVertices[15];
				midCapVertices[5] = midCapVertices[10] = midVertices[7];

				faceSeq(midCapVertices, 12, 4);
			}
		}
		else
		{
			quad[0] = quad[3];
			quad[1] = quad[2];
			thQuad[0] = thQuad[3];
			thQuad[1] = thQuad[2];
		}
	}
}

void placeUshapeRailingPipes(vec origin, vec dir, int steps, float stairsWidth, float midSize, int stepsPerSegment)
{
	dir.y = 0;
	vec localRight = dir * vec::up;
	dir.Normalize();
	localRight.Normalize();

	origin += vec::up * stepHeight + dir * stepSize * 0.5;

	for (int i = 1; i <= steps; i++)
	{
		if (i % railingPipeEachXSteps == 0 || (i - 1) % stepsPerSegment == 0)
		{
			createTwistedRailingPipe(origin + localRight * railingHOffset);
			createTwistedRailingPipe(origin + localRight * (stairsWidth - railingHOffset));
		}

		if (i % stepsPerSegment == 0 && i != steps) // mid
		{
			createTwistedRailingPipe(origin + localRight * railingHOffset + dir * (stepSize * 0.5 + midSize - railingHOffset));
			createTwistedRailingPipe(origin + localRight * (stairsWidth * 2 - railingHOffset) + dir * (stepSize * 0.5 + midSize - railingHOffset));

			origin += localRight * stairsWidth * 2 + vec::up * stepHeight;
			localRight = -localRight;
			dir = -dir;
		}
		else
		{
			origin += vec:: up* stepHeight + dir * stepSize;
		}
	}
}

void createOuterRailing(vec origin, vec dir, int steps, float stairsWidth, float midSize, int stepsPerSegment)
{
	using namespace ml;
	dir.y = 0;
	vec localRight = dir * vec::up;
	dir.Normalize();
	localRight.Normalize();

	int segments = (steps - 1) / stepsPerSegment;
	int lastSegmentSteps = (steps - 1) % stepsPerSegment + 1;
	//cout << segments << ", " << lastSegmentSteps << endl;

	vec cursor = origin + vec::up * (stepHeight * 0.5 + railingPipeHeight + railingVOffset) + localRight * (railingHOffset - railingWidth * 0.5);

	unsigned int railingSeg[8];
	railingSeg[0] = vertex(cursor);
	railingSeg[1] = vertex(cursor + localRight * railingWidth);
	railingSeg[2] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
	railingSeg[3] = vertex(cursor + vec::up * railingThickness);
	face(railingSeg, 4);

	vec stepDisplacement = vec::up * stepHeight + dir * stepSize;

	for (int i = 0; i < segments; i++)
	{
		cursor += stepDisplacement * (stepsPerSegment - 0.5);
		railingSeg[4] = vertex(cursor);
		railingSeg[5] = vertex(cursor + localRight * railingWidth);
		railingSeg[6] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
		railingSeg[7] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, false);

		cursor += dir * (midSize + stepSize * 0.5 - railingHOffset + railingWidth * 0.5);
		railingSeg[0] = vertex(cursor);
		railingSeg[1] = vertex(cursor - dir * railingWidth + localRight * railingWidth);
		railingSeg[2] = vertex(cursor - dir * railingWidth + localRight * railingWidth + vec::up * railingThickness);
		railingSeg[3] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, true);

		cursor += localRight * (railingWidth - railingHOffset + stairsWidth * 2 - railingHOffset);
		railingSeg[4] = vertex(cursor);
		railingSeg[5] = vertex(cursor - localRight * railingWidth - dir * railingWidth);
		railingSeg[6] = vertex(cursor - localRight * railingWidth - dir * railingWidth + vec::up * railingThickness);
		railingSeg[7] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, false);

		cursor += dir * (-railingWidth * 0.5 + railingHOffset - midSize + stepSize * 0.5);
		railingSeg[0] = vertex(cursor);
		railingSeg[1] = vertex(cursor - localRight * railingWidth);
		railingSeg[2] = vertex(cursor - localRight * railingWidth + vec::up * railingThickness);
		railingSeg[3] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, true);

		stepDisplacement.x *= -1;
		stepDisplacement.z *= -1;

		cursor += stepDisplacement * 0.5;
		railingSeg[4] = vertex(cursor);
		railingSeg[5] = vertex(cursor - localRight * railingWidth);
		railingSeg[6] = vertex(cursor - localRight * railingWidth + vec::up * railingThickness);
		railingSeg[7] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, false);

		dir = -dir;
		localRight = -localRight;
		railingSeg[0] = railingSeg[4];
		railingSeg[1] = railingSeg[5];
		railingSeg[2] = railingSeg[6];
		railingSeg[3] = railingSeg[7];
		//vertex(cursor);
	}

	cursor += stepDisplacement * (lastSegmentSteps);
	railingSeg[4] = vertex(cursor);
	railingSeg[5] = vertex(cursor + localRight * railingWidth);
	railingSeg[6] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
	railingSeg[7] = vertex(cursor + vec::up * railingThickness);
	createRailingSeg(railingSeg, false);

	face(railingSeg, 4, 4, true); //cap
}
void createInnerRailing(vec origin, vec dir, int steps, float stairsWidth, float midSize, int stepsPerSegment)
{
	using namespace ml;
	dir.y = 0;
	vec localRight = dir * vec::up;
	dir.Normalize();
	localRight.Normalize();

	int segments = (steps - 1) / stepsPerSegment;
	int lastSegmentSteps = (steps - 1) % stepsPerSegment + 1;
	//cout << segments << ", " << lastSegmentSteps << endl;

	vec cursor = origin + vec::up * (stepHeight * 0.5 + railingPipeHeight + railingVOffset) + localRight * (stairsWidth - railingHOffset - railingWidth * 0.5);

	unsigned int railingSeg[8];
	railingSeg[0] = vertex(cursor);
	railingSeg[1] = vertex(cursor + localRight * railingWidth);
	railingSeg[2] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
	railingSeg[3] = vertex(cursor + vec::up * railingThickness);
	face(railingSeg, 4);

	vec stepDisplacement = vec::up * stepHeight + dir * stepSize;

	for (int i = 0; i < segments; i++)
	{
		cursor += stepDisplacement * (stepsPerSegment);
		railingSeg[4] = vertex(cursor);
		railingSeg[5] = vertex(cursor + localRight * railingWidth);
		railingSeg[6] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
		railingSeg[7] = vertex(cursor + vec::up * railingThickness);
		createRailingSeg(railingSeg, false);
		face(railingSeg, 4, 4, true);

		cursor += localRight * (railingHOffset * 2 + railingWidth);

		stepDisplacement.x *= -1;
		stepDisplacement.z *= -1;

		dir = -dir;
		localRight = -localRight;

		railingSeg[0] = vertex(cursor);
		railingSeg[1] = vertex(cursor + localRight * railingWidth);
		railingSeg[2] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
		railingSeg[3] = vertex(cursor + vec::up * railingThickness);
		face(railingSeg, 4);
		//vertex(cursor);
	}

	cursor += stepDisplacement * (lastSegmentSteps);
	railingSeg[4] = vertex(cursor);
	railingSeg[5] = vertex(cursor + localRight * railingWidth);
	railingSeg[6] = vertex(cursor + localRight * railingWidth + vec::up * railingThickness);
	railingSeg[7] = vertex(cursor + vec::up * railingThickness);
	createRailingSeg(railingSeg, false);

	face(railingSeg, 4, 4, true); //cap
}

void ushape(vec origin, vec dir, int steps, float stairsWidth, float midSize, int stepsPerSegment)
{
	createUshapeBasis(origin, dir, steps, stairsWidth, midSize, stepsPerSegment);

	/*if (createRailings)
	{
		placeUshapeRailingPipes(origin, dir, steps, stairsWidth, midSize, stepsPerSegment);
		createOuterRailing(origin, dir, steps, stairsWidth, midSize, stepsPerSegment);
		createInnerRailing(origin, dir, steps, stairsWidth, midSize, stepsPerSegment);
	}*/
}

void generateModel()
{
	ushape(vec::zero, vec::forward, stepCount, stairsWidth, midSize, stepsPerSegment);
}