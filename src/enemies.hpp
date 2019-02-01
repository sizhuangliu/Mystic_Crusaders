#pragma once

#include "common.hpp"

// general enemies
class Enemies
{
	// Shared between all enemys, no need to load one for each instance
	static Texture enemy_texture;

public:

    virtual bool init() = 0;

    virtual void update(float ms, vec2 target_pos) = 0;

    virtual void destroy() = 0;

	// Returns the current enemy position
	vec2 get_position()const;

	// Sets the new enemy position
	void set_position(vec2 position);

    // 0 is left, 1 is right
    void set_facing(int rotation_value);

	// Returns the enemy' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

    void take_damage(int damage);

    virtual void attack() = 0;

    void set_speed(float speed);

    void apply_momentum(vec2 momentum);

public:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float m_speed; // enemy speed
	int m_face_left_or_right; // 0 is left, 1 is right
    int m_HP;
    int m_damage;
};