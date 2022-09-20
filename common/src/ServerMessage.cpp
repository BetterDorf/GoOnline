#include "ServerMessage.h"

ServerMessage::ServerMessage()
{
	type = serverMessage;
}

ServerMessage::ServerMessage(const MessageType messageType) : ServerMessage()
{
	msgType = messageType;
}

ServerMessage::ServerMessage(const MessageType messageType, const int info) : ServerMessage(messageType)
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
	message.msgType = static_cast<MessageType>(a);
	return packet;
}