#include "StandardPacket.h"

PacketType GetPacketType(sf::Packet& packet)
{
	StandardPacket stdPacket;
	packet >> stdPacket;
	return stdPacket.type;
}

sf::Packet& operator <<(sf::Packet& packet, const StandardPacket& standardPacket)
{
	return packet << standardPacket.type;
}

sf::Packet& operator >>(sf::Packet& packet, StandardPacket& standardPacket)
{
	int a;
	packet >> a;

	standardPacket.type = static_cast<PacketType>(a);
	return packet;
} 