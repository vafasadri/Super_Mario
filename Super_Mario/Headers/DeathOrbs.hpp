#pragma once
#include "Entity.hpp"
#include "Mario.hpp"
#include <iostream>
class DeathOrbs :
	
    public Entity
{
	float centre_x;
	float centre_y;
	float radius;
	float angle;
	bool off;
	Animation turn_on_animation;
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
	DeathOrbs(float i_centre_x, float i_centre_y, float i_radius);
	
	inline void die(DeathCause i_deathCause) override {};
	inline sf::FloatRect get_hit_box() const override {
		return sf::FloatRect{ x,y ,10,10 };
	}
	void draw(const unsigned i_view_x, sf::Sprite& i_sprite) override;	
	inline int get_map_collision_flags() override { return 0; }
	inline int get_collision_flags() const override {
		return 1 << Mario::TypeID();
	}
	void on_entity_collision(Entity* i_entity, bool i_already_colliding) override;
	inline bool instant_death_condition() override {
		// never gonna die
		// (never gonna give you up)
		return false;
	}
	void callback(long long i_callback_code, long long i_arg) override;
	void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) override;

};

