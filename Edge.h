#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random> // for std::mt19937
#include <ctime> // for std::time
#include <array>

const float pi = 3.14159f;

template<typename T>
struct Range
{
    T min;
    T max;

    Range(T min_t, T max_t) : min{ min_t }, max{ max_t }
    {
    }
};

class Edge
{
    enum OrderMode {UpRight, UpLeft, DownRight, DownLeft};

    //Drawing Area Settings
    Range<sf::Vector2f> top_left;
    Range<sf::Vector2f> bottom_right;
    float vertex_radius;

    //Starting Position
    sf::Vector2f starting_point;

    //Rectangle Geometry
    std::array<sf::Vector2f, 4> vertices_position;
    std::array<sf::Vector2f, 4> vertices;

    //Movement Settings    
    sf::Vector2f starting_velocity;
    std::array<unsigned int, 4> order{ 1,2,3,0 };
    OrderMode order_mode{ UpRight };
    float velocity_angle_range;
    Range<float> velocity_range;

    //Draw Settings
    sf::Color ink{ sf::Color::Black };
    sf::Color paper{ sf::Color::White };
    float thickness{ 2 };

    //Randomize vars
    std::mt19937 mersenne;    

    //Pen vars
    sf::Vector2f position;
    sf::Vector2f velocity;
    int current_target;

public:
    Edge(Range<sf::Vector2f> TL, Range<sf::Vector2f> BR, float v_rad, float vel_angle_rng, Range<float> vel_rng);
    float getRandom(float min, float max);
    void generate_parameters();
    void render(sf::RenderWindow& window);
    void generate_order();
    void simulate_pen_movement(sf::RenderWindow& window);
    void create_edge(sf::RenderWindow& window);
    
    friend class Pen;
};