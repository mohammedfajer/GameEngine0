#include "SpritesheetScene.h"
#include "TextureLoader.h"
#include "graphics.h"
#include "TransformComponent.h"
#include "SpriteRendererComponent.h"
#include "OrthographicCameraComponent.h"
#include "Defines.h"
#include "SpritsheetLoader.h"
#include "InputManager.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>  // Include this for glm::lerp
#include "DebugDraw.h"
#include "FontSystem.h"


// Dear IMGUI
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#include "core.h"


namespace TopDownShooter {
	
	static IceEngine::DebugPoint point;
	static IceEngine::DebugCircle circle;
	static IceEngine::DebugCircle circle2;
	static IceEngine::DebugLine line;
	static IceEngine::DebugRect rect;
	static IceEngine::DebugRect rect2;
	static IceEngine::DebugLineQuad lineQuad;
	
	static IceEngine::SDFRoundedRectangle SDFrect;
	static IceEngine::DebugTriangle triangle;
	static IceEngine::ClosedPolygon closedPolygon;

	static IceEngine::Text theText;

	static IceEngine::RoundedRect TheRect;
	static IceEngine::DebugArrow arrow;

	// Our state
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    struct Player {
        IceEngine::Animation animation;
        vec2 position;
        real32 scale_factor;
        
        vec2 forward;
        vec2 upward;
        
      
        
        void load() {
            IceEngine::setup_animation(&animation, "./data/Asset0/wizzard_m.png", 7, 3);
        
            forward = {1.0f, 0.0f};
            upward = {0.0f, -1.0f};
            
            
            
        }
        
        void update(IceEngine::OrthographicCameraComponent *cam) {
            IceEngine::update_animation(&animation);
            
            // Movement
            if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_W)) 
		    {
			  position.y -= 5.0f;
		    }
		    if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_S)) 
		    {
			  position.y += 5.0f;
		    }
		    if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_D)) 
		    {
			  position.x += 5.0f;
		    }
		    if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_A)) 
		    {
			  position.x -= 5.0f;
		    }
		    
		    
		    // Compute dot product between forward and (player to mouse pos) vector
		    // if positive flip = false else flip = true.
		  
		    
		   mat4 model = glm::translate(mat4(1.0f), vec3(position.x, position.y, 0.0f));
	       mat4 view  = cam->GetViewMatrix();
	      
	       
	      
	       vec4 clip_space_point = cam->projection * view * model * vec4(1.0f);
	       
	       vec2 screen_space_point;
	       screen_space_point.x = (clip_space_point.x + 1.0f) * 0.5f * SCREEN_WIDTH;
	       screen_space_point.y = (1.0f - clip_space_point.y) * 0.5f * SCREEN_HEIGHT;
	       
		    
		    
		    
		    vec2 mousePos = IceEngine::InputManager::Instance().GetMousePosition();
		    
		    vec2 vecFromPlayerToMouse = mousePos - screen_space_point;
		    auto normalizedVec = glm::normalize(vecFromPlayerToMouse);
		     
		    animation.flip = glm::dot(forward, vecFromPlayerToMouse) < 0.0f;
		    
        }
        
        void draw() {
            IceEngine::render_animation(&animation, position, 0.0f, scale_factor);
        }
    };
    
    static Player player;
	static glm::vec2 mousePos;

	static const std::string vertex_shader = R"(
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
	
    
		void main() {
			gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			OurColor = aColor;
			TexCoord = aTexCoord;
			TexIdx = aTexIdx;
		}
	)";

	static const std::string fragment_shader = R"(
		#version 330 core
		out vec4 FragColor;

		in vec4 OurColor;
		in vec2 TexCoord;
		in float TexIdx;

		uniform sampler2D u_Textures[32];

		void main() {
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;			
		}
	)";

	float ConvertToUV(float coord, int size) {
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
		
		//auto A = IceEngine::convert_x_from_world_to_ndc(SCREEN_WIDTH / 2.0F);		// 0
		//auto B = IceEngine::convert_y_from_world_to_ndc(SCREEN_HEIGHT / 2.0F);		// 0
		//IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "CONVERTION MATE :) % %", A, B);

		// Parse the .csv files
		// Build a map with layers and tiles types
		// Prepare for render
		// Render


		m_tileset = new IceEngine::Tileset("./data/Asset0/0x72_DungeonTilesetII_v1.6/0x72_DungeonTilesetII_v1.6.png", "./data/Asset0/0x72_DungeonTilesetII_v1.6/tile_list_v1.6");
		m_tilemap = new IceEngine::Tilemap();

		m_tilemap->SetTileset(m_tileset);
		m_tilemap->AddLayerFromCSV("./data/GameMap_Floor.csv");
		m_tilemap->AddLayerFromCSV("./data/GameMap_Wall Corners.csv");

		// Load The sprite sheet tile set file tile_list_v1.6
		// 		
		GetTileInfoFromFile(m_tilesInfo);
		/*
		
			floor_5, floor_5, floor_5, floor_5, ...

		*/
		auto Floor5Tile = GetTileByName("floor_5");

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
		m_cameraComponent->zoom = 2.0F;
		m_cameraComponent->position = { 24.2, 57.2 };
		
		//m_prevMousePos = IceEngine::InputManager::Instance().GetMousePosition();
		m_spriteIndex = 0;


		auto tile = m_tilesInfo[113];
		IceEngine::Logger::Instance().Log(IceEngine::LogLevel::ERROR, "% % % % %", tile.name, tile.x_offset, tile.y_offset, tile.width, tile.height);
		IceEngine::Logger::Instance().Log(IceEngine::LogLevel::ERROR, "[%f %f] [%f %f] [%f %f] [%f %f]",
			tile.textureCoords[0].x, tile.textureCoords[0].y,
			tile.textureCoords[1].x, tile.textureCoords[1].y,
			tile.textureCoords[2].x, tile.textureCoords[2].y,
			tile.textureCoords[3].x, tile.textureCoords[3].y);


		point.pointSize = 7.0f;
		point.color = { 8/255.0, 102/255.0, 255/255.0 };
		point.view = m_cameraComponent->GetViewMatrix();
		point.projection = m_cameraComponent->projection;
		point.setup();

		// Later may have to think about anti aliasing
		circle.radius = 1;
		circle.vCount = 128;
		circle.setup();

		circle2.radius = 1;
		circle2.vCount = 128;
		circle2.setup(false);

		line.setup(glm::vec2(100, 100), glm::vec2(500, 500), 5 * 2.0f);
		
		rect.setup();
		rect.isOutline = true;

		rect2.setup();
		rect2.isOutline = false;

		lineQuad.setup();
	
		triangle.setup();

		SDFrect.size = {100, 200}; // (10, 10)
		SDFrect.radius = 30.0f;
		SDFrect.thickness = 5.0f;
		
		SDFrect.setup();

		TheRect.Setup();
		TheRect.setSize(glm::vec2(50.0f, 150.0f));
		TheRect.setRadius(20.0f);
		TheRect.setThickness(2.0f);

		theText.font_path = "./data/fonts/ThaleahFat.ttf";
		theText.fontSize = 48;
		theText.setup();

		arrow.setup();

		const float PI = glm::pi<float>();
		const int numVertices = 10; // 5 arms with 2 vertices each

		std::vector<glm::vec2> starVertices;

		for (int i = 0; i < numVertices; ++i) {
			float angle = (2 * PI * i) / numVertices - PI / 2.0f;
			float radius = (i % 2 == 0) ? 0.5f : 0.2f; // Alternating long and short arms

			float x = radius * cos(angle);
			float y = radius * sin(angle);

			starVertices.push_back(glm::vec2(x, y));
		}

		glm::vec3 polygonColor = { 1.0f, 0.0f, 0.0f };  // Red color

		closedPolygon.setup(starVertices, polygonColor);

		// Example check
		glm::vec2 startNDC = m_cameraComponent->worldToScreen(glm::vec2(100, 100));
		glm::vec2 endNDC = m_cameraComponent->worldToScreen(glm::vec2(400, 800));

		std::cout << "Start NDC: (" << startNDC.x << ", " << startNDC.y << ")\n";
		std::cout << "End NDC: (" << endNDC.x << ", " << endNDC.y << ")\n";
		
		
		// Player
		{
		
		  
		  player.position = vec2(400, 300);
		  player.scale_factor = 1.5f;
		  
		  player.load();
		}
	}

	SpriteSheetScene::~SpriteSheetScene() {}

	template<typename T>
	T lerp(const T &a, const T &b, float t) {
		return a + t * (b - a);
	}

	void SpriteSheetScene::Update(float deltaTime) {

		// Update the base scene
		Scene::Update(deltaTime);

		static bool toggleFollowPointPeak = false;
		
        mousePos = IceEngine::InputManager::Instance().GetMousePosition();
        
		

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_Z))
		{
			toggleFollowPointPeak = !toggleFollowPointPeak;
			
		}

		if (toggleFollowPointPeak)
		{
		
			
			IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "Mouse Position % %", mousePos.x, mousePos.y);

			// Convert mouse position to world coordinates
			glm::vec2 normalizedMousePos(mousePos / 4.0f);

			// Define a fixed offset for the camera
			glm::vec2 offset(10.0f, -300.0f);  // Adjust the offset as needed

			// Move the camera towards the mouse position with the fixed offset
			glm::vec2 targetPosition = normalizedMousePos - offset;

			// Interpolation factor for smooth camera movement
			const float lerpFactor = 0.01f;  // Adjust this factor to control the smoothness of the transition

			// Update camera position using linear interpolation
			m_cameraComponent->position = glm::lerp(m_cameraComponent->position, targetPosition, lerpFactor);
		}

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_L)) 
		{
			m_cameraComponent->zoom += 0.05f;
		}
		
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_K)) {
			m_cameraComponent->zoom -= 0.05f;
		}
		
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_P))
		{
			m_cameraComponent->zoom = 1.0f;
		}

		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_DOWN))
		{
			m_cameraComponent->position.y += 100 * deltaTime;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_UP))
		{
			m_cameraComponent->position.y -= 100 * deltaTime;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_LEFT))
		{
			m_cameraComponent->position.x -= 100 * deltaTime;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_RIGHT))
		{
			m_cameraComponent->position.x += 100 * deltaTime;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_H))
		{
			IceEngine::Logger::Instance().Log(IceEngine::LogLevel::INFO, "Camera position = {%, %}", m_cameraComponent->position.x, m_cameraComponent->position.y);
		}

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_N))
		{
			m_spriteIndex = m_spriteIndex + 1;
			m_spriteIndex %= m_tilesInfo.size() - 1;

			auto tile = m_tilesInfo[m_spriteIndex];
			IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "% % % % %", tile.name, tile.x_offset, tile.y_offset, tile.width, tile.height);
		}

		// @nocheckin for testing purposes we only render the GUI when we are in editor mode.

		if (IceEngine::Core::app_mode == IceEngine::App_Mode::Editor)
		{
			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window) {
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me")) show_another_window = false;
				ImGui::End();
			}
		}


		// Player
		{
		  player.update( m_cameraComponent );
		}

        // Have ImGUI on editor mode to print the mouse position.
		if (IceEngine::Core::app_mode == IceEngine::App_Mode::Editor) {
			ImGui::Text("Mouse Position %f %f", mousePos.x, mousePos.y);
			
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

	Tile_Info *SpriteSheetScene::GetTileByName(const std::string &name) {
		Tile_Info *ptr = nullptr;

		for (int i = 0; i < m_tilesInfo.size(); i++) {
			if (m_tilesInfo[i].name == name) {
				ptr = &m_tilesInfo[i];
				break;
			}
		}

		return ptr;
	}

	void SpriteSheetScene::Render()
	{
		IceEngine::Color::SetClearColor({ 29, 17, 22 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader->Bind();
		
		IceEngine::Renderer::ResetStats();
		IceEngine::Renderer::BeginBatch();
		
		m_tilemap->Draw();
		glm::vec2 tileSize = {(float)m_tilesInfo[m_spriteIndex].width , (float)m_tilesInfo[m_spriteIndex].height };
		glm::vec2 position = { 200, 200 };
		float tileRotation = 0.0f;
		// Translation matrix with the pivot adjustment
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x - 0.5f * tileSize.x, position.y - 0.5f * tileSize.y, 0.0f));
		// Rotation matrix (assuming rotation is in radians)
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tileRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		// Scale matrix
		glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(tileSize.x, -tileSize.y, 1.0f));
		// Combine the transformations
		glm::mat4 modelMatrix = translation * rotationMatrix * scaling;
		//IceEngine::Logger::Instance().Log(mat4ToString(modelMatrix), IceEngine::LogLevel::SUCCESS);
		tileSize = { (float)m_tilesInfo[m_spriteIndex].width / SCREEN_WIDTH, (float)m_tilesInfo[m_spriteIndex].height / SCREEN_HEIGHT };
		IceEngine::Renderer::DrawQuad(position, tileSize , modelMatrix, m_tilesetTexture, m_tilesInfo[m_spriteIndex].textureCoords);
		auto S = IceEngine::Renderer::GetStats();
		//IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "DrawCount = %, QuadCount = %", S.DrawCount, S.QuadCount);
		
		// Player
		{
		  
		  player.draw();
		}
		
		
		
		IceEngine::Renderer::EndBatch();
		// Setup Camera View Transform
		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		//m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());
		
		
		IceEngine::Renderer::Flush();
		
		point.view = mat4(1.0f);
		//point.draw(mousePos.x, mousePos.y);
		
		
		// Get the point x,y
	    {
	      
	       mat4 model = glm::translate(mat4(1.0f), vec3(player.position.x, player.position.y, 0.0f));
	       mat4 view  = m_cameraComponent->GetViewMatrix();
	       mat4 projection = m_cameraComponent->projection;
	       
	      
	       vec4 clip_space_point = m_cameraComponent->projection * view * model * vec4(1.0f);
	       
	       vec2 screen_space_point;
	       screen_space_point.x = (clip_space_point.x + 1.0f) * 0.5f * SCREEN_WIDTH;
	       screen_space_point.y = (1.0f - clip_space_point.y) * 0.5f * SCREEN_HEIGHT;
	       
	       IceEngine::Logger::Instance().Log(IceEngine::LogLevel::SUCCESS, "Screen Space = % %", screen_space_point.x, screen_space_point.y); 
	       
	       
	      point.draw(screen_space_point.x, screen_space_point.y);
	    }
	   // 777 543
		
		//point.draw(player.position.x, player.position.y);
		
		//point.view  = m_cameraComponent->GetViewMatrix();
	    //point.draw(player.position.x, player.position.y);
	   
	    
	   
	   
	    
		
		//circle.draw(300, 100, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//circle2.draw(400, 300, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//rect.draw(300, 200, 50, 50, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//rect2.draw(200, 300, 50, 80, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//SDFrect.draw({100,600}, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//TheRect.draw({ 300, 90 }, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//line.draw({ 0.0, 1.0, 0.0 }, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//lineQuad.draw(100, 100, 500, 500, 5.0f, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//point.view = m_cameraComponent->GetViewMatrix();
		//point.draw(100, 100);
		//triangle.rotation += 0.5;
		//triangle.draw(400, 400, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//point.draw(400, 400);
		//// Draw the polygon
		//closedPolygon.draw(m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
		//point.draw(50, 50);
		//theText.projection = m_cameraComponent->projection;
		//theText.draw("SCORE", SCREEN_WIDTH / 4, 40, 1.0F, glm::vec3(1.0));
		//theText.draw("ROUND", SCREEN_WIDTH / 2 - 50, 40, 1.0F, glm::vec3(1.0));
		//theText.draw("TIME LEFT", SCREEN_WIDTH * 2.0f/3.0f, 40, 1.0F, glm::vec3(1.0));
		//arrow.draw(430, 100, 60, 60, m_cameraComponent->GetViewMatrix(), m_cameraComponent->projection);
	}
}








