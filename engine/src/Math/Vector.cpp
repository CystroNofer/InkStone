#include "pch.h"

#include "Vector.h"

namespace NXTN {
	float length(const vec2& v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	float length(const vec3& v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	float length(const vec4& v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}

	vec2 normalize(vec2 v)
	{
		float l = length(v);
		if (l)
		{
			return v / l;
		}
		return v;
	}

	vec3 normalize(vec3 v)
	{
		float l = length(v);
		if (l)
		{
			return v / l;
		}
		return v;
	}

	vec4 normalize(vec4 v)
	{
		float l = length(v);
		if (l)
		{
			return v / l;
		}
		return v;
	}

	float dot(const vec2& v1, const vec2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	float dot(const vec3& v1, const vec3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	float dot(const vec4& v1, const vec4& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	float cross(const vec2& v1, const vec2& v2)
	{
		return v1.x * v2.y - v1.y * v2.x;
	}

	vec3 cross(const vec3& v1, const vec3& v2)
	{
		return vec3{ v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
	}

	std::string vec2::ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ")";
		const std::string s = ss.str();
		return s;
	}

	std::string vec2i::ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ")";
		const std::string s = ss.str();
		return s;
	}

	std::string vec3::ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ", " << z << ")";
		const std::string s = ss.str();
		return s;
	}

	std::string vec3i::ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ", " << z << ")";
		const std::string s = ss.str();
		return s;
	}

	std::string vec4::ToString()
	{
		std::stringstream ss;
		ss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
		const std::string s = ss.str();
		return s;
	}
}