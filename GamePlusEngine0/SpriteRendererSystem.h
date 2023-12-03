#pragma once

#include "Shader.h"  // Assuming you have a Shader class
#include "GameObject.h"  // Assuming you have a GameObject class
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferElement.h"
#include "VertexBufferLayout.h"

#include <vector>


namespace IceEngine 
{
	

	class SpriteRendererSystem
	{

	public:
	
		SpriteRendererSystem(const std::vector<glm::vec2>& textureCoords = { {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f} });
		~SpriteRendererSystem();

		void SetShader(Shader* shader);

		void RenderGameObject(GameObject& gameObject);

	private:
		Shader* m_shader;
		
		VertexArray m_vertexArray;
		IndexBuffer m_indexBuffer;

		VertexBuffer m_vertexBuffer;
		VertexBufferLayout m_vertexBufferLayout;

		std::vector<glm::vec2> m_textureCoords;
	};
}

