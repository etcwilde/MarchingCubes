#include "CubesPolygonizer.hpp"

CubesPolygonizer::CubesPolygonizer(Implicit::Object& o, unsigned int max_voxels) :
	Polygonizer(o),
	m_voxels(max_voxels)
{ }

glm::vec3 CubesPolygonizer::VertexInterp(const glm::vec3& x0, const glm::vec3& x1,
			float fx0, float fx1)
{
	if (std::abs(fx0 - fx1) <= FLT_EPSILON) return x0;
	// Standard Linear Interpolation
	return x0 - fx0 * ((x1 - x0) / (fx1 - fx0));
}

Mesh CubesPolygonizer::polygonize()
{
	auto begin = std::chrono::high_resolution_clock::now();
	glm::vec3 minima, maxima;
	std::vector<Mesh::Triangle> tris;
	minima = m_scene.GetBoundingBox().min();
	maxima = m_scene.GetBoundingBox().max();
	float delta_x = maxima.x - minima.x;
	float delta_y = maxima.y - minima.y;
	float delta_z = maxima.z - minima.z;
	float max_delta = std::max(delta_x, std::max(delta_y, delta_z));
	float voxel_size = max_delta/ m_voxels;
	unsigned int x_cubes = std::ceil(delta_x / voxel_size);
	unsigned int y_cubes = std::ceil(delta_y / voxel_size);
	unsigned int z_cubes = std::ceil(delta_z / voxel_size);

	GRIDCELL*** voxels = new GRIDCELL**[x_cubes];
	for (unsigned int x = 0; x < x_cubes; x++)
	{
		voxels[x] = new GRIDCELL*[y_cubes];
		for (unsigned int y = 0; y < y_cubes; y++)
		{
			voxels[x][y] = new GRIDCELL[z_cubes];
			for (unsigned int z = 0; z < z_cubes; z++)
			{
				voxels[x][y][z].p[0] = minima +
					glm::vec3(
							(x+1) * voxel_size,
							y * voxel_size,
							(z+1) * voxel_size
							);

				voxels[x][y][z].p[1] = minima +
					glm::vec3(
							(x+1) * voxel_size,
							y * voxel_size,
							z * voxel_size
							);
				voxels[x][y][z].p[2] = minima +
					glm::vec3(
							x * voxel_size,
							y * voxel_size,
							z * voxel_size
							);

				voxels[x][y][z].p[3] = minima +
					glm::vec3(
							x * voxel_size,
							y * voxel_size,
							(z+1) * voxel_size
							);
				voxels[x][y][z].p[4] = minima +
					glm::vec3(
							(x+1) * voxel_size,
							(y+1) * voxel_size,
							(z+1) * voxel_size
							);
				voxels[x][y][z].p[5] = minima +
					glm::vec3(
							(x+1) * voxel_size,
							(y+1) * voxel_size,
							z * voxel_size
							);
				voxels[x][y][z].p[6] = minima +
					glm::vec3(
							x * voxel_size,
							(y+1) * voxel_size,
							z * voxel_size
							);
				voxels[x][y][z].p[7] = minima +
					glm::vec3(
							x * voxel_size,
							(y+1) * voxel_size,
							(z+1) * voxel_size
							);

				voxels[x][y][z].val[0] = m_scene.Evaluate(voxels[x][y][z].p[0]);
				voxels[x][y][z].val[1] = m_scene.Evaluate(voxels[x][y][z].p[1]);
				voxels[x][y][z].val[2] = m_scene.Evaluate(voxels[x][y][z].p[2]);
				voxels[x][y][z].val[3] = m_scene.Evaluate(voxels[x][y][z].p[3]);
				voxels[x][y][z].val[4] = m_scene.Evaluate(voxels[x][y][z].p[4]);
				voxels[x][y][z].val[5] = m_scene.Evaluate(voxels[x][y][z].p[5]);
				voxels[x][y][z].val[6] = m_scene.Evaluate(voxels[x][y][z].p[6]);
				voxels[x][y][z].val[7] = m_scene.Evaluate(voxels[x][y][z].p[7]);
				ResolveCube(voxels[x][y][z], tris);
			}
			delete voxels[x][y];
		}
		delete voxels[x];
	}
	delete voxels;
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Polygonization Time: " <<
		std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
		<< "ms\n";
	Mesh m(tris);
	return m;
}

int CubesPolygonizer::ResolveCube(GRIDCELL& grid, std::vector<Mesh::Triangle>& triangles)
{
	int i;
	int cube_index = 0;
	glm::vec3 vertlist[12];

	// Determine index into edge table to get vertices inside surface
	if (grid.val[0] > 0) cube_index |= 1;
	if (grid.val[1] > 0) cube_index |= 2;
	if (grid.val[2] > 0) cube_index |= 4;
	if (grid.val[3] > 0) cube_index |= 8;
	if (grid.val[4] > 0) cube_index |= 16;
	if (grid.val[5] > 0) cube_index |= 32;
	if (grid.val[6] > 0) cube_index |= 64;
	if (grid.val[7] > 0) cube_index |= 128;

	// Cube is entirely in out of surface
	if (cube_index == 0) return 0;
	if (edgeTable[cube_index] == 0) return 0;
	// Find vertices where surface intersects cube
	if (edgeTable[cube_index] & 1)
	{
		// edge 0 is between vert 0 and 1
		vertlist[0] = VertexInterp(grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
	}
	if (edgeTable[cube_index] & 2)
	{
		// edge 1 is between vert 1 and 2
		vertlist[1] = VertexInterp(grid.p[1] , grid.p[2], grid.val[1], grid.val[2]);
	}
	if (edgeTable[cube_index] & 4)
	{
		// edge 2 is between vert 2 and 3
		vertlist[2] = VertexInterp(grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
	}
	if (edgeTable[cube_index] & 8)
	{
		// edge 3 is between vert 3 and 0
		vertlist[3] =  VertexInterp(grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
	}
	if (edgeTable[cube_index] & 16)
	{
		// edge 4 is between vert 4 and 5
		vertlist[4] = VertexInterp(grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
	}
	if (edgeTable[cube_index] & 32)
	{
		// edge 5 is between vert 5 and 6
		vertlist[5] = VertexInterp(grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
	}
	if (edgeTable[cube_index] & 64)
	{
		// edge 6 is between vert 6 and 7
		vertlist[6] = VertexInterp(grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
	}
	if (edgeTable[cube_index] & 128)
	{
		// edge 7 is between vert 7 and 4
		vertlist[7] = VertexInterp(grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
	}
	if (edgeTable[cube_index] & 256)
	{
		// edge 8 is between vert 0 and 4
		vertlist[8] = VertexInterp(grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
	}
	if (edgeTable[cube_index] & 512)
	{
		// edge 9 is between vert 1 and 5
		vertlist[9] = VertexInterp(grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
	}
	if (edgeTable[cube_index] & 1024)
	{
		// edge 10 is between vert 2 and 6
		vertlist[10] = VertexInterp(grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
	}
	if (edgeTable[cube_index] & 2048)
	{
		// edge 11 is between vert 3 and 7
		vertlist[11] = VertexInterp(grid.p[3], grid.p[7], grid.val[3], grid.val[7]);
	}
	// Create triangles
	for (i = 0; triTable[cube_index][i] != -1; i+= 3)
	{
		Mesh::Triangle t;
		t.p[0] = vertlist[triTable[cube_index][i+2]];
		t.p[1] = vertlist[triTable[cube_index][i+1]];
		t.p[2] = vertlist[triTable[cube_index][i]];
		t.n[0] = m_scene.Normal(t.p[0]);
		t.n[1] = m_scene.Normal(t.p[1]);
		t.n[2] = m_scene.Normal(t.p[2]);
		triangles.push_back(t);
	}
	return triangles.size();
}



