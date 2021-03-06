#pragma once

#include "common.hpp"
#include "enemies.hpp"
#include "enemy_laser.h"
#include "projectile.h"

// Salmon enemy
class Enemy_01 : public Enemies
{
	// Shared between all enemys, no need to load one for each instance
	static SpriteSheet enemy_texture;

public:

	// Releases all the associated resources
	void destroy(bool reset) override;

	// Creates all the associated render resources and default transform
	bool init(int level) override;

	// Update enemy due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, vec2 target_pos) override;

	// Renders the salmon
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection)override;

    void setTextureLocs(int index);

	bool shoot_projectiles(std::vector<EnemyLaser> & enemy_projectiles);

	bool needFireProjectile;

	void setLastFireProjectileTime(clock_t c);

	bool checkIfCanFire(clock_t currentClock);

	double attackCooldown;

	int powerup();

	bool poweredup;

	vec2 get_bounding_box()const;

	private:
    std::vector<float> texture_rows;
    std::vector<float> texture_cols;
    TexturedVertex texVertices[4];
	clock_t lastFireProjectileTime;
	float projectileSpeed;
    EnemyMoveState m_moveState;
    float m_animTime = 0.0f;
    int numTiles;
	float m_range;
	int powerupType;
};