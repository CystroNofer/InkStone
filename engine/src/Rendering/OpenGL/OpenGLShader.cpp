#include "pch.h"

#include "OpenGLShader.h"

namespace NXTN {
	OpenGLShader::OpenGLShader(const ShaderProgramDescriptor& description)
	{
		Compile(description);
	}

	OpenGLShader::~OpenGLShader()
	{
		if (m_RendererID != 0)
		{
			glDeleteProgram(m_RendererID);
		}
	}

	void OpenGLShader::Compile(const ShaderProgramDescriptor& description)
	{
		if (description.Modules.empty())
		{
			Log::Warning("Cannot create a shader program without shader modules");
			return;
		}

		unsigned int program = glCreateProgram();
		if (program == 0)
		{
			Log::Warning("Failed to create shader program");
			return;
		}

		std::vector<unsigned int> shaderHandles;
		const std::function<void()> discardProgram = [&]()
			{
				for (unsigned int shader : shaderHandles)
				{
					glDetachShader(program, shader);
					glDeleteShader(shader);
				}
				shaderHandles.clear();
				glDeleteProgram(program);
			};

		std::vector<ShaderStage> compiledStages;

		for (const ShaderStageDescriptor& module : description.Modules)
		{
			if (std::find(compiledStages.begin(), compiledStages.end(), module.Stage) != compiledStages.end())
			{
				Log::Warning("Shader program contains a duplicate shader stage at %s", module.Filepath.c_str());
				discardProgram();
				return;
			}

			GLenum openGLStage = 0;
			const char* stageName = nullptr;
			switch (module.Stage)
			{
			case ShaderStage::Vertex:
				openGLStage = GL_VERTEX_SHADER;
				stageName = "vertex";
				break;
			case ShaderStage::Fragment:
				openGLStage = GL_FRAGMENT_SHADER;
				stageName = "fragment";
				break;
			default:
				Log::Warning("Unsupported shader stage at %s", module.Filepath.c_str());
				discardProgram();
				return;
			}

			std::ifstream input(module.Filepath, std::ios::in | std::ios::binary);
			if (!input)
			{
				Log::Warning("Cannot open %s shader source at %s", stageName, module.Filepath.c_str());
				discardProgram();
				return;
			}

			std::ostringstream sourceStream;
			sourceStream << input.rdbuf();
			if (input.bad())
			{
				Log::Warning("Failed to read %s shader source at %s", stageName, module.Filepath.c_str());
				discardProgram();
				return;
			}

			std::string sourceText = sourceStream.str();
			if (sourceText.empty())
			{
				Log::Warning("%s shader source is empty at %s", stageName, module.Filepath.c_str());
				discardProgram();
				return;
			}

			unsigned int shaderHandle = glCreateShader(openGLStage);
			if (shaderHandle == 0)
			{
				Log::Warning("Failed to create %s shader for %s", stageName, module.Filepath.c_str());
				discardProgram();
				return;
			}

			const char* source = sourceText.c_str();
			glShaderSource(shaderHandle, 1, &source, nullptr);
			glCompileShader(shaderHandle);

			int status = 0;
			glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				int msgLength = 0;
				glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &msgLength);
				std::vector<char> msg(static_cast<size_t>(std::max(msgLength, 1)));
				glGetShaderInfoLog(shaderHandle, msgLength, nullptr, msg.data());
				Log::Warning("%s shader failed to compile at %s: %s", stageName, module.Filepath.c_str(), msg.data());
				glDeleteShader(shaderHandle);
				discardProgram();
				return;
			}

			glAttachShader(program, shaderHandle);
			shaderHandles.push_back(shaderHandle);
			compiledStages.push_back(module.Stage);
		}

		glLinkProgram(program);

		int status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (!status)
		{
			int msgLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgLength);
			std::vector<char> msg(static_cast<size_t>(std::max(msgLength, 1)));
			glGetProgramInfoLog(program, msgLength, nullptr, msg.data());
			Log::Warning("Shader program failed to link: %s", msg.data());
			discardProgram();
			return;
		}

		for (unsigned int shader : shaderHandles)
		{
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}
		shaderHandles.clear();
		m_RendererID = program;

		// List shader uniforms
		int count;
		glGetProgramiv(m_RendererID, GL_ACTIVE_UNIFORMS, (GLint*)&count);

		GLsizei length;  // Length of uniform name
		GLint size;      // Size of uniform
		GLenum type;     // Type of uniform
		char name[32];   // Name of uniform (NOTE: The name should not be longer than 32 char)
		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(m_RendererID, (GLuint)i, 32, &length, &size, &type, name);

			switch (type)
			{
			case GL_INT:
				m_Uniforms.push_back({ UniformType::Int, std::string(name) });
				break;
			case GL_FLOAT:
				m_Uniforms.push_back({ UniformType::Float, std::string(name) });
				break;
			case GL_FLOAT_VEC2:
				m_Uniforms.push_back({ UniformType::Float2, std::string(name) });
				break;
			case GL_FLOAT_VEC3:
				m_Uniforms.push_back({ UniformType::Float3, std::string(name) });
				break;
			case GL_FLOAT_VEC4:
				m_Uniforms.push_back({ UniformType::Float4, std::string(name) });
				break;
			case GL_FLOAT_MAT4:
				m_Uniforms.push_back({ UniformType::Mat4, std::string(name) });
				break;
			case GL_BOOL:
				m_Uniforms.push_back({ UniformType::Bool, std::string(name) });
				break;
			case GL_SAMPLER_2D:
				m_Uniforms.push_back({ UniformType::Tex2D, std::string(name) });
				break;
			case GL_SAMPLER_CUBE:
				m_Uniforms.push_back({ UniformType::TexCube, std::string(name) });
				break;
			default:
				break;
			}
		}
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt(const char* name, const int& i)
	{
		glUniform1i(GetUniformPosition(name), i);
	}

	void OpenGLShader::SetUniformUInt(const char* name, const unsigned int& i)
	{
		glUniform1ui(GetUniformPosition(name), i);
	}

	void OpenGLShader::SetUniformFloat(const char* name, const float& f)
	{
		glUniform1f(GetUniformPosition(name), f);
	}

	void OpenGLShader::SetUniformFloat3(const char* name, const float& f0, const float& f1, const float& f2)
	{
		glUniform3f(GetUniformPosition(name), f0, f1, f2);
	}

	void OpenGLShader::SetUniformFloat4(const char* name, const float& f0, const float& f1, const float& f2, const float& f3)
	{
		glUniform4f(GetUniformPosition(name), f0, f1, f2, f3);
	}

	void OpenGLShader::SetUniformMat4(const char* name, const mat4& m)
	{
		glUniformMatrix4fv(GetUniformPosition(name), 1, GL_TRUE, &(m[0][0]));
	}

	int OpenGLShader::GetUniformPosition(const char* name)
	{
		if (m_UniformLocations.find(name) != m_UniformLocations.end())
		{
			return m_UniformLocations[name];
		}

		int location = glGetUniformLocation(m_RendererID, name);
		if (location == -1)
		{
			Log::Warning("Uniform %s does not exist.", name);
		}
		m_UniformLocations[name] = location;
		return location;
	}
}
