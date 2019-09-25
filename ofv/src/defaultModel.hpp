
#define PI 3.14159265358979323846264
namespace defaultModel
{
	float radius = 2;
	float height = 2;
	int hResolution = 150;
	int vResolution = 60;
	int sides = 9;

	void bindings()
	{
		if (::ImGui::SliderFloat("Radius", &radius, 0.1f, 5.0f))
			::haveToGenerateModel = true;
		if (::ImGui::SliderFloat("Height", &height, 0.1f, 5.0f))
			::haveToGenerateModel = true;
		if(::ImGui::SliderInt("Sides", &sides, -30, 30))
			::haveToGenerateModel = true;
		if(::ImGui::SliderInt("H Resolution", &hResolution, 3, 300))
			::haveToGenerateModel = true;
		if(::ImGui::SliderInt("V Resolution", &vResolution, 3, 300))
			::haveToGenerateModel = true;
	}
	void generateModel()
	{
		using namespace ml;
		int realSides = sides + 1;
		float currentRadius = radius;
		float angleStep = 2 * PI / hResolution;
		float yStep = height / vResolution;

		float currentAngle = 0;
		float currentHeight = 0;

		//int vertices[vResolution][hResolution];
		unsigned int** vertices = (unsigned int**)alloca(sizeof(unsigned int*) * vResolution);
		for (int i = 0; i < vResolution; i++) vertices[i] = (unsigned int*)alloca(sizeof(unsigned int) * hResolution);

		unsigned int currentQuad[4];

		for (int i = 0; i < vResolution; i++)
		{
			currentAngle = 0;
			for (int j = 0; j < hResolution; j++)
			{
				vertices[i][j] = vertex((cos(currentAngle) - cos(currentAngle * realSides) / realSides) * currentRadius,
					currentHeight,
					(sin(currentAngle) - sin(currentAngle * realSides) / realSides) * currentRadius);

				if (j > 0 && i > 0)
				{
					currentQuad[0] = vertices[i - 1][j];
					currentQuad[1] = vertices[i][j];
					currentQuad[2] = vertices[i][j - 1];
					currentQuad[3] = vertices[i - 1][j - 1];
					face(currentQuad, 4);
				}
				currentAngle -= angleStep;
			}

			if (i > 0)
			{
				currentQuad[0] = vertices[i - 1][0];
				currentQuad[1] = vertices[i][0];
				currentQuad[2] = vertices[i][hResolution - 1];
				currentQuad[3] = vertices[i - 1][hResolution - 1];
				face(currentQuad, 4);
			}

			currentRadius /= 1.07;
			currentHeight -= yStep;
		}
	}
}