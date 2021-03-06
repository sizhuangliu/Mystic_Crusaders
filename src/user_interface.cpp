// Header
#include "user_interface.hpp"

// stlib
#include <vector>
#include <string>
#include <algorithm>

Texture UserInterface::UserInterface_texture;


bool UserInterface::init(vec2 size, float _max_hp)
{
	//std::vector<Vertex> vertices;
	//std::vector<uint16_t> indices;

	// Load shared texture
	if (!UserInterface_texture.is_valid())
	{
		if (!UserInterface_texture.load_from_file(textures_path("BAR.png")))
		{
			fprintf(stderr, "Failed to load UI texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	w = size.x;
	h = size.y; // scale to an appropriate size for our UI
	float wr = w / (double) 2; // UserInterface_texture.width * 0.5f;
	float hr = h / (double) 2; // UserInterface_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f, };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
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

	// Setting initial values
	max_hp = _max_hp;
	max_mp = 100.f;
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	m_is_alive = true;
	m_position = { (float) w / 2.f, (float) h * 3.5f };
	m_rotation = 0.f;
	m_light_up_countdown_ms = -1.f;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	set_color({ 1.0f,1.0f,1.0f });
	m_light_up = 0;
	advanced = false;
	hp = max_hp;
	mp = max_mp;
	max_exp = 20;
	cur_exp = 0;
	return true;
}

// Releases all graphics resources
void UserInterface::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);

    effect.release();
}

// Called on each frame by World::update()
void UserInterface::update(vec2 hp_mp, vec2 exp, float zoom, float _max_hp)
{
	hp = hp_mp.x;
	max_hp = _max_hp;
	mp = hp_mp.y;
	cur_exp = exp.x;
	max_exp = exp.y;
	if (max_exp == 0)
		max_exp = 15;
	zoom_factor = zoom;
	m_scale = { 1.f / zoom, 1.f / zoom };


}

void UserInterface::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT


	transform_begin();
	transform_translate(m_position);
	transform_rotate(m_rotation);
	vec2 whiteBarScale = m_scale;
	whiteBarScale.y = whiteBarScale.y * 3.f;
	whiteBarScale.x = whiteBarScale.x * 7.1f;
	transform_scale(whiteBarScale);
	transform_end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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
	glBindTexture(GL_TEXTURE_2D, UserInterface_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);


	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT

	/*
	HP bar
	*/
	float HP_scale_factor = hp / max_hp; // a value (theoretically) between 0 and 1
	vec2 hp_scale = { m_scale.x * HP_scale_factor, m_scale.y * 0.51f };
	if (HP_scale_factor > 0.f) {
		// offset for width: 0.5 because we only want to push one way. w and 1 - scale_factor is for
		// finding how far it goes. zoom_factor is for making sure it fits on screen.
		vec2 hp_position = { 15.f/ zoom_factor + m_position.x - 0.5f * w * (1.f - HP_scale_factor) / zoom_factor, m_position.y - 52.f / zoom_factor };
		transform_begin();
		transform_translate(hp_position);
		transform_rotate(m_rotation);
		transform_scale(hp_scale);
		transform_end();

		// Setting shaders
		glUseProgram(effect.program);

		// Enabling alpha channel for textures
		glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		// Getting uniform locations for glUniform* calls
		transform_uloc = glGetUniformLocation(effect.program, "transform");
		color_uloc = glGetUniformLocation(effect.program, "fcolor");
		projection_uloc = glGetUniformLocation(effect.program, "projection");

		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		in_position_loc = glGetAttribLocation(effect.program, "in_position");
		in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
		glEnableVertexAttribArray(in_position_loc);
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, UserInterface_texture.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float colorHP[] = { 1.f, 0.f, 0.f };
		glUniform3fv(color_uloc, 1, colorHP);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}

	/*
	MP bar
	*/
	float MP_scale_factor = mp / max_mp; // a value (theoretically) between 0 and 1
	vec2 mp_scale = { m_scale.x * MP_scale_factor, m_scale.y * 0.51f };
	if (MP_scale_factor > 0.f) {
		// offset for width: 0.5 because we only want to push one way. w and 1 - scale_factor is for
		// finding how far it goes. zoom_factor is for making sure it fits on screen.
		vec2 mp_position = { 15.f / zoom_factor + m_position.x - 0.5f * w * (1.f - MP_scale_factor) / zoom_factor, m_position.y - 20.f / zoom_factor };
		transform_begin();
		transform_translate(mp_position);
		transform_rotate(m_rotation);
		transform_scale(mp_scale);
		transform_end();

		// Setting shaders
		glUseProgram(effect.program);

		// Enabling alpha channel for textures
		glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		// Getting uniform locations for glUniform* calls
		transform_uloc = glGetUniformLocation(effect.program, "transform");
		color_uloc = glGetUniformLocation(effect.program, "fcolor");
		projection_uloc = glGetUniformLocation(effect.program, "projection");

		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		in_position_loc = glGetAttribLocation(effect.program, "in_position");
		in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
		glEnableVertexAttribArray(in_position_loc);
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, UserInterface_texture.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float colorMP[] = { 0.f, 0.f, 1.f };
		glUniform3fv(color_uloc, 1, colorMP);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}

	float EXP_scale_factor = (float)cur_exp / (float)max_exp;
	vec2 exp_scale = { m_scale.x * EXP_scale_factor, m_scale.y * 0.51f };
	/*
	EXP bar
	*/
	if (EXP_scale_factor > 0.f) {
		// offset for width: 0.5 because we only want to push one way. w and 1 - scale_factor is for
		// finding how far it goes. zoom_factor is for making sure it fits on screen.
		vec2 exp_position = { 15.f / zoom_factor + m_position.x - 0.5f * w * (1.f - EXP_scale_factor) / zoom_factor, m_position.y + 13.f / zoom_factor };
		transform_begin();
		transform_translate(exp_position);
		transform_rotate(m_rotation);
		transform_scale(exp_scale);
		transform_end();

		// Setting shaders
		glUseProgram(effect.program);

		// Enabling alpha channel for textures
		glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		// Getting uniform locations for glUniform* calls
		transform_uloc = glGetUniformLocation(effect.program, "transform");
		color_uloc = glGetUniformLocation(effect.program, "fcolor");
		projection_uloc = glGetUniformLocation(effect.program, "projection");

		// Setting vertices and indices
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

		// Input data location as in the vertex buffer
		in_position_loc = glGetAttribLocation(effect.program, "in_position");
		in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
		glEnableVertexAttribArray(in_position_loc);
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, UserInterface_texture.id);

		// Setting uniform values to the currently bound program
		glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
		float colorEXP[] = { 1.f, 1.f, 0.f };
		glUniform3fv(color_uloc, 1, colorEXP);
		glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

		// Drawing!
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	}

}

void UserInterface::set_rotation(float radians)
{
	m_rotation = radians;
}

// Called when the salmon collides with a fish
void UserInterface::light_up()
{
	m_light_up_countdown_ms = 1500.f;
}

void UserInterface::set_color(vec3 in_color)
{
	float color[3] = { in_color.x,in_color.y,in_color.z };
	memcpy(m_color, color, sizeof(color));
}

void UserInterface::change_hp(float d_hp)
{
	hp += d_hp;
	hp = std::min(hp, max_hp);
	hp = std::max(0.5f, hp);
}

void UserInterface::change_mp(float d_mp)
{
	mp += d_mp;
	mp = std::min(mp, max_mp);
	mp = std::max(0.5f, mp);
}

void UserInterface::set_position(vec2 position, int sh, int offset)
{
	m_position = { (position.x + 30.f) / zoom_factor + (float)w / (2.f * zoom_factor) + offset / zoom_factor, position.y / zoom_factor + sh / zoom_factor - (float)h / (1.f * zoom_factor) };
}