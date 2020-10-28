#include "ml.h"
#include <Model.h>
#include <iostream>

namespace ml
{
	struct vertexS
	{
		vec pos;
		vec normal;
		float u;
		float v;

		vertexS(float x, float y, float z) { pos = vec(x, y, z); u = 0.0; v = 0.0; }
		vertexS(vec p) { pos = p; u = 0.0; v = 0.0; }
	};
	struct faceS
	{
		std::vector<unsigned int> verts;
		vec normal;
	};

	bool smoothMode = false;
	bool exporting = false;
	unsigned int lastDrawVertexCount = 0;

	std::string exportFileName = "assets/output/output.obj";
	bool e_fileOpen = false;
	bool e_objectCreated = false;
	std::ofstream e_output;

	unsigned int vertexCounter = 0;
	unsigned int facetedDrawingCounter = 0;
	unsigned int indexCounter = 0;

	std::vector<vertexS> facetedDrawing;
	std::vector<vertexS> vertices;
	std::vector<unsigned int> indexList;

	unsigned int facetedVAO;
	unsigned int facetedVBO;

	unsigned int smoothVAO;
	unsigned int smoothVBO;
	unsigned int smoothIBO;

	void Initialize()
	{
		glGenVertexArrays(1, &facetedVAO);
		glGenBuffers(1, &facetedVBO);
		glBindVertexArray(facetedVAO);
		glBindBuffer(GL_ARRAY_BUFFER, facetedVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*) 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*) offsetof(vertexS, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*) offsetof(vertexS, u));

		glGenVertexArrays(1, &smoothVAO);
		glGenBuffers(1, &smoothVBO);
		glGenBuffers(1, &smoothIBO);
		glBindVertexArray(smoothVAO);
		glBindBuffer(GL_ARRAY_BUFFER, smoothVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smoothIBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*)offsetof(vertexS, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertexS), (void*)offsetof(vertexS, u));

		glBindVertexArray(0);
	}
	void Terminate()
	{
		facetedDrawing.clear();
		indexList.clear();
		vertices.clear();
	}
	void SetExporting()
	{
		e_output.close();
		e_fileOpen = false;
		exporting = true;
	}
	bool SetSmoothMode(bool value)
	{
		if (smoothMode == value)
			return false;
		smoothMode = value;
		return true;
	}
	unsigned int GetLastDrawVertexCount()
	{
		return lastDrawVertexCount;
	}
	void GenerateModel(bool& generationPending)
	{
		// clear model
		vertexCounter = 0;
		indexCounter = 0;
		facetedDrawingCounter = 0;

		Model::GenerateModel();
		generationPending = false;

		lastDrawVertexCount = vertexCounter;
		if (exporting)
		{
			e_output.close();
			exporting = false;
		}
		
		if (smoothMode)
		{
			if (vertices.size() > 0 && indexList.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, smoothVBO);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ml::vertexS), &vertices[0], GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smoothIBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexList.size() * sizeof(unsigned int), &indexList[0], GL_DYNAMIC_DRAW);
			}
		}
		else
		{
			if (facetedDrawing.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, facetedVBO);
				glBufferData(GL_ARRAY_BUFFER, facetedDrawing.size() * sizeof(ml::vertexS), &facetedDrawing[0], GL_DYNAMIC_DRAW);
			}
		}
	}
	void DrawModel()
	{
		if (smoothMode)
		{
			if (vertices.size() > 0)
			{
				glBindVertexArray(smoothVAO);
				glDrawElements(GL_TRIANGLES, indexCounter, GL_UNSIGNED_INT, nullptr);
			}
		}
		else
		{
			if (facetedDrawing.size() > 0)
			{
				glBindVertexArray(facetedVAO);
				glDrawArrays(GL_TRIANGLES, 0, facetedDrawingCounter);
			}
		}
	}

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
		if (facetedDrawing.size() <= facetedDrawingCounter)
			facetedDrawing.push_back(newVertex);
		else
			facetedDrawing[facetedDrawingCounter] = newVertex;
		facetedDrawingCounter++;
	}
	inline void addVertexToVertices(vertexS& newVertex)
	{
		if (vertices.size() <= vertexCounter)
			vertices.push_back(newVertex);
		else
			vertices[vertexCounter] = newVertex;
		vertexCounter++;
	}
	inline vec calcNormal(faceS& theFace)
	{
		return ((vertices[theFace.verts[1]].pos - vertices[theFace.verts[0]].pos) *
			(vertices[theFace.verts[2]].pos - vertices[theFace.verts[1]].pos)).Normalized();
	}
	void generateFace(faceS& theFace) // adds the face to the buffer
	{
		vec normal = calcNormal(theFace);

		if (smoothMode)
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
			vertexS newVertex(vec::zero);
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
	bool isPointInTriangle(const vec& p, const vec& ta, const vec& tb, const vec& tc)
	{
		vec triangleNormal = (tb - ta) * (tc - ta);

		if (triangleNormal.Dot((tb - ta) * (p - ta)) < 0.0f)
			return false;
		if (triangleNormal.Dot((tc - tb) * (p - tb)) < 0.0f)
			return false;
		if (triangleNormal.Dot((ta - tc) * (p - tc)) < 0.0f)
			return false;
		return true;
	}

#include "ml_e_dec.h"

	unsigned int vertex(float x, float y, float z, float u, float v)
	{
		if (exporting)
			/*return */e_vertex(x, y, z);

		vertexS newVertex = vertexS(x, y, z);
		newVertex.u = u;
		newVertex.v = v;
		addVertexToVertices(newVertex);
		return vertexCounter - 1;
	}
	unsigned int vertex(const vec& pos, float u, float v)
	{
		if (exporting)
			/*return */e_vertex(pos);

		vertexS newVertex = vertexS(pos);
		newVertex.u = u;
		newVertex.v = v;
		addVertexToVertices(newVertex);
		return vertexCounter - 1;
	}
	const vec& getVertexPosition(unsigned int v)
	{
		return vertices[v].pos;
	}
	void concaveFace(unsigned int* ids, int length, bool invert)
	{
		// get winding vector and a vector with all the vertices
		vec winding = vec::zero;
		std::vector<unsigned int> points;
		for (int i = 0; i < length; i++)
		{
			int n = (i + 1) % length;
			int nn = (i + 2) % length;
			winding +=
				((vertices[ids[n]].pos - vertices[ids[i]].pos) *
				(vertices[ids[nn]].pos - vertices[ids[i]].pos)) *
				(invert ? -1.0f : 1.0f);

			points.push_back(ids[i]);
		}

		// ear clipping algorithm
		std::vector<unsigned int> triangles;
		sdf:
		for (int i = 0; i < points.size(); i++)
		{
			int n = (i + 1) % points.size();
			int nn = (i + 2) % points.size();
			vec v =
				((vertices[points[n]].pos - vertices[points[i]].pos) *
				(vertices[points[nn]].pos - vertices[points[i]].pos)) *
				(invert ? -1.0f : 1.0f);

			if (winding.Dot(v) > 0.0f) // the triangle winds in the right direction
			{
				bool thereIsAPointInside = false;
				for (unsigned int p : points) // check if the triangle contains a point inside
				{
					if (p == points[i] || p == points[n] || p == points[nn])
						continue;
					if (isPointInTriangle(vertices[p].pos, vertices[points[i]].pos, vertices[points[n]].pos, vertices[points[nn]].pos))
					{
						thereIsAPointInside = true;
						break;
					}
				}

				if (!thereIsAPointInside)
				{
					triangles.push_back(invert ? points[nn] : points[i]);
					triangles.push_back(points[n]);
					triangles.push_back(invert ? points[i] : points[nn]);
					points.erase(points.begin() + n);
					goto sdf;
				}
			}
		}
		faceSeq(triangles, 3);
	}
	void face(unsigned int* ids, int length)
	{
		if (exporting)
		{
			e_face(ids, length);
			//return;
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
			//return;
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
			//return;
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
			//return;
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
			//return;
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
	void faceSeq(std::vector<unsigned int>& vertices, int vertsPerFace, bool invert)
	{
		for (int i = 0; i < vertices.size(); i += vertsPerFace)
		{
			face(&(vertices[i]), vertsPerFace, invert);
			if (exporting)
				e_face(&(vertices[i]), vertsPerFace, invert);
		}
	}
	void setMaterial(const std::string& name)
	{
		if (exporting)
			e_setMaterial(name);
	}

	// exporting functions
	void e_checkObjectAndFile()
	{
		if (!e_fileOpen)
		{
			//e_output.open("output.ofv");
			e_output.open(exportFileName);
			if (!e_output.is_open()) std::cout << "could not open file " << exportFileName << std::endl;
			e_fileOpen = true;
		}
		if (!e_objectCreated)
		{
			//e_output << "o[object]\n";
			e_objectCreated = true;
		}
	}
	void e_vertex(float x, float y, float z)
	{
		e_checkObjectAndFile();
		//e_output << 'v' << vertexCounter << '[' << x << ',' << y << ',' << z << "]\n";
		e_output << "v " << x << ' ' << y << ' ' << z << '\n';
		//vertexCounter++;
		//return vertexCounter - 1;
	}
	void e_vertex(const vec& pos)
	{
		e_checkObjectAndFile();
		//e_output << 'v' << vertexCounter << '[' << pos.x << ',' << pos.y << ',' << pos.z << "]\n";
		e_output << "v " << pos.x << ' ' << pos.y << ' ' << pos.z << '\n';
		//vertexCounter++;
		//return vertexCounter - 1;
	}
	void e_face(unsigned int* ids, int length)
	{
		//e_output << "f[";
		e_output << "f ";
		for (int i = 0; i < length; i++)
		{
			e_output << ids[i] + 1;
			//e_output << 'v' << ids[i];
			if (i == length - 1)
			{
				e_output << '\n';
				//e_output << "]\n";
			}
			else
			{
				e_output << ' ';
				//e_output << ',';
			}
		}
	}
	void e_face(unsigned int* ids, int length, bool invert)
	{
		//e_output << "f[";
		e_output << "f ";
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				e_output << ids[i] + 1;
				//e_output << 'v' << ids[i];
				if (i == 0)
				{
					e_output << '\n';
					//e_output << "]\n";
				}
				else
				{
					e_output << ' ';
					//e_output << ',';
				}
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				e_output << ids[i] + 1;
				//e_output << 'v' << ids[i];
				if (i == length - 1)
				{
					e_output << '\n';
					//e_output << "]\n";
				}
				else
				{
					e_output << ' ';
					//e_output << ',';
				}
			}
		}
	}
	void e_face(unsigned int* ids, int length, int start)
	{
		//e_output << "f[";
		e_output << "f ";
		for (int i = 0; i < length; i++)
		{
			e_output << ids[i + start] + 1;
			//e_output << 'v' << ids[i + start];
			if (i == length - 1)
			{
				e_output << '\n';
				//e_output << "]\n";
			}
			else
			{
				e_output << ' ';
				//e_output << ',';
			}
		}
	}
	void e_face(unsigned int* ids, int length, int start, bool invert)
	{
		//e_output << "f[";
		e_output << "f ";
		if (invert)
		{
			for (int i = length - 1; i > -1; i--)
			{
				e_output << ids[i + start] + 1;
				//e_output << 'v' << ids[i + start];
				if (i == 0)
				{
					e_output << '\n';
					//e_output << "]\n";
				}
				else
				{
					e_output << ' ';
					//e_output << ',';
				}
			}
		}
		else
		{
			for (int i = 0; i < length; i++)
			{
				e_output << ids[i + start] + 1;
				//e_output << 'v' << ids[i + start];
				if (i == length - 1)
				{
					e_output << '\n';
					//e_output << "]\n";
				}
				else
				{
					e_output << ' ';
					//e_output << ',';
				}
			}
		}
	}
	void e_faceSeq(unsigned int* ids, int count, int vertsPerFace)
	{
		//e_output << "f[";
		e_output << "f ";
		for (int i = 0; i < count; i++)
		{
			e_output << 'v' << ids[i];
			if ((i + 1) % vertsPerFace == 0)
			{
				if (i == count - 1)
				{
					e_output << '\n';
					//e_output << "]\n";
				}
				else
				{
					e_output << "\nf ";
					//e_output << "]\nf[";
				}
			}
			else
			{
				e_output << ' ';
				//e_output << ',';
			}
		}
	}
	void e_setMaterial(const std::string& name)
	{
		e_checkObjectAndFile();
		e_output << "usemtl " << name << '\n';
	}
}