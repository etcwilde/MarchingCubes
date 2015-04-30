
#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>

#include <glm/vec3.hpp>

#include "implicit/ImplicitSystem.hpp"
#include "Mesh.hpp"
#include "CubesPolygonizer.hpp"

#ifdef PROFILE
#include <gperftools/profiler.h>
#endif

/**
 * Used to compare marching cubes algorithm with marching triangles algorithm
 */

/**
 * Polygonizer from
 * http://paulbourke.net/geometry/polygonise/
 */
int main()
{
	Implicit::Sphere s(geoffFunction);
	Implicit::Line l(geoffFunction);

	Implicit::Scale line_scale(&l, 2, 1, 1);
	Implicit::Scale sphere_scale(&s, 1.2);
	Implicit::Translate sphere_translate_1(&sphere_scale, 2, 0, 0);
	Implicit::Translate sphere_translate_2(&sphere_scale, -2, 0, 0);
	Implicit::Union caps1(&sphere_translate_1, &sphere_translate_2);
	Implicit::Rotate caps2(&caps1, glm::vec3(0, 1, 0), M_PI/2.f);
	Implicit::Union caps(&caps1, &caps2);

	Implicit::Rotate r1(&line_scale, glm::vec3(0, 1, 0), M_PI/2.f);
	Implicit::Rotate r2(&line_scale, glm::vec3(0, 0, 1), M_PI/2.f);
	Implicit::Union u1(&r1, &r2);
	Implicit::Union u2(&line_scale, &u1);
	Implicit::Union jack(&u2, &caps);
	CubesPolygonizer marching_cubes(jack, 50);

	Mesh m = marching_cubes.Polygonize();

	m.Export("output.obj");
	return 0;
}
