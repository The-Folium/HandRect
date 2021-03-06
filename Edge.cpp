#include "Edge.h"
#include <cmath>
#include "Math.h"
#include <string>

Edge::Edge(std::string filename, Settings& data) : mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) }    
{    
    settings.open(filename);
    std::string key{""};
    while (key != "END")
    {
        char ch{ 0 };
        float arg1, arg2, arg3, arg4;
        while (ch != '#')
        {
            settings >> ch;
        }
            settings >> key;            
            
        if (key != "END")
        {
            if (key == "TOP_LEFT") {
                settings >> arg1 >> arg2 >> arg3 >> arg4;
                top_left = Range<sf::Vector2f>(sf::Vector2f(arg1, arg2), sf::Vector2f(arg3, arg4));
            }
            if (key == "BOTTOM_RIGHT") {
                settings >> arg1 >> arg2 >> arg3 >> arg4;
                bottom_right = Range<sf::Vector2f>(sf::Vector2f(arg1, arg2), sf::Vector2f(arg3, arg4));
            }
            if (key == "VERTEX_DEVIATION_RADIUS") {settings >> vertex_deviation_radius; }
            if (key == "DEBUG_MODE") { settings >> debug_mode; }
            if (key == "VELOCITY_MULTIPLIER_RANGE") { settings >> arg1 >> arg2; velocity_multiplier_range = Range<float>(arg1, arg2); }
            if (key == "MOVEMENT_RESISTANCE_RANGE") { settings >> arg1 >> arg2; movement_resistance_range = Range<float>(arg1, arg2); }
            if (key == "TARGET_REACH_ACCURACY_RANGE") { settings >> arg1 >> arg2; target_reach_accuracy_range = Range<float>(arg1, arg2); }
            if (key == "DEVIATION_PROBABILITY") { settings >> deviation_probability; }
            if (key == "DEVIATION_PARAMETER") { settings >> deviation_parameter; }
            if (key == "START_VELOCITY_ANGLE_SPAN") { settings >> velocity_angle_range; }
            if (key == "START_VELOCITY_RANGE") { settings >> arg1 >> arg2; velocity_range = Range<float>(arg1, arg2); }
            if (key == "START_POSITION_RANGE") { settings >> arg1 >> arg2; start_position_range = Range<float>(arg1, arg2); }
            if (key == "THICKNESS") { settings >> thickness; }
            if (key == "FILE_OUTPUT") { settings >> data.file_output; }
            if (key == "JSON_FILENAME") { settings >> data.json_filename; }
            if (key == "SAMPLE_FILENAME_PREFIX") { settings >> data.sample_filename_prefix; }
            if (key == "SAMPLE_NUMBER") { settings >> data.sample_number; }
            if (key == "PADDING") { settings >> padding; }
            if (key == "INK_COLOR_RGB") { settings >> arg1 >> arg2 >> arg3; ink = sf::Color(static_cast<sf::Uint8>(arg1), static_cast<sf::Uint8>(arg2), static_cast<sf::Uint8>(arg3)); }
            if (key == "BACKGROUND_COLOR_RGB") { settings >> arg1 >> arg2 >> arg3; paper = sf::Color(static_cast<sf::Uint8>(arg1), static_cast<sf::Uint8>(arg2), static_cast<sf::Uint8>(arg3)); }
            if (key == "SAMPLE_RESOLUTION") { settings >> arg1 >> arg2; data.sample_resolution = sf::Vector2i(static_cast<int>(arg1), static_cast<int>(arg2)); }
            if (key == "ANTIALIASING_LEVEL") { settings >> data.antialiasing_level; }
            if (key == "OVERLAP_VALUE_RANGE") { settings >> arg1 >> arg2; overlap_value_range = Range<float>(arg1, arg2); }
          
        }
        std::cout << "\n " << key << " loaded";
        
    }
    left = right = top = bottom = 250;
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
    start_position_coefficient = getRandom(start_position_range.min, start_position_range.max);
    overlap_value = getRandom(overlap_value_range.min, overlap_value_range.max);

    if ((order_mode == UpRight) || (order_mode == UpLeft))
    {
        starting_point.x = start_position_coefficient * vertices[0].x + (1.0f - start_position_coefficient) * vertices[1].x;
        starting_point.y = start_position_coefficient * vertices[0].y + (1.0f - start_position_coefficient) * vertices[1].y;
    }
    if ((order_mode == DownRight) || (order_mode == DownLeft))
    {
        starting_point.x = start_position_coefficient * vertices[2].x + (1.0f - start_position_coefficient) * vertices[3].x;
        starting_point.y = start_position_coefficient * vertices[2].y + (1.0f - start_position_coefficient) * vertices[3].y;
    }

    if ((order_mode == UpRight) || (order_mode == DownLeft)) { overlap_parameter = 1.0f + overlap_value - start_position_coefficient; }
    if ((order_mode == UpLeft) || (order_mode == DownRight)) { overlap_parameter = start_position_coefficient + overlap_value; }


    


    //generating basic velocity
    float abs_velocity = getRandom(velocity_range.min, velocity_range.max);
    float angle = getRandom(-velocity_angle_range, velocity_angle_range);
    float direction{ 0.0f };
    
    if ((order_mode == UpLeft) || (order_mode == DownLeft)) { direction = pi; }
    starting_velocity = sf::Vector2f(abs_velocity * std::cosf(velocity_angle_range+direction), abs_velocity * std::sinf(velocity_angle_range+direction));

    //generate key vars
    velocity_multiplier = getRandom(velocity_multiplier_range.min, velocity_multiplier_range.max);
    movement_resistance = getRandom(movement_resistance_range.min, movement_resistance_range.max);
    target_reach_accuracy = getRandom(target_reach_accuracy_range.min, target_reach_accuracy_range.max);

    
}

void Edge::render_debug(sf::RenderWindow& window)
{
    //window setup   
    sf::CircleShape point(thickness);
    point.setFillColor(sf::Color::Blue);

    for (int i{ 0 }; i < 4; ++i)
    {        
        point.setPosition(vertices[i]);
        window.draw(point);
    }

    sf::RectangleShape rect(sf::Vector2f(right - left + thickness+2*padding, bottom - top + thickness+2*padding));
    rect.setPosition(left-padding, top-padding);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(2);
    rect.setFillColor(sf::Color::Transparent);
    window.draw(rect);

}

void Edge::create_edge(sf::RenderWindow& window, Settings& data)
{
    //reset vars
    way_passed = 0;
    way_tracking = false;

    //execute
    generate_order();
    generate_parameters();    
    simulate_pen_movement(window, data);

    data.outline_coords.x = left-padding;
    data.outline_coords.y = top-padding;
    data.outline_height = bottom - top + thickness+2*padding;
    data.outline_width = right - left + thickness+2*padding;
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
    if (position.x > right) { right = position.x; }
    if (position.x < left) { left = position.x; }
    if (position.y > bottom) { bottom = position.y; }
    if (position.y < top) { top = position.y; }

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
    
    if (way_tracking) { way_passed += abs(translation_vector); }

}

void Edge::draw_point(sf::RenderWindow& window)
{
    sf::CircleShape point(thickness);
    point.setFillColor(sf::Color::Red);
    point.setPosition(position);
    window.draw(point);    
}

void Edge::simulate_pen_movement(sf::RenderWindow& window, Settings& data)
{
    if (!data.file_output)
    {
        std::cout << "\nActual settings:\n";
        std::cout << "velocity multiplier = " << velocity_multiplier << "\n";
        std::cout << "movement_resistance = " << movement_resistance << "\n";
        std::cout << "target_reach_accuracy = " << target_reach_accuracy << "\n";
        std::cout << "start_position_coef = " << start_position_coefficient << "\n";
        std::cout << "overlap_parameter = " << overlap_parameter << "\n";
    }
  
    
    //init
    previous_position = position = starting_point;
    left = right = position.x;
    top = bottom = position.y;

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
        

        //draw point
        
        //window.display();

        //new position

        position += velocity;//semi-Euler
        draw_line(window);        
        previous_position = position;
        


        sf::Vector2f diff(vertices[order[current_target % period]] - position);
        velocity += velocity_multiplier*diff;
        velocity *= movement_resistance;
        velocity += deviation;
        
        
        if (randomEvent(deviation_probability)) { deviation = sf::Vector2f(getRandom(-abs(velocity)*deviation_parameter, abs(velocity) * deviation_parameter),
                                                            getRandom(-abs(velocity) * deviation_parameter, abs(velocity) * deviation_parameter)); }
        deviation *= 0.8f;

        if (abs(diff) < target_reach_accuracy) { ++current_target; }

        if ((current_target == period) && (way_tracking == false)) 
        { 
            way_tracking = true;           
            deviation_probability = 0;
        }

        if (way_passed >= overlap_parameter * (vertices[2].x - vertices[0].x)) { break; }
        
    }
    if (debug_mode)
    {
        render_debug(window);
    }
    
}


