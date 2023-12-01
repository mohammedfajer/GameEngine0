#pragma once

#include <stdint.h>

namespace IceEngine
{
	class IndexBuffer {
	public:
		IndexBuffer() = default;
		IndexBuffer(const uint32_t* data, size_t count);
		~IndexBuffer();

		void SetData(const uint32_t* data, size_t count);

		void Bind() const;
		void Unbind() const;

		size_t GetCount() const { return m_count; }

	private:
		uint32_t m_EBO_ID;
		size_t m_count;
	};
}