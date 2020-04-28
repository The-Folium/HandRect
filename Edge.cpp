#include "Edge.h"
#include <cmath>
#include "Math.h"

Edge::Edge(Range<sf::Vector2f> TL, Range<sf::Vector2f> BR, float v_rad, float vel_angle_rng, Range<float> vel_rng) :
    top_left{ TL }, bottom_right{ BR }, vertex_radius{ v_rad }, velocity_angle_range{vel_angle_rng}, velocity_range(vel_rng),
    mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) }
{
    
}

float Edge::getRandom(float min, float max)
{
    std::uniform_real_distribution<float> value{ min, max };
    return value(mersenne);
}

void Edge::generate_parameters()
{
    //generating circle centers
    vertices_position[0] = sf::Vector2f(getRandom(top_left.min.x, top_left.max.x), getRandom(top_left.min.y, top_left.max.y));
    vertices_position[1] = sf::Vector2f(getRandom(bottom_right.min.x, bottom_right.max.x), vertices_position[0].y);
    vertices_position[2] = sf::Vector2f(vertices_position[1].x, getRandom(bottom_right.min.y, bottom_right.max.y));
    vertices_position[3] = sf::Vector2f(vertices_position[0].x, vertices_position[2].y);
    //generating vertices
    for(int i{ 0 }; i < 4; ++i)
    {
        float temp_radius{ getRandom(0.0f, vertex_radius) };
        float temp_angle{ getRandom(0.0f, 2 * pi) };
        
        vertices[i].x = vertices_position[i].x + temp_radius * std::cosf(temp_angle);
        vertices[i].y = vertices_position[i].y + temp_radius * std::sinf(temp_angle);

        std::cout << vertices[i].x << ",  " << vertices[i].y << std::endl;
    }

    //generating starting point
    if ((order_mode == UpRight) || (order_mode == UpLeft))
    {
        starting_point.x = 0.5f * (vertices[0].x + vertices[1].x);
        starting_point.y = 0.5f * (vertices[0].y + vertices[1].y);
    }
    if ((order_mode == DownRight) || (order_mode == DownLeft))
    {
        starting_point.x = 0.5f * (vertices[2].x + vertices[3].x);
        starting_point.y = 0.5f * (vertices[2].y + vertices[3].y);
    }

    //generating basic velocity
    float abs_velocity = getRandom(velocity_range.min, velocity_range.max);
    float angle = getRandom(-velocity_angle_range, velocity_angle_range);
    float direction{ 0.0f };
    
    if ((order_mode == UpLeft) || (order_mode == DownLeft)) { direction = pi; }
    starting_velocity = sf::Vector2f(abs_velocity * std::cosf(velocity_angle_range+direction), abs_velocity * std::sinf(velocity_angle_range+direction));
}

void Edge::render(sf::RenderWindow& window)
{
    //window setup
    window.clear(paper);
    window.display();
    sf::CircleShape point(thickness);
    point.setFillColor(ink);

    for (int i{ 0 }; i < 4; ++i)
    {        
        point.setPosition(vertices[i]);
        window.draw(point);
    }
}

void Edge::create_edge(sf::RenderWindow& window)
{
    generate_order();
    generate_parameters();    
    simulate_pen_movement(window);
    
}

void Edge::generate_order()
{
    std::uniform_int_distribution<int> rand(0, 3);
    int order_case{ rand(mersenne) };
    switch (order_case)
    {
    case 0: order = { 1,2,3,0 }; break;
    case 1: order = { 0,3,2,1 }; break;
    case 2: order = { 2,1,0,3 }; break;
    case 3: order = { 3,0,1,2 }; break;
    }
    order_mode = static_cast<OrderMode>(order_case);
}

void Edge::simulate_pen_movement(sf::RenderWindow& window)
{
    //init
    position = starting_point;
    velocity = starting_velocity;
    current_target = 0;
    window.clear(paper);

    sf::Vector2f mistarget(getRandom(-4.0f, 4.0f), getRandom(-4.0f, 4.0f));
    //loop
    while (true)
    {
        //draw point
        sf::CircleShape point(thickness);
        point.setFillColor(ink);
        point.setPosition(position);
        window.draw(point);
        window.display();

        //new position
        position += velocity;
        
        sf::Vector2f diff(vertices[order[current_target]] - position);
        velocity += 0.001f*unitVector(diff);
        velocity = velocity * 0.993;
        //velocity = unitVector(velocity);

        if (abs(diff) < 10) { ++current_target; }
        if (current_target > 3) { current_target = 0; }
        
    }
}

