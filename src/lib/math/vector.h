#ifndef LIB_VECTOR_H
#define LIB_VECTOR_H

#include "fixed.h"
#include <cmath>
#include <vector>
#include <tuple>
#ifdef _WIN32
	#include <corecrt_math_defines.h>
#endif
#include <iostream>

struct Vec2f;
struct Vec2F;
struct Vec2d;
struct Vec3d;

extern fixed32_t abs(const fixed32_t &n);
extern fixed64_t abs(const fixed64_t &n);
extern fixed32_t sqrt(const fixed32_t &n);
extern fixed64_t sqrt(const fixed64_t &n);
extern fixed32_t operator/(const double &n, const fixed32_t &f);
extern fixed32_t operator*(const double &n, const fixed32_t &f);
extern fixed32_t sin(const fixed32_t &n);
extern fixed32_t cos(const fixed32_t &n);
extern fixed32_t acos(const fixed32_t &n);

//////////////
//vector types
//////////////

struct Vec2d
{
	Vec2d() : m_x(0.0), m_y(0.0) {}
	Vec2d(double x, double y) : m_x(x), m_y(y) {}
	bool operator==(const Vec2d &p) const { return std::tie(m_x, m_y) == std::tie(p.m_x, p.m_y); }
	bool operator!=(const Vec2d &p) const { return std::tie(m_x, m_y) != std::tie(p.m_x, p.m_y); }
	bool operator<(const Vec2d &p) const { return std::tie(m_x, m_y) < std::tie(p.m_x, p.m_y); }
	Vec2d operator+(const Vec2d &p) const { return Vec2d(m_x + p.m_x, m_y + p.m_y); }
	Vec2d operator-(const Vec2d &p) const { return Vec2d(m_x - p.m_x, m_y - p.m_y); }
	Vec2d operator*(const Vec2d &p) const { return Vec2d(m_x * p.m_x, m_y * p.m_y); }
	template <typename T> Vec2d operator*(const T &n) const { return Vec2d(m_x * n, m_y * n); }
	Vec2d operator/(const Vec2d &p) const { return Vec2d(m_x / p.m_x, m_y / p.m_y); }
	double dot(const Vec2d &p) const { return m_x * p.m_x + m_y * p.m_y; }
	double length() const { return sqrt(dot(*this)); }
	Vec2d reflect(const Vec2d &n) const { return *this - n * (dot(n) * 2.0); }
	Vec2d norm() { auto l = length(); if (l == 0.0) return Vec2d(0.0, 0.0); return *this * (1.0 / l); }
	Vec2d perp() const { return Vec2d(-m_y, m_x); }
	Vec2d abs() const { return Vec2d(std::abs(m_x), std::abs(m_y)); }
	double sum() const { return m_x + m_y; }
	double max() const { return std::max(m_x, m_y); }
	double m_x;
	double m_y;
};

struct Vec3d
{
	Vec3d() : m_x(0.0), m_y(0.0), m_z(0.0) {}
	Vec3d(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}
	bool operator==(const Vec3d &p) const { return std::tie(m_x, m_y, m_z) == std::tie(p.m_x, p.m_y, p.m_z); }
	bool operator!=(const Vec3d &p) const { return std::tie(m_x, m_y, m_z) != std::tie(p.m_x, p.m_y, p.m_z); }
	bool operator<(const Vec3d &p) const { return std::tie(m_x, m_y, m_z) < std::tie(p.m_x, p.m_y, p.m_z); }
	Vec3d operator+(const Vec3d &p) const { return Vec3d(m_x + p.m_x, m_y + p.m_y, m_z + p.m_z); }
	Vec3d operator-(const Vec3d &p) const { return Vec3d(m_x - p.m_x, m_y - p.m_y, m_z - p.m_z); }
	Vec3d operator*(const Vec3d &p) const { return Vec3d(m_x * p.m_x, m_y * p.m_y, m_z * p.m_z); }
	template <typename T> Vec3d operator*(const T &n) const { return Vec3d(m_x * n, m_y * n, m_z * n); }
	Vec3d operator/(const Vec3d &p) const { return Vec3d(m_x / p.m_x, m_y / p.m_y, m_z / p.m_z); }
	double dot(const Vec3d &p) const { return m_x * p.m_x + m_y * p.m_y + m_z * p.m_z; }
	double length() const { return sqrt(dot(*this)); }
	Vec3d reflect(const Vec3d &n) const { return *this - n * (dot(n) * 2.0); }
	Vec3d norm() { auto l = length(); if (l == 0.0) return Vec3d(0.0, 0.0, 0.0); return *this * (1.0 / l); }
	Vec3d abs() const { return Vec3d(std::abs(m_x), std::abs(m_y), std::abs(m_z)); }
	double sum() const { return m_x + m_y + m_z; }
	double max() const { return std::max(std::max(m_x, m_y), m_z); }
	double m_x;
	double m_y;
	double m_z;
};

struct Vec2f
{
	Vec2f() : m_x(0), m_y(0) {}
	Vec2f(fixed32_t x, fixed32_t y) : m_x(x), m_y(y) {}
	Vec2f(fixed64_t x, fixed64_t y) : m_x(x), m_y(y) {}
	Vec2f(double x, double y) : m_x(x), m_y(y) {}
	Vec2f(const Vec2F &p);
	bool operator==(const Vec2f &p) const { return std::tie(m_x, m_y) == std::tie(p.m_x, p.m_y); }
	bool operator!=(const Vec2f &p) const { return std::tie(m_x, m_y) != std::tie(p.m_x, p.m_y); }
	bool operator<(const Vec2f &p) const { return std::tie(m_x, m_y) < std::tie(p.m_x, p.m_y); }
	Vec2f operator+(const Vec2f &p) const { return Vec2f(m_x + p.m_x, m_y + p.m_y); }
	Vec2f operator-(const Vec2f &p) const { return Vec2f(m_x - p.m_x, m_y - p.m_y); }
	Vec2f operator*(const Vec2f &p) const { return Vec2f(m_x * p.m_x, m_y * p.m_y); }
	template <typename T> Vec2f operator*(const T &n) const { return Vec2f(m_x * n, m_y * n); }
	Vec2f operator/(const Vec2f &p) const { return Vec2f(m_x / p.m_x, m_y / p.m_y); }
	fixed32_t dot(const Vec2f &p) const { return m_x * p.m_x + m_y * p.m_y; }
	fixed32_t length() const { return sqrt(dot(*this)); }
	Vec2f reflect(const Vec2f &n) const { return *this - n * (dot(n) * fixed32_t(2.0)); }
	Vec2f norm() { auto l = length(); if (l == fixed32_t(0)) return Vec2f(0, 0); return *this * (fixed32_t(1.0) / l); }
	Vec2f perp() const { return Vec2f(-m_y, m_x); }
	Vec2f asr(const int &s) const { return Vec2f(m_x >> s, m_y >> s); }
	Vec2f abs() const { return Vec2f(::abs(m_x), ::abs(m_y)); }
	fixed32_t sum() const { return m_x + m_y; }
	fixed32_t max() const { return std::max(m_x, m_y); }
	fixed32_t m_x;
	fixed32_t m_y;
};

struct Vec2F
{
	Vec2F() : m_x(0), m_y(0) {}
	Vec2F(fixed64_t x, fixed64_t y) : m_x(x), m_y(y) {}
	Vec2F(fixed64_t x) : m_x(x), m_y(0) {}
	Vec2F(fixed32_t x, fixed32_t y) : m_x(x), m_y(y) {}
	Vec2F(double x, double y) : m_x(x), m_y(y) {}
	Vec2F(const Vec2f &p);
	bool operator==(const Vec2F &p) const { return std::tie(m_x, m_y) == std::tie(p.m_x, p.m_y); }
	bool operator!=(const Vec2F &p) const { return std::tie(m_x, m_y) != std::tie(p.m_x, p.m_y); }
	bool operator<(const Vec2F &p) const { return std::tie(m_x, m_y) < std::tie(p.m_x, p.m_y); }
	Vec2F operator+(const Vec2F &p) const { return Vec2F(m_x + p.m_x, m_y + p.m_y); }
	Vec2F operator-(const Vec2F &p) const { return Vec2F(m_x - p.m_x, m_y - p.m_y); }
	Vec2F operator*(const Vec2F &p) const { return Vec2F(m_x * p.m_x, m_y * p.m_y); }
	template <typename T> Vec2F operator*(const T &n) const { return Vec2F(m_x * n, m_y * n); }
	Vec2F operator/(const Vec2F &p) const { return Vec2F(m_x / p.m_x, m_y / p.m_y); }
	fixed64_t dot(const Vec2F &p) const { return m_x * p.m_x + m_y * p.m_y; }
	fixed64_t length() const { return sqrt(dot(*this)); }
	Vec2F reflect(const Vec2F &n) const { return *this - n * (dot(n) * fixed64_t(2.0)); }
	Vec2F norm() { auto l = length(); if (l == fixed64_t(0)) return Vec2f(0, 0); return *this * (fixed64_t(1.0) / l); }
	Vec2F perp() const { return Vec2F(-m_y, m_x); }
	Vec2F asr(const int &s) const { return Vec2F(m_x >> s, m_y >> s); }
	Vec2F abs() const { return Vec2F(::abs(m_x), ::abs(m_y)); }
	fixed64_t sum() const { return m_x + m_y; }
	fixed64_t max() const { return std::max(m_x, m_y); }
	fixed64_t m_x;
	fixed64_t m_y;
};

///////////////////////
//distance metric stuff
///////////////////////

template <typename T>
auto manhattan_distance(const T &p1, const T &p2)
{
	return (p1 - p2).abs().sum();
}

template <typename T>
auto euclidean_distance(const T &p1, const T &p2)
{
	return (p1 - p2).length();
}

template <typename T>
auto squared_euclidean_distance(const T &p1, const T &p2)
{
	auto p = p2 - p1;
	return p.dot(p);
}

template <typename T>
auto chebyshev_distance(const T &p1, const T &p2)
{
	return (p1 - p2).abs().max();
}

//////////////////
//specific lengths
//////////////////

template <typename T>
auto det_v2(const T &p1, const T &p2)
{
	return p1.m_x * p2.m_y - p1.m_y * p2.m_x;
}

template <typename T>
auto distance(const T &p1, const T &p2)
{
	return (p2 - p1).length();
}

template <typename T>
auto distance_squared(const T &p1, const T &p2)
{
	auto p = p2 - p1;
	return p.dot(p);
}

template <typename T>
auto distance_to_line(const T &p, const T &p1, const T &p2)
{
	auto lv = p2 - p1;
	auto pv = p - p1;
	auto c1 = pv.dot(lv);
	if (c1 <= 0.0) return distance(p, p1);
	auto c2 = lv.dot(lv);
	if (c2 <= c1) return distance(p, p2);
	return distance(p, p1 + lv * (c1/c2));
}

template <typename T>
auto distance_squared_to_line(const T &p, const T &p1, const T &p2)
{
	auto lv = p2 - p1;
	auto pv = p - p1;
	auto c1 = pv.dot(lv);
	if (c1 <= 0.0) return distance_squared(p, p1);
	auto c2 = lv.dot(lv);
	if (c2 <= c1) return distance_squared(p, p2);
	return distance_squared(p, p1 + lv * (c1/c2));
}

template <typename T>
bool collide_lines_v2(const T &l1_p1, const T &l1_p2, const T &l2_p1, const T &l2_p2)
{
	auto av = l1_p2 - l1_p1;
	auto bv = l2_p2 - l2_p1;
	auto cv = l2_p2 - l1_p1;
	auto axb = det_v2(av, bv);
	auto axc = det_v2(av, cv);
	auto cxb = det_v2(cv, bv);
	if (axb == 0.0) return false;
	if (axb > 0.0)
	{
		if ((axc < 0.0) || (axc > axb)) return false;
		if ((cxb < 0.0) || (cxb > axb)) return false;
	}
	else
	{
		if ((axc > 0.0) || (axc < axb)) return false;
		if ((cxb > 0.0) || (cxb < axb)) return false;
	}
	return true;
}

template <typename T>
bool collide_thick_lines(const T &tl1_p1, const T &tl1_p2,
	 					const T &tl2_p1, const T &tl2_p2, double r)
{
	if (collide_lines_v2(tl1_p1, tl1_p2, tl2_p1, tl2_p2)) return true;
	r *= r;
	if (distance_squared_to_line(tl2_p1, tl1_p1, tl1_p2) <= r) return true;
	if (distance_squared_to_line(tl2_p2, tl1_p1, tl1_p2) <= r) return true;
	if (distance_squared_to_line(tl1_p1, tl2_p1, tl2_p2) <= r) return true;
	if (distance_squared_to_line(tl1_p2, tl2_p1, tl2_p2) <= r) return true;
	return false;
}

template <typename T>
auto mod_v3(const T &p1, const T &p2)
{
	auto x = std::fmod(p1.m_x, p2.m_x);
	auto y = std::fmod(p1.m_y, p2.m_y);
	auto z = std::fmod(p1.m_z, p2.m_z);
	if (x < 0.0) x += p2.m_x;
	if (y < 0.0) y += p2.m_y;
	if (z < 0.0) z += p2.m_z;
	return T(x, y, z);
}

template <typename T>
auto frac_v3(const T &p)
{
	double intpart;
	auto x = std::modf(p.m_x, &intpart);
	auto y = std::modf(p.m_y, &intpart);
	auto z = std::modf(p.m_z, &intpart);
	if (x < 0.0) x += 1.0;
	if (y < 0.0) y += 1.0;
	if (z < 0.0) z += 1.0;
	return T(x, y, z);
}

template <typename T>
auto floor_v3(const T &p)
{
	return T(std::floor(p.m_x), std::floor(p.m_y), std::floor(p.m_z));
}

template <typename T>
auto clamp_v3(const T &p1, const T &p2, const T &p3)
{
	return T(
		std::min(std::max(p1.m_x, p2.m_x), p3.m_x),
		std::min(std::max(p1.m_y, p2.m_y), p3.m_y),
		std::min(std::max(p1.m_z, p2.m_z), p3.m_z));
}

////////////////////
//generic path stuff
////////////////////

template <typename T>
auto circle_as_lines(const T &p, double radius, int resolution)
{
	auto out_points = std::vector<T>{}; out_points.reserve(resolution+1);
	auto rvx = 0.0;
	auto rvy = radius;
	for (auto i = 0; i <= resolution; ++i)
	{
		auto angle = (i * M_PI * 2.0) / resolution;
		auto s = double(sin(angle));
		auto c = double(cos(angle));
		auto rv = T(rvx*c - rvy*s, rvx*s + rvy*c);
		out_points.push_back(p - rv);
	}
	out_points.push_back(out_points[0]);
	return out_points;
}

template <typename T>
auto torus_as_tristrip(const T &p, double radius1, double radius2, int resolution)
{
	auto out_points = std::vector<T>{}; out_points.reserve(resolution*2+2);
	auto rvx1 = 0.0;
	auto rvy1 = radius1;
	auto rvx2 = 0.0;
	auto rvy2 = radius2;
	for (auto i = 0; i <= resolution; ++i)
	{
		auto angle = (i * M_PI * 2.0) / resolution;
		auto s = double(sin(angle));
		auto c = double(cos(angle));
		auto rv1 = T(rvx1*c - rvy1*s, rvx1*s + rvy1*c);
		auto rv2 = T(rvx2*c - rvy2*s, rvx2*s + rvy2*c);
		out_points.push_back(p - rv1);
		out_points.push_back(p - rv2);
	}
	out_points.push_back(out_points[0]);
	out_points.push_back(out_points[1]);
	return out_points;
}

template <typename T>
auto circle_as_trifan(const T &p, double radius, int resolution)
{
	auto out_points = std::vector<T>{}; out_points.reserve(resolution*2+2);
	auto rvx1 = 0.0;
	auto rvy1 = radius;
	out_points.push_back(p);
	for (auto i = 0; i <= resolution; ++i)
	{
		auto angle = (i * M_PI * 2.0) / resolution;
		auto s = double(sin(angle));
		auto c = double(cos(angle));
		auto rv1 = T(rvx1*c - rvy1*s, rvx1*s + rvy1*c);
		out_points.push_back(p - rv1);
	}
	out_points.push_back(out_points[0]);
	return out_points;
}

enum
{
	cap_butt,
	cap_square,
	cap_tri,
	cap_arrow,
	cap_round,
};

enum
{
	join_mitre,
	join_bevel,
	join_round,
};

template <typename T, typename T1>
auto stroke_path(const std::vector<T> &path, T1 radius, uint32_t resolution, uint32_t join_style, uint32_t cap1_style, uint32_t cap2_style)
{
	auto index = 0;
	auto step = 1;
	auto out_points = std::vector<T>{};
	for (;;)
	{
		auto p1 = path[index];
		index += step;
		auto p2 = path[index];
		index += step;
		auto l2_v = p2 - p1;
		auto l2_pv = l2_v.perp();
		auto l2_npv = l2_pv.norm();
		auto rv = l2_npv * radius;
		switch (step > 0 ? cap1_style : cap2_style)
		{
			case cap_butt:
			{
				//butt cap
				out_points.push_back(p1 - rv);
				out_points.push_back(p1 + rv);
				break;
			}
			case cap_square:
			{
				//square cap
				auto p0 = p1 + rv.perp();
				out_points.push_back(p0 - rv);
				out_points.push_back(p0 + rv);
				break;
			}
			case cap_tri:
			{
				//triangle cap
				out_points.push_back(p1 - rv);
				out_points.push_back(p1 + rv.perp());
				out_points.push_back(p1 + rv);
				break;
			}
			case cap_arrow:
			{
				//arrow cap
				auto p0 = rv * 2;
				out_points.push_back(p1 - p0);
				out_points.push_back(p1 + p0.perp());
				out_points.push_back(p1 + p0);
				break;
			}
			default:
			{
				//round cap
				auto rvx = rv.m_x;
				auto rvy = rv.m_y;
				for (auto i = 0; i <= resolution; ++i)
				{
					T1 angle = (i * -M_PI) / resolution;
					auto s = sin(angle);
					auto c = cos(angle);
					auto rv = T(rvx*c - rvy*s, rvx*s + rvy*c);
					out_points.push_back(p1 - rv);
				}
			}
		}
		while ((index != -1) && (index != static_cast<int>(path.size())))
		{
			auto p1 = p2;
			auto l1_v = l2_v;
			auto l1_npv = l2_npv;
			p2 = path[index];
			index += step;
			l2_v = p2 - p1;
			l2_pv = l2_v.perp();
			l2_npv = l2_pv.norm();
			auto nbv = ((l1_npv + l2_npv) * T1(0.5)).norm();
			auto c = nbv.dot(l1_v.norm());
			if (c <= T1(0)) goto mitre_join;
			switch (join_style)
			{
				case join_mitre:
				{
				mitre_join:
					//mitre join
					auto s = sin(acos(c));
					auto bv = nbv * (radius/s);
					out_points.push_back(p1 + bv);
					break;
				}
				case join_bevel:
				{
					//bevel join
					out_points.push_back(p1 + l1_npv * radius);
					out_points.push_back(p1 + l2_npv * radius);
					break;
				}
				default:
				{
					//round join
					auto rv = l1_npv * radius;
					auto rvx = rv.m_x;
					auto rvy = rv.m_y;
					auto theta = -(acos(l1_npv.dot(l2_npv)));
					auto segs = int((theta/-M_PI)*resolution) + 1;
					for (auto i = 0; i <= segs; ++i)
					{
						T1 angle = (theta * i) / segs;
						auto s = sin(angle);
						auto c = cos(angle);
						auto rv = T(rvx*c - rvy*s, rvx*s + rvy*c);
						out_points.push_back(p1 + rv);
					}
				}
			}
		}
		if (step < 0) break;
		step = -step;
		index += step;
	}
	return out_points;
}

template <typename T, typename T1>
auto stroke_joins(const std::vector<T> &path, int32_t step, T1 radius, uint32_t resolution, uint32_t join_style)
{
	auto out_points = std::vector<T>{};
	auto len = static_cast<int>(path.size());
	auto index = step > 0 ? 0 : len - 1;
	auto p1 = path[(len + (index - (step * 2))) % len];
	auto p2 = path[(len + (index - (step * 1))) % len];
	auto l2_v = p2 - p1;
	auto l2_pv = l2_v.perp();
	auto l2_npv = l2_pv.norm();
	while ((index != -1) && (index != len))
	{
		p1 = p2;
		auto l1_v = l2_v;
		auto l1_npv = l2_npv;
		p2 = path[index];
		index += step;
		l2_v = p2 - p1;
		l2_pv = l2_v.perp();
		l2_npv = l2_pv.norm();
		auto nbv = ((l1_npv + l2_npv) * T1(0.5)).norm();
		auto c = nbv.dot(l1_v.norm());
		if (c <= T1(0)) goto mitre_join;
		switch (join_style)
		{
			case join_mitre:
			{
			mitre_join:
				//mitre join
				auto s = sin(acos(c));
				auto bv = nbv * (radius/s);
				out_points.push_back(p1 + bv);
				break;
			}
			case join_bevel:
			{
				//bevel join
				out_points.push_back(p1 + l1_npv * radius);
				out_points.push_back(p1 + l2_npv * radius);
				break;
			}
			default:
			{
				//round join
				auto rv = l1_npv * radius;
				auto rvx = rv.m_x;
				auto rvy = rv.m_y;
				auto theta = -acos(l1_npv.dot(l2_npv));
				auto segs = int((theta/-M_PI)*resolution) + 1;
				for (auto i = 0; i <= segs; ++i)
				{
					T1 angle = (theta * i) / segs;
					auto s = sin(angle);
					auto c = cos(angle);
					auto rv = T(rvx*c - rvy*s, rvx*s + rvy*c);
					out_points.push_back(p1 + rv);
				}
			}
		}
	}
	return out_points;
}

#endif
