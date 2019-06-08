#include "ml.h"

namespace ml
{
	/*struct vertexS
	{
		int id;
		v pos;
	};*/
	struct faceS
	{
		std::vector<int> verts;
		//v normal;
	};

	int vertexCounter = 0;
	//std::map <int, v> vertices;
	std::vector<v> vertices;

	void clearModel()
	{
		//vertices.clear();
		vertexCounter = 0;
		//faces.clear();
	}

	inline v calcNormal(faceS& theFace)
	{
		//v a = theFace.verts[1].pos - theFace.verts[0].pos;
		//v b = theFace.verts[2].pos - theFace.verts[1].pos;

		return (vertices[theFace.verts[1]] - vertices[theFace.verts[0]]) *
			(vertices[theFace.verts[2]] - vertices[theFace.verts[1]]);
		//theFace.normal = theFace.normal.Normalized();
	}

	void drawFace(faceS& theFace)
	{
		glBegin(GL_POLYGON);
		//glNormal3f(theFace.normal.x, theFace.normal.y, theFace.normal.z);
		v n = calcNormal(theFace);
		glNormal3f(n.x, n.y, n.z);
		for (int h : theFace.verts)
		{
			glVertex3f(vertices[h].x, vertices[h].y, vertices[h].z);
		}
		glEnd();
	}

	int vertex(float x, float y, float z)
	{
		if (vertices.size() <= vertexCounter)
			vertices.push_back(v(x, y, z));
		else
			vertices[vertexCounter] = v(x, y, z);

		vertexCounter++;
		return vertexCounter - 1;
		/*vertexS vtx;
		vtx.id = vertexCounter;
		vtx.pos.x = x;
		vtx.pos.y = y;
		vtx.pos.z = z;

		mapping.insert(std::pair<int, vertexS>(vertexCounter, vtx));

		vertexCounter++;
		return vertexCounter - 1;*/
	}

	int vertex(v pos)
	{
		if (vertices.size() <= vertexCounter)
			vertices.push_back(pos);
		else
			vertices[vertexCounter] = pos;

		vertexCounter++;
		return vertexCounter - 1;
		/*vertexS vtx;
		vtx.id = vertexCounter;
		vtx.pos.x = pos.x;
		vtx.pos.y = pos.y;
		vtx.pos.z = pos.z;

		mapping.insert(std::pair<int, vertexS>(vertexCounter, vtx));

		vertexCounter++;
		return vertexCounter - 1;*/
	}

	void face(int* ids, int length)
	{
		faceS fce;
		for (int i = 0; i < length; i++)
		{
			//fce.verts.push_back(mapping[ids[i]]);
			fce.verts.push_back(ids[i]);
		}
		//calcNormal(fce);
		drawFace(fce);
	}
	void face(int* ids, int length, bool invert)
	{
		faceS fce;
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				//fce.verts.push_back(mapping[ids[i]]);
				fce.verts.push_back(ids[i]);
			}
			//calcNormal(fce);
			drawFace(fce);
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				//fce.verts.push_back(mapping[ids[i]]);
				fce.verts.push_back(ids[i]);
			}
			//calcNormal(fce);
			drawFace(fce);
		}
	}
	void face(int* ids, int length, int start)
	{
		faceS fce;
		for (int i = 0; i < length; i++)
		{
			//fce.verts.push_back(mapping[ids[i + start]]);
			fce.verts.push_back(ids[i + start]);
		}
		//calcNormal(fce);
		drawFace(fce);
	}
	void face(int* ids, int length, int start, bool invert)
	{
		faceS fce;
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				//fce.verts.push_back(mapping[ids[i + start]]);
				fce.verts.push_back(ids[i + start]);
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				//fce.verts.push_back(mapping[ids[i + start]]);
				fce.verts.push_back(ids[i + start]);
			}
		}
		//calcNormal(fce);
		drawFace(fce);
	}
	void faceSeq(int* ids, int count, int vertsPerFace)
	{
		int length = count / vertsPerFace;
		faceS* fce = (faceS*)alloca(length * sizeof(faceS));

		int curFce = 0;

		for (int i = 0; i < count; i++)
		{
			//fce[curFce].verts.push_back(mapping[ids[i]]);
			fce[curFce].verts.push_back(ids[i]);
			if ((i + 1) % vertsPerFace == 0)
			{
				//calcNormal(fce[curFce]);
				drawFace(fce[curFce]);
				curFce++;
			}
		}
	}
}