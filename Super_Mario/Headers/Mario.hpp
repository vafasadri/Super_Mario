#pragma once
#include "Entity.hpp"

class Mario : public Entity //: Entity
{
	friend class Mushroom;

	bool crouching : 1;	
	bool flipped : 1;
	bool on_ground : 1;
	bool invincible : 1;
	bool growing : 1;

	//The longer we press the jump button, the higher Mario jumps. And we use this variable for that.

	//I was thinking about adding Fire Mario.
	//But I figured that it's gonna be too much work and I'm too lazy for that.
	//Maybe in the next part?
	unsigned char jump_timer;
	unsigned short coins;
	//Oh, this is a funny story!
	//So I was working on level 1-2 when I discovered a simple bug.
	//The bug was that when 2 Goombas stand SUPER close to each other and Mario jumps on them, one goomba dies and the other kills Mario.
	//This was happening because the first Goomba was setting Mario's vertical speed below 0 after dying.
	//Then the second Goomba checked the collision with Mario and saw that Mario wasn't squishing anymore and instead was going upwards.
	//So he was killing Mario.
	//And by adding this variable, I fixed the bug.
	//Hehe.
	//float horizontal_speed;
	//float vertical_speed;
	//float x;
	//float y;
	unsigned short blinking_iterator;
	unsigned short invincible_timer;
	
	//std::vector<Mushroom> mushrooms;
	

	const std::vector<sf::Texture>* texture_set;
	Animation big_walk_animation;
	Animation walk_animation;
	MapManager* map_manager;
	std::list<Entity*>* entities;
protected:
	unsigned char powerup_state;
public:
	static int TypeID() {
		static int id = -1;
		if (id == -1) 
			id = Entity::RegisterTypeID();
		return id;
	}
	int get_type_id() const override {
		return TypeID();
	}
	//inline static int ID = Entity::RegisterDerivingId();
	Mario(MapManager& i_manager,std::list<Entity*>& i_entities);

	
	int get_collision_flags() const override;
	float get_x() const;
	void grow_up();
	void die(DeathCause i_deathcause) override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	void reset();
	void set_position(const float i_x, const float i_y);
	void callback(long long i_callback_code, long long i_arg) override;
	void update(MapManager& i_map_manager);
	void on_entity_collision(Entity* entity,bool i_already_colliding) override;
	bool instant_death_condition() override { 
		return y + get_hit_box().height > SCREEN_HEIGHT - CELL_SIZE; }
	int get_map_collision_flags() override { return /* all cells*/ -1 & /* except Cell::Empty*/ ~1 ; }
	void on_map_collision_vertical(short x, short y,const std::vector<std::vector<Cell>>&) override;
	void on_map_collision_horizontal(short x, short y, const std::vector<std::vector<Cell>>&) override;
	sf::FloatRect get_hit_box()  const override;
};