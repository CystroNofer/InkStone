#pragma once

#include "Core.h"

#include "Log.h"
#include "Math/Math.h"

namespace NXTN {

	enum class UniformType
	{
		None = 0, Int, Float, Float2, Float3, Float4, Mat4, Bool, Tex2D, TexCube
	};

	struct Uniform
	{
		UniformType type;
		std::string name;
	};

	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformInt(const char* name, const int& i) = 0;
		virtual void SetUniformUInt(const char* name, const unsigned int& i) = 0;
		virtual void SetUniformFloat(const char* name, const float& f) = 0;
		virtual void SetUniformFloat3(const char* name, const float& f0, const float& f1, const float& f2) = 0;
		virtual void SetUniformFloat4(const char* name, const float& f0, const float& f1, const float& f2, const float& f3) = 0;
		virtual void SetUniformMat4(const char* name, const mat4& m) = 0;

		virtual const std::vector<Uniform>& GetUniformList() = 0;

		static Shader* Create(const std::string& filepath);
	};
}

