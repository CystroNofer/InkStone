#pragma once

#include "Core.h"

#include "Log.h"
#include "Math/Math.h"

namespace NXTN {
	enum class ShaderStage
	{
		Vertex,
		Fragment
	};

	struct ShaderStageDescriptor
	{
		// Multiple sub-shader per stage is allowed by some APIs
		// Though uncommon
		ShaderStage stage;
		std::string filepath;
	};

	enum class ShaderVariableType
	{
		ErrorType = 0, Int, UInt, Float, Float2, Float3, Float4, Mat4, Bool, Tex2D, TexCube
	};

	struct ShaderVariableDescriptor
	{
		std::string name;
		ShaderVariableType type;
	};

	struct ShaderStageIntermediateDescriptor
	{
		ShaderStage stage;
		std::vector<ShaderVariableDescriptor> inputs;
		std::vector<ShaderVariableDescriptor> uniforms;
		std::vector<ShaderVariableDescriptor> outputs;
		std::string code;
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

		virtual std::vector<ShaderVariableDescriptor> GetUniformList() const = 0;
	};
}

