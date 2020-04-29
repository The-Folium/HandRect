#include "Edge.h"
#include <cmath>
#include "Math.h"
#include <string>

Edge::Edge(std::string filename, Range<sf::Vector2f> TL, Range<sf::Vector2f> BR, float v_rad, float vel_angle_rng, Range<float> vel_rng, bool debug_md) :
    vertex_deviation_radius{ v_rad }, velocity_angle_range{vel_angle_rng}, velocity_range(vel_rng),
    mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) },
    debug_mode{debug_md}
{    
    settings.open(filename);
    std::string key{""};
    while (key != "END")
    {
        char ch{ 0 };
        float a, b, c, d;
        while (ch != '#')
        {
            settings >> ch;
        }
            settings >> key;            
            
        if (key != "END")
        {
            if (key == "TOP_LEFT") {
                settings >> a >> b >> c >> d;
                top_left = Range<sf::Vector2f>(sf::Vector2f(a, b), sf::Vector2f(c, d));
            }
            if (key == "BOTTOM_RIGHT") {
                settings >> a >> b >> c >> d;
                bottom_right = Range<sf::Vector2f>(sf::Vector2f(a, b), sf::Vector2f(c, d));
            }
            if (key == "VERTEX_DEVIATION_RADIUS") {
                settings >> vertex_deviation_radius;
            }
            if (key == "START_VELOCITY_ANGLE_SPAN") {
                
            }

        }
        std::cout << "\n " << key;
        
    }
}

float Edge::getRandom(float min, float max)
{
    std::uniform_real_distribution<float> value{ min, max };
    return value(mersenne);
}

bool Edge::randomEvent(float probability)
{
    return (getRandom(0.0f, 1.0f)<=probability);    
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
        float temp_radius{ getRandom(0.0f, vertex_deviation_radius) };
        float temp_angle{ getRandom(0.0f, 2 * pi) };
        
        vertices[i].x = vertices_position[i].x + temp_radius * std::cosf(temp_angle);
        vertices[i].y = vertices_position[i].y + temp_radius * std::sinf(temp_angle);
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

    //generate key vars
    velocity_multiplier = getRandom(0.001f, 0.01f);
    float max_resistance{ 0.93f };
    //std::min(0.006f/velocity_multiplier,0.93f)
    movement_resistance = getRandom(0.88f, 0.92f);
    target_reach_accuracy = getRandom(18.0f, 25.0f);

}

void Edge::render(sf::RenderWindow& window)
{
    //window setup   
    sf::CircleShape point(thickness);
    point.setFillColor(sf::Color::Green);

    for (int i{ 0 }; i < 4; ++i)
    {        
        point.setPosition(vertices[i]);
        window.draw(point);
    }
}

void Edge::create_edge(sf::RenderWindow& window)
{
    //reset vars
    way_passed = 0;
    way_tracking = false;

    //execute
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

void Edge::draw_line(sf::RenderWindow& window)
{
    sf::CircleShape point(thickness);
    point.setFillColor(ink);

    sf::Vector2f translation_vector{ position - previous_position };
    int steps{ static_cast<int>(abs(translation_vector)) + 1 };
    sf::Vector2f delta{ translation_vector / static_cast<float>(steps) };
    for (int i{ 0 }; i < steps; ++i)
    {        
        point.setPosition(previous_position+delta*static_cast<float>(i));
        window.draw(point);
    }
    //window.display();
    //if (way_tracking) { way_passed += abs(translation_vector); }

}

void Edge::draw_point(sf::RenderWindow& window)
{
    sf::CircleShape point(thickness);
    point.setFillColor(sf::Color::Red);
    point.setPosition(position);
    window.draw(point);    
}

void Edge::simulate_pen_movement(sf::RenderWindow& window)
{
    std::cout << "velocity multiplier = " << velocity_multiplier << "\n";
    std::cout << "movement_resistance = " << movement_resistance << "\n";
    std::cout << "target_reach_accuracy = " << target_reach_accuracy << "\n";
    std::cout << "k = " << velocity_multiplier * movement_resistance << "        0.006/v = " << 0.006 / velocity_multiplier << "\n";
    
    //init
    previous_position = position = starting_point;
    
    velocity = starting_velocity;
    current_target = 0;
    window.clear(paper);

    sf::Vector2f deviation = nullvec;
    //loop
        
    sf::CircleShape point(thickness);
    point.setFillColor(ink);
    point.setPosition(position);
    window.draw(point);

    while (true)
    {
        if (debug_mode) { render(window); }

        //draw point
        
        window.display();

        //new position

        position += velocity;//semi-Euler
        
        draw_line(window);
        //draw_point(window);

        sf::Vector2f translation_vector{ position - previous_position };
        if (way_tracking) { way_passed += abs(translation_vector); }

        previous_position = position;
        


        sf::Vector2f diff(vertices[order[current_target % 4]] - position);
        velocity += velocity_multiplier*diff;
        velocity *= movement_resistance;
        velocity += deviation;
        //velocity = unitVector(velocity);
        
        if (randomEvent(0.004f)) { deviation = sf::Vector2f(getRandom(-abs(velocity)*deviation_parameter, abs(velocity) * deviation_parameter),
                                                            getRandom(-abs(velocity) * deviation_parameter, abs(velocity) * deviation_parameter)); }
        deviation *= 0.8f;

        if (abs(diff) < target_reach_accuracy) { ++current_target;  }

        if (current_target == 4) { way_tracking = true; }
        if (way_passed >= 0.7 * (vertices[2].x - vertices[0].x)) { break; }
        
    }
    window.display();
}

