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
#include "Headers/DeathOrbs.hpp"

sf::Texture turnon_texture = get_textures({ "Resources/Images/deathorb_turn_on.png" })[0];

DeathOrbs::DeathOrbs(float i_centre_x, float i_centre_y, float i_radius) : Entity(i_centre_x, i_centre_y - i_radius),
centre_x(i_centre_x),
centre_y(i_centre_y),
angle(3.14f / 2),
radius(i_radius),
turn_on_animation(10,&turnon_texture, 7),
off(0)
{

	vertical_acceleration = -GRAVITY;
	register_animation(&turn_on_animation, false);
}

void DeathOrbs::draw(const unsigned i_view_x, sf::Sprite& i_sprite)
{
	if (!off) {
		sf::Texture texture;	
		texture.loadFromFile("Resources/Images/deathorb.png");
		//sprite.setTextureRect(sf::IntRect(0, 0, get_hit_box().width, get_hit_box().height));
		i_sprite.setPosition(x, y);
		i_sprite.setTexture(texture,true);		
	}
	else {
		turn_on_animation.set_position(x, y);
		turn_on_animation.draw(i_sprite);
	}
}

void DeathOrbs::callback(long long i_callback_code, long long i_arg)
{
	switch (i_callback_code)
	{
	case 0:
		off = false;
		turn_on_animation.disable();
		break;
		//turn_on_animation.set_animation_speed(10);
	case 1:
		turn_on_animation.set_animation_speed(DEATHORB_TURN_ON_ANIMATION_SPEED);
		set_timer(8, 0);
		break;	
	}
}
void DeathOrbs::update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario)
{
	x = centre_x + cos(angle) * radius - 5;
	y = centre_y - sin(angle) * radius - 5;
	angle -= 3.14f / 180.0f;
}
void DeathOrbs::on_entity_collision(Entity* i_entity, bool i_already_colliding) {	
	if (off) return;
	i_entity->die(normal_death);
	off = true;
	turn_on_animation.set_animation_speed(DEATHORB_TWILIGHT_ANIMATION_SPEED);
	turn_on_animation.enable();
	//set_timer(110);
	set_timer(9 * DEATHORB_TWILIGHT_ANIMATION_SPEED, 1);
	//std::cout << i_already_colliding << std::endl;
}


