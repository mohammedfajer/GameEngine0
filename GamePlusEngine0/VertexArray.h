#pragma once
#include <stdint.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <glm/glm.hpp>

namespace IceEngine
{

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoords;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout);
		void Bind() const;
		void Unbind() const;

	private:
		uint32_t rendererID;

	};
}