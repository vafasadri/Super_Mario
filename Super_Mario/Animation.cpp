#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"

Animation::Animation(const unsigned short i_frame_width, const sf::Texture* i_texture, const unsigned short i_animation_speed) :
	enabled(0),
	flipped(0),
	animation_iterator(0),
	animation_speed(std::max<unsigned short>(1, i_animation_speed)),
	current_frame(0),
	frame_width(i_frame_width),
	texture(i_texture)
{
	//texture.loadFromFile(i_texture_location);

	total_frames = texture->getSize().x / frame_width;
}

void Animation::draw(sf::Sprite& i_sprite)
{
	i_sprite.setPosition({ x,y });
	i_sprite.setTexture(*texture,true);

	if (0 == flipped)
	{
		i_sprite.setTextureRect(sf::IntRect(current_frame * frame_width, 0, frame_width, texture->getSize().y));
	}
	else
	{
		//This is why I love SFML.
		//It allows you to read the texture from right to left using negative numbers.
		i_sprite.setTextureRect(sf::IntRect(frame_width * (1 + current_frame), 0, -frame_width, texture->getSize().y));
	}
}

void Animation::update()
{
	while (animation_iterator >= animation_speed)
	{
		animation_iterator -= animation_speed;

		current_frame = (1 + current_frame) % total_frames;
	}

	animation_iterator++;
}