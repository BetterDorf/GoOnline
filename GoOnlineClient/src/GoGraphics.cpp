#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
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
		goBorderTopTxt_.loadFromFile("data/BorderTop.png");
		goBorderRightTxt_.loadFromFile("data/BorderRight.png");
		goBorderLeftTxt_.loadFromFile("data/BorderLeft.png");
		goBorderBottomTxt_.loadFromFile("data/BorderBottom.png");
		goCornerTLTxt_.loadFromFile("data/CornerTL.png");
		goCornerTRTxt_.loadFromFile("data/CornerTR.png");
		goCornerBLTxt_.loadFromFile("data/CornerBL.png");
		goCornerBRTxt_.loadFromFile("data/CornerBR.png");

		pixelSize_ = goTileTxt_.getSize().x;
	}

	void GoGraphics::UpdateMove(const golc::Goban& goban)
	{
		// read and update stones positions and current capture counts
		goban.ReadBoardInfo(board_, bCaps_, wCaps_);
	}

	void GoGraphics::UpdateCaptureCount(const golc::Goban& goban)
	{
		goban.ReadCaptures(bCaps_, wCaps_);
	}

	void GoGraphics::UpdateDeadGroup(const golc::Goban& goban, int groupId)
	{
		auto groups = goban.GroupsById()[groupId];
		for (const auto& [fst, snd] : groups)
		{
			deadBoard_.at(fst).at(snd) = !deadBoard_.at(fst).at(snd);
		}
	}

	void GoGraphics::ResetDeadGroups()
	{
		deadBoard_.clear();

		for (int x = 0; x < size_; x++)
		{
			deadBoard_.emplace_back(std::vector<bool>());

			for (int y = 0; y < size_; y++)
			{
				deadBoard_.at(x).emplace_back(false);
			}
		}
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

	void GoGraphics::ApplyDeadGroupsToBoard(golc::Goban& goban)
	{
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				if (deadBoard_.at(x).at(y))
				{
					golc::Coord coord(x, y);
					goban.KillStoneScoring(coord);
				}
			}
		}
	}

	void GoGraphics::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		float furthest = 0.0f;

		sf::Sprite sprite;
		sf::Sprite sprite2;

		const auto transparentColour = sf::Color(255, 255, 255, 122);

		// draw squares and mouse selection
		sprite.setTexture(goTileTxt_);

		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				sf::Sprite* spriteToDraw = &sprite;

				// Check for borders and Corners
				if (x == 0)
				{
					spriteToDraw = &sprite2;
					if (y == 0)
					{
						sprite2.setTexture(goCornerTLTxt_);
					}
					else if (y == size_ - 1)
					{
						sprite2.setTexture(goCornerBLTxt_);
					}
					else
					{
						sprite2.setTexture(goBorderLeftTxt_);
					}
				}
				else if (x == size_ - 1)
				{
					spriteToDraw = &sprite2;
					if (y == 0)
					{
						sprite2.setTexture(goCornerTRTxt_);
					}
					else if (y == size_ - 1)
					{
						sprite2.setTexture(goCornerBRTxt_);
					}
					else
					{
						sprite2.setTexture(goBorderRightTxt_);
					}
				}
				else if (y == 0)
				{
					spriteToDraw = &sprite2;
					sprite2.setTexture(goBorderTopTxt_);
				}
				else if (y == size_ -1)
				{
					spriteToDraw = &sprite2;
					sprite2.setTexture(goBorderBottomTxt_);
				}

				spriteToDraw->setPosition(getPosition().x + x * pixelSize_, getPosition().y + y * pixelSize_);
				target.draw(*spriteToDraw);

				if (spriteToDraw->getPosition().x > furthest)
					furthest = spriteToDraw->getPosition().x;
			}
		}

		// Draw mouse selection
		sprite2.setTexture(playerColour_ == black ? bStonesTxt_ : wStonesTxt_);
		sprite2.setColor(transparentColour);
		if (mouseHasSelection_)
		{
			sprite2.setPosition(getPosition().x + mouseSelection_.x * pixelSize_,
				getPosition().y + mouseSelection_.y * pixelSize_);
			target.draw(sprite2);
		}
		sprite2.setColor(sf::Color::White);

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
		sprite.setTexture(bStonesTxt_);
		sprite2.setTexture(wStonesTxt_);
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{
				const Stone stone = board_.at(y).at(x);

				sf::Sprite* spriteToDraw = &sprite2;

				if (stone == empty)
				{
					continue;
				}

				if (stone == black)
				{
					spriteToDraw = &sprite;
				}
				else if (stone == white)
				{
					spriteToDraw = &sprite2;
				}

				if (deadBoard_.at(y).at(x))
					spriteToDraw->setColor(transparentColour);

				spriteToDraw->setPosition(getPosition().x + x * pixelSize_, getPosition().y + y * pixelSize_);
				target.draw(*spriteToDraw);

				spriteToDraw->setColor(sf::Color::White);
			}
		}
	}
}