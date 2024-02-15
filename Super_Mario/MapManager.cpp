#include <array>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
sf::Texture coin;
sf::Texture questionBlock;
sf::RenderTexture rx;
sf::Texture* get_coin_texture() {
	coin.loadFromFile("Resources/Images/Coin.png");
	return &coin;
}
sf::Texture* get_questionBlock_texture() {
	questionBlock.loadFromFile("Resources/Images/QuestionBlock.png");
	return &questionBlock;
}
MapManager::MapManager() :
	coin_animation(CELL_SIZE, get_coin_texture(), COIN_ANIMATION_SPEED),
	question_block_animation(CELL_SIZE, get_questionBlock_texture(), QUESTION_BLOCK_ANIMATION_SPEED)
{
	//questionBlock

	map_texture.loadFromFile("Resources/Images/Map.png");

	cell_sprite.setTexture(map_texture);
}

unsigned short MapManager::get_map_sketch_height() const
{
	return map_sketch.getSize().y;
}

unsigned short MapManager::get_map_sketch_width() const
{
	return map_sketch.getSize().x;
}

unsigned short MapManager::get_map_width() const
{
	return map.size();
}

void MapManager::add_brick_particles(const unsigned short i_x, const unsigned short i_y)
{
	//Adding brick particles.
	//I was too lazy to add randomness.
	//It still looks cool, in my opinion.
	brick_particles.push_back(Object(i_x, i_y, -0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y, 0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x, i_y + 0.5f * CELL_SIZE, -0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
	brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y + 0.5f * CELL_SIZE, 0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
}

void MapManager::add_question_block_coin(const unsigned short i_x, const unsigned short i_y)
{
	question_block_coins.push_back(Object(i_x, i_y, 0, COIN_JUMP_SPEED));
}
void MapManager::draw_map_background(const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window) {
	unsigned short map_end = ceil((SCREEN_WIDTH + i_view_x) / static_cast<float>(CELL_SIZE));
	unsigned short map_height = floor(map_sketch.getSize().y / 3.f);
	unsigned short map_start = floor(i_view_x / static_cast<float>(CELL_SIZE));
	//We're drawing the coin before drawing the blocks because we want it to appear behind the question block.	
	for (unsigned short a = map_start; a < map_end; a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			unsigned short sprite_x = 0;
			unsigned short sprite_y = 0;

			cell_sprite.setPosition(CELL_SIZE * a, CELL_SIZE * b);

			//This code is a big mess.
			//But it works.
			//Keep that in mind before judging me.


			sf::Color pixel = map_sketch.getPixel(a, b + 2 * map_height);
			sf::Color pixel_down = sf::Color(0, 0, 0, 0);
			sf::Color pixel_left = sf::Color(0, 0, 0, 0);
			sf::Color pixel_right = sf::Color(0, 0, 0, 0);
			sf::Color pixel_up = sf::Color(0, 0, 0, 0);

			if (255 == pixel.a)
			{
				if (0 < a)
				{
					pixel_left = map_sketch.getPixel(a - 1, b + 2 * map_height);
				}

				if (0 < b)
				{
					pixel_up = map_sketch.getPixel(a, b + 2 * map_height - 1);
				}

				if (a < map_sketch.getSize().x - 1)
				{
					pixel_right = map_sketch.getPixel(1 + a, b + 2 * map_height);
				}

				if (b < map_height - 1)
				{
					pixel_down = map_sketch.getPixel(a, 1 + b + 2 * map_height);
				}

				if (sf::Color(255, 255, 255) == pixel)
				{
					sprite_x = 8;

					if (sf::Color(255, 255, 255) == pixel_up)
					{
						sprite_y = 1;
					}

					if (sf::Color(255, 255, 255) == pixel_left)
					{
						if (sf::Color(255, 255, 255) != pixel_right)
						{
							sprite_x = 9;
						}
					}
					else if (sf::Color(255, 255, 255) == pixel_right)
					{
						sprite_x = 7;
					}
				}
				else if (sf::Color(146, 219, 0) == pixel)
				{
					sprite_x = 5;
				}
				else if (sf::Color(146, 146, 0) == pixel)
				{
					sprite_x = 4;
				}
				else if (sf::Color(146, 182, 0) == pixel)
				{
					sprite_x = 6;
				}
				else if (sf::Color(0, 73, 0) == pixel)
				{
					sprite_y = 1;

					if (sf::Color(0, 109, 0) == pixel_left)
					{
						if (sf::Color(0, 109, 0) != pixel_right)
						{
							sprite_x = 2;
						}
					}
					else if (sf::Color(0, 109, 0) == pixel_right)
					{
						sprite_x = 1;
					}
				}
				else if (sf::Color(0, 109, 0) == pixel)
				{
					sprite_y = 1;

					if (sf::Color(0, 73, 0) == pixel_left)
					{
						sprite_x = 3;
					}
					else if (sf::Color(0, 73, 0) == pixel_right)
					{
						sprite_x = 5;
					}
					else
					{
						sprite_x = 4;
					}
				}
				else if (sf::Color(109, 255, 85) == pixel)
				{
					sprite_x = 12;

					if (sf::Color(109, 255, 85) == pixel_up)
					{
						sprite_y = 1;
					}
				}

				cell_sprite.setTextureRect(sf::IntRect(CELL_SIZE * sprite_x, CELL_SIZE * sprite_y, CELL_SIZE, CELL_SIZE));

				i_window.draw(cell_sprite);
			}
		}
	}
}
void MapManager::draw_map(const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window)
{
	unsigned short map_end = ceil((SCREEN_WIDTH + i_view_x) / static_cast<float>(CELL_SIZE));
	unsigned short map_height = floor(map_sketch.getSize().y / 3.f);
	unsigned short map_start = floor(i_view_x / static_cast<float>(CELL_SIZE));

	//We're drawing the coin before drawing the blocks because we want it to appear behind the question block.

	sf::Sprite sprite;
	for (const Object& question_block_coin : question_block_coins)
	{
		coin_animation.set_position(question_block_coin.x, question_block_coin.y);
		//coin_animation.update();
		coin_animation.draw(sprite);
		i_window.draw(sprite);
	}


	for (unsigned short a = map_start; a < map_end; a++)
	{
		for (unsigned short b = 0; b < map_height; b++)
		{
			unsigned short sprite_x = 0;
			unsigned short sprite_y = 0;

			cell_sprite.setPosition(CELL_SIZE * a, CELL_SIZE * b);

			//This code is a big mess.
			//But it works.
			//Keep that in mind before judging me.		
			switch (map[a][b])
			{
			case Cell::Empty:
				continue;
			case Cell::Coin:
				coin_animation.set_position(CELL_SIZE * a, CELL_SIZE * b);
				coin_animation.draw(sprite);
				i_window.draw(sprite);
				continue;
			case Cell::QuestionBlock:
				question_block_animation.set_position(CELL_SIZE * a, CELL_SIZE * b);

				question_block_animation.draw(sprite);
				i_window.draw(sprite);
				continue;
			default:	
				//Since the underground blocks have a different look, I placed their texture 2 cells below the regular ones in the map texture			
				//sprite_y = 2 * i_underground;
				switch (map[a][b])
				{
				case Cell::ActivatedQuestionBlock:
					sprite_x = 6;
					sprite_y = 2 * i_underground + 1;					
					break;
				case Cell::Pipe:
					if (sf::Color(0, 182, 0) == map_sketch.getPixel(a, b))
					{
						sprite_y = 1;

						if (Cell::Pipe == map[a - 1][b])
						{
							sprite_x = 11;
						}
						else
						{
							sprite_x = 10;
						}
					}
					else if (sf::Color(0, 146, 0) == map_sketch.getPixel(a, b))
					{
						sprite_y = 0;

						if (sf::Color(0, 146, 0) == map_sketch.getPixel(a - 1, b))
						{
							sprite_x = 11;
						}
						else if (sf::Color(0, 146, 0) == map_sketch.getPixel(1 + a, b))
						{
							sprite_x = 10;
						}
						else
						{
							sprite_x = 10;

							if (sf::Color(0, 146, 0) == map_sketch.getPixel(a, b - 1))
							{
								sprite_y = 3;
							}
							else
							{
								sprite_y = 2;
							}
						}
					}
					else if (sf::Color(0, 219, 0) == map_sketch.getPixel(a, b))
					{
						if (sf::Color(0, 182, 0) == map_sketch.getPixel(1 + a, b))
						{
							sprite_x = 12;
						}
						else
						{
							sprite_x = 11;
						}

						if (sf::Color(0, 219, 0) == map_sketch.getPixel(a, b - 1))
						{
							sprite_y = 3;
						}
						else
						{
							sprite_y = 2;
						}
					}
					break;
				case Cell::Wall:
					if (sf::Color(0, 0, 0) == map_sketch.getPixel(a, b))
					{
						sprite_x = 2;
					}
					else
					{
						sprite_x = 3;
					}
					break;				
				}

				cell_sprite.setTextureRect(sf::IntRect(CELL_SIZE * sprite_x, CELL_SIZE * sprite_y, CELL_SIZE, CELL_SIZE));
				i_window.draw(cell_sprite);
			}
		
		}
	}
	//Drawing brick particles.

	for (const Object& brick_particle : brick_particles)
	{
		cell_sprite.setPosition(brick_particle.x, brick_particle.y);
		cell_sprite.setTextureRect(sf::IntRect(0.25f * CELL_SIZE, CELL_SIZE * (0.25f + 2 * i_underground), 0.5f * CELL_SIZE, 0.5f * CELL_SIZE));
		i_window.draw(cell_sprite);
	}

}

void MapManager::set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell)
{
	map[i_x][i_y] = i_cell;
}

void MapManager::set_map_size(const unsigned short i_new_size)
{
	map.clear();
	map.resize(i_new_size);
}

void MapManager::update_map_sketch(const unsigned char i_current_level)
{
	map_sketch.loadFromFile("Resources/Images/LevelSketch" + std::to_string(static_cast<unsigned short>(i_current_level)) + ".png");
}

void MapManager::update()
{
	for (Object& question_block_coin : question_block_coins)
	{
		question_block_coin.vertical_speed += GRAVITY;

		question_block_coin.y += question_block_coin.vertical_speed;
	}

	for (Object& brick_particle : brick_particles)
	{
		brick_particle.vertical_speed += GRAVITY;

		brick_particle.x += brick_particle.horizontal_speed;
		brick_particle.y += brick_particle.vertical_speed;
	}

	brick_particles.erase(remove_if(brick_particles.begin(), brick_particles.end(), [](const Object& i_brick_particle)
		{
			return SCREEN_HEIGHT <= i_brick_particle.y;
		}), brick_particles.end());

	question_block_coins.erase(remove_if(question_block_coins.begin(), question_block_coins.end(), [](const Object& i_question_block_coin)
		{
			return 0 <= i_question_block_coin.vertical_speed;
		}), question_block_coins.end());

	coin_animation.update();
	question_block_animation.update();
}
bool MapManager::map_collision(const sf::FloatRect& i_hitbox, short& x, short& y, std::vector<std::vector<Cell>>& i_cells, int i_check_cells) const
{
	std::vector<std::vector<Cell>>& output = i_cells;
	bool o_collision = false;
	short top = floor(i_hitbox.top / CELL_SIZE);
	short bottom = floor((ceil(i_hitbox.height + i_hitbox.top) - 1) / CELL_SIZE);
	short left = floor(i_hitbox.left / CELL_SIZE);
	short right = floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE);
	output.resize(right - left + 1);
	x = left;
	y = top;
	for (short b = left; b <= right; b++)
	{
		output[b - left].resize(bottom - top + 1);
		for (short a = top; a <= bottom; a++)
		{
			if (0 <= b && b < map.size())
			{
				if (0 <= a && a < map[0].size())
				{
					output[b - left][a - top] = map[b][a];
					if (i_check_cells & (1 << (int)map[b][a]))
					{
						//We're gonna return a vector of numbers. Each number is a binary representation of collisions in a single row.
						o_collision = true;
					}
				}
			}
			//We're assuming that the map borders have walls.
			else {
				output[b - left][a - top] = Cell::Wall;
				if (i_check_cells & (1 << (int)Cell::Wall))
				{
					o_collision = true;
				}
			}
		}
	}
	return o_collision;
}

//std::vector<unsigned char> MapManager::map_collision(int i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const
//{
//	std::vector<unsigned char> output;
//
//	i_collision_cells.clear();
//
//	for (short a = floor(i_hitbox.top / CELL_SIZE); a <= floor((ceil(i_hitbox.height + i_hitbox.top) - 1) / CELL_SIZE); a++)
//	{
//		output.push_back(0);
//
//		for (short b = floor(i_hitbox.left / CELL_SIZE); b <= floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE); b++)
//		{
//			if (0 <= b && b < map.size())
//			{
//				if (0 <= a && a < map[0].size())
//				{
//					if (i_check_cells & (1 << (int) map[b][a]))
//					{
//						//Since C++ doesn't support returning 2 vectors, we're gonna change the argument vector.
//						i_collision_cells.push_back(sf::Vector2i(b, a));
//
//						output[a - floor(i_hitbox.top / CELL_SIZE)] |= 1 << ((short) floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE) - b);
//					}
//				}
//			}
//			else if (i_check_cells & (1 << (int) Cell::Wall))
//			{
//				output[a - floor(i_hitbox.top / CELL_SIZE)] |=  1 << ((short) floor((ceil(i_hitbox.left + i_hitbox.width) - 1) / CELL_SIZE) - b);
//			}
//		}
//	}
//
//	return output;
//}

sf::Color MapManager::get_map_sketch_pixel(const unsigned short i_x, const unsigned short i_y) const
{
	return map_sketch.getPixel(i_x, i_y);
}
