#pragma once
#include "Entity.hpp"
class Mushroom : public Entity
{	
	bool in_box;
	char horizontal_direction;
	//Before the mushroom begins to move, it needs to get out of the question block.
	unsigned short start_y;

	//sf::Sprite sprite;

	sf::Texture texture;
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
	Mushroom(const float i_x, const float i_y);
	void callback(long long i_callback_code, long long i_arg) override;
	int get_collision_flags() const override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	void die(DeathCause i_value) override;
	//void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) override;
	void on_entity_collision(Entity* entity,bool i_already_colliding) override;
	sf::FloatRect get_hit_box() const override;
};