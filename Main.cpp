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
    
    Edge edge(Range<sf::Vector2f>(sf::Vector2f(50,50), sf::Vector2f(60,60)), Range<sf::Vector2f>(sf::Vector2f(400, 100), sf::Vector2f(450, 150)), 15.f, 0.5f, Range<float>(0.01f,0.05f));

    edge.create_edge(window);
    window.display();

    std::cin.get();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
               
        window.display();
    }

    return 0;
}