#include "GoGraphics.h"

namespace gog
{
	GoGraphics::GoGraphics(const int size, const sf::Font& font)
	{
		size_ = size;
		font_ = font;

		bCaps_ = 0;
		wCaps_ = 0;

		for (int x = 0; x < size ; x++)
		{
			deadBoard_.emplace_back(std::vector<bool>());

			for (int y = 0; y < size; y++)
			{
				deadBoard_.at(x).emplace_back(false);
			}
		}

		bStonesTxt_.loadFromFile("data/BStone.png");
		wStonesTxt_.loadFromFile("data/WStone.png");
		tempStonesTxt_.loadFromFile("data/TempStone.png");
		goTileTxt_.loadFromFile("data/Board.png");

		pixelSize_ = goTileTxt_.getSize().x;
	}

	void GoGraphics::UpdateMove(const golc::Goban& goban)
	{
		// read and update stones positions and current capture counts
		goban.ReadBoardInfo(board_, bCaps_, wCaps_);
	}

	void GoGraphics::UpdateMouse(const sf::Vector2i mousePos)
	{
		const sf::Vector2f mousePosf(mousePos);
		const sf::Vector2f relativePos = (mousePosf /static_cast<float>(pixelSize_)) - getPosition();
		const sf::Vector2i mouseGridPos((int)relativePos.x, (int)relativePos.y);

		if (mouseGridPos.x < 0 || mouseGridPos.y < 0 || mouseGridPos.x >= size_ || mouseGridPos.y >= size_)
		{
			mouseHasSelection_ = false;
			return;
		}

		mouseHasSelection_ = true;
		mouseSelection_ = mouseGridPos;
	}

	void GoGraphics::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		float furthest = 0.0f;

		sf::Sprite sprite;
		sf::Sprite sprite2;

		// draw squares and mouse selection
		sprite.setTexture(goTileTxt_);
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				sprite.setPosition(getPosition().x + x * pixelSize_, getPosition().y + y * pixelSize_);
				target.draw(sprite);

				if (sprite.getPosition().x > furthest)
					furthest = sprite.getPosition().x;
			}
		}

		// Draw mouse selection
		sprite2.setTexture(tempStonesTxt_);
		if (mouseHasSelection_)
		{
			sprite2.setPosition(getPosition().x + mouseSelection_.x * pixelSize_,
				getPosition().y + mouseSelection_.y * pixelSize_);
			target.draw(sprite2);
		}

		// Draw captures
		sf::Text text;
		text.setFont(font_);
		text.setString("White captures : " + std::to_string(wCaps_));
		text.setPosition(sf::Vector2f(furthest + pixelSize_ + 25, 150));
		target.draw(text);
		text.setString("Black captures : " + std::to_string(bCaps_));
		text.setPosition(sf::Vector2f(furthest + pixelSize_ + 25, 200));
		target.draw(text);

		if (board_.empty())
			return;

		// draw stones
		// TODO draw them transparently if they are dead on the dead board
		sprite.setTexture(bStonesTxt_);
		sprite2.setTexture(wStonesTxt_);
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				const Stone stone = board_.at(y).at(x);

				if (stone == black)
				{
					sprite.setPosition(getPosition().x + x * pixelSize_, getPosition().y + y * pixelSize_);
					target.draw(sprite);
				}
				else if (stone == white)
				{
					sprite2.setPosition(getPosition().x + x * pixelSize_, getPosition().y + y * pixelSize_);
					target.draw(sprite2);
				}
			}
		}

		// TODO draw capture count

		// TODO draw mouse ui preview
	}
}