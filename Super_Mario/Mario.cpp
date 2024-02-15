#include <array>
#include <chrono>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
#include <iostream>
#include "Headers/Koopa.hpp"
#include "Headers/Goomba.hpp"
enum {
	idle = 0, jumping = 1, death = 2, brake = 3, crouched = 4
};
static std::vector<sf::Texture> marioTextureSet = get_textures({ "Resources/Images/MarioIdle.png","Resources/Images/MarioJump.png","Resources/Images/MarioDeath.png","Resources/Images/MarioBrake.png","Resources/Images/MarioIdle.png","Resources/Images/MarioWalk.png" });
static std::vector<sf::Texture> big_marioTextureSet = get_textures({ "Resources/Images/BigMarioIdle.png","Resources/Images/BigMarioJump.png","Resources/Images/BigMarioDeath.png","Resources/Images/BigMarioBrake.png","Resources/Images/BigMarioCrouch.png","Resources/Images/BigMarioWalk.png" });


Mario::Mario(MapManager& i_manager, std::list<Entity*>& i_entities) : Entity(0, 0),
blinking_iterator(0),
entities(&i_entities),
map_manager(&i_manager),
crouching(0),
flipped(0),
on_ground(0),
jump_timer(0),
powerup_state(0),
invincible_timer(0),
invincible(0),
coins(0),
big_walk_animation(CELL_SIZE, &big_marioTextureSet.back(), MARIO_WALK_ANIMATION_SPEED),
walk_animation(CELL_SIZE, &marioTextureSet.back(), MARIO_WALK_ANIMATION_SPEED)
{	
	alive = move = apply_gravity = map_collisions = entity_collisions = true;
	texture_set = &marioTextureSet;
}


int Mario::get_collision_flags() const
{
	return (1 << Koopa::TypeID() | 1 << Goomba::TypeID() | 1 << Mushroom::TypeID());

}


float Mario::get_x() const
{
	return x;
}

void Mario::grow_up()
{
	powerup_state = 1;
	//growth_timer = MARIO_GROWTH_DURATION;
	growing = true;
	y -= CELL_SIZE;
	set_timer(MARIO_GROWTH_DURATION,2);	
}

void Mario::die(DeathCause i_deathcause)
{
	//Mario instantly dies and it doesn't matter if he's big or small.
	if (instant_death == i_deathcause)
	{
		if (1 == alive) {
			entity_collisions = map_collisions = alive = apply_gravity = false;					
			vertical_speed = 0;
			horizontal_acceleration = 0;					
			horizontal_speed = 0;
			set_timer(MARIO_DEATH_DURATION, 1);		
		}
	}
	//Mario dies, unless he's big.
	else if (growing == 0 && invincible_timer == 0)
	{
		if (0 == powerup_state)
		{
			die(instant_death);

		}
		else
		{
			powerup_state = 0;
			set_timer(MARIO_INVINCIBILITY_DURATION, 0);
			invincible_timer = MARIO_INVINCIBILITY_DURATION;

			if (0 == crouching)
			{
				y += CELL_SIZE;
			}
			else
			{
				crouching = 0;
			}
		}
	}
}

void Mario::draw(const unsigned i_view_x, sf::Sprite& i_sprite)
{
	const auto& set = *texture_set;
	const sf::Texture* texture;
	//When Mario is invincible, his sprite will blink.
	i_sprite.setTextureRect({ 0, 0, 0, 0 });
	if (0 == invincible_timer / MARIO_BLINKING % 2)
	{
		if (texture_set != &big_marioTextureSet && powerup_state > 0 && 0 == crouching) {
			i_sprite.setPosition(x, y + CELL_SIZE);
		}
		else i_sprite.setPosition(x, y);
		if (0 == alive) {
			texture = &set[death];
		}
		else if (crouching) {
			texture = &set[crouched];
		}
		else if (0 == on_ground) {
			texture = &set[jumping];
		}
		else if (0 == horizontal_speed) {
			texture = &set[idle];
		}
		else if (0 == (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && (
			(0 < horizontal_speed && 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) ||
			(0 > horizontal_speed && 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
				))) {
			texture = &set[brake];
		}
		else {
			Animation& walk = texture_set == &big_marioTextureSet ? big_walk_animation : walk_animation;
			walk.set_flipped(flipped);
			auto& pos = i_sprite.getPosition();
			walk.set_position(pos.x, pos.y);
			walk.draw(i_sprite);		
			return;
		}
		i_sprite.setTexture(*texture,true);
		
		if (flipped) {
			sf::IntRect rect = i_sprite.getTextureRect();
			rect.left += rect.width;
			rect.width *= -1;
			i_sprite.setTextureRect(rect);
		}
	}
}

void Mario::reset()
{
	clear_data();
	crouching = 0;	
	alive = move = apply_gravity = map_collisions = entity_collisions = true;
	erase = false;
	growing = false;
	flipped = 0;
	on_ground = 0;
	horizontal_speed = 0;
	vertical_speed = 0;
	x = 0;
	y = 0;

	jump_timer = 0;
	powerup_state = 0;

	//If it wasn't for this, everything would be zerfect! (see what I did there?)
	//...
	//...
	//...
	//Yeah, I agree. That was bad.
	invincible_timer = 0;
	//mushrooms.clear();
	texture_set = &marioTextureSet;
	//texture.loadFromFile("Resources/Images/MarioIdle.png");	
	big_walk_animation.set_animation_speed(MARIO_WALK_ANIMATION_SPEED);
	big_walk_animation.set_flipped(0);

	walk_animation.set_animation_speed(MARIO_WALK_ANIMATION_SPEED);
	walk_animation.set_flipped(0);
}

void Mario::set_position(const float i_x, const float i_y)
{
	x = i_x;
	y = i_y;
}

void Mario::callback(long long i_callback_code, long long i_arg)
{
	switch (i_callback_code)
	{
	case 0:
		invincible = false;
		break;
	case 1:
		vertical_speed = MARIO_JUMP_SPEED;
		apply_gravity = true;
		break;
	case 2:
		growing = false;
		texture_set = &big_marioTextureSet;		
	default:
		break;
	}
}

void Mario::update(MapManager& i_map_manager)
{
	//We make Mario bounce after updating all the enemies to prevent a bug (Go to Mario.hpp for explanation).
	/*if (0 != enemy_bounce_speed)
	{
		vertical_speed = enemy_bounce_speed;

		enemy_bounce_speed = 0;
	}

	for (Mushroom& mushroom : mushrooms)
	{
		mushroom.update(i_view_x, i_map_manager);
	}*/

	if (growing) {

		if ((blinking_iterator / MARIO_BLINKING) % 2 == 0) {
			texture_set = &big_marioTextureSet;
		}
		else texture_set = &marioTextureSet;
		blinking_iterator++;
	}
	else {
		blinking_iterator = 0;
		if (powerup_state > 0) {
			texture_set = &big_marioTextureSet;
		}
		else texture_set = &marioTextureSet;
	}
	if (1 ==alive)
	{
		bool moving = 0;

		//So basically, the map_collision function returns a vector of numbers. Each number is a binary representation of the collisions in a single row. And we're storing that vector in this vector.
		std::vector<std::vector<Cell>> collision;
		short t_x, t_y;
		//Oh yeah, the map_collision function can also return the coordinates of cells intersecting the hitbox. We need that too.
		//std::vector<sf::Vector2i> cells;

		sf::FloatRect hit_box = get_hit_box();

		//on_ground = 0;
		hit_box = get_hit_box();
		hit_box.top++;

		on_ground = i_map_manager.map_collision(hit_box, t_x, t_y, collision, MapManager::packCellList({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }));	
		if (0 < invincible_timer)
		{
			invincible_timer--;
		}
		if (0 == crouching)
		{

			if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) horizontal_speed -= MARIO_ACCELERATION;
			
			if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) horizontal_speed += MARIO_ACCELERATION;	

			moving = (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) != sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
			horizontal_speed = std::clamp(horizontal_speed, -MARIO_WALK_SPEED, MARIO_WALK_SPEED);
		}

		if (moving == 0)
		{
			if (0 < horizontal_speed)
			{
				horizontal_speed = std::max<float>(0, horizontal_speed - MARIO_ACCELERATION);
			}
			else if (0 > horizontal_speed)
			{
				horizontal_speed = std::min<float>(0, MARIO_ACCELERATION + horizontal_speed);
			}
		}

		if (0 < powerup_state)
		{
			if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::C) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				if (0 == crouching)
				{
					crouching = 1;
					if (on_ground) {
						y += CELL_SIZE;
					}
				}
			}
			else if (1 == crouching)
			{
				hit_box = get_hit_box();
				hit_box.height += CELL_SIZE;
				hit_box.top -= CELL_SIZE;

				//Making sure we can stand up without hitting anything.	
				if (0 == i_map_manager.map_collision(hit_box, t_x, t_y,collision, MapManager::packCellList({ Cell::ActivatedQuestionBlock, Cell::Brick, Cell::Pipe, Cell::QuestionBlock, Cell::Wall })))
				{
					crouching = 0;
					if (on_ground) {
						y -= CELL_SIZE;
					}
				}
				else
				{
					//collision = i_map_manager.map_collision({ Cell::ActivatedQuestionBlock, Cell::Pipe, Cell::QuestionBlock, Cell::Wall }, hit_box);

					//But if it happens to be bricks, we'll destroy them.
					if (1 == std::all_of(collision.begin(), collision.end(), [](std::vector<Cell>& column)
						{
							return std::all_of(column.begin(), column.end(), [](Cell ce) { return ce == Cell::Empty || ce == Cell::Brick; });
						}))
					{
						crouching = 0;

						y -= CELL_SIZE;

						//i_map_manager.map_collision({ Cell::Brick }, cells, hit_box);
						for (int j = 0; j < collision.size(); j++)
						{
							for (int i = 0; i < collision[j].size(); i++)
							{
								short cellx = t_x + j;
								short celly = t_y + i;
								Cell cell = collision[j][i];
								if (cell == Cell::Brick) {
									i_map_manager.set_map_cell(cellx, celly, Cell::Empty);
									i_map_manager.add_brick_particles(CELL_SIZE * cellx, CELL_SIZE * celly);
								}
							}
						}
					}
				}
			}
		}
	
		hit_box = get_hit_box();
		hit_box.top++;		
		
		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			if (0 == vertical_speed && on_ground)
			{
				vertical_speed = MARIO_JUMP_SPEED;

				jump_timer = MARIO_JUMP_TIMER;
			}
			else if (0 < jump_timer) //The longer we press the jump button, the higher Mario jumps.
			{
				vertical_speed = MARIO_JUMP_SPEED;

				jump_timer--;
			}		
		}
		else
		{
			
			jump_timer = 0;
		}
		
		if (0 == horizontal_speed)
		{
			if (1 == moving)
			{
				flipped = !flipped;
			}
		}
		else if (0 < horizontal_speed)
		{
			flipped = 0;
		}
		else if (0 > horizontal_speed)
		{
			flipped = 1;
		}

		if (y >= SCREEN_HEIGHT - get_hit_box().height)
		{
			die(instant_death);
		}

		if (0 == powerup_state)
		{
			walk_animation.set_animation_speed(MARIO_WALK_ANIMATION_SPEED * MARIO_WALK_SPEED / abs(horizontal_speed));
			walk_animation.update();
		}
		else
		{
			big_walk_animation.set_animation_speed(MARIO_WALK_ANIMATION_SPEED * MARIO_WALK_SPEED / abs(horizontal_speed));
			big_walk_animation.update();
		}
	}
	

	//Deleting mushrooms from the vector.
	/*mushrooms.erase(remove_if(mushrooms.begin(), mushrooms.end(), [](const Mushroom& i_mushroom)
		{
			return 1 == i_mushroom.get_dead();
		}), mushrooms.end());*/
}





sf::FloatRect Mario::get_hit_box() const
{
	//The hitbox will be small if Mario is small or crouching.
	if (1 == crouching || 0 == powerup_state)
	{
		return sf::FloatRect(x + CELL_SIZE / 8, y, CELL_SIZE - CELL_SIZE / 8, CELL_SIZE);
	}
	else
	{
		return sf::FloatRect(x + CELL_SIZE / 8, y, CELL_SIZE - CELL_SIZE / 8, 2 * CELL_SIZE);
	}
}
void Mario::on_entity_collision(Entity* entity, bool i_already_colliding)
{

}

void Mario::on_map_collision_vertical(short x, short y, const std::vector<std::vector<Cell>>& i_cells)
{
	bool real_collision = false;
	for (int j = 0; j < i_cells.size(); j++)
	{
		for (int i = 0; i < i_cells[j].size(); i++)
		{
			short cellx = x + j;
			short celly = y + i;
			Cell cell = i_cells[j][i];
			//Destroying bricks!!!!
			if (0 > vertical_speed) {
				if (cell == Cell::Brick || cell == Cell::QuestionBlock) {
					for (auto& entity : *entities)
					{
						if (entity->y < celly * CELL_SIZE) {
							auto htbx = entity->get_hit_box();
							htbx.top++;
							if (htbx.intersects(sf::FloatRect{ (float)cellx * CELL_SIZE,(float)celly * CELL_SIZE,CELL_SIZE,CELL_SIZE })) {
								entity->die(sliding_koopa);
							}
						}
					}
				}
				if (0 == crouching && cell == Cell::Brick && 0 < powerup_state) {					
						map_manager->set_map_cell(cellx, celly, Cell::Empty);
						map_manager->add_brick_particles(CELL_SIZE * cellx, CELL_SIZE * celly);								
				}
				//Activating question blocks!!!!
				else if (cell == Cell::QuestionBlock) {
					map_manager->set_map_cell(cellx, celly, Cell::ActivatedQuestionBlock);
					//It can be either a mushroom or a coin, depending on the color of the pixel in the sketch.
					if (sf::Color(255, 73, 85) == map_manager->get_map_sketch_pixel(cellx, celly))
					{
						entities->push_front(new Mushroom(CELL_SIZE * cellx, CELL_SIZE * celly));
					}
					else
					{

						map_manager->add_question_block_coin(CELL_SIZE * cellx, CELL_SIZE * celly);
						coins++;
					}
				}
			}
			if (cell == Cell::Coin) {
				map_manager->set_map_cell(cellx, celly, Cell::Empty);
				coins++;
			}
			else if (cell != Cell::Empty) {
				real_collision = true;
			}
		}
	}
	if (real_collision) {


		Entity::on_map_collision_vertical(x, y, i_cells);

		jump_timer = 0;

		
	}
}

void Mario::on_map_collision_horizontal(short x, short y, const std::vector<std::vector<Cell>>& i_cells)
{
	bool real_collision = false;
	for (int j = 0; j < i_cells.size(); j++)
	{
		for (int i = 0; i < i_cells[j].size(); i++)
		{
			short cellx = x + j;
			short celly = y + i;
			Cell cell = i_cells[j][i];
			if (cell == Cell::Coin) {
				map_manager->set_map_cell(cellx, celly, Cell::Empty);		
				coins++;
			}
			else if (cell != Cell::Empty) {
				real_collision = true;
			}
		}
	}
	if (real_collision) {		
		Entity::on_map_collision_horizontal(x, y, i_cells);
		horizontal_speed = 0;
	}
}
