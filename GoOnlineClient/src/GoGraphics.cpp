#include "GoGraphics.h"

#include <math.h>

namespace goc
{
	GoGraphics::GoGraphics(int size)
	{
		size_ = size;

		bCaps_ = 0;
		wCaps_ = 0;

		bStonesTxt_.loadFromFile("data/BStone.png");
		wStonesTxt_.loadFromFile("data/WStone.png");
		tempStonesTxt_.loadFromFile("data/TempStone.png");
		goTileTxt_.loadFromFile("data/Board.png");

		pixelSize_ = goTileTxt_.getSize().x;
	}

	void GoGraphics::UpdateMove(golc::Goban& goban)
	{
		// read and update stones positions and current capture counts
		goban.ReadBoardInfo(board_, bCaps_, wCaps_);
	}

	void GoGraphics::UpdateMouse(sf::Vector2i mousePos)
	{
		sf::Vector2f mousePosf(mousePos);
		sf::Vector2f relativePos = (mousePosf /(float) pixelSize_) - getPosition();
		sf::Vector2i mouseGridPos((int)relativePos.x, (int)relativePos.y);

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

		if (board_.empty())
			return;

		// draw stones
		sprite.setTexture(bStonesTxt_);
		sprite2.setTexture(wStonesTxt_);
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				Stone stone = board_.at(y).at(x);

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