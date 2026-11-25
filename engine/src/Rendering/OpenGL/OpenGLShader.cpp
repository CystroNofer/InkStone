#include "pch.h"

#include "OpenGLShader.h"

namespace NXTN {
	namespace {
		ShaderVariableType ConvertGLShaderUniformType(GLenum type)
		{
			switch (type)
			{
			case GL_INT:          return ShaderVariableType::Int;
			case GL_UNSIGNED_INT: return ShaderVariableType::UInt;
			case GL_FLOAT:        return ShaderVariableType::Float;
			case GL_FLOAT_VEC2:   return ShaderVariableType::Float2;
			case GL_FLOAT_VEC3:   return ShaderVariableType::Float3;
			case GL_FLOAT_VEC4:   return ShaderVariableType::Float4;
			case GL_FLOAT_MAT4:   return ShaderVariableType::Mat4;
			case GL_BOOL:         return ShaderVariableType::Bool;
			case GL_SAMPLER_2D:   return ShaderVariableType::Tex2D;
			case GL_SAMPLER_CUBE: return ShaderVariableType::TexCube;
			default:              return ShaderVariableType::ErrorType;
			}
		}

		std::string ShaderVariableTypeToString(ShaderVariableType type)
		{
			switch (type)
			{
			case ShaderVariableType::Int:          return "int";
			case ShaderVariableType::UInt:         return "uint";
			case ShaderVariableType::Float:        return "float";
			case ShaderVariableType::Float2:       return "vec2";
			case ShaderVariableType::Float3:       return "vec3";
			case ShaderVariableType::Float4:       return "vec4";
			case ShaderVariableType::Mat4:         return "mat4";
			case ShaderVariableType::Bool:         return "bool";
			case ShaderVariableType::Tex2D:        return "sampler2D";
			case ShaderVariableType::TexCube:      return "samplerCube";
			default:                               return "unknown";
			}
		}
	}

	std::expected<OpenGLShader*, std::string> OpenGLShader::Create(
		const std::vector<ShaderStageIntermediateDescriptor>& stageIntermediates
	)
	{
		// Assume intermediates are valid from the caller

		unsigned int program = glCreateProgram();
		if (program == 0)
		{
			return std::unexpected("Failed to create shader program");
		}
		std::vector<unsigned int> compiledShaders;
		const std::function<void()> discardProgram = [&]()
			{
				for (unsigned int shader : compiledShaders)
				{
					glDetachShader(program, shader);
					glDeleteShader(shader);
				}
				compiledShaders.clear();
				glDeleteProgram(program);
			};
		std::vector<ShaderStage> compiledStages;

		for (const ShaderStageIntermediateDescriptor& intermediate : stageIntermediates) {
			if (std::find(
				compiledStages.begin(),
				compiledStages.end(),
				intermediate.stage
			) != compiledStages.end())
			{
				discardProgram();
				return std::unexpected("Duplicate shader stage found");
			}

			// Emit GLSL code
			std::string stageCode = "#version 450 core\n\n";
			// Input
			if (intermediate.stage == ShaderStage::Vertex)
			{
				size_t location = 0;
				for (const ShaderVariableDescriptor& input : intermediate.inputs) {
					stageCode +=
						"layout(location = " +
						std::to_string(location++) +
						") in " +
						ShaderVariableTypeToString(input.type) +
						" " +
						input.name +
						";\n";
				}
			}
			else {
				for (const ShaderVariableDescriptor& input : intermediate.inputs) {
					stageCode +=
						"in " +
						ShaderVariableTypeToString(input.type) +
						" " +
						input.name +
						";\n";
				}
			}

			// Uniform
			stageCode +=
R"(
uniform mat4 NXTN_MATRIX_M;
uniform mat4 NXTN_MATRIX_VP;
uniform mat4 NXTN_MATRIX_MVP;
)";
			for (const ShaderVariableDescriptor& uniform : intermediate.uniforms) {
				stageCode +=
					"uniform " +
					ShaderVariableTypeToString(uniform.type) +
					" " +
					uniform.name +
					";\n";
			}

			// Output
			stageCode += "\n";
			for (const ShaderVariableDescriptor& output : intermediate.outputs) {
				stageCode +=
					"out " +
					ShaderVariableTypeToString(output.type) +
					" " +
					output.name +
					";\n";
			}

			// Code
			stageCode += "\n";
			stageCode += intermediate.code;

			Log::Info(stageCode);

			// Compile
			GLenum openGLStage;
			std::string stageName;  // For error messages
			switch (intermediate.stage)
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
				discardProgram();
				return std::unexpected("Unsupported shader stage");
			}

			unsigned int shaderHandle = glCreateShader(openGLStage);
			if (shaderHandle == 0)
			{
				discardProgram();
				return std::unexpected("Failed to create OpenGL shader handle for " + stageName + " shader");
			}

			const char* stageCodeCStr = stageCode.c_str();
			glShaderSource(shaderHandle, 1, &stageCodeCStr, nullptr);
			glCompileShader(shaderHandle);

			int status = 0;
			glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				int msgLength = 0;
				glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &msgLength);  // Error message length
				std::vector<char> msg(static_cast<size_t>(std::max(msgLength, 1)));
				glGetShaderInfoLog(shaderHandle, msgLength, nullptr, msg.data());
				glDeleteShader(shaderHandle);
				discardProgram();
				return std::unexpected(
					"Failed to compile " + stageName + " shader: " + std::string(msg.data())
				);
			}

			glAttachShader(program, shaderHandle);
			compiledShaders.push_back(shaderHandle);
			compiledStages.push_back(intermediate.stage);
		}

		glLinkProgram(program);

		int status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (!status)
		{
			int msgLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &msgLength);  // Error message length
			std::vector<char> msg(static_cast<size_t>(std::max(msgLength, 1)));
			glGetProgramInfoLog(program, msgLength, nullptr, msg.data());
			discardProgram();
			return std::unexpected("Failed to link shader program:\n" + std::string(msg.data()));
		}

		for (unsigned int shader : compiledShaders)
		{
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}
		compiledShaders.clear();

		// Uniform reflection
		std::unordered_map<std::string, OpenGLShaderUniformInfo> uniforms;
		int uniformCount;  // Number of ACTIVE uniforms
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, (GLint*)&uniformCount);

		GLsizei length;  // Length of uniform name
		GLint size;      // Size of uniform
		GLenum type;     // Type of uniform
		char name[64];   // Name of uniform (NOTE: The name should not be longer than 64 char)
		for (int i = 0; i < uniformCount; i++)
		{
			glGetActiveUniform(program, (GLuint)i, 64, &length, &size, &type, name);
			GLint location = glGetUniformLocation(program, name);

			switch (type)
			{
			case GL_INT:
				uniforms[std::string(name)] = { ShaderVariableType::Int, location };
				break;
			case GL_FLOAT:
				uniforms[std::string(name)] = { ShaderVariableType::Float, location };
				break;
			case GL_FLOAT_VEC2:
				uniforms[std::string(name)] = { ShaderVariableType::Float2, location };
				break;
			case GL_FLOAT_VEC3:
				uniforms[std::string(name)] = { ShaderVariableType::Float3, location };
				break;
			case GL_FLOAT_VEC4:
				uniforms[std::string(name)] = { ShaderVariableType::Float4, location };
				break;
			case GL_FLOAT_MAT4:
				uniforms[std::string(name)] = { ShaderVariableType::Mat4, location };
				break;
			case GL_BOOL:
				uniforms[std::string(name)] = { ShaderVariableType::Bool, location };
				break;
			case GL_SAMPLER_2D:
				uniforms[std::string(name)] = { ShaderVariableType::Tex2D, location };
				break;
			case GL_SAMPLER_CUBE:
				uniforms[std::string(name)] = { ShaderVariableType::TexCube, location };
				break;
			default:
				Log::Warning("Active uniform of unknown type: %s", name);
				break;
			}
		}

		return new OpenGLShader(program, std::move(uniforms));
	}

	OpenGLShader::OpenGLShader(
		unsigned int id,
		std::unordered_map<std::string, OpenGLShaderUniformInfo>&& uniforms
	)
		: m_RendererID(id), m_UniformInfo(std::move(uniforms))
	{
	}

	OpenGLShader::~OpenGLShader()
	{
		if (m_RendererID != 0)
		{
			glDeleteProgram(m_RendererID);
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

	std::vector<ShaderVariableDescriptor> OpenGLShader::GetUniformList() const
	{
		return std::ranges::to<std::vector>(
			std::views::transform(
				m_UniformInfo,
				[](const std::pair<const std::string, OpenGLShaderUniformInfo>& entry)
				{
					return ShaderVariableDescriptor{
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
