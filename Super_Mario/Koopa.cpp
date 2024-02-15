#include <array>
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
#include "Headers/Entity.hpp"
#include "Headers/Koopa.hpp"
#include "Headers/Goomba.hpp"
static auto normal_texture_set = get_textures({ "Resources/Images/KoopaWalk.png","Resources/Images/KoopaGetOut.png","Resources/Images/KoopaShell.png","Resources/Images/KoopaDeath.png" });
static auto underground_texture_set = get_textures({ "Resources/Images/UndergroundKoopaWalk.png","Resources/Images/UndergroundKoopaGetOut.png","Resources/Images/UndergroundKoopaShell.png","Resources/Images/UndergroundKoopaDeath.png" });
Koopa::Koopa(const bool i_underground, const float i_x, const float i_y) :
	Entity(i_x, i_y),
	flipped(1),
	underground(i_underground),
	state(0),
	get_out_timer(KOOPA_GET_OUT_DURATION),
	texture_set(i_underground ? underground_texture_set : normal_texture_set),
	walk_animation(CELL_SIZE, &texture_set[0], KOOPA_WALK_ANIMATION_SPEED),
	get_out_animation(CELL_SIZE, &texture_set[1], KOOPA_GET_OUT_ANIMATION_SPEED)
{
	move = entity_collisions = apply_gravity = map_collisions = alive = true;
	register_animation(&get_out_animation, false);
	register_animation(&walk_animation, true);

	horizontal_speed = -KOOPA_SPEED;

	texture = &texture_set[2];
}


int Koopa::get_collision_flags() const
{
	return (1 << Mario::TypeID()) | (1 << Goomba::TypeID()) | (1 << Koopa::TypeID());
}

void Koopa::on_map_collision_horizontal_secondary(short x, short y, const std::vector<std::vector<Cell>>&)
{
	horizontal_speed *= -1;
	flipped = !flipped;
}

void Koopa::die(DeathCause i_death_type)
{
	switch (i_death_type)
	{
	case instant_death:
		//Instant death.
		// just delete this entity
		// don't do anything else
		erase = true;	
		break;
	case sliding_koopa:

		//Dying from Koopa's shell.
		// don't collide with anything anymore
		map_collisions = entity_collisions = false;
		
		vertical_speed = 0.5f * MARIO_JUMP_SPEED;
		walk_animation.disable();
		texture = &texture_set[3];
		break;
	}
}

void Koopa::draw(const unsigned i_view_x, sf::Sprite& i_sprite)
{
	if (-CELL_SIZE < round(y) && round(x) > static_cast<int>(i_view_x) - CELL_SIZE && round(x) < SCREEN_WIDTH + i_view_x && round(y) < SCREEN_HEIGHT)
	{

		if (1 == alive && 1 == state && get_out_timer <= 0.25f * KOOPA_GET_OUT_DURATION)
		{
			get_out_animation.set_position(round(x), round(y));
			get_out_animation.draw(i_sprite);
		}
		else if (0 != state || 0 == alive)
		{
			i_sprite.setPosition(round(x), round(y));
			i_sprite.setTexture(*texture,true);		
		}
		else
		{
			walk_animation.set_flipped(flipped);
			walk_animation.set_position(round(x), round(y));
			walk_animation.draw(i_sprite);
		}
	}
}

//void Koopa::update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario)
//{
//	//I've already explained most of the code here in the Mario and Goomba classes.
//	//I'm so bad at writing comments lol.
//
//		if (0 == get_dead(normal))
//		{					
//			if (0 < horizontal_speed)
//			{
//				flipped = 0;
//			}
//			else if (0 > horizontal_speed)
//			{
//				flipped = 1;
//			}			
//			//walk_animation.update();
//		}		
//}

void Koopa::on_entity_collision(Entity* entity, bool i_already_colliding)
{
	if (i_already_colliding) return;

	if (0 == state)
	{
		//Turning into a shell.
		if (0 < entity->vertical_speed)
		{
			//We check the collision only once after we collide with Mario.

			horizontal_speed = 0;

			state = 1;
			get_out_timer = KOOPA_GET_OUT_DURATION;
			set_timer(0, 0, (long long)entity);
		}
		else if (entity->get_type_id() == Mario::TypeID())
		{
			entity->die(normal_death);
		}
	}
	//Start sliding.
	else if (1 == state)
	{
		state = 2;
		//The direction is based on where Mario kicked Koopa.

		if (x + 0.5 * CELL_SIZE < entity->x)
		{
			horizontal_speed = -KOOPA_SHELL_SPEED;
			flipped = true;
		}
		else
		{
			horizontal_speed = KOOPA_SHELL_SPEED;
			flipped = false;
		}

		if (0 < entity->vertical_speed)
		{
			set_timer(0, 0, (long long)entity);
			//i_mario.set_vertical_speed();
		}
	}
	else
	{
		//Stop sliding.
		if (0 < entity->vertical_speed)
		{
			//flipped = horizontal_speed >= 0;
			horizontal_speed = 0;

			state = 1;

			get_out_animation.enable();
			set_timer(KOOPA_GET_OUT_DURATION, 1);
			set_timer(0, 0, (long long)entity);
		}
		//Don't stand in front of a sliding shell.
		else if ((0 < horizontal_speed && x + 0.5 * CELL_SIZE < entity->x) ||
			(0 > horizontal_speed && x + 0.5 * CELL_SIZE > entity->x))
		{
			entity->die(sliding_koopa);
		}
	}
}


void Koopa::callback(long long i_callback_code, long long i_arg)
{
	switch (i_callback_code)
	{
	case 0:
		((Entity*)i_arg)->vertical_speed = 0.75f * MARIO_JUMP_SPEED;
		break;
	case 1:
		state = 0;
		horizontal_speed = (flipped ? -KOOPA_SPEED : KOOPA_SPEED);
		get_out_animation.disable();
		break;
	}
}
