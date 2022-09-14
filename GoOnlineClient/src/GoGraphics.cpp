#include "GoGraphics.h"

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

	// TODO draw stones

	// TODO draw capture count

	// TODO draw mouse ui preview
}