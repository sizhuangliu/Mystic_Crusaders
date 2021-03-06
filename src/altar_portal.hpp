#pragma once
class Hero;
class Enemies;
#include "common.hpp"
#include "hero.hpp"
#include "enemies.hpp"
#include "projectile.h"
#include "enemies.hpp"
#include "Thunder.h"

class AltarPortal: public Renderable
{
	static SpriteSheet altar_texture;

public:

    bool init(vec2 screen);

    bool update(float ms, int points, int max_points);

    void destroy();

	// Returns the current enemy position
	vec2 get_position();

	// Sets the new enemy position
	void set_position(vec2 position);

	// Returns the enemy' bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

    void draw(const mat3& projection)override;

	bool collides_with(Projectile &projectile);
	bool collides_with(Hero &hero);
	bool collides_with(Enemies &enemy);

    void setIsPortal(bool portal);

    bool getIsPortal();

	void killAll(std::vector<Thunder*> &thunders);

	void AltarPortal::setTextureLocs(int index);

private:
	vec2 m_position; // Window coordinates
	vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    bool isPortal;
	bool justChangedToPortal;
	vec2 m_screen;
	float animation_time;
	TexturedVertex texVertices[4];
	std::vector<float> texture_locs;
};