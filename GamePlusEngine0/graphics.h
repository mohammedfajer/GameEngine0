#pragma once
#include <stdint.h>
#include <initializer_list>
#include <array>
#include <GL/glew.h>
#include "config.h"
#include "OrthographicCameraComponent.h"
#include "FontSystem.h"
#include "Texture2D.h"

START_NAMESPACE

struct Rect {
	float x;
	float y;
	float w;
	float h;
};

struct Application {
	OrthographicCameraComponent *m_camera;
	Text mode;
	void setup();
	void update(float dt);
	void render();
};

extern Application g_application;

struct Orthographic_Camera {
	vec2 position;
	mat4 projection;
	mat4 view;
	real32 zoom;

	Orthographic_Camera(real32 left, real32 right, real32 bottom, real32 top, real32 zoom_level = 1.0f);
	
	void follow_position(const vec2 &player_pos, const vec2 &screen_size);

	vec2 world_to_screen(const vec2 &world_coordinates);

	mat4 view_matrix();
};

struct Animation {
	   i32 num_frames;
	   bool32 looping;
	   bool32 flip;
	   i32 fps;
	   i32 frames_counter;
	   i32 current_frame;
	   IceEngine::Texture2D texture;
	   std::string name;
	   IceEngine::Rect frame_rect;
	   vec2 frame_size;
};

void setup_animation(Animation *anim, const char* file_path, i32 fps = 5, i32 num_frames = 0, bool32 flip = false, bool32 looping = true );
void update_animation(Animation *anim);
void render_animation(Animation *anim, vec2 position, real32 rotation, real32 scale_factor);

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	Color() : r(0), g(0), b(0), a(255) {}

	// Constructor using individual arguments
	Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
		: r(red), g(green), b(blue), a(alpha) {}

	Color(std::initializer_list <uint8_t> values) {
		auto it = values.begin();
		r = (it != values.end()) ? *it : 0; ++it;
		g = (it != values.end()) ? *it : 0; ++it;
		b = (it != values.end()) ? *it : 0; ++it;
		a = (it != values.end()) ? *it : 255;
	}

	std::array<float, 4> flat_list() const {
		std::array<float, 4> result;
		result[0] = r / 255.0f;
		result[1] = g / 255.0f;
		result[2] = b / 255.0f;
		result[3] = a / 255.0f;
		return result;
	}

	// Wrapper function to use glClearColor with a Color object
	static void SetClearColor(Color &c) {
		auto values = c.flat_list();
		glClearColor(values[0], values[1], values[2], values[3]);
	}

	static void SetClearColor(std::initializer_list<uint8_t> values) {
		auto c = Color(values);
		SetClearColor(c);
	}
};


END_NAMESPACE


