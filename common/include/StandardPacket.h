#pragma once
#include <SFML/Network/Packet.hpp>

enum PacketType : int
{
	serverMessage = 0,
	move = 1,
	deadGroup = 2
};

struct StandardPacket
{
	PacketType type;
};

/// <summary>
/// Read from a packet and return the type of packet contained. Internally calls the '>>' operator on the packet.
/// </summary>
/// <param name="packet">The sfPacket to read from</param>
/// <returns>The type of packet contained within the sf::packet</returns>
PacketType GetPacketType(sf::Packet& packet);

sf::Packet& operator <<(sf::Packet& packet, const StandardPacket& standardPacket);

sf::Packet& operator >>(sf::Packet& packet, StandardPacket& standardPacket);