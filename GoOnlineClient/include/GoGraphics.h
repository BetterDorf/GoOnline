#pragma once
#include <sfml/graphics.hpp>

#include "Goban.h"

namespace goc
{
	class GoGraphics : public sf::Drawable, public sf::Transformable
	{
	public:
		GoGraphics(int size, int pixelSize);

		void UpdateMove(golc::Goban& goban);
		void UpdateMouse();

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		int size_;
		// size in pixel of a gird square
		int pixelSize_;
	};
}