#pragma once
#include <sfml/graphics.hpp>

#include "Goban.h"

namespace gog
{
	class GoGraphics final : public sf::Drawable, public sf::Transformable
	{
	public:
		GoGraphics(int size, const sf::Font& font);

		void SetPlayerColour(const Stone playerColour) { playerColour_ = playerColour; }
		void UpdateMove(const golc::Goban& goban);
		void UpdateCaptureCount(const golc::Goban& goban);
		void UpdateDeadGroup(const golc::Goban& goban, int groupId);
		void ResetDeadGroups();
		void UpdateMouse(sf::Vector2i mousePos);
		bool HasMouseSelection() const { return mouseHasSelection_; }
		sf::Vector2i getMouseSelection() const { return mouseSelection_; }

		void ApplyDeadGroupsToBoard(golc::Goban& goban);

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		int size_;
		Stone playerColour_ = black;
		// size in pixel of a grid square
		int pixelSize_;
		
		bool mouseHasSelection_ = false;
		sf::Vector2i mouseSelection_ = { 0,0 };

		int wCaps_ = 0;
		int bCaps_ = 0;
		golc::Board board_ = golc::Board();
		std::vector<std::vector<bool>> deadBoard_;
		golc::Coord lastMove_ = {0,0};

		sf::Font font_;
		sf::Texture bStonesTxt_;
		sf::Texture wStonesTxt_;
		sf::Texture tempStonesTxt_;
		sf::Texture goTileTxt_;
		sf::Texture goBorderTopTxt_;
		sf::Texture goBorderRightTxt_;
		sf::Texture goBorderBottomTxt_;
		sf::Texture goBorderLeftTxt_;
		sf::Texture goCornerTLTxt_;
		sf::Texture goCornerTRTxt_;
		sf::Texture goCornerBLTxt_;
		sf::Texture goCornerBRTxt_;
		sf::Texture whiteCircleTxt_;
		sf::Texture blackCircleTxt_;
		sf::Texture hoshiTxt_;
	};
}