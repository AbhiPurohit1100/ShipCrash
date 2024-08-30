#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath> // for std::cos and std::sin
#include <algorithm> // for std::remove_if

using namespace std;
using namespace sf;

int main()
{
    ContextSettings set;

    RenderWindow window(VideoMode(1200, 800), "Game", Style::Default, set);

    // Load textures
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Assets/texture/space-bg.jpg"))
    {
        // Handle error
        return -1;
    }
    Sprite backgroundSprite(backgroundTexture);

    Texture player1text;
    Texture player1flame;
    Texture bullet_texture;
    Texture asteroid_texture; // Texture for asteroids

    player1text.loadFromFile("Assets/texture/ship.png");
    player1flame.loadFromFile("Assets/texture/flameSS.png");
    bullet_texture.loadFromFile("Assets/texture/bult.png");
    asteroid_texture.loadFromFile("Assets/texture/patthar.png"); // Load asteroid texture

    Sprite player1flames(player1flame);
    Sprite player1ship(player1text);
    FloatRect bounds = player1ship.getLocalBounds();
    player1ship.setOrigin(bounds.width / 2, bounds.height / 2);
    player1ship.setPosition(window.getSize().x / 2, window.getSize().y - 100);

    Vector2i flameFrameSize(33, 54);
    int flameFrameCount = 9;
    player1flames.setTextureRect(sf::IntRect(10, 16, flameFrameSize.x, flameFrameSize.y));
    player1flames.setOrigin(flameFrameSize.x / 2, flameFrameSize.y / 2);

    float frameDuration = 0.1f;
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
    player1ship.setScale(0.5, 0.5);
    player1flames.setScale(0.5, 0.5);

    Vector2u windowSize = window.getSize();
    FloatRect spriteBounds = player1ship.getGlobalBounds();

    // Bullet system
    vector<Sprite> bullets;
    Clock bullet_clock;
    Time timeBetweenShots = sf::seconds(0.3f);
    Time lastShotTime = sf::Time::Zero;

    // Asteroid system
    vector<Sprite> asteroids;
    Clock asteroid_clock;
    Time timeBetweenAsteroids = sf::seconds(1.0f); // Adjust spawn rate as needed
    Time lastAsteroidTime = sf::Time::Zero;

    while (window.isOpen()) {
        Event event1;
        while (window.pollEvent(event1)) {
            if (event1.type == Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        // Flame animation
        bool showflame = false;
        static float timeSinceLastFrame = 0;
        timeSinceLastFrame += deltaTime;
        if (timeSinceLastFrame >= frameDuration) {
            currentFlameFrame = (currentFlameFrame + 1) % flameFrameCount;
            int left = currentFlameFrame * flameFrameSize.x;
            player1flames.setTextureRect(sf::IntRect(left, 0, flameFrameSize.x, flameFrameSize.y));
            timeSinceLastFrame = 0;
        }

        // Flame position
        acceleration = sf::Vector2f(0.f, 0.f);
        float angle = player1ship.getRotation();
        float angleRadians = angle * (3.14159f / 180.0f);
        sf::Transform transform = player1ship.getTransform();
        sf::Vector2f localOffset(38.f, offsetDistance);
        sf::Vector2f flamePosition = transform.transformPoint(localOffset);
        player1flames.setPosition(flamePosition);
        player1flames.setRotation(player1ship.getRotation());

        // Ship movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            showflame = true;
            acceleration.y -= cos(angleRadians) * accelerationRate;
            acceleration.x = sin(angleRadians) * accelerationRate;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (velocity.x != 0) {
                velocity.x = 0;
            }
            if (velocity.y != 0) {
                velocity.y = 0;
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
        if (velocity.x > maxSpeed) velocity.x = maxSpeed;
        if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
        if (velocity.y > maxSpeed) velocity.y = maxSpeed;
        if (velocity.y < -maxSpeed) velocity.y = -maxSpeed;
        position += velocity * deltaTime;
        player1ship.setPosition(position);

        // Shooting bullets
        sf::Time currentTime = bullet_clock.getElapsedTime();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && currentTime - lastShotTime > timeBetweenShots) {
            sf::Sprite bullet(bullet_texture);
            float bullet_angle = player1ship.getRotation();
            bullet.setScale(0.5, 0.5);
            bullet.setOrigin(bullet.getGlobalBounds().width / 2, bullet.getGlobalBounds().height / 2);
            bullet.setRotation(bullet_angle);
            bullet.setPosition(player1ship.getPosition().x, player1ship.getPosition().y);
            bullets.push_back(bullet);
            lastShotTime = currentTime;
        }

        // Update bullets
        float ship_angle = player1ship.getRotation();
        float bullet_x = sin(ship_angle * 3.14159 / 180) * 5;
        float bullet_y = -cos(ship_angle * 3.14159 / 180) * 5;
        for (auto& bullet : bullets) {
            bullet.move(bullet_x * 0.5, bullet_y * 0.5);
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(), [&](const Sprite& bullet) {
            return bullet.getPosition().x < 0 || bullet.getPosition().x > windowSize.x ||
                bullet.getPosition().y < 0 || bullet.getPosition().y > windowSize.y;
            }), bullets.end());

        // Spawn asteroids
        if (asteroid_clock.getElapsedTime() - lastAsteroidTime > timeBetweenAsteroids) {
            sf::Sprite asteroid(asteroid_texture);
            float x = rand() % windowSize.x;
            float y = 0;
            asteroid.setPosition(x, y);
            asteroid.setScale(0.5, 0.5);
            float asteroidSpeed = 100.f; // Speed of asteroid falling down
            asteroids.push_back(asteroid);
            lastAsteroidTime = asteroid_clock.getElapsedTime();
        }

        // Update asteroids
        for (auto& asteroid : asteroids) {
            asteroid.move(0, 100 * deltaTime); // Move asteroid down
        }
        asteroids.erase(remove_if(asteroids.begin(), asteroids.end(), [&](const Sprite& asteroid) {
            return asteroid.getPosition().y > windowSize.y;
            }), asteroids.end());

        // Clear and draw
        window.clear();
        window.draw(backgroundSprite);
        window.draw(player1ship);
        if (showflame) {
            window.draw(player1flames);
        }
        for (const auto& bullet : bullets) {
            window.draw(bullet);
        }
        for (const auto& asteroid : asteroids) {
            window.draw(asteroid);
        }
        window.display();
    }

    return 0;
}
