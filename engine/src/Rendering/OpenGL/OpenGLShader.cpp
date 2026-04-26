#include "pch.h"

#include "OpenGLShader.h"

namespace NXTN {
	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_Filepath(filepath)
	{
		Compile();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Compile()
	{
		// Create a program object.
		m_RendererID = glCreateProgram();

		// File content
		std::string content;
		// File stream
		std::ifstream in(m_Filepath, std::ios::in, std::ios::binary);
		if (!in)
		{
			Log::Error("Cannot open shader source at %s", m_Filepath.c_str());
		}

		// Load file content
		in.seekg(0, std::ios::end);
		content.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&content[0], content.size());

		// Shader handles for each shader
		std::vector<unsigned int> shaderHandles;

		const char* headerToken = "#type ";
		// Length of "#type "
		size_t headerLength = 6;

		size_t start = content.find(headerToken, 0);

		int status = 0;
		while (start != std::string::npos && start < content.size() - 1)
		{
			// Start of shader type
			start += headerLength;
			// End of line
			size_t end = content.find_first_of("\r\n", start);
			if (end == std::string::npos)
			{
				Log::Error("Shader syntax error: No shader type provided");
				continue;
			}
			// Shader type
			std::string shaderType = content.substr(start, end - start);

			// Shader body
			start = content.find_first_not_of("\r\n", end);
			if (start == std::string::npos)
			{
				Log::Error("Shader syntax error: No shader body provided");
				continue;
			}
			end = content.find(headerToken, start);
			if (end == std::string::npos)
			{
				end = content.size() - 1;
			}
			std::string shaderBody = content.substr(start, end - start);

			// Increment
			start = end;

			// Create an empty shader handle
			unsigned int shaderHandle = 0;
			//std::cout << "#type " << shaderType << std::endl;
			if (shaderType == "vertex")
			{
				shaderHandle = glCreateShader(GL_VERTEX_SHADER);
			}
			else if (shaderType == "fragment")
			{
				shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
			}
			else
			{
				Log::Error("Unsupported shader type: %s", shaderType.c_str());
				continue;
			}

			// Send the vertex shader source code to GL
			// Note that std::string.c_str is NULL character terminated (Idk how this matters but whatever)
			const char* source = shaderBody.c_str();
			//std::cout << source << std::endl;
			glShaderSource(shaderHandle, 1, &source, 0);

			// Compile the vertex shader
			glCompileShader(shaderHandle);

			// Check compilation status
			glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				// Get log information string length
				int msgLength = 0;
				glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &msgLength);

				// The maxLength includes the NULL character
				char* msg = (char*)malloc(msgLength * sizeof(char));
				glGetShaderInfoLog(shaderHandle, msgLength, &msgLength, &msg[0]);

				// Log information
				Log::Error("Shader (%s) failed to compile: %s", shaderType.c_str(), msg);
				free(msg);

				// Discard the shader
				glDeleteShader(shaderHandle);

				continue;
			}

			// Attach the shaders to the program
			// Note that once a shader is attached to a program,
			// Calling glDeleteShader will instead defer the deletion
			// Until the program attached to is deleted
			// This is similar shared_ptr, which uses a reference counter
			glAttachShader(m_RendererID, shaderHandle);

			shaderHandles.push_back(shaderHandle);
		}

		// Link the program
		if (shaderHandles.size() > 0)
		{
			glLinkProgram(m_RendererID);
		}
		else
		{
			glDeleteProgram(m_RendererID);
			Log::Error("No shader compiled, shader program aborted");
		}

		// Check linking status
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &status);
		if (!status)
		{
			// Get log information string length
			int msgLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &msgLength);

			// The maxLength includes the NULL character
			char* msg = (char*)malloc(msgLength * sizeof(char));
			glGetProgramInfoLog(m_RendererID, msgLength, &msgLength, &msg[0]);

			// Log information
			Log::Error("Shader program failed to link: %s", msg);
			free(msg);

			// Delete the program
			glDeleteProgram(m_RendererID);

			// Delete the shaders
			for (unsigned int sh : shaderHandles)
			{
				glDeleteShader(sh);
			}

			return;
		}

		// Detach the shaders
		for (unsigned int sh : shaderHandles)
		{
			glDetachShader(m_RendererID, sh);
		}

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
