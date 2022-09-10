#include "ServerMessage.h"

ServerMessage::ServerMessage()
{
	type = serverMessage;
}

ServerMessage::ServerMessage(MessageType messageType) : ServerMessage()
{
	msgType = messageType;
}

ServerMessage::ServerMessage(MessageType messageType, int info) : ServerMessage(messageType)
{
	additionalInfo = info;
}

sf::Packet& operator <<(sf::Packet& packet, const ServerMessage& message)
{
	return packet << message.type << message.msgType << message.additionalInfo;
}

sf::Packet& operator >>(sf::Packet& packet, ServerMessage& message)
{
	int a;
	packet >> a >> message.additionalInfo;
	message.msgType = (MessageType)a;
	return packet;
}