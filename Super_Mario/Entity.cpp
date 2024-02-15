#include <array>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <assert.h>
#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Mushroom.hpp"
#include "Headers/Mario.hpp"
#include "Headers/Entity.hpp"


int Entity::register_counter = 0;
decltype(Entity::key_listeners) Entity::key_listeners{ };

Entity::Entity(const float i_x, const float i_y) :
	entity_collisions(0),
	map_collisions(0),
	alive(0),
	erase(0),
	move(0),
	apply_gravity(0),
	horizontal_speed(0),
	vertical_speed(0),
	x(i_x),
	y(i_y)
{

}

void Entity::die(DeathCause i_death_type)
{
	erase = true;
}

sf::FloatRect Entity::get_hit_box() const
{
	return sf::FloatRect(x, y, CELL_SIZE, CELL_SIZE);
}

void Entity::on_map_collision_vertical(short t_x, short t_y, const std::vector<std::vector<Cell>>& n)
{	
	y += vertical_speed;
	auto hitbox = get_hit_box();
	if (vertical_speed < 0) {
		y -= remainder(hitbox.top, CELL_SIZE);
	}
	else {
		y -= remainder(hitbox.top + hitbox.height , CELL_SIZE);
	}

	vertical_speed = 0;
	on_map_collision_vertical_secondary(x, y, n);
}

void Entity::on_map_collision_horizontal(short t_x, short t_y, const std::vector<std::vector<Cell>>& n)
{
	x += horizontal_speed;
	auto hitbox = get_hit_box();
	if (horizontal_speed < 0) {
		x -= remainder(hitbox.left, CELL_SIZE);
	}
	else {
		x -= remainder(hitbox.left + hitbox.width, CELL_SIZE);
	}
	on_map_collision_horizontal_secondary(t_x, t_y, n);
}

void Entity::set_timer(unsigned short frames, Callback callback, long long args) {
	TimerData data{};
	data.instance = this;
	data.callback_function = callback;
	data.frames_left = frames;
	data.callback_args = args;
	timers.push_back(data);
}
void Entity::clear_data()
{
	timers.remove_if([this](TimerData& data) {
		return data.instance == this;
		});

	std::_Erase_nodes_if(collision_tracker, [this](const std::pair<std::pair<void*, void*>, bool>& p) {
		//return (p.first >> 32) == (reinterpret_cast<long long>(entity) & 0xFFFFFFFF) || (p.first & 0xFFFFFFFF) == (reinterpret_cast<long long>(entity) & 0xFFFFFFFF);
		return p.first.first == this || p.first.second == this;
		});	
	animations.remove_if([this](const AnimationData& anime) {
		return anime.owner == this;
		});
	intervals.remove_if([this](const IntervalData& interval) {
		return interval.instance == this;
		});
}
decltype(Entity::intervals)::iterator Entity::set_interval(unsigned short frames, Callback callback, long long args)
{
	IntervalData data{};
	data.instance = this;
	data.callback_function = callback;
	data.interval = frames;
	data.iterator = 0;
	data.callback_args = args;
	intervals.push_back(data);	
	auto end = intervals.end();
	end--;
	return end;
}
void Entity::unset_interval(decltype(intervals)::iterator handle)
{	
	if (handle->instance != this) throw std::exception();
	intervals.erase(handle);
}
void Entity::register_animation(Animation* anim, bool enabled)
{
	AnimationData data{};
	data.animation = anim;
	data.owner = this;
	animations.push_back(data);
	if (enabled) anim->enable();
}
bool Entity::collides_with(int i_type_id) {
	return this->get_collision_flags() & (1 << i_type_id);
}
void Entity::check_collision(Entity* left, Entity* right) {

	assert(left != right);	
	std::pair<void*, void*> id = { left,right };
	if (left->collides_with(right->get_type_id()) || right->collides_with(left->get_type_id()))
	{
		if (left->get_hit_box().intersects(right->get_hit_box())) {
			// check if they were already colliding in the previous frame
			bool already = collision_tracker[id];
			left->on_entity_collision(right, already);
			right->on_entity_collision(left, already);
			collision_tracker[id] = true;
		}
		// track that they're not colliding anymore
		else collision_tracker[id] = false;
	}

}
void Entity::update_key_presses()
{	
	for (auto& i : key_listeners)
	{
		if (sf::Keyboard::isKeyPressed(i.first)) {
			i.second->on_key_pressed(i.first);
		}
	}
}
void Entity::clear_all_data() {
	collision_tracker.clear();
	timers.clear();
	animations.clear();
}
void Entity::update_intervals() {
	for (auto& data : intervals)
	{
		if (0 == ++data.iterator % data.interval ) {
			(data.instance->*data.callback_function)(data.callback_args);
		}
	}
}
void Entity::update_timers()
{
	timers.remove_if([](TimerData& data)
		{
			if (0 == data.frames_left--) {
				
				(data.instance->*data.callback_function)(data.callback_args);
				return true;
			}
			return false;
		});
}

void Entity::update_animations()
{	
	for (auto& i : animations)
	{
		if (i.animation->get_enabled()) i.animation->update();
	}
}
