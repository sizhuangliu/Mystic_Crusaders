#pragma once
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "common.hpp"

class Thunderskilltex : public Renderable
{
	Texture thunder_texture;
public:

	// Creates all the associated render resources and default transform
	bool init(vec2 screen, int skill_num); 

	// Releases all the associated resources
	void destroy();

	void draw(const mat3& projection)override;

	void update_ice(bool paused, float degree); //call on world

	void get_texture(int loc);

	vec2 get_position() const;

	void light_up();
	void blue_up();

private:
	vec2 m_scale;
	vec2 m_position;

	std::string element;

	TexturedVertex vertices[4];

	int m_light_up;
};

