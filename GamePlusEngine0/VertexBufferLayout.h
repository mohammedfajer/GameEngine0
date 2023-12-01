#pragma once

#include <stdint.h>
#include <vector>
#include "VertexBufferElement.h"

namespace IceEngine
{
	class VertexBufferLayout
	{
	public:
		VertexBufferLayout();

		template<typename T>
		void Push(uint32_t count);

		const std::vector<VertexBufferElement>& GetElements() const { return m_elements; }
		uint32_t GetStride() const { return m_stride; }
	private:
		std::vector<VertexBufferElement> m_elements;
		uint32_t m_stride;
	};

	
	
}
