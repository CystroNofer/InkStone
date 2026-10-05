#pragma once

#include "Rendering/Shader.h"

#include <GLAD/glad.h>

namespace NXTN {
	struct OpenGLShaderUniformInfo
	{
		ShaderUniformType type;
		GLint location;
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const ShaderProgramDescriptor& description);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt(const std::string& name, int i) override;
		virtual void SetUniformUInt(const std::string& name, unsigned int i) override;
		virtual void SetUniformFloat(const std::string& name, float f) override;
		virtual void SetUniformFloat3(const std::string& name, const vec3& f3) override;
		virtual void SetUniformFloat3(const std::string& name, float f0, float f1, float f2) override;
		virtual void SetUniformFloat4(const std::string& name, const vec4& f4) override;
		virtual void SetUniformFloat4(const std::string& name, float f0, float f1, float f2, float f3) override;
		virtual void SetUniformMat4(const std::string& name, const mat4& m) override;
		virtual void SetUniformTex2D(const std::string& name, unsigned int slot) override;

		virtual inline std::vector<ShaderUniformDescriptor> GetUniformList() const override;
		inline bool IsValid() const { return m_RendererID != 0; }

	private:
		void Compile(const ShaderProgramDescriptor& description);

		GLint GetUniformPosition(const std::string& name);

	private:
		unsigned int m_RendererID = 0;

		// std::vector<ShaderUniformDescriptor> m_Uniforms;
		std::unordered_map<std::string, OpenGLShaderUniformInfo> m_UniformInfo;
	};
}

