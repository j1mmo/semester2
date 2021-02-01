#pragma once
#include "GLSetup.h"
#include "RenderingContext.h"
#include <vector>
#include <string>

struct Vertex
{
	float xyz[3];
	float nxyz[3];
	int next, smoothinggroup;
};

struct Poly
{
	UINT indicies[3];
	int smoothinggroup;
};

class Object3D
{
public:
	static Object3D* loadObject(const wchar_t* filename);
	~Object3D();
	void createVBOs();
	void drawObject(RenderingContext& context);
private:
	UINT vbos[2];
	std::vector<Poly*> polygons;
	std::vector<Vertex*> verts;
	Object3D();

	template<typename arithmatic>
	void addHelper(const char* text, arithmatic* data, arithmatic ((*convFunction)(const char*)));
	void addVertex(const std::string& data);
	void addPolygon(const std::string& data, int smoothing);
	void processSmoothingGroups();
};

inline float atof_ex(const char* text) { return (float)atof(text); }
inline UINT atoi_ex(const char* text) { return (UINT)(atoi(text) - 1); }

inline void Object3D::drawObject(RenderingContext& context)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glVertexAttribPointer(context.poshandle, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(context.normalhandle, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glDrawElements(GL_TRIANGLES, 3 * polygons.size(), GL_UNSIGNED_INT, 0);
}
