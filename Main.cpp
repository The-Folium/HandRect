#include <SFML/Graphics.hpp>
#include <iostream>
#include <random> // for std::mt19937
#include <ctime> // for std::time
#include "Edge.h"


void generateRectVertex(float radius)
{

}

int main()
{    

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(500, 500), "Handwrite Rectangle",sf::Style::Default, settings);
    
    Settings data;
    Edge edge("settings.txt", data);
    
    if (!data.file_output)
    {
        while (true)
        {
            edge.create_edge(window, data);
            window.display();

            std::cout << "ready" << std::endl;
            std::cin.get();
        }
    }
    else
    {
        std::ofstream json_file;
        json_file.open("SAMPLES\\" + data.json_filename + ".json");
        json_file << "[";
        //std::ofstream sample_file;
        for (int counter{ 0 }; counter < data.sample_number; ++counter)
        {
            edge.create_edge(window, data);
            sf::Vector2u windowSize = window.getSize();
            sf::Texture texture;
            texture.create(windowSize.x, windowSize.y);
            texture.update(window);
            sf::Image image = texture.copyToImage();

            //sample_file.open(data.sample_filename_prefix + std::to_string(counter));
            std::string current_filename{ data.sample_filename_prefix + std::to_string(counter)+".jpg" };
            image.saveToFile("SAMPLES\\"+current_filename);
            json_file << "{\"image\":\"" + current_filename + "\",\"annotations\":[{\"label\":\"rect\",\"coordinates\":{\"x\":" + std::to_string(static_cast<int>(data.outline_coords.x)) +
                ",\"y\":" + std::to_string(static_cast<int>(data.outline_coords.y)) + ",\"width\":" + std::to_string(static_cast<int>(data.outline_width)) +
                ",\"height\":" + std::to_string(static_cast<int>(data.outline_height)) + "}}]}";
            if (counter != (data.sample_number - 1)) { json_file << ",";}
        }
        json_file << "]";
        json_file.close();
    }

    //while (window.isOpen())
    //{
    //    sf::Event event;
    //    while (window.pollEvent(event))
    //    {
    //        if (event.type == sf::Event::Closed)
    //            window.close();
    //    }
    //           
    //    window.display();
    //}

    return 0;
}