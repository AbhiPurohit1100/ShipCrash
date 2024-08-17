#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include<iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace sf;


int main()
{
	class Bullet {
	public:
		sf::CircleShape shape;
		sf::Vector2f velocity;

		Bullet(float radius = 5.f) {
			shape.setRadius(radius);
			shape.setFillColor(sf::Color::Red);
		}

		void update() {
			shape.move(velocity);
		}
	};


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

	vector<Bullet> bullets;

	// Set bullet speed
	float bulletSpeed = -5.0f;
	float bulletCooldown = 0.5f; // Time in seconds between bullets
	float timeSinceLastShot = 0.0f;

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


		//bullets
		float deltaTimebullet = clock.restart().asSeconds();
		timeSinceLastShot += deltaTimebullet;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && timeSinceLastShot >= bulletCooldown) {
			Bullet bullet;
			bullet.shape.setPosition(player1ship.getPosition().x + player1ship.getScale().x / 2 - bullet.shape.getRadius(), player1ship.getPosition().y);
			bullet.velocity = sf::Vector2f(0.0f, bulletSpeed);
			bullets.push_back(bullet);

			timeSinceLastShot = 0.0f; // Reset cooldown timer
		}
		for (auto& bullet : bullets) {
			bullet.update();
		}
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

		window.clear(Color::Blue);
		window.draw(player1ship);
		if (showflame) {
			window.draw(player1flames);
		}
		for (const auto& bullet : bullets) {
			window.draw(bullet.shape);
		}
		window.display();
	}
}