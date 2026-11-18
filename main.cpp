// Fluid Flow Visualization in a Circular Pipe
//
// Real-time visualization of fully developed laminar (Hagen-Poiseuille) flow.
// Tracer particles move with the axial velocity of their radial position:
//
//     u(r) = u_max * (1 - (r / R)^2),   u_max = 2 * u_avg
//
// Particles are coloured by speed (blue = slow near the wall, red = fast at the
// centreline) and the parabolic velocity profile is drawn at the pipe inlet.
//
// Controls: Up/Down change the mean velocity, Space pauses, R re-seeds the
// particles, Esc quits.
//
// Builds against SFML 2.6 and SFML 3.

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

namespace {

constexpr unsigned kWindowWidth = 900;
constexpr unsigned kWindowHeight = 420;
constexpr float kPipeLeft = 120.f;          // leave room for the profile plot
constexpr float kPipeRight = kWindowWidth - 40.f;
constexpr float kPipeRadius = 120.f;        // pixels
constexpr float kPixelsPerUnit = 100.f;     // pixels per velocity unit per second
constexpr int kNumParticles = 400;
constexpr float kParticleRadius = 2.5f;
constexpr double kMinAvgVelocity = 0.25;
constexpr double kMaxAvgVelocity = 6.0;

const float kCentreY = kWindowHeight / 2.f;

struct Particle {
    sf::CircleShape shape;
    double r;  // signed radial offset from the centreline, in pixels (-R..R)
};

// Laminar (Poiseuille) axial velocity at signed radial offset r.
double poiseuilleVelocity(double r, double radius, double uMax) {
    const double rr = std::min(std::abs(r) / radius, 1.0);
    return uMax * (1.0 - rr * rr);
}

// Map a normalized speed in [0, 1] to a blue -> cyan -> yellow -> red colour.
sf::Color speedColor(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const auto lerp = [](double a, double b, double s) {
        return static_cast<std::uint8_t>(a + (b - a) * s);
    };
    if (t < 1.0 / 3) {
        const double s = t * 3;
        return {lerp(30, 0, s), lerp(80, 220, s), lerp(255, 255, s)};
    }
    if (t < 2.0 / 3) {
        const double s = (t - 1.0 / 3) * 3;
        return {lerp(0, 255, s), lerp(220, 230, s), lerp(255, 40, s)};
    }
    const double s = (t - 2.0 / 3) * 3;
    return {255, lerp(230, 50, s), lerp(40, 30, s)};
}

std::vector<Particle> seedParticles(std::mt19937& rng) {
    std::uniform_real_distribution<double> radial(-kPipeRadius + kParticleRadius,
                                                  kPipeRadius - kParticleRadius);
    std::uniform_real_distribution<float> axial(kPipeLeft, kPipeRight);

    std::vector<Particle> particles;
    particles.reserve(kNumParticles);
    for (int i = 0; i < kNumParticles; ++i) {
        const double r = radial(rng);
        sf::CircleShape dot(kParticleRadius);
        dot.setOrigin(sf::Vector2f(kParticleRadius, kParticleRadius));
        dot.setPosition(sf::Vector2f(axial(rng), kCentreY + static_cast<float>(r)));
        dot.setFillColor(speedColor(poiseuilleVelocity(r, kPipeRadius, 1.0)));
        particles.push_back({dot, r});
    }
    return particles;
}

// Line strip tracing u(r) to the left of the inlet, plus the inlet reference line.
void drawProfile(sf::RenderWindow& window, double uMax) {
    // Fastest possible profile (u_max = 2 * kMaxAvgVelocity) spans 85 px.
    const float scale = 85.f / static_cast<float>(2.0 * kMaxAvgVelocity);
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);
    sf::VertexArray arrows(sf::PrimitiveType::Lines);
    const float baseX = kPipeLeft - 95.f;

    for (int i = 0; i <= 60; ++i) {
        const double r = -kPipeRadius + 2.0 * kPipeRadius * i / 60.0;
        const double u = poiseuilleVelocity(r, kPipeRadius, uMax);
        const sf::Vector2f p(baseX + static_cast<float>(u) * scale, kCentreY + static_cast<float>(r));
        curve.append(sf::Vertex{p, sf::Color(230, 230, 230)});
        if (i % 6 == 0) {
            const sf::Color c = speedColor(u / uMax);
            arrows.append(sf::Vertex{sf::Vector2f(baseX, p.y), c});
            arrows.append(sf::Vertex{p, c});
        }
    }
    sf::VertexArray axis(sf::PrimitiveType::Lines);
    axis.append(sf::Vertex{sf::Vector2f(baseX, kCentreY - kPipeRadius), sf::Color(120, 120, 120)});
    axis.append(sf::Vertex{sf::Vector2f(baseX, kCentreY + kPipeRadius), sf::Color(120, 120, 120)});

    window.draw(axis);
    window.draw(arrows);
    window.draw(curve);
}

}  // namespace

int main() {
    std::mt19937 rng(std::random_device{}());

    sf::RenderWindow window(sf::VideoMode({kWindowWidth, kWindowHeight}),
                            "Fluid Flow in a Circular Pipe (laminar profile)");
    window.setFramerateLimit(60);
    window.requestFocus();

    std::cout << "Laminar pipe-flow visualization\n"
                 "  Up/Down : change mean velocity\n"
                 "  Space   : pause / resume\n"
                 "  R       : re-seed particles\n"
                 "  Esc     : quit\n";

    sf::RectangleShape pipe(sf::Vector2f(kPipeRight - kPipeLeft, 2 * kPipeRadius));
    pipe.setPosition(sf::Vector2f(kPipeLeft, kCentreY - kPipeRadius));
    pipe.setFillColor(sf::Color(45, 45, 52));

    sf::RectangleShape wallTop(sf::Vector2f(kPipeRight - kPipeLeft, 4.f));
    wallTop.setPosition(sf::Vector2f(kPipeLeft, kCentreY - kPipeRadius - 4.f));
    wallTop.setFillColor(sf::Color(160, 160, 170));
    sf::RectangleShape wallBottom = wallTop;
    wallBottom.setPosition(sf::Vector2f(kPipeLeft, kCentreY + kPipeRadius));

    std::vector<Particle> particles = seedParticles(rng);
    double uAvg = 2.0;
    bool paused = false;
    sf::Clock clock;

    const auto handleKey = [&](sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Key::Escape) {
            window.close();
        } else if (key == sf::Keyboard::Key::Up) {
            uAvg = std::min(uAvg * 1.25, kMaxAvgVelocity);
        } else if (key == sf::Keyboard::Key::Down) {
            uAvg = std::max(uAvg / 1.25, kMinAvgVelocity);
        } else if (key == sf::Keyboard::Key::Space) {
            paused = !paused;
        } else if (key == sf::Keyboard::Key::R) {
            particles = seedParticles(rng);
        } else {
            return;
        }
        std::cout << "u_avg = " << uAvg << (paused ? "  [paused]" : "") << '\n';
    };

    while (window.isOpen()) {
#if SFML_VERSION_MAJOR >= 3
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                handleKey(key->code);
            }
        }
#else
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                handleKey(event.key.code);
            }
        }
#endif

        // Clamp dt so dragging/minimizing the window doesn't teleport particles.
        const float dt = std::min(clock.restart().asSeconds(), 0.05f);
        const double uMax = 2.0 * uAvg;

        if (!paused) {
            for (auto& p : particles) {
                const double u = poiseuilleVelocity(p.r, kPipeRadius, uMax);
                p.shape.move(sf::Vector2f(static_cast<float>(u * kPixelsPerUnit * dt), 0.f));
                if (p.shape.getPosition().x > kPipeRight) {
                    p.shape.setPosition(sf::Vector2f(kPipeLeft, kCentreY + static_cast<float>(p.r)));
                }
            }
        }

        window.clear(sf::Color(18, 18, 22));
        window.draw(pipe);
        for (const auto& p : particles) {
            window.draw(p.shape);
        }
        window.draw(wallTop);
        window.draw(wallBottom);
        drawProfile(window, uMax);
        window.display();
    }

    return 0;
}
