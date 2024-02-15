#include "CannonAimDot.hpp"
sf::Texture s_texture = get_textures({ "Resources/Images/deathorb.png" })[0];
void CannonAimDot::on_map_collision_vertical(short x, short y, const std::vector<std::vector<Cell>>&)
{
	erase = true;
}
void CannonAimDot::on_map_collision_horizontal(short x, short y, const std::vector<std::vector<Cell>>&)
{
	erase = true;
}
CannonAimDot::CannonAimDot(const float i_x, const float i_y, const float i_hor_speed, const float i_vert_speed) :
	Entity(i_x, i_y)
{
	horizontal_speed = i_hor_speed;
vertical_speed = i_vert_speed;
move = apply_gravity = entity_collisions = map_collisions = true;
}

int CannonAimDot::get_collision_flags() const
{
	return 0;
}

void CannonAimDot::draw(const unsigned i_view_x, sf::Sprite& i_window)
{
	i_window.setPosition(x, y);
	i_window.setScale({ 0.5,0.5 });
	i_window.setTexture(s_texture);
}

void CannonAimDot::on_entity_collision(Entity* entity, bool i_already_colliding)
{
	erase = true;
}

void CannonAimDot::callback(long long i_callback_code, long long i_arg)
{
}

void CannonAimDot::update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario)
{
}
