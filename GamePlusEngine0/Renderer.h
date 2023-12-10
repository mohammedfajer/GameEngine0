#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace IceEngine
{

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginBatch();
		static void EndBatch();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, uint32_t textureID);

		static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, uint32_t textureID, std::vector<glm::vec2> textureCoords);
		static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::mat4 &modelMatrix, uint32_t textureID, std::vector<glm::vec2> textureCoords);
		


		
		// Stats
		struct Stats
		{
			uint32_t DrawCount = 0;
			uint32_t QuadCount = 0;

			Stats() = default;
		};

		static const Stats& GetStats();
		static void ResetStats();

	};
}

