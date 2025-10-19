#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    const int windowWidth = 800;
    const int windowHeight = 400;

    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Fluid Flow in Circular Pipe");
    window.setFramerateLimit(60);
    
    // Force window to focus
    window.requestFocus();
    
    cout << "Window created! Look for the window..." << endl;
    cout << "Press ESC or close the window to exit." << endl;

    double pipeLength = windowWidth - 100;
    double pipeRadius = 100;
    double v_avg = 2.0;
    double v_max = 2 * v_avg;
    double R = pipeRadius;

    sf::RectangleShape pipe(sf::Vector2f(static_cast<float>(pipeLength), static_cast<float>(2 * R)));
    pipe.setPosition(sf::Vector2f(50.f, static_cast<float>(windowHeight / 2 - R)));
    pipe.setFillColor(sf::Color(50, 50, 50));

    struct Particle {
        sf::CircleShape shape;
        double yOffset;
    };

    vector<Particle> particles;
    int numParticles = 200;
    for (int i = 0; i < numParticles; ++i) {
        double yOffset = ((double)rand() / RAND_MAX) * 2 * R - R;
        sf::CircleShape dot(3.f);
        dot.setFillColor(sf::Color::Cyan);
        dot.setPosition(sf::Vector2f(
            static_cast<float>(50 + rand() % static_cast<int>(pipeLength)), 
            static_cast<float>(windowHeight / 2 + yOffset)
        ));
        particles.push_back({dot, yOffset});
    }

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event; // Declare the event object outside the loop
        while (window.pollEvent(event)) {
            // 1. Handle Window Close
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // 2. Handle Key Press (including ESC)
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
        }

        float dt = clock.restart().asSeconds();

        for (auto &p : particles) {
            double r = fabs(p.yOffset);
            if (r > R) r = R;

            double v = v_max * (1 - pow(r / R, 2));
            double pixelSpeed = v * 100 * dt;
            
            p.shape.move(sf::Vector2f(static_cast<float>(pixelSpeed), 0.f));

            if (p.shape.getPosition().x > pipe.getPosition().x + pipeLength) {
                p.shape.setPosition(sf::Vector2f(
                    pipe.getPosition().x, 
                    static_cast<float>(windowHeight / 2 + p.yOffset)
                ));
            }
        }

        window.clear(sf::Color(20, 20, 20));
        window.draw(pipe);
        for (auto &p : particles) {
            window.draw(p.shape);
        }
        window.display();
    }
    
    cout << "Program ended." << endl;
    return 0;
}