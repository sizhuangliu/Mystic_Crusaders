#pragma once

// internal
#include "common.hpp"
#include "salmon.hpp"
#include "turtle.hpp"
#include "enemy_01.hpp"
#include "enemy_02.hpp"
#include "enemy_03.hpp"
#include "fish.hpp"
#include "water.hpp"
#include "hero.hpp"
#include "projectile.h"
#include "fireball.h"
#include "enemy_laser.h"
#include "map_screen.hpp"
#include "start_screen.hpp"
#include "user_interface.hpp"
#include "treetrunk.hpp"
#include "tree.hpp"
#include "skilltree.hpp"
#include "ice_skill_tex.hpp"
#include "Thunder.h"
#include "screen_button.hpp"
#include "tutorial_screen.hpp"
#include "bar_description.hpp"
#include "in_game.hpp"
#include "altar_portal.hpp"
#include "vine.h"
#include "phoenix.h"
#include "skill_switch_UI.hpp"
#include "scrollable.hpp"
#include "story.hpp"
#include "Shop.h"
#include "shop_screen.hpp"
#include "box.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;
	Shop shop;
private:
	// Generates a new enemy
	bool spawn_enemy_01();
	bool spawn_enemy_02();
	bool spawn_enemy_03();
	bool spawn_treetrunk();
	bool spawn_tree();
	bool spawn_vine();
	bool spawn_box();

	bool shootingFireBall;

	// Generates a new fish
	bool spawn_fish();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
	void on_mouse_click(GLFWwindow* window, int button, int action, int mods);
	bool initTrees();
	void on_mouse_wheel(GLFWwindow* window, double xoffset, double yoffset);

	void startGame();

	vec3 number_to_vec(int number, bool kill);

	std::string find_item(int item_num);

	void doNothing();


private:
	// Window handle
	GLFWwindow* m_window;

	Startscreen start;
    Text map_text;
    Text skill_text;
    Text hp_text;
    Text mp_text;
    Text exp_text;
	Skilltree stree;
	Shop_screen shop_screen;
	Bar_description hme;
	In_game ingame;
	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;
	Mapscreen map;
	// Water effect
	Water m_water;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int m_points;
	unsigned int previous_point;

	int m_game_level;
	int m_level;
	int pass_points;
	int cur_points_needed;

	//zoom
	float zoom_factor;

	//start screen
	bool start_is_over;
	vec3 level_num;
	vec3 kill_num;

	bool game_is_paused;
	bool shopping;
	int item_num;
	int page_num;
	vec2 mouse_pos;
	int used_skillpoints;
	vec3 ice_skill_set;
	vec3 thunder_skill_set;
	vec3 fire_skill_set;
	int skill_num;
	std::string skill_element;

	// Game entities
	Hero m_hero;
	AltarPortal m_portal;
	std::vector<Enemy_01> m_enemys_01;
	std::vector<Enemy_02> m_enemys_02;
	std::vector<Enemy_03> m_enemys_03;
	std::vector<Treetrunk> m_treetrunk;
	std::vector<Tree> m_tree;
	std::vector<Vine> m_vine;
	std::vector<Box> m_box;
	std::vector<Projectile*> hero_projectiles;
	std::vector<EnemyLaser> enemy_projectiles;
	std::vector<EnemyLaser> enemy_powerup_projectiles;
	std::vector<Thunder*> thunders;
	std::vector<phoenix*> phoenix_list;
	UserInterface m_interface;

	//Treetrunk m_treetrunk;
	//Tree m_tree;

	float m_current_speed;
	float m_next_enemy1_spawn;
	float m_next_enemy2_spawn;
	float m_next_enemy3_spawn;
	float m_next_fish_spawn;

	Mix_Music* m_background_music;
	Mix_Music* m_background_music2;
	Mix_Music* m_background_music3;
	Mix_Music* m_homescreen_music;
	Mix_Music* m_intro_music;
	Mix_Chunk* m_salmon_dead_sound;
	Mix_Chunk* m_salmon_eat_sound;
	Mix_Chunk* m_levelup_sound;
	Mix_Chunk* m_lightning_sound;
	Mix_Chunk* m_ice_sound;
	Mix_Chunk* m_fireball_sound;
	Mix_Chunk* m_laser_sound;
	Mix_Chunk* m_transition_sound;
	Mix_Chunk* m_amplify_sound;
	Mix_Chunk* m_phoenix_sound;
	Mix_Chunk* m_shop_sound;
	Mix_Chunk* m_tutorial_sound;



	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	float m_window_width;
	float m_window_height;

	int m_tree_number;
	std::vector<vec2> m_treetrunk_position;
	std::vector<vec2> m_box_position;
	vec2 mouse_position;

	clock_t lastFireProjectileTime;
	Button button_play;
	Button button_tutorial;
	Button button_tutorial_next_page;
	Button button_tutorial_prevous_page;
	Button button_shop;
	Button button_back_to_menu;
	Button button_back_to_menu2;
	Button button_back_from_skillscreen;
	Button button_skip_intro;

	bool display_tutorial = false;
	TutorialScreen m_tutorial;
	SkillSwitch m_skill_switch;

	bool passed_level;

	bool drawIntro;
	Scrollable intro_text;
	Story m_story;

	int current_stock;
	//int total_stock;
	int current_price;
	float interest;
	int balance;

};