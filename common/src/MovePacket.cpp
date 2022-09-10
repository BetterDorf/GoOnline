#include "MovePacket.h"

MovePacket::MovePacket()
{
	type = move;
}

sf::Packet& operator <<(sf::Packet& packet, const MovePacket& move)
{
	return packet << move.type << move.moveType << move.x << move.y;
}

sf::Packet& operator >>(sf::Packet& packet, MovePacket& move)
{
	int a;
	packet >> a >> move.x >> move.y;
	move.moveType = (MoveType)a;
	return packet;
}