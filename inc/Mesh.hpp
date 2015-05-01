#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

#include <glm/glm.hpp>

#include "implicit/ImplicitSystem.hpp"

#define THREADS 8


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
	Mesh();
	Mesh(Implicit::Object& o, std::vector<TRIANGLE> tris);
	Mesh(Mesh&);
	Mesh(const Mesh&);

	void operator = (const Mesh& m);


	/*
	Mesh operator= (const Mesh& m);
	const Mesh& Mesh::operator=(const Mesh& m)
	*/


	void Generate(Implicit::Object& o, std::vector<TRIANGLE> tris);
	void AddFace(Implicit::Object& o, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	void AddFace(Implicit::Object& o, const TRIANGLE& tri);
	void AddFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3);

	void Export(std::string fname);



private:
	void build(Implicit::Object& o, std::vector<TRIANGLE> tris);

	void find_verts(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			unsigned int* index_1, unsigned int* index_2, unsigned int* index_3);

	void find_norms(const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3,
			unsigned int* index_1, unsigned int* index_2, unsigned int* index_3);

	std::vector<INDEX_TRIANGLE> m_triangles;
	std::vector<glm::vec3> m_verts;
	std::vector<glm::vec3> m_norms;

	std::mutex m_verts_mux;
	std::mutex m_norms_mux;
	std::mutex m_tris_mux;
};

#endif//MESH_HPP
