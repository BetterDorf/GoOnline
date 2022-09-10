#pragma once
#include <SFML/Network/TcpSocket.hpp>
#include "Stone.h"

struct Player
{
	sf::TcpSocket clientConnection;
	Stone colour = black;
};