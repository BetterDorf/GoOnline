#include "Button.h"
#include <SFML/Graphics/RenderTarget.hpp>

Button::Button(sf::Vector2i size)
{
	shape_ = sf::RectangleShape();
}

Button::Button(sf::Vector2i size, std::string text, sf::Font font) : Button(size)
{
	text_ = sf::Text();
	text_.setFont(font);
	text_.setString(text);

	drawText_ = true;
}

Button::Button(sf::Vector2i size, std::string text, sf::Font font, sf::Color buttonColor, sf::Color textColor) :
	Button(size, text, font)
{
	text_.setColor(textColor);
	shape_.setFillColor(buttonColor);
}

bool Button::IsIn(sf::Vector2f mousePos)
{
	sf::FloatRect bounds = shape_.getGlobalBounds();

	return bounds.contains(mousePos);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(shape_);
	target.draw(text_);
}

void Button::setPosition(const sf::Vector2f& position)
{
	text_.setPosition(position);
	shape_.setPosition(position);
}