#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random> // for std::mt19937
#include <ctime> // for std::time
#include <array>
#include <string>
#include <fstream>

const float pi = 3.14159f;

template<typename T>
struct Range
{
    T min;
    T max;

    Range()
    {
    }

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
    float vertex_deviation_radius;
    bool debug_mode{ false };


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
    float way_passed{ 0 };
    bool way_tracking{ false };
    float deviation_parameter{ 0.1 };

    float velocity_multiplier;
    float movement_resistance;
    float target_reach_accuracy;

    //Draw Settings
    sf::Color ink{ sf::Color::Black };
    sf::Color paper{ sf::Color::White };
    float thickness{ 2 };

    //Randomize vars
    std::mt19937 mersenne;    

    //Pen vars
    sf::Vector2f position;
    sf::Vector2f previous_position;
    sf::Vector2f velocity;
    int current_target;

    std::ifstream settings;
    

public:
    Edge(std::string filename, Range<sf::Vector2f> TL, Range<sf::Vector2f> BR, float v_rad, float vel_angle_rng, Range<float> vel_rng, bool debug_md);
    float getRandom(float min, float max);
    bool randomEvent(float probability);
    void generate_parameters();
    void render(sf::RenderWindow& window);
    void generate_order();
    void draw_line(sf::RenderWindow& window);   
    void draw_point(sf::RenderWindow& window);
    void simulate_pen_movement(sf::RenderWindow& window);
    void create_edge(sf::RenderWindow& window);
    
    friend class Pen;
};