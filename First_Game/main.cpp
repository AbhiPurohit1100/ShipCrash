#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include<iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace sf;


int main()
{


	ContextSettings set;

	RenderWindow window(VideoMode(1200, 800), "Game", Style::Default, set);

	set.antialiasingLevel = 8;

	Texture player1text;
	Texture player1flame;
	Texture bullet_texture;

	player1text.loadFromFile("Assets/texture/ship.png");
	player1flame.loadFromFile("Assets/texture/flameSS.png");
	bullet_texture.loadFromFile("Assets/texture/bult.png");
	

	Sprite player1flames(player1flame);
	Sprite player1ship(player1text);
	FloatRect bounds = player1ship.getLocalBounds();

	// Set the origin to the center of the sprite
	player1ship.setOrigin(bounds.width / 2, bounds.height / 2);
	player1ship.setPosition(window.getSize().x / 2, window.getSize().y - 100);

	Vector2i flameFrameSize(33, 54);
	int flameFrameCount = 9;

	player1flames.setTextureRect(sf::IntRect(10, 16, flameFrameSize.x, flameFrameSize.y));
	player1flames.setOrigin(flameFrameSize.x / 2, flameFrameSize.y / 2);

	float frameDuration = 0.1f;  // Duration of each frame in seconds
	int currentFlameFrame = 0;

	sf::Vector2f position(100.f, 100.f);
	sf::Vector2f velocity(0.f, 0.f);
	sf::Vector2f acceleration(0.f, 0.f);

	float maxSpeed = 500.f;
	float deceleration = 600.f;
	float accelerationRate = 300.f;

	sf::Clock clock;

	player1flames.setPosition(bounds.width / 2, bounds.height / 2);
	float offsetDistance = 100.f;



	// make a bullet system that shoots bullet from the ship
	vector<Sprite> bullets;
	Clock bullet_clock;
	Time timeBetweenShots = sf::seconds(0.3f);  // Time delay between shots
	Time lastShotTime = sf::Time::Zero;
	

	while (window.isOpen()) {
		;
		Event event1;
		while (window.pollEvent(event1)) {
			if (event1.type == Event::Closed)
				window.close();
		}
		
		float deltaTime = clock.restart().asSeconds();

		//flame
		

		bool showflame = false;	
		
		static float timeSinceLastFrame = 0;
		timeSinceLastFrame += deltaTime;
		if (timeSinceLastFrame >= frameDuration) {
			currentFlameFrame = (currentFlameFrame + 1) % flameFrameCount;
			int left = currentFlameFrame * flameFrameSize.x;
			player1flames.setTextureRect(sf::IntRect(left, 0, flameFrameSize.x, flameFrameSize.y));
			timeSinceLastFrame = 0;
		}

		//*flame

		
		acceleration = sf::Vector2f(0.f, 0.f);
		float angle = player1ship.getRotation();
		float angleRadians = angle * (3.14159f / 180.0f);


		sf::Transform transform = player1ship.getTransform();

		// Calculate the flame's position behind the ship using the transform
		sf::Vector2f localOffset(38.f, offsetDistance);  // Offset along the local Y-axis (backward)
		sf::Vector2f flamePosition = transform.transformPoint(localOffset);

		// Set the flame's position and rotation
		player1flames.setPosition(flamePosition);
		player1flames.setRotation(player1ship.getRotation());
		
		Vector2f C_position = player1ship.getPosition();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			showflame = true;
			acceleration.y -= cos(angleRadians)*accelerationRate;
			acceleration.x = sin(angleRadians) * accelerationRate;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			if (velocity.x != 0) {
				velocity.x = 0;

			}
		
			if (velocity.y != 0) {
				velocity.y=0;
			}
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player1ship.rotate(-0.3);
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player1ship.rotate(0.3);
			
		}
		velocity += acceleration * deltaTime;

		if (acceleration.x == 0) {
			if (velocity.x > 0)
				velocity.x -= deceleration * deltaTime;
			else if (velocity.x < 0)
				velocity.x += deceleration * deltaTime;
		}

		if (acceleration.y == 0) {
			if (velocity.y > 0)
				velocity.y -= deceleration * deltaTime;
			else if (velocity.y < 0)
				velocity.y += deceleration * deltaTime;
		}

		// Clamp velocity to max speed
		if (velocity.x > maxSpeed) velocity.x = maxSpeed;
		if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
		if (velocity.y > maxSpeed) velocity.y = maxSpeed;
		if (velocity.y < -maxSpeed) velocity.y = -maxSpeed;


		position += velocity * deltaTime;
		player1ship.setPosition(position);

		sf::Time currentTime = bullet_clock.getElapsedTime();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && currentTime - lastShotTime > timeBetweenShots) {
			sf::Sprite bullet(bullet_texture);
			float bullet_angle = player1ship.getRotation();
			bullet.setRotation(bullet_angle);
			bullet.setOrigin(bullet.getGlobalBounds().width / 2, bullet.getGlobalBounds().height / 2);

			// Position bullet at the center-top of the spaceship
			bullet.setPosition(player1ship.getPosition().x ,player1ship.getPosition().y);

			bullets.push_back(bullet);
			lastShotTime = currentTime;
		}

		// Update bullets
		float ship_angle = player1ship.getRotation();
		float bullet_x = sin(ship_angle * 3.14159 / 180) * 5;
		float bullet_y = -cos(ship_angle * 3.14159 / 180) * 5;
		for (auto& bullet : bullets) {
			bullet.move(bullet_x*0.5,bullet_y*0.5);  // Move bullet upwards
		}

		// Remove bullets that are off the screen
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
			[&](const sf::Sprite& bullet) { return bullet.getPosition().y + bullet.getGlobalBounds().height < 0; }), bullets.end());


		window.clear(Color::Blue);
		window.draw(player1ship);
		if (showflame) {
			window.draw(player1flames);
		}
		for (const auto& bullet : bullets) {
			window.draw(bullet);
		}

		window.display();
	}
}