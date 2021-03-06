#include "enemyPowerupWave.h"

SpriteSheet EnemyPowerupWave::texture;
bool EnemyPowerupWave::init(vec2 position, vec3 color)
{
	// Load shared texture
	if (!texture.is_valid())
	{
		if (!texture.load_from_file(textures_path("power_wave.png")))
		{
			fprintf(stderr, "Failed to load EnemyPowerupWave texture!");
			return false;
		}
	}

	texture.totalTiles = 3; // custom to current sprite sheet
	texture.subWidth = 99; // custom to current sprite sheet

	// The position corresponds to the center of the texture
	float wr = texture.subWidth * 0.5f;
	float hr = texture.height * 0.5f;

	texVertices[0].position = { -wr, +hr, -0.01f };
	//vertices[0].texcoord = { 0.f, 1.f };
	texVertices[1].position = { +wr, +hr, -0.01f };
	//vertices[1].texcoord = { 1.f, 1.f, };
	texVertices[2].position = { +wr, -hr, -0.01f };
	//vertices[2].texcoord = { 1.f, 0.f };
	texVertices[3].position = { -wr, -hr, -0.01f };
	//vertices[3].texcoord = { 0.f, 0.f };

	for (int i = 0; i <= texture.totalTiles; i++) {
		texture_locs.push_back((float)i * texture.subWidth / texture.width);
	}

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, texVertices, GL_STATIC_DRAW);

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

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1.f;
	m_scale.y = 1.f;
	m_rotation = 0;

	m_position = {position.x, position.y};
	animation_time = 0.0f;
	custom_color = color;
	first_time = true;

	return true;
}

void EnemyPowerupWave::destroy(bool reset)
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	if(reset) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDetachShader(effect.program, effect.vertex);
		glDetachShader(effect.program, effect.fragment);
	}
}

void EnemyPowerupWave::setTextureLocs(int index)
{
	texVertices[0].texcoord = { texture_locs[index], 1.f }; //top left
	texVertices[1].texcoord = { texture_locs[index + 1], 1.f }; //top right
	texVertices[2].texcoord = { texture_locs[index + 1], 0.f }; //bottom right
	texVertices[3].texcoord = { texture_locs[index], 0.f }; //bottom left

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Clear memory allocation
	if (!first_time) {
		destroy(false);
	}

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, texVertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);
	first_time = false;
}

void EnemyPowerupWave::update(float ms)
{
	float animation_speed = 0.3f;
	animation_time += animation_speed * 2;
	int curidx = 0;
	curidx += (int)animation_time % texture.totalTiles;
	setTextureLocs(curidx);
}

void EnemyPowerupWave::draw(const mat3 &projection)
{
	transform_begin();
	transform_translate({m_position.x, m_position.y - 50.f});
	transform_rotate(m_rotation);
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
	float color[] = {custom_color.x, custom_color.y, custom_color.z};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}



