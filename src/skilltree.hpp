#pragma once
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "common.hpp"
#include "ice_skill_tex.hpp"
#include "thunder_skill_tex.hpp"
#include "skillup.hpp"

class Skilltree : public Renderable
{
	
	static Texture screen_texture;


public:

	// Creates all the associated render resources and default transform
	//bool init(vec2 screen);
	bool init(vec2 screen, int element);

	// Releases all the associated resources
	void destroy();

	void draw(const mat3& projection)override;

	void update_skill(bool paused, int total, int used, vec3 ice_num, vec3 thunder_num, int skill_num);

	std::string get_element();

	vec2 set_scale(float w, float h, vec2 screen);

	bool inside(vec2 h, vec2 w, vec2 pos);

	bool level_position(vec2 mouse_pos);

	int ice_position(vec2 mouse_pos, std::string element);

	//int ice_position(vec2 mouse_pos);

	int thunder_position(vec2 mouse_pos);

	std::string element_position(vec2 mouse_pos);

	int element_position(vec2 mouse_pos, int element);
	void reset();

private:
	Iceskilltex ices1;
	Iceskilltex ices2;
	Iceskilltex ices3;

	Thunderskilltex thunder1;
	Thunderskilltex thunder2;
	Thunderskilltex thunder3;

	Skillup skillup;
	vec2 m_scale;
	vec2 m_position;
	// skill element
	std::string front_element;
	std::string upper_element; 
	std::string lower_element;

	TexturedVertex vertices[4];
	TexturedVertex vertices0[4];
	TexturedVertex vertices2[4];

	// level and degree for skills
	int level;
	int degree;
	int times;

	int skillpoints;
	int used_skillpoints;
	int free_skillpoints;
	int conversion = 10;


	// incre # of amo
	// decre mp cost
	// incre damage

	// vec3 skill{fire, 2,2, times}
};