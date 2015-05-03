#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <thread> // Maybe
#include <future> // Maybe
#include <regex>

#include <string.h>

#include <glm/glm.hpp>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

//glNormal3f


// In General
// TODO: Figure out how to handle empty normals
// Answer: Calculate new normals based on cross-product, add that, and stick
// that in there
//
// In Import Method
//TODO: allow a user to select which object to use from a file
//TODO: Cleanup vertices on import
//TODO: Only insert verts, normals, and faces on good file



class Mesh
{
public:
	//DATASTRUCTURES
	typedef struct
	{
		glm::vec3 p[3];
		glm::vec3 n[3];
	} Triangle;

public:
	//METHODS
	Mesh();
	Mesh(Mesh& m);
	Mesh(const Mesh& m);
	Mesh(const std::vector<Triangle>& triangles);
	Mesh(const std::list<Triangle>& triangles);

	void operator=(const Mesh& m);
	Triangle operator[](unsigned int index);

	Triangle Get(unsigned int index);


	void AddFace(const Triangle& t);

	void AddFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3);

	unsigned int normals() const;
	unsigned int vertices() const;

	const std::vector<glm::vec3>& get_normals() const;
	const std::vector<glm::vec3>& get_vertices() const;
	const std::vector<Triangle> get_triangles() const;

	void Export(const std::string& fname);
	void Import(const std::string& fname);
	void Draw();

#ifdef DEBUG
	void Details();
#endif

protected:
	//DATASTRUCTURES
	typedef struct
	{
		unsigned int p[3];
		unsigned int n[3];
	} Index_Triangle;

protected:
	//METHODS

	void find_verts(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			unsigned int& index_1, unsigned int& index_2, unsigned int& index_3);

	void find_norms(const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3,
			unsigned int& index_1, unsigned int& index_2, unsigned int& index_3);

	// Repairs bad triangles
	void fix_norms(std::vector<unsigned int>& tris_to_fix);



protected:
	//VARIABLES

	// Todo: Profile the allocation time and the access time to see where
	// the bottle neck arises.
	// If bottleneck is reference time, change these to lists and use
	// iterators for linear
	std::vector<Index_Triangle> m_triangles;
	std::vector<glm::vec3> m_verts;
	std::vector<glm::vec3> m_norms;

private:
	//METHODS

	void draw_Triangle(const Index_Triangle& t);

private:
	//VARIABLES

};

#endif//MESH_HPP
