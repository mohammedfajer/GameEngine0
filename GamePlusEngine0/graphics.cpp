
#include "graphics.h"
#include "InputManager.h"
#include "core.h"
#include "Renderer.h"
#include "SpritsheetLoader.h"
#include "TextureLoader.h"

START_NAMESPACE

Application g_application;

// Application

void Application::setup() {
	m_camera = new OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	m_camera->zoom = 2.0F;
	//m_camera->position = { 24.2, 57.2 };
	mode.font_path = "./data/fonts/arial.ttf";
	mode.fontSize = 58;
	mode.projection = m_camera->projection;
	mode.setup();
}

void Application::update(float dt) {
	// Toggle Play mode or editor mode
	if (InputManager::Instance().IsKeyPressed(SDL_SCANCODE_E)) {
		Core::app_mode = (Core::app_mode == App_Mode::Game) ? App_Mode::Editor : App_Mode::Game;
	}
}

void Application::render() {
	if (Core::app_mode == IceEngine::App_Mode::Game) {
		mode.draw("Game Mode", 10, 50, 1.0f, glm::vec3(0, 100, 100));
	} else {
		mode.draw("Editor Mode", 10, 50, 1.0f, glm::vec3(0, 100, 100));
	}
}

// Orthographic Camera

Orthographic_Camera::Orthographic_Camera(real32 left, real32 right, real32 bottom, real32 top, real32 zoom_level)
	: position(0.0f, 0.0f), zoom(zoom_level) {

	projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

	view = view_matrix();
}

void Orthographic_Camera::follow_position(const vec2 &player_pos, const vec2 &screen_size) {
	position = player_pos - screen_size / (2.0f * zoom);
}

vec2 Orthographic_Camera::world_to_screen(const vec2 &world_coordinates) {
	vec4 clip_space = projection * view * vec4(world_coordinates - position, 0.0f, 1.0f);
	clip_space /= clip_space.w;
	
	vec2 screen_coordinates;
	screen_coordinates.x = (clip_space.x + 1.0f) * 0.5f * SCREEN_WIDTH;
	screen_coordinates.y = (1.0f - clip_space.y) * 0.5f * SCREEN_HEIGHT;

	return screen_coordinates;
}

mat4 Orthographic_Camera::view_matrix() {
	view = glm::translate(mat4(1.0f), vec3(-position.x, -position.y, 1.0f))
				 * glm::scale(mat4(1.0f), vec3(zoom, zoom, 1.0f));
	return view;
}


// Animation

// Data
	/*
	- Animation Speed FPS
	- Animation State Machine 
	- Looping or Play Once
	- Flip
	- File I/O And Slicing into Frames
	etc...
	etc...
	
	
	Animation_State_Machine
	Animation
	Animation_Setting
	
	animator.play("Animation Name");
	
	
    
	*/
	

void setup_animation(Animation *anim, const char *file_path, i32 fps, i32 num_frames, bool32 flip, bool32 looping) {
	memset(anim, 0, sizeof(Animation));
	anim->texture = TextureLoader::LoadTexture(file_path);;
	anim->frame_rect = {0.0f, 0.0f, (float)anim->texture.width / 9, (float)anim->texture.height};
	anim->looping = looping;
	anim->flip = flip;
	anim->fps = fps;
	
	anim->frame_size = vec2(anim->frame_rect.w, anim->frame_rect.h);
	anim->num_frames = num_frames;
		
}
	
void update_animation(Animation *anim) {
   anim->frames_counter++;
   
   if(anim->frames_counter >= (120 / anim->fps)) {
       anim->frames_counter = 0;
       anim->current_frame++;
       
       if(anim->current_frame > anim->num_frames)  {
           if(anim->looping) {
               anim->current_frame = 0;
           }
       }
       
       anim->frame_rect.x = (float) anim->current_frame * anim->frame_size.x;
   }
}


void render_animation(Animation *anim, vec2 position, real32 rotation, real32 scale_factor) {
   auto texture_coordinates = IceEngine::GetTextureCoords(anim->texture, anim->frame_rect.y,
                            anim->frame_rect.x, anim->frame_rect.w, anim->frame_rect.h);
   
   mat4 translation = glm::translate(mat4(1.0f), vec3(position.x, position.y, 0.0f));
   mat4 rotation_matrix = glm::rotate(mat4(1.0f), glm::radians(rotation), vec3(0.0f, 0.0f, 1.0f));
   mat4 scaling = glm::scale(mat4(1.0f), vec3(anim->frame_size.x * scale_factor, -anim->frame_size.y * scale_factor, 1.0f));
   
   if(anim->flip) {
	   scaling = glm::scale(mat4(1.0f), vec3(-anim->frame_size.x * scale_factor, -anim->frame_size.y * scale_factor, 1.0f));
   }
   mat4 model = translation * rotation_matrix * scaling;
   
   vec2 frame_size = { (float) anim->frame_rect.w / SCREEN_WIDTH, (float) anim->frame_rect.h / SCREEN_HEIGHT};
   
   IceEngine::Renderer::DrawQuad(position, frame_size, model, anim->texture.id, texture_coordinates);
   
  

}




END_NAMESPACE
