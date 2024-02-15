#pragma once
#include "Headers/Entity.hpp"
class Cannon :
    public Entity
{
private:
	sf::RenderTexture texture_target;
	double angle;
	int dot_iterator;
	std::list<Entity*>* entities;
	Entity* entity_inside;
public:
	static int TypeID() {
		static int id = -1;
		if (id == -1) id = Entity::RegisterTypeID();
		return id;
	}
	inline int get_type_id() const override {
		return TypeID();
	}

	Cannon(const float i_x, const float i_y);
	sf::FloatRect get_hit_box()  const override {
		return { x,y,32,32 };
	}
	int get_collision_flags() const override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	void on_entity_collision(Entity* entity, bool i_already_colliding) override;
	void RemoveEntity(long long arg);
	void callback(long long i_callback_code, long long i_arg) override;
	void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) override;

	~Cannon() override
	{
	}
};

