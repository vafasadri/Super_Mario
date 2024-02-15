#pragma once
#include "Headers/Entity.hpp"
class Trampoline : public Entity
{
public:
	Trampoline(float i_x, float i_y);
	static int TypeID() {
		static int id = -1;
		if (id == -1)
			id = Entity::RegisterTypeID();
		return id;
	}

	int get_type_id() const override {
		return TypeID();
	}
	int get_collision_flags() const override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	void on_entity_collision(Entity* i_entity, bool i_already_colliding) override;
	//void timer_callback(long long i_callback_code, long long i_arg) override;
	sf::FloatRect get_hit_box() const override;
};

