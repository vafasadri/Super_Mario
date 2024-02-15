#pragma once
#include "Headers/Entity.hpp"
class CannonAimDot : public Entity
{
	
	public:
		static int TypeID() {
			static int id = -1;
			if (id == -1) id = Entity::RegisterTypeID();
			return id;
		}
		inline int get_type_id() const override {
			return TypeID();
		}
		void on_map_collision_vertical(short x, short y, const std::vector<std::vector<Cell>>&) override;
		void on_map_collision_horizontal(short x, short y, const std::vector<std::vector<Cell>>&) override;
		CannonAimDot(const float i_x, const float i_y,const float i_hor_speed,const float i_vert_speed);
		int get_collision_flags() const override;
		void draw(const unsigned i_view_x, sf::Sprite& i_window) override;

		sf::FloatRect get_hit_box() const override {
			return sf::FloatRect(x, y, 0.5 * CELL_SIZE, 0.5 * CELL_SIZE);
		}
		void on_entity_collision(Entity* entity, bool i_already_colliding) override;
		void callback(long long i_callback_code, long long i_arg) override;
		void update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario) override;

		~CannonAimDot() override
		{
		}
};


