#pragma once
#include <SFML/Network/Packet.hpp>

#include "StandardPacket.h"

enum MessageType
{
	waiting,
	gameStarted,
	invalidMove,
	win,
	lose
};

struct ServerMessage : StandardPacket
{
	ServerMessage();
	ServerMessage(MessageType);
	ServerMessage(MessageType, int);

	MessageType msgType = waiting;
	int additionalInfo = 0;
};

sf::Packet& operator <<(sf::Packet& packet, const ServerMessage&);

sf::Packet& operator >>(sf::Packet& packet, ServerMessage&);