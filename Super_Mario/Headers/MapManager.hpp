#pragma once
#include <list>
#include "Global.hpp"
#include "Animation.hpp"
class MapManager
{
	std::list<Object> brick_particles;
	std::list<Object> question_block_coins;

	sf::Image map_sketch;

	sf::Sprite cell_sprite;
	sf::RenderTexture background_cache;
	sf::RenderTexture cells_cache;
	sf::Texture map_texture;

	Animation coin_animation;
	Animation question_block_animation;

	Map map;
public:
	MapManager();

	unsigned short get_map_sketch_height() const;
	unsigned short get_map_sketch_width() const;
	unsigned short get_map_width() const;
	void shake_block(const unsigned short i_x, const unsigned short i_y);
	void add_brick_particles(const unsigned short i_x, const unsigned short i_y);
	void add_question_block_coin(const unsigned short i_x, const unsigned short i_y);
	void draw_map_background(const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window);
	void draw_map(const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window);
	void set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell);
	void set_map_size(const unsigned short i_new_size);
	void update();
	void update_map_sketch(const unsigned char i_current_level);
	static constexpr int packCellList(const std::initializer_list<Cell>& i_cells) {
		int output = 0;
		for (auto i : i_cells)
		{
			output |= (1 << (int) i);
		}
		return output;
	}
	bool map_collision(const sf::FloatRect& i_hitbox, short& x,short& y, std::vector<std::vector<Cell>>& i_cells, int i_check_cells) const;
	void cache_blocks();
	sf::Color get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const;
};