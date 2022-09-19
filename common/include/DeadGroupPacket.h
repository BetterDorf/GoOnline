#pragma once
#include "StandardPacket.h"

struct DeadGroupPacket : StandardPacket
{
	DeadGroupPacket();

	int groupId;
};

sf::Packet& operator <<(sf::Packet& packet, const DeadGroupPacket&);

sf::Packet& operator >>(sf::Packet& packet, DeadGroupPacket&);