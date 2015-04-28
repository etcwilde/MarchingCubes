#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>


typedef struct
{
	glm::vec3 p[3];
} TRIANGLE;

typedef struct
{
	unsigned int p[3];
} INDEX_TRIANGLE;

class Mesh
{
public:
	Mesh(std::vector<TRIANGLE> tris);

	void AddFace(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
	void AddFace(const TRIANGLE& tri);

	void Export(std::string fname);
private:
	void build(std::vector<TRIANGLE> tris);
	std::vector<INDEX_TRIANGLE> m_triangles;
	std::vector<glm::vec3> m_verts;
};

#endif//MESH_HPP
