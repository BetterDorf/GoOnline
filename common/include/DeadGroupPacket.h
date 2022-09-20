#pragma once
#include "StandardPacket.h"

enum ScoringSteps
{
	continueScoring,
	resumePlay,
	acceptScoring
};

struct DeadGroupPacket : StandardPacket
{
	DeadGroupPacket();

	ScoringSteps step = continueScoring;
	int groupId = 0;
};

sf::Packet& operator <<(sf::Packet& packet, const DeadGroupPacket&);

sf::Packet& operator >>(sf::Packet& packet, DeadGroupPacket&);