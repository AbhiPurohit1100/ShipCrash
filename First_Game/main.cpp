#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath> 
#include <algorithm> 

using namespace std;
using namespace sf;

bool checkCollision(const Sprite& sprite1, const Sprite& sprite2) {
    return sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds());
}

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
    Texture asteroid_texture; 
    Texture collectible_texture;

    player1text.loadFromFile("Assets/texture/ship.png");
    player1flame.loadFromFile("Assets/texture/flameSS.png");
    bullet_texture.loadFromFile("Assets/texture/bult.png");

    asteroid_texture.loadFromFile("Assets/texture/patthar.png"); 
    collectible_texture.loadFromFile("Assets/texture/coin2.png");

    Sprite player1flames(player1flame);
    Sprite player1ship(player1text);

    Vector2u windowSize = window.getSize();


    player1ship.setPosition(windowSize.x/2 , windowSize.y/2);
    
    FloatRect bounds = player1ship.getLocalBounds();
    
    player1ship.setOrigin(bounds.width / 2, bounds.height / 2);
    

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
   
    float offsetDistance = 100.f;

    player1ship.setScale(0.5, 0.5);
    player1flames.setScale(0.5, 0.5);

    
    

    FloatRect spriteBounds = player1ship.getGlobalBounds();

    //bullets
    vector<Sprite> bullets;

    Clock bullet_clock;
    Time timeBetweenShots = sf::seconds(0.3f);
    Time lastShotTime = sf::Time::Zero;

 
    //asteroids

    vector<Sprite> asteroids;
    Clock asteroid_clock;
    Time timeBetweenAsteroids = sf::seconds(0.7f); 
    Time lastAsteroidTime = sf::Time::Zero;

    // colectibles
    Sprite collectible(collectible_texture);
    bool collectibleActive = false;
    float collectibleSpawnTime = 10.0f; 
    sf::Clock collectible_clock;


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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && currentTime - lastShotTime > timeBetweenShots) {
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
            asteroid.setScale(0.3, 0.2);

            for(int i =0; i<100; i++){
                float asteroidSpeed = 100.f+ i;
            }
            asteroids.push_back(asteroid);
            lastAsteroidTime = asteroid_clock.getElapsedTime();
        }
        //colectibles

        if (!collectibleActive || collectible_clock.getElapsedTime().asSeconds() > collectibleSpawnTime) {
            collectible.setScale(0.07,0.07);
            collectible.setPosition(rand() % windowSize.x, rand() % windowSize.y);
            collectibleActive = true;
            collectible_clock.restart();
        }


        if (collectibleActive && checkCollision(player1ship, collectible)) {
            collectibleActive = false;
        }

        // Update asteroids
        for (auto& asteroid : asteroids) {
            asteroid.move(0, 100 * deltaTime); 
        }

        asteroids.erase(remove_if(asteroids.begin(), asteroids.end(), [&](const Sprite& asteroid) {
            return asteroid.getPosition().y > windowSize.y;
            }), asteroids.end());


        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            bool bulletRemoved = false;
            for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end();) {
                if (checkCollision(*bulletIt, *asteroidIt)) {
                    
                    bulletIt = bullets.erase(bulletIt);
                    asteroidIt = asteroids.erase(asteroidIt);
                    bulletRemoved = true;
                    break; 
                }
                else {
                    ++asteroidIt;
                }
            }
            if (!bulletRemoved) {
                ++bulletIt;
            }
        }

        // close game
        if (player1ship.getPosition().x < 0 || player1ship.getPosition().x > windowSize.x ||
            player1ship.getPosition().y < 0 || player1ship.getPosition().y > windowSize.y) {
            window.close();
        }
        for (const auto& asteroid : asteroids) {
            if (checkCollision(player1ship, asteroid)) {
                window.close();
            }
        }

       

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
        if (collectibleActive) {
            window.draw(collectible);
        }

        window.display();
    }

    return 0;
}
