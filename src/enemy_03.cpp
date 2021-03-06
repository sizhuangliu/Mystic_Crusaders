// Header
#include "enemy_03.hpp"

#include <cmath>
#include <algorithm>

Texture Enemy_03::enemy_texture;

bool Enemy_03::init(int level)
{
	// Load shared texture
	if (!enemy_texture.is_valid())
	{
		if (!enemy_texture.load_from_file(textures_path("enemy_03.png")))
		{
			fprintf(stderr, "Failed to load enemy texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = enemy_texture.width * 0.5f;
	float hr = enemy_texture.height * 0.5f;

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

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture
	m_scale.x = 1.5f;
	m_scale.y = 1.5f;
	needFireProjectile = false;
	m_rotation = 0.f;
	enemyRandMoveAngle = 0.f;
	lastFireProjectileTime = clock();
	randMovementTime = clock();
	m_is_alive = true;

	float f = (float)rand() / RAND_MAX;
    float randAttributeFactor = 1.0f + f * (2.0f - 1.0f);

	m_speed = std::min(50.0f + (float)level * 0.4f * randAttributeFactor, 140.0f);
	attackCooldown = std::max(5000.0 - (double)level * 6.0 * randAttributeFactor, 1500.0);
	randMovementCooldown = std::max(1000.0 - (double)level * 2.6 * randAttributeFactor, 250.0);
	m_range = std::min(400.0 + (double)level * 3.0 * randAttributeFactor, 700.0);
	hp = std::min(50.0f + (float)level * 0.6f * randAttributeFactor, 140.f);
	hp = 70.f;
	deceleration = 1.0f;
	momentum_factor = 1.0f;
	momentum.x = 0.f;
	momentum.y = 0.f;
	m_level = level;
	waved = false;
	enemyColor = {1.f,1.f,1.f};
	wave.init(m_position, enemyColor);
	dangerPos = {NULL, NULL};

	return true;
}

// Call if init() was successful
// Releases all graphics resources
void Enemy_03::destroy(bool reset)
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);
	effect.release();
	if((waved && !m_is_alive) || reset) {
		glDeleteVertexArrays(1, &mesh.vao);
		glDetachShader(effect.program, effect.vertex);
		glDetachShader(effect.program, effect.fragment);
		wave.destroy(true);
	}
}


void Enemy_03::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform_begin();
	transform_translate(m_position);

	if(m_face_left_or_right == 1){
		m_scale.x = -1.5f;
	} else {
		m_scale.x = 1.5f;
	}

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
	glBindTexture(GL_TEXTURE_2D, enemy_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform);
	float color[] = { 1.f, 1.f, 1.f };
	if (waved){
		enemyColor = {1.f, 1.f, 1.f};
		switch (recentPowerupType)
		{
			case 0:
				enemyColor.x = 0.2f;
				enemyColor.z = 0.2f;
				break;
			case 1:
				enemyColor.y = 0.2f;
				enemyColor.z = 0.2f;
				break;
			case 2:
				enemyColor.x = 1.f;
				enemyColor.y = 0.f;
				enemyColor.z = 1.f;
				break;
			case 3:
				enemyColor.x = 0.f;
				enemyColor.y = 0.f;
				enemyColor.z = 1.f;
				break;
		}
	}
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	if(waved) {
		wave.draw(projection);
	}
}

void Enemy_03::update(float ms, vec2 target_pos)
{
	//momentum first
	if (stunned)
		ms = ms * 0.2f;

	m_position.x += momentum.x;
	m_position.y += momentum.y;

	if (momentum.x > 0.5f) {
		momentum.x = std::max(momentum.x - deceleration, 0.f);
	}
	if (momentum.x < -0.5f) {
		momentum.x = std::min(momentum.x + deceleration, 0.f);
	}

	if (momentum.y > 0.5f) {
		momentum.y = std::max(momentum.y - deceleration, 0.f);
	}
	if (momentum.y < -0.5f) {
		momentum.y = std::min(momentum.y + deceleration, 0.f);
	}

	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	float x_diff =  m_position.x - target_pos.x;
	float y_diff =  m_position.y - target_pos.y;
	float distance = std::sqrt(x_diff * x_diff + y_diff * y_diff);
	float enemy_angle = atan2(y_diff, x_diff);
	if(dangerPos.x != NULL && dangerPos.y != NULL) {
		float x_diff2 =  m_position.x - dangerPos.x;
		float y_diff2 =  m_position.y - dangerPos.y;
		float danger_angle = atan2(y_diff2, x_diff2);
		if (enemy_angle - danger_angle < 0.3f && enemy_angle - danger_angle > 0.f) {
			enemy_angle += 0.3f;
		} else if (danger_angle - enemy_angle < 0.3f && danger_angle - enemy_angle > 0.f) {
			enemy_angle -= 0.3f;
		}
	}
	int facing = 1;
	if (x_diff > 0.0) {
		facing = 0;
	}
	set_facing(facing);
	set_rotation(enemy_angle);
	clock_t currentTime = clock();
	if (distance <= 100.f) {
		needFireProjectile = false;
		float step = m_speed * (ms / 1000);
		m_position.x += cos(enemy_angle)*step;
		m_position.y += sin(enemy_angle)*step;
	} else if (distance <= m_range && checkIfCanFire(currentTime)) {
		needFireProjectile = true;
		setLastFireProjectileTime(currentTime);
	} else if (distance <= m_range) {
		needFireProjectile = false;
		float step = -m_speed * (ms / 1000);
		m_position.x += cos(enemyRandMoveAngle)*step;
		m_position.y += sin(enemyRandMoveAngle)*step;
	} else {
		needFireProjectile = false;

		float step = -m_speed * (ms / 1000);
		m_position.x += cos(enemy_angle)*step;
		m_position.y += sin(enemy_angle)*step;
	}
	if (checkIfCanChangeDirectionOfMove(currentTime)){
		float LO = enemy_angle - 2.0f;
		float HI = enemy_angle + 2.0f;
		enemyRandMoveAngle = LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
		setRandMovementTime(currentTime);
		dangerPos = {NULL, NULL};
	}

	stunned = false;
	if (waved) {
		if (clock() - waveTime > 1500.f){
			waved = false;
		} else {
			wave.update(ms);
			wave.m_position = {m_position.x, m_position.y - 30.f};
			wave.custom_color = enemyColor;
		}
	}
}


bool Enemy_03::checkIfCanFire(clock_t currentClock)
{
	if ((double)(currentClock - lastFireProjectileTime) > attackCooldown) {
		return true;
	}
	return false;
}

void Enemy_03::setLastFireProjectileTime(clock_t c)
{
	lastFireProjectileTime = c;
}

