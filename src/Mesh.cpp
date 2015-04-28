#include "Mesh.hpp"

Mesh::Mesh(std::vector<TRIANGLE> tris)
{
	build(tris);
}

void Mesh::AddFace(const TRIANGLE& tri)
{
	AddFace(tri.p[0], tri.p[1], tri.p[2]);
}

void Mesh::AddFace(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	if (v1 == v2) return;
	if (v1 == v3) return;
	if (v2 == v3) return;
	INDEX_TRIANGLE t;

	bool v1_found = false;
	bool v2_found = false;
	bool v3_found = false;

	for (unsigned int index =0; index < m_verts.size(); index++)
	{
		if (v1 == m_verts[index])
		{
			t.p[0] = index + 1;
			v1_found = true;
		}
		if (v2 == m_verts[index])
		{
			t.p[1] = index + 1;
			v2_found = true;
		}
		if (v3 == m_verts[index])
		{
			t.p[2] = index + 1;
			v3_found = true;
		}
	}

	if (!v1_found)
	{
		m_verts.push_back(v1);
		t.p[0] = m_verts.size();
	}
	if (!v2_found)
	{
		m_verts.push_back(v2);
		t.p[1] = m_verts.size();
	}
	if (!v3_found)
	{
		m_verts.push_back(v3);
		t.p[2] = m_verts.size();
	}
	m_triangles.push_back(t);
}

void Mesh::build(std::vector<TRIANGLE> tris)
{
	// For each triangle

	for (auto tri = tris.begin(); tri != tris.end(); tri++)
	{
		AddFace(*tri);
	}
}

void Mesh::Export(std::string fname)
{
	std::filebuf fb;
	fb.open(fname.c_str(), std::ios::out);
	std::ostream os(&fb);
	std::string comment = "# Marching Cubes Polygonization\n"
		"# Program Written By Evan Wilde 2015\n";
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

	// Write Face
	for (auto face = m_triangles.begin(); face != m_triangles.end(); face++)
	{
		os << "f "
			<< (*face).p[0] << "// "
			<< (*face).p[1] << "// "
			<< (*face).p[2] << "//\n";
	}
	os << "# " << m_verts.size() << " Vertices written\n";
	os << "# " << m_triangles.size() << " Triangles Written\n";
	fb.close();
}
