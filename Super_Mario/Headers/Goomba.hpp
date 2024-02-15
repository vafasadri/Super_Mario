#pragma once

#include "Animation.hpp"
#include "Entity.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
class Goomba : public Entity
{
	const static std::vector<sf::Texture> normal_texture_set;
	const static std::vector<sf::Texture> underground_texture_set;

	const std::vector<sf::Texture>& texture_set;
	const sf::Texture* texture;

	Animation walk_animation;
public:
	static int TypeID() {
		static int id = -1;
		if (id == -1) id = Entity::RegisterTypeID();
		return id;
	}
	inline int get_type_id() const override  {
		return TypeID();
	}

	Goomba(const bool i_underground, const float i_x, const float i_y);
	int get_collision_flags() const override;
	void die(DeathCause i_death_type) override;
	void draw(const unsigned i_view_x, sf::Sprite& i_window) override;
	void on_entity_collision(Entity* entity,bool i_already_colliding) override;
	void callback(long long i_callback_code, long long i_arg) override;	
	~Goomba() override
	{			
	}
};
