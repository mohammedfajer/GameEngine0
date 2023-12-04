#pragma once

#include "Shader.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "OrthographicCameraComponent.h"
#include "TransformComponent.h"
#include <array>

namespace IceEngine
{
#pragma pack(push, 1)
	struct MyVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 TexCoords;
		float TexID;
	};
#pragma pack(pop)


	class SpriteBatch
	{
	public:
		void Begin();
		void Render();
		void Update(float deltaTime);
		void End();
	private:
		Shader* m_shader;

		VertexArray m_vertexArray;
		IndexBuffer m_indexBuffer;

		VertexBuffer m_vertexBuffer;
		VertexBufferLayout m_vertexBufferLayout;

		OrthographicCameraComponent* m_cameraComponent;
		TransformComponent* m_transformComponent;

		uint32_t m_coinTextureId;
		uint32_t m_playerTextureId;

		std::array<MyVertex, 1000> vertices;
		MyVertex* buffer;
	};
}