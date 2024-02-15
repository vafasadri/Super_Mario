#pragma once

//I used the keyword "this", which returns the memory address of the object. But since I switched to smart pointers, this class must inherit std::enable_shared_from_this<Enemy>. C++ is weird.
#include "MapManager.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include <set>
#include <map>
class Mario;

class Entity {
public:
	using Callback = void(Entity::*)(long long arg);
private:
	struct TimerData
	{
		Entity* instance;
		Callback callback_function;
		unsigned short frames_left;
		long long callback_args;
	};
	struct IntervalData
	{
		Entity* instance;
		Callback callback_function;
		long long callback_args;
		unsigned int iterator;
		unsigned short interval;
	};
	struct AnimationData
	{
		Animation* animation;
		Entity* owner;
	};
	static std::list<TimerData> timers;
	static std::list<IntervalData> intervals;
	static std::list<AnimationData> animations;
	static std::map<std::pair<void*, void*>, bool> collision_tracker;
	static std::multimap<sf::Keyboard::Key,Entity*> key_listeners;
	static int register_counter;
public:
	
	float horizontal_speed;
	float vertical_speed;
	float x;
	float y;	
		bool erase : 1;
		bool move : 1;
		bool entity_collisions : 1;
		bool map_collisions : 1;
		bool apply_gravity : 1;
		bool alive : 1; // reserved flag, no action is taken when set			
	Entity(const float i_x, const float i_y);
	void clear_data();
	// every entity type needs a numeric id mentioned inside its object so we can tell different entities apart
	virtual int get_type_id() const = 0;
	// this function is called when a timer you've set is activated
	virtual void die(DeathCause i_deathCause);
	//Apparently, we can set the function declaration to 0 and that's called a pure virtual function. Again, C++ is weird.
	// Another guy speaking: yeah pure virtual functions don't have a default definition inside the base class.
	//	  They are identical to abstract functions in other programming languages
	// a class that contains one of these is called an abstract class, which you cannot create an instance of 
	//	  Deriving classes need to override every pure virtual function 
	// of their parent or else they will be marked abstract as well
	virtual void draw(const unsigned i_view_x, sf::Sprite& i_window) = 0;
	virtual void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) {};

	virtual int get_map_collision_flags() {
		// All cells except Cell::Coin and Cell::Empty
		return ~(1 << (int)Cell::Coin | 1 << (int)Cell::Empty);
	}
	virtual void on_key_pressed(sf::Keyboard::Key) {};
	virtual int get_collision_flags() const = 0;
	virtual sf::FloatRect get_hit_box() const;
	virtual void on_entity_collision(Entity* i_entity, bool i_already_colliding) = 0;
	virtual void on_map_collision_vertical(short x, short y, const std::vector<std::vector<Cell>>&);
	virtual void on_map_collision_horizontal(short x, short y, const std::vector<std::vector<Cell>>&);
	inline virtual bool instant_death_condition() {
		return SCREEN_HEIGHT <= y;
	}
	virtual ~Entity() {
		clear_data();
	};	
	static void update_timers();
	static void update_animations();
	static void update_intervals();
	static void clear_all_data();
	
	static void check_collision(Entity* i_entity_1, Entity* i_entity_2);
	static void update_key_presses();
protected:
	void ListenForKey(sf::Keyboard::Key key) {
	}
	decltype(intervals)::iterator set_interval(unsigned short frames,Callback callback, long long args = 0);
	void unset_interval(decltype(intervals)::iterator handle);
	void set_timer(unsigned short frames, Callback callback, long long args = 0);
	void register_animation(Animation* i_animation, bool i_enabled);
	bool collides_with(int i_type_id);

	inline virtual void on_map_collision_vertical_secondary(short x, short y, const std::vector<std::vector<Cell>>&) {};
	inline virtual void on_map_collision_horizontal_secondary(short x, short y, const std::vector<std::vector<Cell>>&) {
		horizontal_speed *= -1;
	};
	static int RegisterTypeID() {
		
		return register_counter++;
	}
};
