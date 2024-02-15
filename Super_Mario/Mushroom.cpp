#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
Mushroom::Mushroom(const float i_x, const float i_y) :
	Entity(i_x, i_y),
	horizontal_direction(1),
	start_y(i_y),
	in_box(1)
{
	alive = move = entity_collisions = true;
	//vertical_acceleration = -GRAVITY;
	vertical_speed = -MUSHROOM_SPEED;
	set_timer(CELL_SIZE / MUSHROOM_SPEED);
	texture.loadFromFile("Resources/Images/Mushroom.png");
}

void Mushroom::callback(long long i_callback_code, long long i_arg)
{
	y = start_y - CELL_SIZE;
	start_y = 2 * SCREEN_HEIGHT;
	horizontal_speed = MUSHROOM_SPEED;
	in_box = false;
	vertical_speed = 0;
	map_collisions = apply_gravity = true;
}


int Mushroom::get_collision_flags() const
{
	return 1 << Mario::TypeID() | 1 << Mushroom::TypeID();
}

void Mushroom::draw(const unsigned i_view_x, sf::Sprite& i_sprite)
{
	if (-CELL_SIZE < round(y) && round(x) > static_cast<int>(i_view_x) - CELL_SIZE && round(x) < SCREEN_WIDTH + i_view_x && round(y) < SCREEN_HEIGHT)
	{
		i_sprite.setPosition(round(x), round(y));
		
		i_sprite.setTexture(texture);
		if (in_box) {
			i_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE,round( start_y - y)));
		}
		if (0 ==  alive) {
			i_sprite.setTextureRect(sf::IntRect{ (int) texture.getSize().x,(int) texture.getSize().y,- (int)texture.getSize().x,- (int) texture.getSize().y });
		}
	}
}

void Mushroom::die(DeathCause i_deathCause)
{
	if (i_deathCause == sliding_koopa) {
		//Goomba dies from Koopa's shell.
		alive = entity_collisions = map_collisions = false;
		vertical_speed = 0.25f * MARIO_JUMP_SPEED;
		//horizontal_speed = 0;
	}
	else erase = true;
}

//void Mushroom::update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario)
//{
//	//Mushrooms behave the same way Goombas do so go check out the Goomba class.
//}

void Mushroom::on_entity_collision(Entity* entity,bool i_already_colliding)
{
	if (entity->get_type_id() == Mario::TypeID()) {
		die(instant_death);		
		Mario* mario = dynamic_cast<Mario*>(entity);
		if (0 == mario->powerup_state)
		{
			mario->grow_up();
		}
		
	}
}

sf::FloatRect Mushroom::get_hit_box() const
{
	return sf::FloatRect(x, y, CELL_SIZE, CELL_SIZE);
}

