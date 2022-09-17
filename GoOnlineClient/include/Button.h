#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>

class Button : public sf::Drawable
{
public:
	Button(const sf::Vector2f& size);
	Button(const sf::Vector2f& size, std::string text, sf::Font& font);
	Button(const sf::Vector2f& size, std::string text, sf::Font& font, sf::Color buttonColor, sf::Color textColor);

	bool Contains(const sf::Vector2i& mousePos);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setPosition(const sf::Vector2f& position);
private:
	sf::RectangleShape shape_;
	bool drawText_ = false;
	sf::Text text_;
};