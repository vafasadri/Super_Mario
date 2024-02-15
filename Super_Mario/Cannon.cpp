#include "Cannon.hpp"
#include "CannonAimDot.hpp"
auto s_textureSet = get_textures({ 
	"Resources/Images/Cannon.png",
	"Resources/Images/CannonPipe.png",
	"Resources/Images/CannonPipeBackground.png" });
const sf::Texture s_bodyTexture = s_textureSet[0];
const sf::Texture s_pipeTexture = s_textureSet[1];
const sf::Texture s_pipeBackground = s_textureSet[2];
Cannon::Cannon(const float i_x, const float i_y) : Entity(i_x, i_y),
entities(nullptr),
angle(0),
entity_inside(nullptr),
dot_iterator(0)
{
	texture_target.create(50, 50);
	apply_gravity = entity_collisions = map_collisions = alive = move =  true;
}

int Cannon::get_collision_flags() const
{
	return ~(1<< CannonAimDot::TypeID());
}



void Cannon::draw(const unsigned i_view_x, sf::Sprite& i_window)
{
	sf::RenderTexture& fullTexture = this->texture_target;
	sf::RenderTexture& pipeTexture = this->texture_target;

	pipeTexture.create(32, 32);
	fullTexture.clear(sf::Color::Transparent);
	sf::Sprite sp;
	sp.setTexture(s_pipeBackground);
	pipeTexture.draw(sp);
	sp = sf::Sprite();
	sp.setTexture(s_pipeTexture);
	pipeTexture.draw(sp);
	// pipe texture is ready now
	sp = sf::Sprite();
	sp.setTexture(s_bodyTexture);
	sp.setPosition(23 + 18,18);	
	fullTexture.draw(sp);
	sp = sf::Sprite();
	i_window.setPosition(x, y);
	i_window.setTexture(fullTexture.getTexture());
	//sp.setPosition(23 + 18, 18);
	//sp.setOrigin({ 20,43 });
	//fullTexture = pipeTexture;
	
	//sp.setRotation(-angle * 3.14 / 180);
	//fullTexture.draw(sp);
	//i_window.setTexture(fullTexture.getTexture());
	//i_window.setPosition({ x - 20,y - 20 });
}

void Cannon::on_entity_collision(Entity* entity, bool i_already_colliding)
{
	if (i_already_colliding) return;
	if (entity_inside == nullptr) {
		entity_inside = entity;
		entity->x = x;
		entity->y = y;
		set_timer(0, (Entity::Callback)RemoveEntity, (long long) entity_inside);		
	}
}
void Cannon::RemoveEntity(long long arg) {
	entities->remove(((Entity*)arg));
}
void Cannon::callback(long long i_callback_code, long long i_arg)
{
	switch (i_callback_code)
	{
	case 0:		
		break;
	case 1:
		entities->remove(((Entity*)i_arg));

		break;
	default:
		break;
	}
	
}

void Cannon::update(const unsigned i_view_x, std::list<Entity*>& i_enemies, MapManager& i_map_manager, Mario& i_mario)
{
	float horizontal_boost = cos(angle) * CANNON_POWER;
	float vertical_boost = -sin(angle) * CANNON_POWER;
	entities = &i_enemies;
	// launch the entity
	if (entity_inside) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			
			entity_inside->vertical_speed += vertical_boost;
			entity_inside->horizontal_speed += horizontal_boost;
			if (horizontal_boost > 0) entity_inside->horizontal_speed = ho;
			else if (horizontal_boost < 0) entity_inside->horizontal_acceleration = 0.05;
			set_timer(ceil(horizontal_boost / 0.05),0, (long long)entity_inside);
			//set_timer()
			
			entities->push_front(entity_inside);
			entity_inside = nullptr;
		}
		else if (dot_iterator++ % 10 == 0) {
			
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			angle -= 3.14 / 180;
		}
		else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			angle += 3.14 / 180;
		}
	}
	else {
		dot_iterator = 0;
	}
}
static void CreateDot(Entity* entity, long long arg) {
	auto dot = new CannonAimDot(entity->x, y, horizontal_boost + entity_inside->horizontal_speed, vertical_boost + entity_inside->vertical_speed);
	if (horizontal_boost > 0) dot->horizontal_acceleration = -0.05;
	else if (horizontal_boost < 0) dot->horizontal_acceleration = 0.05;
	//set_timer(ceil(horizontal_boost / 0.05), 0, (long long)dot);
	entities->push_front(dot);
}
