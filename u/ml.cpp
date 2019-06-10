#include "ml.h"

namespace ml
{
	bool exporting = false;
	bool e_fileOpen = false;
	bool e_objectCreated = false;
	std::ofstream e_output;

	bool calculateVertexNormals = true;

	unsigned int lastDrawVertexCount = 0;

	unsigned int vertexCounter = 0;
	unsigned int flatDrawingCounter = 0;
	unsigned int indexCounter = 0;

	std::vector<vertexS> flatDrawing; // duplicate vertices to create facets if not calculating vertex normals
	std::vector<vertexS> vertices;
	std::vector<unsigned int> indexList;

	inline void addIndex(unsigned int newIndex)
	{
		if (indexList.size() <= indexCounter)
			indexList.push_back(newIndex);
		else
			indexList[indexCounter] = newIndex;
		indexCounter++;
	}
	inline void addVertexToFlatDrawing(vertexS& newVertex)
	{
		if (flatDrawing.size() <= flatDrawingCounter)
			flatDrawing.push_back(newVertex);
		else
			flatDrawing[flatDrawingCounter] = newVertex;
		flatDrawingCounter++;
	}
	inline void addVertexToVertices(vertexS& newVertex)
	{
		if (vertices.size() <= vertexCounter)
			vertices.push_back(newVertex);
		else
			vertices[vertexCounter] = newVertex;
		vertexCounter++;
	}

	void destroyEverything()
	{
		flatDrawing.clear();
		indexList.clear();
		vertices.clear();
	}
	void setExporting()
	{
		exporting = true;
	}
	void setUseVertexNormals(bool useVertexNormals)
	{
		calculateVertexNormals = useVertexNormals;
	}
	int getLastDrawVertexCount()
	{
		return lastDrawVertexCount;
	}

	void drawModel()
	{
		if (calculateVertexNormals)
		{
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ml::vertexS), &vertices[0], GL_DYNAMIC_DRAW);						// update vertices
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexList.size() * sizeof(unsigned int), &indexList[0], GL_DYNAMIC_DRAW);		// update indices to draw

			glDrawElements(GL_TRIANGLES, /*indexList.size()*/indexCounter, GL_UNSIGNED_INT, nullptr);
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, flatDrawing.size() * sizeof(ml::vertexS), &flatDrawing[0], GL_DYNAMIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, /*flatDrawing.size()*/flatDrawingCounter);
		}
	}
	void clearModel()
	{
		lastDrawVertexCount = vertexCounter;
		vertexCounter = 0;
		indexCounter = 0;
		flatDrawingCounter = 0;

		if (exporting)
		{
			e_output.close();
			exporting = false;
		}
	}

	inline v calcNormal(faceS& theFace)
	{
		return ((vertices[theFace.verts[1]].pos - vertices[theFace.verts[0]].pos) *
			(vertices[theFace.verts[2]].pos - vertices[theFace.verts[1]].pos)).Normalized();
	}

	void generateFace(faceS& theFace) // adds the face to the buffer
	{
		v normal = calcNormal(theFace);

		if (calculateVertexNormals)
		{
			for (unsigned int theVertex : theFace.verts) // update all vertex normals of the face
			{
				vertices[theVertex].normal += normal;
				//vertices[theVertex].normal.Normalize();
			}
			for (int i = 2; i < theFace.verts.size(); i++) // triangulate
			{
				addIndex(theFace.verts[0]);
				addIndex(theFace.verts[i-1]);
				addIndex(theFace.verts[i]);
			}
		}
		else
		{
			vertexS newVertex(::v::zero);
			for (int i = 2; i < theFace.verts.size(); i++) // triangulate
			{
				newVertex = vertices[theFace.verts[0]];
				newVertex.normal = normal;				// set vertex normals equal to face normals
				addVertexToFlatDrawing(newVertex);
				newVertex = vertices[theFace.verts[i-1]];
				newVertex.normal = normal;
				addVertexToFlatDrawing(newVertex);
				newVertex = vertices[theFace.verts[i]];
				newVertex.normal = normal;
				addVertexToFlatDrawing(newVertex);
			}
		}
	}

#include "ml_e_dec.h"

	int vertex(float x, float y, float z)
	{
		if (exporting)
			return e_vertex(x, y, z);

		vertexS newVertex = vertexS(x, y, z);
		addVertexToVertices(newVertex);
		return vertexCounter - 1;
	}
	int vertex(::v& pos)
	{
		if (exporting)
			return e_vertex(pos);

		vertexS newVertex = vertexS(pos);
		addVertexToVertices(newVertex);
		return vertexCounter - 1;
	}
	int vertex(::v pos)
	{
		if (exporting)
			return e_vertex(pos);

		vertexS newVertex = vertexS(pos);
		addVertexToVertices(newVertex);
		return vertexCounter - 1;
	}
	
	void face(unsigned int* ids, int length)
	{
		if (exporting)
		{
			e_face(ids, length);
			return;
		}

		faceS fce;
		for (int i = 0; i < length; i++)
		{
			fce.verts.push_back(ids[i]);
		}
		generateFace(fce);
	}
	void face(unsigned int* ids, int length, bool invert)
	{
		if (exporting)
		{
			e_face(ids, length, invert);
			return;
		}

		faceS fce;
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				fce.verts.push_back(ids[i]);
			}
			generateFace(fce);
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				fce.verts.push_back(ids[i]);
			}
			generateFace(fce);
		}
	}
	void face(unsigned int* ids, int length, int start)
	{
		if (exporting)
		{
			e_face(ids, length, start);
			return;
		}

		faceS fce;
		for (int i = 0; i < length; i++)
		{
			fce.verts.push_back(ids[i + start]);
		}
		generateFace(fce);
	}
	void face(unsigned int* ids, int length, int start, bool invert)
	{
		if (exporting)
		{
			e_face(ids, length, start, invert);
			return;
		}

		faceS fce;
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				fce.verts.push_back(ids[i + start]);
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				fce.verts.push_back(ids[i + start]);
			}
		}
		generateFace(fce);
	}
	void faceSeq(unsigned int* ids, int count, int vertsPerFace)
	{
		if (exporting)
		{
			e_faceSeq(ids, count, vertsPerFace);
			return;
		}

		int length = count / vertsPerFace;
		faceS* fce = (faceS*)alloca(length * sizeof(faceS));

		int curFce = 0;

		for (int i = 0; i < count; i++)
		{
			fce[curFce].verts.push_back(ids[i]);
			if ((i + 1) % vertsPerFace == 0)
			{
				generateFace(fce[curFce]);
				curFce++;
			}
		}
	}

	// exporting functions
	void e_checkObjectAndFile()
	{
		if (!e_fileOpen)
		{
			e_output.open("output.ofv");
			e_fileOpen = true;
		}
		if (!e_objectCreated)
		{
			e_output << "o[object]\n";
			e_objectCreated = true;
		}
	}

	int e_vertex(float x, float y, float z)
	{
		e_checkObjectAndFile();
		e_output << 'v' << vertexCounter << '[' << x << ',' << y << ',' << z << "]\n";
		vertexCounter++;
		return vertexCounter - 1;
	}
	int e_vertex(v& pos)
	{
		e_checkObjectAndFile();
		e_output << 'v' << vertexCounter << '[' << pos.x << ',' << pos.y << ',' << pos.z << "]\n";
		vertexCounter++;
		return vertexCounter - 1;
	}

	void e_face(unsigned int* ids, int length)
	{
		e_output << "f[";
		for (int i = 0; i < length; i++)
		{
			e_output << 'v' << ids[i];
			if (i == length - 1)
			{
				e_output << "]\n";
			}
			else
			{
				e_output << ',';
			}
		}
	}
	void e_face(unsigned int* ids, int length, bool invert)
	{
		e_output << "f[";
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				e_output << 'v' << ids[i];
				if (i == 0)
				{
					e_output << "]\n";
				}
				else
				{
					e_output << ',';
				}
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				e_output << 'v' << ids[i];
				if (i == length - 1)
				{
					e_output << "]\n";
				}
				else
				{
					e_output << ',';
				}
			}
		}
	}
	void e_face(unsigned int* ids, int length, int start)
	{
		e_output << "f[";
		for (int i = 0; i < length; i++)
		{
			e_output << 'v' << ids[i + start];
			if (i == length - 1)
			{
				e_output << "]\n";
			}
			else
			{
				e_output << ',';
			}
		}
	}
	void e_face(unsigned int* ids, int length, int start, bool invert)
	{
		e_output << "f[";
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				e_output << 'v' << ids[i + start];
				if (i == 0)
				{
					e_output << "]\n";
				}
				else
				{
					e_output << ',';
				}
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				e_output << 'v' << ids[i + start];
				if (i == length - 1)
				{
					e_output << "]\n";
				}
				else
				{
					e_output << ',';
				}
			}
		}
	}
	void e_faceSeq(unsigned int* ids, int count, int vertsPerFace)
	{
		e_output << "f[";
		for (int i = 0; i < count; i++)
		{
			e_output << 'v' << ids[i];
			if ((i + 1) % vertsPerFace == 0)
			{
				if (i == count - 1)
				{
					e_output << "]\n";
				}
				else
				{
					e_output << "]\nf[";
				}
			}
			else
			{
				e_output << ',';
			}
		}
	}
}