#include "path.h"

Path *Path::push_back(fixed32_t x, fixed32_t y)
{
	m_points.push_back(x);
	m_points.push_back(y);
	return this;
}

Path *Path::gen_quadratic(fixed32_t p1x, fixed32_t p1y, fixed32_t p2x, fixed32_t p2y,
	fixed32_t p3x, fixed32_t p3y, fixed32_t tol)
{
	auto stack = std::vector<fixed32_t>{};

	//output first point
	push_back(p1x, p1y);

	for (;;)
	{
		//calculate the mid-path
		auto p12x = (p1x + p2x) >> 1;
		auto p12y = (p1y + p2y) >> 1;
		auto p23x = (p2x + p3x) >> 1;
		auto p23y = (p2y + p3y) >> 1;

		auto p123x = (p12x + p23x) >> 1;
		auto p123y = (p12y + p23y) >> 1;

		//flatness test
		if (std::abs(p1x + p3x - p2x - p2x)
			+ std::abs(p1y + p3y - p2y - p2y) <= tol)
		{
			//output point
			push_back(p123x, p123y);
		}
		else
		{
			//continue subdivision
			stack.push_back(p123x), stack.push_back(p123y);
			stack.push_back(p23x), stack.push_back(p23y);
			stack.push_back(p3x), stack.push_back(p3y);
			stack.push_back(p1x), stack.push_back(p1y);
			stack.push_back(p12x), stack.push_back(p12y);
			stack.push_back(p123x), stack.push_back(p123y);
		}

		//finished ?
		if (!stack.size()) break;

		p3y = stack.back(), stack.pop_back();
		p3x = stack.back(), stack.pop_back();
		p2y = stack.back(), stack.pop_back();
		p2x = stack.back(), stack.pop_back();
		p1y = stack.back(), stack.pop_back();
		p1x = stack.back(), stack.pop_back();
	}

	//output last point
	push_back(p3x, p3y);
	return this;
}

Path *Path::gen_cubic(fixed32_t p1x, fixed32_t p1y, fixed32_t p2x, fixed32_t p2y,
	fixed32_t p3x, fixed32_t p3y, fixed32_t p4x, fixed32_t p4y, fixed32_t tol)
{
	auto stack = std::vector<fixed32_t>{};

	//output first point
	push_back(p1x, p1y);

	for (;;)
	{
		//calculate the mid-path
		auto p12x = (p1x + p2x) >> 1;
		auto p12y = (p1y + p2y) >> 1;
		auto p23x = (p2x + p3x) >> 1;
		auto p23y = (p2y + p3y) >> 1;
		auto p34x = (p3x + p4x) >> 1;
		auto p34y = (p3y + p4y) >> 1;

		auto p123x = (p12x + p23x) >> 1;
		auto p123y = (p12y + p23y) >> 1;
		auto p234x = (p23x + p34x) >> 1;
		auto p234y = (p23y + p34y) >> 1;

		auto p1234x = (p123x + p234x) >> 1;
		auto p1234y = (p123y + p234y) >> 1;

		//flatness test
		if (std::abs(p1x + p3x - p2x - p2x)
			+ std::abs(p1y + p3y - p2y - p2y)
			+ std::abs(p2x + p4x - p3x - p3x)
			+ std::abs(p2y + p4y - p3y - p3y) <= tol)
		{
			//output point
			push_back(p1234x, p1234y);
		}
		else
		{
			//continue subdivision
			stack.push_back(p1234x), stack.push_back(p1234y);
			stack.push_back(p234x), stack.push_back(p234y);
			stack.push_back(p34x), stack.push_back(p34y);
			stack.push_back(p4x), stack.push_back(p4y);
			stack.push_back(p1x), stack.push_back(p1y);
			stack.push_back(p12x), stack.push_back(p12y);
			stack.push_back(p123x), stack.push_back(p123y);
			stack.push_back(p1234x), stack.push_back(p1234y);
		}

		//finished ?
		if (!stack.size()) break;

		p4y = stack.back(), stack.pop_back();
		p4x = stack.back(), stack.pop_back();
		p3y = stack.back(), stack.pop_back();
		p3x = stack.back(), stack.pop_back();
		p2y = stack.back(), stack.pop_back();
		p2x = stack.back(), stack.pop_back();
		p1y = stack.back(), stack.pop_back();
		p1x = stack.back(), stack.pop_back();
	}

	//output last point
	push_back(p4x, p4y);
	return this;
}
