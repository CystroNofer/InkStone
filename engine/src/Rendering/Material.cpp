#include "pch.h"

#include "Material.h"

#include "Asset/TextureManager.h"

namespace NXTN {
	Material::Material(Handle<Shader> shaderHandle)
		: m_ShaderHandle(shaderHandle)
	{
		for (const ShaderUniformDescriptor& uniform : ShaderManager::Get(shaderHandle)->GetUniformList())
		{
			switch (uniform.type)
			{
			case ShaderUniformType::Int:
				m_Parameters[uniform.name] = { uniform.type, 0 };
				break;
			case ShaderUniformType::UInt:
				m_Parameters[uniform.name] = { uniform.type, 0u };
				break;
			case ShaderUniformType::Float:
				m_Parameters[uniform.name] = { uniform.type, 0.0f };
				break;
			case ShaderUniformType::Float3:
				m_Parameters[uniform.name] = { uniform.type, vec3(0.0f) };
				break;
			case ShaderUniformType::Float4:
				m_Parameters[uniform.name] = { uniform.type, vec4(0.0f) };
				break;
			case ShaderUniformType::Mat4:
				m_Parameters[uniform.name] = { uniform.type, mat4(1.0f) };
				break;
			case ShaderUniformType::Tex2D:
				m_Parameters[uniform.name] = { uniform.type, Handle<Texture2D>() };
				break;
			default:
				Log::Warning("Unsupported uniform type for material: %s", uniform.name.c_str());
				break;
			}
			//std::cout << "Material uniform: " << uniform.name << ", type: " << static_cast<int>(uniform.type) << std::endl;
		}
	}

	void Material::Bind()
	{
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		shader->Bind();

		unsigned int textureSlot = 0;

		for (const auto& [name, param] : m_Parameters)
		{
			switch (param.type)
			{
			case ShaderUniformType::Int:
				shader->SetUniformInt(name, std::get<int>(param.value));
				break;
			case ShaderUniformType::UInt:
				shader->SetUniformUInt(name, std::get<unsigned int>(param.value));
				break;
			case ShaderUniformType::Float:
				shader->SetUniformFloat(name, std::get<float>(param.value));
				break;
			case ShaderUniformType::Float3:
				shader->SetUniformFloat3(name, std::get<vec3>(param.value).x, std::get<vec3>(param.value).y, std::get<vec3>(param.value).z);
				break;
			case ShaderUniformType::Float4:
				shader->SetUniformFloat4(name, std::get<vec4>(param.value).x, std::get<vec4>(param.value).y, std::get<vec4>(param.value).z, std::get<vec4>(param.value).w);
				break;
			case ShaderUniformType::Mat4:
				shader->SetUniformMat4(name, std::get<mat4>(param.value));
				break;
			case ShaderUniformType::Tex2D:
			{
				Texture2D* texPtr = TextureManager::Get(std::get<Handle<Texture2D>>(param.value));
				if (texPtr == nullptr)
				{
					Log::Warning("Material texture is invalid: %s", name.c_str());
					break;
				}
				texPtr->Bind(textureSlot);
				shader->SetUniformTex2D(name, textureSlot);
				textureSlot++;
				break;
			}
			default:
				Log::Warning("Unsupported uniform type for material: %s", name.c_str());
				break;
			}
		}
	}

	void Material::Unbind()
	{
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		shader->Unbind();
	}

	void Material::SetUniformInt(const std::string& name, int i)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Int)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Int, i };
	}

	void Material::SetUniformUInt(const std::string& name, unsigned int i)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::UInt)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::UInt, i };
	}

	void Material::SetUniformFloat(const std::string& name, float f)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Float)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Float, f };
	}

	void Material::SetUniformFloat3(const std::string& name, const vec3& f3)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Float3)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Float3, f3 };
	}

	void Material::SetUniformFloat4(const std::string& name, const vec4& f4)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Float4)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Float4, f4 };
	}

	void Material::SetUniformMat4(const std::string& name, const mat4& m)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Mat4)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Mat4, m };
	}

	void Material::SetUniformTex2D(const std::string& name, Handle<Texture2D> texHandle)
	{
		if (m_Parameters.find(name) == m_Parameters.end())
		{
			Log::Warning("Uniform name %s not found in material.", name.c_str());
			return;
		}
		if (m_Parameters[name].type != ShaderUniformType::Tex2D)
		{
			Log::Warning("Uniform %s type mismatch.", name.c_str());
			return;
		}
		Shader* shader = ShaderManager::Get(m_ShaderHandle);
		if (!shader)
		{
			Log::Warning("Material shader is invalid.");
			return;
		}
		m_Parameters[name] = { ShaderUniformType::Tex2D, texHandle };
	}
}
