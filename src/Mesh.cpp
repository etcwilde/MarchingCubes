#include "Mesh.hpp"
Mesh::Mesh()
{
}

Mesh::Mesh(Implicit::Object& o, std::vector<TRIANGLE> tris)
{
	build(o, tris);
}

Mesh::Mesh(Mesh& m) :
	m_triangles(m.m_triangles),
	m_verts(m.m_verts),
	m_norms(m.m_norms)
{ }

Mesh::Mesh(const Mesh& m) :
	m_triangles(m.m_triangles),
	m_verts(m.m_verts),
	m_norms(m.m_norms)
{ }


void Mesh::operator=(const Mesh& m)
{
	m_triangles = m.m_triangles;
	m_verts = m.m_verts;
	m_norms = m.m_norms;
}


void Mesh::Generate(Implicit::Object& o, std::vector<TRIANGLE> tris)
{
	build(o, tris);
}

void Mesh::AddFace(Implicit::Object& O, const TRIANGLE& tri)
{
	AddFace(tri.p[0], tri.p[1], tri.p[2],
			O.Normal(tri.p[0]), O.Normal(tri.p[1]),
			O.Normal(tri.p[2]));
}

void Mesh::AddFace(Implicit::Object& O, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	AddFace(v1, v2, v3, O.Normal(v1), O.Normal(v2), O.Normal(v3));

}

void Mesh::AddFace(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3)
{
	if (v1 == v2) return;
	if (v1 == v3) return;
	if (v2 == v3) return;
	INDEX_TRIANGLE t;
	find_verts(v1, v2, v3, &t.p[0], &t.p[1], &t.p[2]);
	find_norms(n1, n2, n3, &t.n[0], &t.n[1], &t.n[2]);
	m_triangles.push_back(t);
}


void Mesh::build(Implicit::Object& o, std::vector<TRIANGLE> tris)
{
	// For each triangle

	std::cout << "Building Geometry...\n";
	auto begin = std::chrono::high_resolution_clock::now();
	for (auto tri = tris.begin(); tri != tris.end(); tri++)
	{
		AddFace(o, *tri);
		std::cout << "Triangles: " << m_triangles.size() << "/" << tris.size() << '\r' << std::flush;
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "\nBuild Time: " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
		<< "ms\n";
}


void Mesh::find_verts(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3,
			unsigned int* index_1, unsigned int* index_2, unsigned int* index_3)
{
	bool v1_found = false;
	bool v2_found = false;
	bool v3_found = false;



	m_verts_mux.lock();
	for (unsigned int index = 0; index < m_verts.size(); index++)
	{
		if (!v1_found && v1 == m_verts[index])
		{
			*index_1 = index + 1;
			v1_found = true;
		}
		if (!v2_found && v2 == m_verts[index])
		{
			*index_2 = index + 1;
			v2_found = true;
		}
		if (!v3_found && v3 == m_verts[index])
		{
			*index_3 = index + 1;
			v3_found = true;
		}
		if (v1_found && v2_found && v3_found) break;
	}

	if (!v1_found)
	{
		m_verts.push_back(v1);
		*index_1 = m_verts.size();
	}
	if(!v2_found)
	{
		m_verts.push_back(v2);
		*index_2 = m_verts.size();
	}
	if(!v3_found)
	{
		m_verts.push_back(v3);
		*index_3 = m_verts.size();
	}
	m_verts_mux.unlock();
}

void Mesh::find_norms(const glm::vec3& n1, const glm::vec3& n2, const glm::vec3& n3,
			unsigned int* index_1, unsigned int* index_2, unsigned int* index_3)
{
	bool n1_found = false;
	bool n2_found = false;
	bool n3_found = false;
	m_norms_mux.lock();
	for(unsigned int index = 0; index < m_norms.size(); index++)
	{
		if (!n1_found && n1 == m_norms[index])
		{
			*index_1 = index + 1;
			n1_found = true;
		}
		if (!n2_found && n2 == m_norms[index])
		{
			*index_2 = index + 1;
			n2_found = true;
		}
		if (!n3_found && n3 == m_norms[index])
		{
			*index_3 = index + 1;
			n3_found = true;
		}
		if (n1_found && n2_found && n3_found) break;
	}
	if (!n1_found)
	{
		m_norms.push_back(n1);
		*index_1 = m_norms.size();
	}
	if (!n2_found)
	{
		m_norms.push_back(n2);
		*index_2 = m_norms.size();
	}
	if(!n3_found)
	{
		m_norms.push_back(n3);
		*index_3 = m_norms.size();
	}
	m_norms_mux.unlock();
}


void Mesh::Export(std::string fname)
{
	std::filebuf fb;
	fb.open(fname.c_str(), std::ios::out);
	std::ostream os(&fb);
	std::string comment = "# Marching Cubes Polygonization\n"
		"# Program Written By Evan Wilde 2015\n";
	std::cout << "Exporting Mesh...\n";
	os << comment;
	// Write Vertex
	os << "\n# Vertices\n";
	for (unsigned int i = 0; i < m_verts.size(); ++i)
	{
		os << "v "
			<< m_verts[i].x << ' '
			<< m_verts[i].y << ' '
			<< m_verts[i].z << '\n';
	}

	os << "\n# Normals\n";
	for (unsigned int i = 0; i < m_norms.size(); ++i)
	{
		os << "n "
			<< m_norms[i].x << ' '
			<< m_norms[i].y << ' '
			<< m_norms[i].z << '\n';
	}

	// Write Face
	for (auto face = m_triangles.begin(); face != m_triangles.end(); face++)
	{
		os << "f "
			<< (*face).p[0] << "//" << (*face).n[0] << ' '
			<< (*face).p[1] << "//" << (*face).n[1] << ' '
			<< (*face).p[2] << "//" << (*face).n[2] << '\n';
	}
	os << "# " << m_verts.size() << " Vertices written\n";
	os << "# " << m_triangles.size() << " Triangles Written\n";
	fb.close();
}
