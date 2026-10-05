#include "pch.h"

#include "OpenGLShader.h"

namespace NXTN {
	ShaderUniformType ConvertGLShaderUniformType(GLenum type)
	{
		switch (type)
		{
		case GL_INT:          return ShaderUniformType::Int;
		case GL_UNSIGNED_INT: return ShaderUniformType::UInt;
		case GL_FLOAT:        return ShaderUniformType::Float;
		case GL_FLOAT_VEC2:   return ShaderUniformType::Float2;
		case GL_FLOAT_VEC3:   return ShaderUniformType::Float3;
		case GL_FLOAT_VEC4:   return ShaderUniformType::Float4;
		case GL_FLOAT_MAT4:   return ShaderUniformType::Mat4;
		case GL_BOOL:         return ShaderUniformType::Bool;
		case GL_SAMPLER_2D:   return ShaderUniformType::Tex2D;
		case GL_SAMPLER_CUBE: return ShaderUniformType::TexCube;
		default:              return ShaderUniformType::ErrorType;
		}
	}

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
		if (description.stages.empty())
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

		for (const ShaderStageDescriptor& module : description.stages)
		{
			if (std::find(compiledStages.begin(), compiledStages.end(), module.stage) != compiledStages.end())
			{
				Log::Warning("Shader program contains a duplicate shader stage at %s", module.filepath.c_str());
				discardProgram();
				return;
			}

			GLenum openGLStage = 0;
			const char* stageName = nullptr;
			switch (module.stage)
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
				Log::Warning("Unsupported shader stage at %s", module.filepath.c_str());
				discardProgram();
				return;
			}

			std::ifstream input(module.filepath, std::ios::in | std::ios::binary);
			if (!input)
			{
				Log::Warning("Cannot open %s shader source at %s", stageName, module.filepath.c_str());
				discardProgram();
				return;
			}

			std::ostringstream sourceStream;
			sourceStream << input.rdbuf();
			if (input.bad())
			{
				Log::Warning("Failed to read %s shader source at %s", stageName, module.filepath.c_str());
				discardProgram();
				return;
			}

			std::string sourceText = sourceStream.str();
			if (sourceText.empty())
			{
				Log::Warning("%s shader source is empty at %s", stageName, module.filepath.c_str());
				discardProgram();
				return;
			}

			unsigned int shaderHandle = glCreateShader(openGLStage);
			if (shaderHandle == 0)
			{
				Log::Warning("Failed to create %s shader for %s", stageName, module.filepath.c_str());
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
				Log::Warning("%s shader failed to compile at %s: %s", stageName, module.filepath.c_str(), msg.data());
				glDeleteShader(shaderHandle);
				discardProgram();
				return;
			}

			glAttachShader(program, shaderHandle);
			shaderHandles.push_back(shaderHandle);
			compiledStages.push_back(module.stage);
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
		char name[64];   // Name of uniform (NOTE: The name should not be longer than 64 char)
		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(m_RendererID, (GLuint)i, 64, &length, &size, &type, name);
			GLint location = glGetUniformLocation(m_RendererID, name);

			switch (type)
			{
			case GL_INT:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Int, location };
				break;
			case GL_FLOAT:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Float, location };
				break;
			case GL_FLOAT_VEC2:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Float2, location };
				break;
			case GL_FLOAT_VEC3:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Float3, location };
				break;
			case GL_FLOAT_VEC4:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Float4, location };
				break;
			case GL_FLOAT_MAT4:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Mat4, location };
				break;
			case GL_BOOL:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Bool, location };
				break;
			case GL_SAMPLER_2D:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::Tex2D, location };
				break;
			case GL_SAMPLER_CUBE:
				m_UniformInfo[std::string(name) ] = { ShaderUniformType::TexCube, location };
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

	// Shader uniform setters do NOT check types for now
	// Trust owners (Material) to reject mistyped settings
	// Subject to change in the future

	void OpenGLShader::SetUniformInt(const std::string& name, int i)
	{
		glUniform1i(GetUniformPosition(name), i);
	}

	void OpenGLShader::SetUniformUInt(const std::string& name, unsigned int i)
	{
		glUniform1ui(GetUniformPosition(name), i);
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float f)
	{
		glUniform1f(GetUniformPosition(name), f);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const vec3& f3)
	{
		glUniform3f(GetUniformPosition(name), f3.x, f3.y, f3.z);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, float f0, float f1, float f2)
	{
		glUniform3f(GetUniformPosition(name), f0, f1, f2);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const vec4& f4)
	{
		glUniform4f(GetUniformPosition(name), f4.x, f4.y, f4.z, f4.w);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, float f0, float f1, float f2, float f3)
	{
		glUniform4f(GetUniformPosition(name), f0, f1, f2, f3);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const mat4& m)
	{
		glUniformMatrix4fv(GetUniformPosition(name), 1, GL_TRUE, &(m[0][0]));
	}

	void OpenGLShader::SetUniformTex2D(const std::string& name, unsigned int slot)
	{
		// Texture sampler is set by glUniform1i instead of the unsigned counterpart
		glUniform1i(GetUniformPosition(name), slot);
	}

	std::vector<ShaderUniformDescriptor> OpenGLShader::GetUniformList() const
	{
		return std::ranges::to<std::vector>(
			std::views::transform(
				m_UniformInfo,
				[](const std::pair<const std::string, OpenGLShaderUniformInfo>& entry)
				{
					return ShaderUniformDescriptor{
						.name = entry.first,
						.type = entry.second.type
					};
				}
			)
		);
	}

	GLint OpenGLShader::GetUniformPosition(const std::string& name)
	{
		if (m_UniformInfo.find(name) != m_UniformInfo.end())
		{
			return m_UniformInfo[name].location;
		}

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
		{
			Log::Warning("Uniform %s does not exist.", name.c_str());
			return -1;
		}

		GLuint index = glGetProgramResourceIndex(
			m_RendererID, GL_UNIFORM, name.c_str()
		);
		if (index == GL_INVALID_INDEX)
		{
			Log::Warning("Cannot inspect uniform %s.", name.c_str());
			return -1;
		}

		const GLenum property = GL_TYPE;
		GLint glType = 0;
		glGetProgramResourceiv(
			m_RendererID,
			GL_UNIFORM,
			index,
			1, &property,
			1, nullptr, &glType
		);

		m_UniformInfo.emplace(
			name,
			OpenGLShaderUniformInfo{
				ConvertGLShaderUniformType(static_cast<GLenum>(glType)),
				location
			}
		);

		return location;
	}
}
