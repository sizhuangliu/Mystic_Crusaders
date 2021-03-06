#include "particles.h"


Texture particles::texture;


particles::particles(float lifetime, float scale, vec2 position, vec2 initial_velocity)
{
	init(lifetime, scale, position, initial_velocity);
}

bool particles::init(float lifetime, float scale, vec2 position, vec2 initial_velocity)
{
	if (!texture.is_valid())
	{
		if (!texture.load_from_file(textures_path("particle.png")))
		{
			fprintf(stderr, "Failed to load particles texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = texture.width * 0.5f;
	float hr = texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
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

	m_position = position;
	m_scale = { scale,scale };
    m_color = { 1.f, 0.8f, 0.05f };
	velocity = initial_velocity;
	elapsed_time = 0.f;
	life_time = lifetime;
	can_remove = false;
	return true;
}

void particles::update(float ms, vec2 source_position)
{
    life_time -= ms;
    if (life_time > 0.0f)
    {	// particle is alive, thus update
        m_position = source_position;
        m_position.x -= velocity.x * (ms / 1000);
        m_position.y -= velocity.y * (ms / 1000);
    }
    else {
        can_remove = true;
    }

	/*if (elapsed_time < life_time)
	{
		int change_velocity = rand() % 10;
		vec2 d_v = { 0.f,0.f };
		if (change_velocity < 4)
		{
			int base = 100;
			d_v.x = float(rand() % base) / base;
			d_v.y = float(rand() % base) / base;
		}
		velocity.x = velocity.x + d_v.x;
		velocity.y = velocity.y + d_v.y;

		m_position.x = m_position.x + velocity.x * (ms /1000);
		m_position.y = m_position.y + velocity.y * (ms /1000);

		elapsed_time += ms;
	}
	else
	{
		can_remove = true;
	}*/

}

void particles::draw(const mat3 & projection)
{
	if (can_remove)
		return;

	transform_begin();
	transform_translate(m_position);
	transform_rotate(0.f);
	transform_scale(m_scale);
	transform_end();

	// Setting shaders
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
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { m_color.x, m_color.y, m_color.z };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void particles::set_lifetime(float t) {
    life_time = t;
}

float particles::get_lifetime() {
    return life_time;
}

void particles::set_position(vec2 pos) {
    m_position = pos;
}

vec2 particles::get_position() {
    return m_position;
}

void particles::set_scale(float scale) {
    m_scale = { scale,scale };
}

void particles::set_velocity(vec2 v) {
    velocity = v;
}

vec2 particles::get_velocity() {
    return velocity;
}

void particles::set_color_green(float g) {
    m_color = { 1.f, g, 0.05f };
}

float particles::get_color_green() {
    return m_color.y;
}

void particles::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

    effect.release();
}
particles::~particles()
{
}
