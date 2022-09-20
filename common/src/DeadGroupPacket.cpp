#include "DeadGroupPacket.h"

DeadGroupPacket::DeadGroupPacket()
{
	type = deadGroup;
}

sf::Packet& operator <<(sf::Packet& packet, const DeadGroupPacket& deadPacket)
{
	packet << deadPacket.step << deadPacket.groupId;
	return packet;
}

sf::Packet& operator >>(sf::Packet& packet, DeadGroupPacket& deadPacket)
{
	int a;
	packet >> a >> deadPacket.groupId;
	deadPacket.step = static_cast<ScoringSteps>(a);
	return packet;
}
