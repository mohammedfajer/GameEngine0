#include "SpritesheetScene.h"

#include "TextureLoader.h"
#include "Color.h"

#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "OrthographicCameraComponent.h"
#include "Defines.h"

#include "SpritsheetLoader.h"
#include "InputManager.h"
#include "Renderer.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace TopDownShooter
{
	const std::string vertex_shader = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec4 aColor;
		layout (location = 2) in vec2 aTexCoord;
		layout (location = 3) in float aTexIdx;

		out vec4 OurColor;
		out vec2 TexCoord;
		out float TexIdx;

		uniform mat4 projection;
		uniform mat4 view;
	
    
		void main()
		{
			gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			OurColor = aColor;
			TexCoord = aTexCoord;
			TexIdx = aTexIdx;
		}
	)";

	const std::string fragment_shader = R"(
		#version 330 core
		out vec4 FragColor;

		in vec4 OurColor;
		in vec2 TexCoord;
		in float TexIdx;

		uniform sampler2D u_Textures[32];

		void main()
		{
			int index = int(TexIdx);

			
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;
			
		}
	)";

	float ConvertToUV(float coord, int size)
	{
		return coord / static_cast<float>(size);  // Adjust for 0-based indexing
	}

	

	void GetTileInfoFromFile(std::vector<Tile_Info> &tilesInfo) {
		auto Text = IceEngine::LoadTextFile("./data/Asset0/0x72_DungeonTilesetII_v1.6/tile_list_v1.6");

		if (!Text) return;

		std::string line = "";
		int indexStr = 1;

		for (int i = 0; Text[i] != '\0'; i++) {
			if (Text[i] != '\n') line += Text[i];
			else {
				line += '\n';
				std::array<std::string, 5> lineInfo;
				// 0 1 2 3 4      i++ % 4
				int inc = 0;
				int structIndx = 0;
				std::string subLineStr = "";
				while (true) {
					if (line[inc] == ' ' || line[inc] == '\n') {
						switch (structIndx) {
						case 0: lineInfo[0] = subLineStr; break;
						case 1: lineInfo[1] = subLineStr; break;
						case 2: lineInfo[2] = subLineStr; break;
						case 3: lineInfo[3] = subLineStr; break;
						case 4: {
							lineInfo[4] = subLineStr;

							Tile_Info tile_info = {
								lineInfo[0],			// name
								std::stoi(lineInfo[1]), // x_offset
								std::stoi(lineInfo[2]), // y_offset
								std::stoi(lineInfo[3]), // width
								std::stoi(lineInfo[4])  // height 
							};
							tilesInfo.push_back(tile_info);

							lineInfo[0] = lineInfo[1] = lineInfo[2] = lineInfo[3] = lineInfo[4] = "";
						} break;
						}

						structIndx = structIndx + 1 % 4;
						subLineStr = "";

						if (line[inc] == '\n') break;
					}
					else {
						subLineStr += line[inc];
					}
					inc++;
				}


				line = "";

			}
		}
	}

	SpriteSheetScene::SpriteSheetScene()
	{
		m_name = "SpriteSheetScene";

		// Load The sprite sheet tile set file tile_list_v1.6

		// 		
	
		GetTileInfoFromFile(m_tilesInfo);

		// load sprite sheet texture
		IceEngine::Texture2D tilesetTexture = IceEngine::TextureLoader::LoadTexture("./data/Asset0/0x72_DungeonTilesetII_v1.6/0x72_DungeonTilesetII_v1.6.png");
		m_tilesetTexture = tilesetTexture.id;

		for (auto &tile : m_tilesInfo) {
			IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "% % % % %", tile.name, tile.x_offset, tile.y_offset, tile.width, tile.height);

			tile.textureCoords = IceEngine::GetTextureCoords(tilesetTexture, tile.y_offset, tile.x_offset, tile.width, tile.height);	
		}

		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		m_shader->Bind();
		auto loc = glGetUniformLocation(m_shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++) {
			samplers[i] = i;
		}
		glUniform1iv(loc, 32, samplers);

		IceEngine::Renderer::Init();

		m_cameraComponent = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		
		//m_prevMousePos = IceEngine::InputManager::Instance().GetMousePosition();
		m_spriteIndex = 0;
	}

	SpriteSheetScene::~SpriteSheetScene()
	{
		
	}

	void SpriteSheetScene::Update(float deltaTime)
	{
		// Update the base scene
		Scene::Update(deltaTime);
	
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_L))
		{
			m_cameraComponent->zoom += 0.05f;
			
		}
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_K))
		{
			m_cameraComponent->zoom -= 0.05f;
		}
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_P))
		{
			m_cameraComponent->zoom = 1.0f;
		}

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_N))
		{
			m_spriteIndex = m_spriteIndex + 1;
			m_spriteIndex %= m_tilesInfo.size() - 1;
		}

		
	}

	std::string mat4ToString(const glm::mat4 &matrix) {
		std::ostringstream oss;

		// glm::mat4 is a column-major matrix
		const float *matPtr = glm::value_ptr(matrix);

		// Set precision for floating-point numbers
		oss << std::setprecision(4) << std::fixed;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				oss << matPtr[j * 4 + i] << "\t";
			}
			oss << "\n";
		}

		return oss.str();
	}

	void SpriteSheetScene::Render()
	{

		IceEngine::Color::SetClearColor({ 29, 17, 22 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader->Bind();

		IceEngine::Renderer::ResetStats();
		IceEngine::Renderer::BeginBatch();


	
		glm::vec2 tileSize = {(float)m_tilesInfo[m_spriteIndex].width , (float)m_tilesInfo[m_spriteIndex].height };
		glm::vec2 position = { 100, 100 };
		float tileRotation = 0.0f;

		// Translation matrix with the pivot adjustment
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x - 0.5f * tileSize.x, position.y - 0.5f * tileSize.y, 0.0f));

		// Rotation matrix (assuming rotation is in radians)
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tileRotation), glm::vec3(0.0f, 0.0f, 1.0f));

		// Scale matrix
		glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(tileSize.x, -tileSize.y, 1.0f));

		// Combine the transformations
		glm::mat4 modelMatrix = translation * rotationMatrix * scaling;

		IceEngine::Logger::Instance().Log(mat4ToString(modelMatrix), IceEngine::LogLevel::SUCCESS);

		tileSize = { (float)m_tilesInfo[m_spriteIndex].width / SCREEN_WIDTH, (float)m_tilesInfo[m_spriteIndex].height / SCREEN_HEIGHT };

		

		IceEngine::Renderer::DrawQuad(position, tileSize , modelMatrix, m_tilesetTexture, m_tilesInfo[m_spriteIndex].textureCoords);


		auto S = IceEngine::Renderer::GetStats();

		IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "DrawCount = %, QuadCount = %", S.DrawCount, S.QuadCount);
		IceEngine::Renderer::EndBatch();

		// Setup Camera View Transform
		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		//m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

		IceEngine::Renderer::Flush();
	}
}








