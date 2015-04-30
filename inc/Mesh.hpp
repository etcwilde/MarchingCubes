#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>

#include "implicit/ImplicitSystem.hpp"


typedef struct
{
	glm::vec3 p[3];
} TRIANGLE;

typedef struct
{
	unsigned int p[3];
	unsigned int n[3];
} INDEX_TRIANGLE;

class Mesh
{
public:
	Mesh(Implicit::Object& o, std::vector<TRIANGLE> tris);
	Mesh();

	void Generate(Implicit::Object& o, std::vector<TRIANGLE> tris);


	void AddFace(Implicit::Object& o, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	void AddFace(Implicit::Object& o, const TRIANGLE& tri);
	void AddFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3);

	void Export(std::string fname);
private:
	void build(Implicit::Object& o, std::vector<TRIANGLE> tris);
	std::vector<INDEX_TRIANGLE> m_triangles;
	std::vector<glm::vec3> m_verts;
	std::vector<glm::vec3> m_norms;
};

#endif//MESH_HPP
