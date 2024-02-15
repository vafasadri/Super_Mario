#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
class Animation
{
	float x;
	float y;
	bool flipped;
	bool enabled;
	//We'll increment the current_frame as soon as this iterator reaches the animation_speed.
	unsigned short animation_iterator; 
	//After how many frames do we change the current_frame?
	unsigned short animation_speed;
	unsigned short current_frame;
	unsigned short frame_width;
	unsigned short total_frames;	
	//sf::Sprite sprite;
	const sf::Texture* texture;
public:
	Animation(const unsigned short i_frame_width,const sf::Texture* i_texture, const unsigned short i_animation_speed = 1);

	void draw(sf::Sprite& i_window);
	constexpr bool get_enabled() { return enabled; }
	constexpr void enable() { enabled = true; }
	constexpr void disable() { enabled = false; }
	constexpr void set_animation_speed(const unsigned short i_animation_speed) {
		animation_speed = std::max<unsigned short>(1, i_animation_speed);
	}
	constexpr void set_flipped(const bool i_value) {
		flipped = i_value;
	}
	inline void set_position(const short i_x, const short i_y) {
		x = i_x;
		y = i_y;
	}
	inline void set_texture(const sf::Texture* i_texture) {
		texture = i_texture;
	}
	void update();
};
