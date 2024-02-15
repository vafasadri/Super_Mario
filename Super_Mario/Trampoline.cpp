#include <array>
#include <cmath>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/Mario.hpp"
#include "Trampoline.h"
sf::Texture texture = get_textures({ "Resources/Images/trampoline.png" })[0];
Trampoline::Trampoline(float i_x,float i_y) : Entity(i_x,i_y)
{
	move = entity_collisions = map_collisions = apply_gravity = true;
}

int Trampoline::get_collision_flags() const
{
	return 1 << Mario::TypeID();
}

void Trampoline::draw(const unsigned i_view_x, sf::Sprite& i_sprite)
{
	i_sprite.setPosition(x, y - 6);
	i_sprite.setTexture(texture,true);
}

void Trampoline::on_entity_collision(Entity* i_entity, bool i_already_colliding)
{
	if (i_entity->vertical_speed > 0 && i_entity->y + i_entity->get_hit_box().height - i_entity->vertical_speed  < y) {
		i_entity->vertical_speed *= -1;
	}
}

sf::FloatRect Trampoline::get_hit_box() const
{

	return sf::FloatRect(x, y,32, 20 - 6);
}

