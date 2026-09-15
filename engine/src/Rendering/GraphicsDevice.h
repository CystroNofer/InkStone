#pragma once

#include "Core.h"

namespace NXTN {
	class FrameBuffer;
	class IndexBuffer;
	class Renderer;
	class Shader;
	struct ShaderProgramDescriptor;
	class Texture2D;
	class UI;
	class VertexArray;
	class VertexArrayLayout;
	class VertexBuffer;

	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() = default;

		static bool Init();
		static GraphicsDevice* Get();

		virtual Renderer* CreateRenderer() = 0;
		virtual UI* CreateUI(unsigned int width, unsigned int height) = 0;

		virtual VertexBuffer* CreateVertexBuffer(const float* vertices, unsigned int count) = 0;
		virtual IndexBuffer* CreateIndexBuffer(const unsigned int* indices, unsigned int count) = 0;
		virtual VertexArray* CreateVertexArray(VertexBuffer* vertexBuffer, const VertexArrayLayout& layout) = 0;
		virtual Texture2D* CreateTexture2D(const std::string& filepath) = 0;
		virtual FrameBuffer* CreateFrameBuffer(unsigned int width, unsigned int height) = 0;
		virtual Shader* CreateShader(const ShaderProgramDescriptor& description) = 0;

	protected:
		GraphicsDevice() = default;

	private:
		static std::unique_ptr<GraphicsDevice> s_Device;
	};
}
