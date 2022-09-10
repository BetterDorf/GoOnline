#pragma once

#include "StandardPacket.h"

enum MoveType
{
	stonePlacement,
	pass,
	abandon
};

struct MovePacket : StandardPacket
{
	MovePacket();

	MoveType moveType = stonePlacement;
	int x = 1;
	int y = 1;
};

sf::Packet& operator <<(sf::Packet& packet, const MovePacket&);

sf::Packet& operator >>(sf::Packet& packet, MovePacket&);