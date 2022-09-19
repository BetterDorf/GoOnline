#pragma once

enum ClientState
{
	login,
	connecting,
	waitingForPlayer,
	playing,
	scoringPhase,
	done
};