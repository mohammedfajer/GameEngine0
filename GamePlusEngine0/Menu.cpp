

#include "Menu.h"

#include <GL/glew.h>
#include "Defines.h"
#include <SDL.h>
#include "Color.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "FontSystem.h"
#include "OrthographicCameraComponent.h"
#include "InputManager.h"
#include "Texture2D.h"
#include "TextureLoader.h"

#include "Renderer.h"

#include "Shader.h"

#include "SceneManager.h"

namespace TopDownShooter
{
	// Scene Transition

	// Menu Image

	// Menu Options

	// Music and Sound Effects

	const char *scene_transition_vertex_shader_source_code = R"(
		#version 330 core
		layout (location = 0) in vec2 aPos;
		void main() {
			gl_Position = vec4(aPos, 0.0, 1.0);
		}
	)";

	const char *scene_transition_fragment_shader_source_code = R"(
		#version 330 core
		
		out vec4 FragColor;
			
		uniform float iTime;
		uniform float iDuration;
		uniform vec2 iResolution;
		uniform vec3 ibgColor;

		void main() {
			float transitionDuration = iDuration;
			float progress = smoothstep(0.0, 1.0, mod(iTime, transitionDuration) / transitionDuration);
			float rectanglePositionTop = 1.0 - smoothstep(0.0, 1.0, progress);
			float rectanglePositionBottom = smoothstep(0.0, 1.0, progress);
			float screenHeight = iResolution.y;
			float rectangleHeight = screenHeight * 0.5;
			float rectangleYTop = mix(0.0, screenHeight * 0.5, rectanglePositionTop);
			float rectangleYBottom = mix(screenHeight * 0.5, screenHeight, rectanglePositionBottom);
			vec3 finalColor = mix(vec3(0.0), ibgColor, step(rectangleYTop, gl_FragCoord.y) * step(gl_FragCoord.y, rectangleYBottom));
			FragColor = vec4(finalColor, 1.0);			
		}
		
	)";


	struct Timer {
		Uint32 startTime;
		Uint32 duration;

		Timer() = default;

		Timer(Uint32 duration) : duration(duration * 1000)
		{
			reset();
		}

		void reset() { startTime = SDL_GetTicks(); }

		bool isExpired() const
		{
			Uint32 currentTime = SDL_GetTicks();
			return (currentTime - startTime) >= duration;
		}
	};

	struct SceneTransitionEffect
	{
		GLuint shader_program;
		GLuint VAO, VBO;

		float duration = 1.0f;

		Timer timer;

		bool timerStarted = false;
		
		uint32_t startTime;

		void setup()
		{
			startTime = SDL_GetTicks();
			timer.duration = duration * 1000.0f; // to seconds
			

			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &scene_transition_vertex_shader_source_code, NULL);
			glCompileShader(vertex_shader);
			IceEngine::checkShaderCompileError(vertex_shader);

			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &scene_transition_fragment_shader_source_code, NULL);
			glCompileShader(fragment_shader);
			IceEngine::checkShaderCompileError(fragment_shader);

			shader_program = glCreateProgram();
			glAttachShader(shader_program, vertex_shader);
			glAttachShader(shader_program, fragment_shader);
			glLinkProgram(shader_program);
			IceEngine::checkProgramLinkError(shader_program);

			float vertices[] =
			{
				-1.0f, -1.0f,
				1.0f, -1.0f,
				-1.0f, 1.0f,
				1.0f, 1.0f
			};

			// Initialize VAO and VBO for a quad
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glUseProgram(shader_program);
			GLint bgColorLoc = glGetUniformLocation(shader_program, "ibgColor");
			glUniform3fv(bgColorLoc, 1, glm::value_ptr(glm::vec3(72 / 255.0f, 59 / 255.0f, 58 / 255.0f)));
		}

		void reset_transition()
		{
			timerStarted = false;
		}

		void draw()
		{
			if (!timerStarted)
			{
				timer.reset();
				timerStarted = true;
			}

			if (timerStarted && timer.isExpired()) return;

			glUseProgram(shader_program);
			glBindVertexArray(VAO);



			// Set the model matrix uniform
			GLint timeLoc = glGetUniformLocation(shader_program, "iTime");
			GLint durationLoc = glGetUniformLocation(shader_program, "iDuration");
			GLint resolutionLoc = glGetUniformLocation(shader_program, "iResolution");

			

			uint32_t currTime = SDL_GetTicks();

			double elapsedTime = (currTime - startTime) / 1000.0; // Convert to seconds.
			glUniform1f(timeLoc, elapsedTime * 0.9);  // Set time to the duration to complete the transition
			glUniform1f(durationLoc, duration);
			glUniform2f(resolutionLoc, SCREEN_WIDTH, SCREEN_HEIGHT);

			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

			glUseProgram(0);
		}
	};

	SceneTransitionEffect transition;

	IceEngine::Text menuText;


	IceEngine::OrthographicCameraComponent *camera;

	int indexSelected = 0;

	struct MenuText_Data
	{
		std::string text;
		glm::vec2 startPoint;
		glm::vec2 endPoint;

		glm::vec3 color;
		bool selected;
	};

	std::vector<MenuText_Data> menuTextList;
	
	IceEngine::Texture2D menuTexture;
	IceEngine::Shader *shader;

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

		void main() {
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;			
		}
	)";

	bool startGame = false;

	MenuScene::MenuScene()
	{
		camera = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		camera->zoom = 2.0F;
		camera->position = { 24.2, 57.2 };


		transition.duration = 3.0f;
		transition.setup();

		menuText.font_path = "./data/fonts/ThaleahFat.ttf";
		menuText.fontSize = 48;
		menuText.projection = camera->projection;
		menuText.setup();


		// load sprite sheet texture
		menuTexture = IceEngine::TextureLoader::LoadTexture("./data/Asset0/menu_img.png");

		shader = new IceEngine::Shader();
		shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		shader->Bind();
		auto loc = glGetUniformLocation(shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++) {
			samplers[i] = i;
		}
		glUniform1iv(loc, 32, samplers);

		IceEngine::Renderer::Init();

		

		

		// Text			Start Position			End Position			Color		Selected or not
		menuTextList.push_back({ { "Play" }, { -500, SCREEN_HEIGHT - 300 }, { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 300 }, glm::vec3(1.0, 0.8f, 0.2f), true });
		menuTextList.push_back({ { "Fullscreen OFF" }, { -100, SCREEN_HEIGHT - 250 }, {SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT - 250 }, glm::vec3(1.0, 0.8f, 0.2f), false });
		menuTextList.push_back({ { "Quit" }, { -500, SCREEN_HEIGHT - 200 }, {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 200 }, glm::vec3(1.0, 0.8f, 0.2f), false });
	
	}

	MenuScene::~MenuScene()
	{
	}

	void MenuScene::Update(float deltaTime)
	{
		// Update the base scene
		Scene::Update(deltaTime);

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_2))
		{
			if (indexSelected == 0)
			{
				startGame = true;
				transition.reset_transition();
				
			}
		}

		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_DOWN))
		{
			indexSelected = indexSelected + 1;
			indexSelected %= 3;
		}
		if (IceEngine::InputManager::Instance().IsKeyPressed(SDL_SCANCODE_UP))
		{
		
			indexSelected = indexSelected - 1;

			// Ensure indexSelected is non-negative
			indexSelected = (indexSelected % 3 + 3) % 3;
		}

		

		for (int i = 0; i < menuTextList.size(); i++)
		{
			if (i == indexSelected)
			{
				menuTextList[i].selected = true;
			}
			else
			{
				menuTextList[i].selected = false;
			}
		}
	}

	void MenuScene::Render()
	{
		IceEngine::Color::SetClearColor({ 72, 59, 58, 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		if (startGame)
		{
			
			IceEngine::SceneManager::Instance().SetActiveScene("SpriteSheetScene");
		}


		static float fakeElapsedTime = 0.0f; // Initialize fake time
		float interpolateDuration = 8.0f;    // one second

		// Manually update the fake time
		fakeElapsedTime += 0.016f; // Assuming 60 frames per second (you can adjust this value based on your frame rate)

		// Calculate a single interpolation factor for all texts
		float interpolateFactor = std::min(1.0f, fakeElapsedTime / interpolateDuration);

		for (auto &textData : menuTextList)
		{
			if (textData.selected)
				textData.color = glm::vec3(1.0);
			else
				textData.color = glm::vec3(1.0, 0.8f, 0.2f);


			// Linear interpolation for x and y positions
			float interpolateX = glm::mix(textData.startPoint.x, textData.endPoint.x, interpolateFactor);
			float interpolateY = glm::mix(textData.startPoint.y, textData.endPoint.y, interpolateFactor);

			menuText.draw(textData.text, interpolateX, interpolateY, 1.0f, textData.color);
		}

		shader->Bind();

		shader->SetMat4("projection", camera->projection);
		shader->SetMat4("view", camera->GetViewMatrix());

		IceEngine::Renderer::BeginBatch();
		IceEngine::Renderer::DrawQuad(glm::vec2(SCREEN_WIDTH/4.0f - 1024/7.5f , 50), glm::vec2(1024*.3, 512 *0.3f), menuTexture.id);
		IceEngine::Renderer::EndBatch();


		IceEngine::Renderer::Flush();


		transition.draw();
	}

}