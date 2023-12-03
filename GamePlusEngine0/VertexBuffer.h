#pragma once

#include <stdint.h>

namespace IceEngine
{
	class VertexBuffer 
	{
	public:
		VertexBuffer() = default;
		VertexBuffer(const void* data, size_t size);
		~VertexBuffer();

		void SetData(const void* data, size_t size);
		void SetDataDynamics(size_t size);

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t rendererID;
	};
}