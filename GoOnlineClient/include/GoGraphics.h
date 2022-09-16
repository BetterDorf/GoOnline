#pragma once
#include <sfml/graphics.hpp>

#include "Goban.h"

namespace goc
{
	class GoGraphics : public sf::Drawable, public sf::Transformable
	{
	public:
		GoGraphics(int size);

		void UpdateMove(golc::Goban& goban);
		void UpdateMouse(sf::Vector2i mousePos);
		bool HasMouseSelection() { return mouseHasSelection_; }
		sf::Vector2i getMouseSelection() { return mouseSelection_; }

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		int size_;
		// size in pixel of a gird square
		int pixelSize_;
		
		bool mouseHasSelection_ = false;
		sf::Vector2i mouseSelection_ = { 0,0 };

		int wCaps_ = 0;
		int bCaps_ = 0;
		golc::Board board_ = golc::Board();

		sf::Texture bStonesTxt_;
		sf::Texture wStonesTxt_;
		sf::Texture tempStonesTxt_;
		sf::Texture goTileTxt_;
	};
}