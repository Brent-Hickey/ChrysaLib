#ifndef PATH_H
#define PATH_H

#include "../math/vector.h"
#include <vector>
#include <cstddef>

class Path
{
public:
	Path() {}
	Path *push_back(const Vec2f &p);
	Path *pop_back();
	Vec2f &operator[](const uint64_t &i) { return m_points[i]; }
	const Vec2f &operator[](const uint64_t &i) const { return m_points[i]; }
	size_t size() const { return m_points.size(); };
	Path *gen_quadratic(Vec2f p1, Vec2f p2, Vec2f p3, fixed32_t tol);
	Path *gen_cubic(Vec2f p1, Vec2f p2, Vec2f p3, Vec2f p4, fixed32_t tol);
	Path *filter_polyline(fixed32_t tol);
	Path *filter_polygon(fixed32_t tol);
	std::vector<Vec2f> m_points;
};

#endif
