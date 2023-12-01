#pragma once

#include "Shader.h"  // Assuming you have a Shader class
#include "GameObject.h"  // Assuming you have a GameObject class
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferElement.h"
#include "VertexBufferLayout.h"

namespace IceEngine 
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoords;
	};

	class SpriteRendererSystem
	{

	public:
		SpriteRendererSystem();
		~SpriteRendererSystem();

		void SetShader(Shader* shader);

		void RenderGameObject(GameObject& gameObject);

	private:
		Shader* m_shader;
		
		VertexArray m_vertexArray;
		IndexBuffer m_indexBuffer;

		VertexBuffer m_vertexBuffer;
		VertexBufferLayout m_vertexBufferLayout;
	};
}

