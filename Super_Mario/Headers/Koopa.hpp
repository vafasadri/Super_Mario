#pragma once
#include "Entity.hpp"

class Koopa : public Entity
{
	bool flipped;
	//Look at the declaration of the Goomba class. I don't wanna explain this again.
	//I know I can just copy and paste, but I want you to do some work too, y'know.
	//I mean, why do I have to do everything for you?
	//Why do I have to waste my time copying and pasting 4 lines of comments when you can just open the file I told you to open?
	//Are you better than me?	
	bool underground;

	//0 - Koopa walk
	//1 - Koopa shell
	//2 - Koopa shell slide
	unsigned char state;

	unsigned short get_out_timer;

	
	const std::vector<sf::Texture>& texture_set;
	const sf::Texture* texture;

	Animation get_out_animation;
	Animation walk_animation;
public:
	static int TypeID() {
		static int id = -1;
		if (id == -1) id = Entity::RegisterTypeID();
		return id;	
		
	}
	inline int get_type_id() const override {
		return TypeID();		
	}
	Koopa(const bool i_underground, const float i_x, const float i_y);

	int get_collision_flags() const override;
	void on_map_collision_horizontal_secondary(short x, short y, const std::vector<std::vector<Cell>>&) override;
	void die(DeathCause i_death_type) override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	//void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) override;
	void on_entity_collision(Entity* entity,bool i_already_colliding);
	void callback(long long i_callback_code, long long i_arg);
	~Koopa() override
	{		
	}
};