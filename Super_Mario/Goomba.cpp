#include <array>
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
#include "Headers/Goomba.hpp"
#include "Headers/Koopa.hpp"
const std::vector<sf::Texture> Goomba::normal_texture_set = get_textures({ "Resources/Images/GoombaWalk.png","Resources/Images/GoombaDeath0.png","Resources/Images/GoombaDeath1.png" });
const std::vector<sf::Texture>  Goomba::underground_texture_set = get_textures({ "Resources/Images/UndergroundGoombaWalk.png","Resources/Images/UndergroundGoombaDeath0.png","Resources/Images/UndergroundGoombaDeath1.png" });
static bool load_textures = true;
Goomba::Goomba(const bool i_underground, const float i_x, const float i_y) :
	Entity(i_x, i_y),
	texture_set(i_underground ? underground_texture_set : normal_texture_set),	
	walk_animation(CELL_SIZE, &texture_set[0], GOOMBA_WALK_ANIMATION_SPEED)
{
	move = entity_collisions = map_collisions = alive = apply_gravity = true;
	register_animation(&walk_animation, true);
	horizontal_speed = -GOOMBA_SPEED;
	texture = &texture_set[1];
}


int Goomba::get_collision_flags() const
{
	return (1 << Mario::TypeID()) | (1 << Goomba::TypeID()) | (1 << Koopa::TypeID());
}

void Goomba::die(DeathCause i_death_type)
{
	switch (i_death_type)
	{
	case instant_death:
	{
		//Instant death. Setting dead to 1 will immediately delete the object.
		erase = true;
		break;
	}
	case normal_death:
	{
		if (alive) {
			set_timer(GOOMBA_DEATH_DURATION, 1);
			entity_collisions = alive = false;				
		}
		horizontal_speed = 0;
		//Goomba is squished by Mario.
		/*if (0 == no_collision_dying)
		{
			death_timer--;
		}*/

		break;
	}
	case sliding_koopa:
	{
		
		if (1 == alive)
		{
			//Goomba dies from Koopa's shell.
			map_collisions = entity_collisions = alive = false;		
			vertical_speed = 0.5f * MARIO_JUMP_SPEED;

			texture = &texture_set[2];
		}
	}
	}
}

void Goomba::draw(const unsigned i_view_x, sf::Sprite& i_window)
{
	//Making sure we don't draw Goomba outside the view.
	if (-CELL_SIZE < round(y) && round(x) > static_cast<int>(i_view_x) - CELL_SIZE && round(x) < SCREEN_WIDTH + i_view_x && round(y) < SCREEN_HEIGHT)
	{
		if (0 == alive)
		{
			i_window.setPosition(round(x), round(y));
			i_window.setTexture(*texture,true);		
		}
		else
		{
			walk_animation.set_position(round(x), round(y));
			walk_animation.draw(i_window);
		}
	}
}

void Goomba::on_entity_collision(Entity* entity, bool i_already_colliding)
{
	if (1 == i_already_colliding || 0 == alive) return;

	if (entity->vertical_speed > 0 && vertical_speed == 0 )
	{
		die(normal_death);
		//We make Mario bounce after one frame to prevent a bug
		set_timer(0, 3, (long long)entity);
	}
	else if (entity->get_type_id() == Mario::TypeID()) {
		entity->die(normal_death);
	}
	else {		
		horizontal_speed *= -1;
	}
}

void Goomba::callback(long long i_callback_code, long long i_arg)
{
	switch (i_callback_code) {
	case 0:
		horizontal_speed = i_arg;
		break;
	case 1:
		erase = true;	
		break;
	case 2:
		vertical_speed = 0;
		break;
	case 3:
		((Entity*)i_arg)->vertical_speed = 0.5f * MARIO_JUMP_SPEED;
		break;
	}

}

