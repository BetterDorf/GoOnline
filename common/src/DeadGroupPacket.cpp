#include "DeadGroupPacket.h"

DeadGroupPacket::DeadGroupPacket()
{
	type = deadGroup;
}

sf::Packet& operator <<(sf::Packet& packet, const DeadGroupPacket& deadPacket)
{
	packet << deadPacket.groupId;
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, DeadGroupPacket& deadPacket)
{
	packet >> deadPacket.groupId;
	return packet;
}
