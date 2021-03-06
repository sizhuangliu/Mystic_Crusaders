#pragma once

#include "common.hpp"
#include "projectile.h"
#include "fireball.h"
#include "enemy_01.hpp"
#include "enemy_02.hpp"
#include "enemy_03.hpp"
#include "hero.hpp"

class Enemy_01;
class Fish;

class Treetrunk : public Renderable
{

public:
	// Creates all the associated render resources and default transform
	//hero attributes

	//std::vector<skill> skill_list;

	vec2 m_screen;
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	void update(float ms);

	// Renders the tree trunk
	void draw(const mat3& projection)override;

	// Returns the current salmon position
	vec2 get_position()const;

	void set_color(vec3 color);

	void set_position(vec2 position);

	bool collide_with(Hero &hero);

	bool collide_with(Projectile &projectile);

	bool collide_with(Enemies &e);


private:
	double w;
	double h;
	float zoom_factor;
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
	float m_rotation; // in radians
	size_t m_num_indices; // passed to glDrawElements

	//add salmon speed
	vec2 m_direction;
	int m_light_up;
	vec3 m_color;
};
