#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

const sf::Vector2f nullvec(0, 0);

inline float abs(sf::Vector2f vect)
{
	return sqrtf(vect.x * vect.x + vect.y * vect.y);
}

inline sf::Vector2f operator/ (sf::Vector2f vector, float scalar)
{
	return sf::Vector2f(vector.x / scalar, vector.y / scalar);
}

inline sf::Vector2f operator* (sf::Vector2f vector, float scalar)
{
	return sf::Vector2f(vector.x * scalar, vector.y * scalar);
}

inline sf::Vector2f operator* (float scalar, sf::Vector2f vector)
{
	return sf::Vector2f(vector.x * scalar, vector.y * scalar);
}

inline sf::Vector2f unitVector(sf::Vector2f vector)
{
	return vector / abs(vector);
}

inline std::string toString(sf::Vector2f vector)
{
	return "(" + std::to_string(vector.x) + "," + std::to_string(vector.y) + ")";
}