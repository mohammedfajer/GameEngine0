#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"

namespace IceEngine
{
	class SpriteBatch
	{
	public:
		void Begin();
		void Render();
		void End();
	private:
		Shader* m_shader;

		VertexArray m_vertexArray;
		IndexBuffer m_indexBuffer;

		VertexBuffer m_vertexBuffer;
		VertexBufferLayout m_vertexBufferLayout;

		OrthographicCameraComponent* m_cameraComponent;
		TransformComponent* m_transformComponent;
	};
}