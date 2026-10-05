#pragma once

#include "Core.h"

#include "Log.h"
#include "Texture.h"
#include "Math/Math.h"

namespace NXTN {

	enum class ShaderStage
	{
		Vertex,
		Fragment
	};

	struct ShaderStageDescriptor
	{
		ShaderStage stage;
		std::string filepath;
	};

	struct ShaderProgramDescriptor
	{
		std::vector<ShaderStageDescriptor> stages;
	};

	enum class ShaderUniformType
	{
		ErrorType = 0, Int, UInt, Float, Float2, Float3, Float4, Mat4, Bool, Tex2D, TexCube
	};

	struct ShaderUniformDescriptor
	{
		std::string name;
		ShaderUniformType type;
	};

	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformInt(const std::string& name, int i) = 0;
		virtual void SetUniformUInt(const std::string& name, unsigned int i) = 0;
		virtual void SetUniformFloat(const std::string& name, float f) = 0;
		virtual void SetUniformFloat3(const std::string& name, const vec3& f3) = 0;
		virtual void SetUniformFloat3(const std::string& name, float f0, float f1, float f2) = 0;
		virtual void SetUniformFloat4(const std::string& name, const vec4& f4) = 0;
		virtual void SetUniformFloat4(const std::string& name, float f0, float f1, float f2, float f3) = 0;
		virtual void SetUniformMat4(const std::string& name, const mat4& m) = 0;
		virtual void SetUniformTex2D(const std::string& name, unsigned int slot) = 0;

		virtual std::vector<ShaderUniformDescriptor> GetUniformList() const = 0;
	};
}

