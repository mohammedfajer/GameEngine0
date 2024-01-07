#include "effects.h"
#include "math.h"


START_NAMESPACE

/*
	Transition_Manager tm (5.0f);

	// Update
	tm.update(&effect, dt);
*/

Transition_Manager::Transition_Manager(float duration_in_seconds) {
	duration = duration_in_seconds;
}

void Transition_Manager::update(Transition_Effect *effect, float dt) {
	float increment_size = (1.0f / (duration * 1000.0f)) * dt;
	effect->progress += lerp(0.0f, 1.0f, increment_size);
	effect->progress = clamp(effect->progress);
}

void init_transition_effect(Transition_Effect *effect, const char *vs, const char *fs) {
	effect->shader.LoadShaderFromString(vs, fs);

	// Set up vertex data (assuming a rectangle covering the entire screen)
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // bottom-right
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top-right
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top-left
	};

	GLuint indices[] = {
		0, 1, 2,  // first triangle
		0, 2, 3   // second triangle
	};

	// Vertex Array Object
	glGenVertexArrays(1, &effect->VAO);
	glBindVertexArray(effect->VAO);

	// Vertex Buffer Object
	glGenBuffers(1, &effect->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, effect->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Element Buffer Object
	glGenBuffers(1, &effect->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, effect->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

}

void set_mask_texture(Transition_Effect *effect) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, effect->texture_id);

	//GLint boundTextureID;
	//glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTextureID);
	//std::cout << "Bound Texture ID: " << boundTextureID << std::endl;

	effect->shader.Bind();
	effect->shader.SetInt("mask", 0);
	effect->shader.UnBind();
}

void set_cutoff(Transition_Effect *effect) {
	effect->shader.Bind();
	effect->shader.SetFloat("cutoff", effect->progress);
	effect->shader.UnBind();
}

void switch_transition_animation(Transition_Effect *effect, bool v) {
	effect->shader.Bind();
	effect->shader.SetInt("u_switch_animation", v);
	effect->shader.UnBind();
}

void render_transition_effect(Transition_Effect *effect) {
	effect->shader.Bind();
	glBindTexture(GL_TEXTURE_2D, effect->texture_id);
	glBindVertexArray(effect->VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	effect->shader.UnBind();
}

END_NAMESPACE