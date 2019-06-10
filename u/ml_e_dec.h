int e_vertex(float x, float y, float z);
int e_vertex(v& pos);

void e_face(unsigned int* ids, int length);
void e_face(unsigned int* ids, int length, bool invert);
void e_face(unsigned int* ids, int length, int start);
void e_face(unsigned int* ids, int length, int start, bool invert);
void e_faceSeq(unsigned int* ids, int count, int vertsPerFace);