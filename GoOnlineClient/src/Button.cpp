#include "Button.h"
#include <SFML/Graphics/RenderTarget.hpp>

Button::Button(const sf::Vector2f& size)
{
	shape_ = sf::RectangleShape(size);
}

Button::Button(const sf::Vector2f& size, std::string text, sf::Font& font) : Button(size)
{
	text_ = sf::Text();
	text_.setFont(font);
	text_.setString(text);

	drawText_ = true;
}

Button::Button(const sf::Vector2f& size, std::string text, sf::Font& font, sf::Color buttonColor, sf::Color textColor) :
	Button(size, text, font)
{
	text_.setColor(textColor);
	shape_.setFillColor(buttonColor);
}

bool Button::Contains(const sf::Vector2i& mousePos)
{
	sf::Vector2f mousePosf(mousePos.x, mousePos.y);

	sf::FloatRect bounds = shape_.getGlobalBounds();

	return bounds.contains(mousePosf);
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

void Button::SetButtonColor(const sf::Color& color)
{
	shape_.setFillColor(color);
}
