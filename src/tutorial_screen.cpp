#include "tutorial_screen.hpp"

#include <iostream>

#include <string.h>
#include <cassert>
#include <sstream>

#include <gl3w.h>
Texture TutorialScreen::tutorial_screen;

bool TutorialScreen::init(vec2 screen) {
	tutorial_screen.load_from_file(textures_path("tutorial screen.png"));
	float w = tutorial_screen.width;
	float h = tutorial_screen.height;
	float wr = w * 0.5f;
	float hr = h * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, 0.f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, 0.f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, 0.f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, 0.f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;
	m_scale = set_scale(w, h, screen);
	m_rotation = 0.f;
	s_is_over = false;
	m_position = { float(screen.x / 2), float(screen.y / 2) };
	return true;
}

void TutorialScreen::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
	//g_level = 1;
}

void TutorialScreen::draw(const mat3& projection) {
	if (!s_is_over) {
		gl_flush_errors();

		transform_begin();
		transform_translate(m_position);
		transform_scale(m_scale);
		transform_end();

		glUseProgram(effect.program);

		// Enabling alpha channel for textures
		glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		// Getting uniform locations for glUniform* calls
		GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
		GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
		GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
		glEnableVertexAttribArray(in_position_loc);
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tutorial_screen.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float color[] = { 1.f, 1.f, 1.f };
		glUniform3fv(color_uloc, 1, color);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}
}

vec2 TutorialScreen::set_scale(float w, float h, vec2 screen)
{
	// temp code, will change after get window is possible
	float xscale = screen.x / w;
	float yscale = screen.y / h;
	//return{ 1.f,1.f };
	return { xscale, yscale };
}