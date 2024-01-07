#pragma once

#include "config.h"
#include <GL/glew.h>
#include "Defines.h"
#include <SDL.h>
#include "Shader.h"

START_NAMESPACE

struct Transition_Effect {
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	GLuint texture_id;
	float progress = 0.0f;
	Shader shader;
};

struct Transition_Manager {
	float duration;
	Transition_Manager(float duration_in_seconds);
	void update(Transition_Effect *effect, float dt);
};

extern void init_transition_effect(Transition_Effect *effect, const char *vs, const char *fs);
extern void set_mask_texture(Transition_Effect *effect);
extern void set_cutoff(Transition_Effect *effect);
extern void switch_transition_animation(Transition_Effect *effect, bool v);
extern void render_transition_effect(Transition_Effect *effect);


END_NAMESPACE