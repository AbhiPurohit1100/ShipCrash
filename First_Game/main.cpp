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

	RenderWindow window(VideoMode(800, 600), "Game", Style::Default, set);

	set.antialiasingLevel = 8;
	Texture player1text;
	Texture player1flame;

	player1text.loadFromFile("Assets/texture/ship.png");
	player1flame.loadFromFile("Assets/texture/flameSS.png");

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
		
		Vector2f Cposition = player1ship.getPosition();


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			showflame = true;
			acceleration.y -= cos(angleRadians)*accelerationRate;
			acceleration.x = sin(angleRadians) * accelerationRate;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (velocity.x != 0) {
				sleep(sf::milliseconds(1));
				velocity.x--;

			}
			else {
				velocity.x = 0;
			}
			if (velocity.y != 0) {
				sleep(sf::milliseconds(1));
				velocity.y--;

			}
			else {
				velocity.y = 0;
			}
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player1ship.rotate(-0.1);
			
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player1ship.rotate(0.1);
			
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

		window.clear(Color::Blue);
		window.draw(player1ship);
		if (showflame) {
			window.draw(player1flames);
		}
		window.display();
	}
}