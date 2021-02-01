#include "Object3D.h"
#include <fstream>
#include <assert.h>

Object3D* Object3D::loadObject(const wchar_t* filename)
{
	int smoothing = 0;
	Object3D* obj = new Object3D();
	std::ifstream file(filename);
	if (file)
	{
		std::string line;
		std::getline(file, line);
		while (!file.eof())
		{
			if (line.length())
			{
				switch (line[0])
				{
				case 'v' :
				case 'V': obj->addVertex(line); break;

				case 'f':
				case 'F': obj->addPolygon(line, smoothing); break;

				case 's':
				case 'S': smoothing = atoi(line.c_str() + 1); break;
				}
			}
			std::getline(file, line);
		}
		file.close();
	}
	obj->processSmoothingGroups();
	return obj;
}

Object3D::Object3D()
{

}

Object3D::~Object3D()
{
	for (auto& i : verts)
	{
		delete i;
	}
	for (auto& i : polygons)
	{
		delete i;
	}
	verts.clear();
	polygons.clear();
}

template<typename arithmatic>
void Object3D::addHelper(const char* text, arithmatic* data, arithmatic ((*convFunction)(const char*)))
{
	int index = 0;
	data[index++] = (*convFunction)(text + 1);
	int start = 1;

	do
	{
		while (text[start] == ' '
			|| text[start] == '\t'
			|| text[start] == '-')
		{
			assert(start < strlen(text));
			start++;
		}
		while (text[start] != ' ' && text[start] != '\t')
		{
			assert(start < strlen(text));
			start++;
		}
		while (text[start] != ' ' && text[start] != '\t')
		{
			assert(start < strlen(text));
			start++;
		}
		data[index++] = (*convFunction)(text + start);
	} while (index < 3);
}

void Object3D::addVertex(const std::string& data)
{
	assert(data.length() && ((data[0] & 0xFFDF) == 'V'));

	Vertex* vertex = (Vertex*)malloc(sizeof(Vertex));
	if (vertex)
	{
		addHelper(data.c_str(), vertex->xyz, &atof_ex);
		vertex->next = -1;
		vertex->smoothinggroup = -1;
		verts.push_back(vertex);
	}
}
void Object3D::processSmoothingGroups()
{
	for (int i = 0; i < polygons.size(); i++)
	{
		Poly* poly = polygons[i];
		int group = poly->smoothinggroup;
		for (int j = 0; j < 3; j++)
		{
			int index = poly->indicies[j];
			Vertex* vertex = verts[index];
			if (vertex->smoothinggroup == 1)
			{
				vertex->smoothinggroup = group;
			} 
			else
			{
				while (vertex->smoothinggroup != group && vertex->next != -1)
				{
					index = vertex->next;
					vertex = verts[index];
				}
				if (vertex->smoothinggroup == group)
				{
					poly->indicies[j] = index;
				}
				else
				{
					Vertex* duplicate = (Vertex*)malloc(sizeof(Vertex));
					if (duplicate)
					{
						poly->indicies[j] = vertex->next = verts.size();

						memcpy(duplicate, vertex, sizeof(Vertex));
						duplicate->smoothinggroup = group;
						duplicate->next = -1;
						verts.push_back(duplicate);
					}
				}
			}
		}
	}
}

void Object3D::addPolygon(const std::string& data, int smoothing)
{
	assert(data.length() && ((data[0] & 0xFFDF) == 'F'));

	Poly * polygon = (Poly*)malloc(sizeof(Poly));
	if (polygon)
	{
		addHelper(data.c_str(), polygon->indicies, &atoi_ex);
		polygon->smoothinggroup = smoothing;
		polygons.push_back(polygon);
	}
}


void Object3D::createVBOs()
{
	int vertexSize = sizeof(float) * verts.size()*6;
	float* vertexData = (float*)malloc(vertexSize);

	int polySize = sizeof(int) * polygons.size() * 3;
	UINT* polyData = (UINT*)malloc(polySize);

	if (vertexData && polyData)
	{
		glGenBuffers(2, vbos);

		int offset = 0;
		for (int i = 0; i < verts.size(); i++, offset += 6)
		{
			memcpy(vertexData + offset, verts[i]->xyz, 3 * sizeof(float));
			memcpy(vertexData + offset + 3, verts[i]->nxyz, 3 * sizeof(float));
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);

		offset = 0;
		for (int i = 0; i < polygons.size(); i++, offset += 3)
		{
			memcpy(polyData + offset, polygons[i]->indicies, 3 * sizeof(float));
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, polySize, polyData, GL_STATIC_DRAW);

		free(vertexData);
		free(polyData);
	}
}
