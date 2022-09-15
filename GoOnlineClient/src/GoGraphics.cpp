#include "GoGraphics.h"

namespace goc
{
	GoGraphics::GoGraphics(int size, int pixelSize)
	{
		size_ = size;
		pixelSize_ = pixelSize;
	}

	void GoGraphics::UpdateMove(golc::Goban& goban)
	{
		// TODO read and update stones positions and current capture counts
	}

	void GoGraphics::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// TODO draw lines
		for (int x = 0; x < size_; x++)
		{
			for (int y = 0; y < size_; y++)
			{

			}
		}

		// TODO draw stones

		// TODO draw capture count

		// TODO draw mouse ui preview
	}
}