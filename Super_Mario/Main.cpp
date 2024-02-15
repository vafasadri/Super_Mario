#include <array>
#include <chrono>
#include <cmath>
#include <list>

#include <SFML/Graphics.hpp>
#include <assert.h>
#include <iostream>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
#include "Headers/Entity.hpp"
#include "Headers/ConvertSketch.hpp"
#include "Trampoline.h"
#include "resource.h"
#include <Windows.h>
HMODULE appModule;
std::vector<sf::Texture> get_textures(const std::initializer_list<std::string>& srcs) {
	if (appModule == 0) {
		appModule = GetModuleHandle(0);
	}	
	std::vector <sf::Texture> output;
	for (auto& i : srcs)
	{
		sf::Texture texture;
		texture.loadFromFile(i);
		output.push_back(texture);
		
	}
	return output;
}
int main()
{
	
	unsigned char current_level = 0;

	unsigned short level_finish = 0;

	//We'll use this to make the game framerate-independent.
	std::chrono::microseconds lag(0);

	std::chrono::steady_clock::time_point previous_time;

	//Using smart pointer because I'm smart.
	//(Because we need to store both Goomba and Koopa objects in the same vector).
	std::list<Entity*> entities;

	sf::Color background_color = sf::Color(0, 219, 255);

	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(SCREEN_RESIZE * SCREEN_WIDTH, SCREEN_RESIZE * SCREEN_HEIGHT), "Super Mario Bros", sf::Style::Close);
	window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y - 90));

	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

	MapManager map_manager;

	Mario mario(map_manager, entities);
	convert_sketch(current_level, level_finish, entities, background_color, map_manager, mario);

	previous_time = std::chrono::steady_clock::now();
	window.setFramerateLimit(60);
	while (1 == window.isOpen())
	{
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time += delta_time;

		//Soooooo, how was your day?
		//Mine was good. I ate some cookies.
		//Then I watched Youtube.
		//...
		//...
		//...
		//Yeah.

		while (FRAME_DURATION <= lag)
		{
			unsigned view_x;

			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();

					break;
				}
				case sf::Event::KeyPressed:
				{
					switch (event.key.code)
					{
					case sf::Keyboard::Enter:
						//enemies.clear();
						entities.pop_back();
						for (auto& i : entities)
						{
							delete i;
						}
						Entity::clear_all_data();
						entities.clear();
						mario.reset();
						convert_sketch(current_level, level_finish, entities, background_color, map_manager, mario);
						break;
					}
				}


				}
			}

			//Once Mario goes beyond the finish, we move on to the next level.
			if (CELL_SIZE * level_finish <= mario.get_x())
			{
				current_level++;
				entities.pop_back();
				for (auto i : entities)
				{
					delete i;
				}
				Entity::clear_all_data();
				entities.clear();
				mario.reset();

				convert_sketch(current_level, level_finish, entities, background_color, map_manager, mario);
			}

			//Keeping Mario at the center of the view.
			view_x = std::clamp<int>(round(mario.get_x()) - 0.5f * (SCREEN_WIDTH - CELL_SIZE), 0, CELL_SIZE * map_manager.get_map_width() - SCREEN_WIDTH);
			map_manager.update();
			// checks if the entity is withing view range
			auto validateEntity = [view_x](const Entity* entity) {
				return 	entity->x >= static_cast<int>(view_x) - CELL_SIZE - ENTITY_UPDATE_AREA && entity->x < ENTITY_UPDATE_AREA + SCREEN_WIDTH + view_x && entity->y < SCREEN_HEIGHT;
			};

			mario.update(map_manager);
			// Checking for entity collsions			
			for (auto i = entities.begin(); i != entities.end(); i++)
			{
				Entity* entity = *i;
				if (!validateEntity(entity)) continue;
				entity->update(view_x, entities, map_manager, mario);
				entity->horizontal_speed = std::clamp(entity->horizontal_speed, -MAX_HORIZONTAL_SPEED, MAX_HORIZONTAL_SPEED);
				
				if (entity->apply_gravity) {
					entity->vertical_speed += GRAVITY;
				}
				entity->vertical_speed = std::clamp(entity->vertical_speed, -MAX_VERTICAL_SPEED, MAX_VERTICAL_SPEED);
				
				if (entity->map_collisions) {
					// check for block collsions
					short x, y;
					bool collision = false;
					auto hitbox_vertical = entity->get_hit_box();
					hitbox_vertical.top += entity->vertical_speed;
					std::vector<std::vector<Cell>> collision_cells;
					collision = map_manager.map_collision(hitbox_vertical, x, y, collision_cells, entity->get_map_collision_flags());
					if (collision) entity->on_map_collision_vertical(x, y, collision_cells);

					auto hitbox_horizontal = entity->get_hit_box();
					hitbox_horizontal.left += entity->horizontal_speed;
					collision = map_manager.map_collision(hitbox_horizontal, x, y, collision_cells, entity->get_map_collision_flags());
					if (collision) entity->on_map_collision_horizontal(x, y, collision_cells);

				}
				if (entity->entity_collisions) {
					// check for entity-entity collisions
					auto j = i;
					j++;
					for (; j != entities.end(); j++)
					{
						Entity* entity2 = *j;
						if (0 == entity2->entity_collisions || !validateEntity(entity2)) continue;
						Entity::check_collision(entity, entity2);
					}
				}
				if (entity->move) {
					
					entity->x += entity->horizontal_speed;
					entity->y += entity->vertical_speed;

				}
			
				//if (enemy1 == &mario) std::cout << enemy1->vertical_speed;
				if (entity->instant_death_condition())
				{
					entity->die(instant_death);
				}
			}

			// delete dead entities
			entities.remove_if([](Entity* entity) {
				if (entity->erase) {

					//entity->clear_data();
					delete entity;

					return true;
				}

				return false;
				});			
			Entity::update_timers();
			Entity::update_animations();
			if (FRAME_DURATION > lag)
			{
				view.reset(sf::FloatRect(view_x, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

				window.setView(view);
				window.clear(background_color);

				//If the background color is sf::Color(0, 0, 85), the level is underground.
				map_manager.draw_map_background( sf::Color(0, 0, 85) == background_color, view_x, window);

				//mario.draw_mushrooms(view_x, window);

				map_manager.draw_map(sf::Color(0, 0, 85) == background_color, view_x, window);

				
				for (Entity* a : entities)
				{
					sf::Sprite sprite;
					a->draw(view_x, sprite);
					window.draw(sprite);
				}
				
				//mario.draw(view_x, window);

				window.display();
			}
 		}
	}
}



