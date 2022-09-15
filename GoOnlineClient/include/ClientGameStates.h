#pragma once

enum ClientState
{
	login,
	connecting,
	waitingForPlayer,
	playing,
	done
};