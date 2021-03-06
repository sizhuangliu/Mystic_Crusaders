#pragma once
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "common.hpp"

class Skillup : public Renderable
{
	static Texture level_texture;
public:

	// Creates all the associated render resources and default transform
	bool init(vec2 screen);

	// Releases all the associated resources
	void destroy();

	void draw(const mat3& projection)override;

	void update_leveltex(bool paused, int freepoints, int skill_num); //call on world

	void update_itemlevel(bool paused, int stock, int afforable, int skill_num);

	void get_texture(int loc);

	vec2 get_position() const;

	void change_position(vec2 screen);

	void change_scale(vec2 screen);


private:
	vec2 m_scale;
	vec2 m_position;
	TexturedVertex vertices[4];

};

