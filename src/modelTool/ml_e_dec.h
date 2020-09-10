void e_vertex(float x, float y, float z);
void e_vertex(const vec& pos);

void e_face(unsigned int* ids, int length);
void e_face(unsigned int* ids, int length, bool invert);
void e_face(unsigned int* ids, int length, int start);
void e_face(unsigned int* ids, int length, int start, bool invert);
void e_faceSeq(unsigned int* ids, int count, int vertsPerFace);

void e_setMaterial(const std::string& name);