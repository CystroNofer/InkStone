#pragma once

#include "Rendering/Shader.h"

#include <GLAD/glad.h>

namespace NXTN {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt(const char* name, const int& i) override;
		virtual void SetUniformUInt(const char* name, const unsigned int& i) override;
		virtual void SetUniformFloat(const char* name, const float& f) override;
		virtual void SetUniformFloat3(const char* name, const float& f0, const float& f1, const float& f2) override;
		virtual void SetUniformFloat4(const char* name, const float& f0, const float& f1, const float& f2, const float& f3) override;
		virtual void SetUniformMat4(const char* name, const mat4& m) override;

		virtual inline const std::vector<Uniform>& GetUniformList() override { return m_Uniforms; };

	private:
		void Compile();

		int GetUniformPosition(const char* name);

	private:
		unsigned int m_RendererID;

		std::string m_Filepath;

		std::vector<Uniform> m_Uniforms;
		std::unordered_map<std::string, int> m_UniformLocations;
	};
}

