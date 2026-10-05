#pragma once

#include "Asset/ShaderManager.h"

namespace NXTN {
	using MaterialParamVal = std::variant<
		//bool,
		int,
		unsigned int,
		float,
		//vec2,
		vec3,
		vec4,
		mat4,
		Handle<Texture2D>
	>;

	struct MaterialParam
	{
		ShaderUniformType type;
		MaterialParamVal value;
	};

	class Material
	{
	public:
		Material(Handle<Shader> shaderHandle);
		~Material() {}

		void Bind();
		void Unbind();
		
		void SetUniformInt(const std::string& name, int i);
		void SetUniformUInt(const std::string& name, unsigned int i);
		void SetUniformFloat(const std::string& name, float f);
		void SetUniformFloat3(const std::string& name, const vec3& f3);
		void SetUniformFloat4(const std::string& name, const vec4& f4);
		void SetUniformMat4(const std::string& name, const mat4& m);
		void SetUniformTex2D(const std::string& name, Handle<Texture2D> texHandle);

	private:
		Handle<Shader> m_ShaderHandle;
		std::unordered_map<std::string, MaterialParam> m_Parameters;
	};
}

