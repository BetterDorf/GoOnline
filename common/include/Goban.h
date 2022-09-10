#pragma once
#include <unordered_set>
#include <vector>

#include "Stone.h"

class Goban
{
public:
	Goban(int x, int y);

	bool PlayStone(int x, int y, Stone team);

	int CountLiberties(int x, int y);
	int CountLiberties(const std::vector<std::pair<int, int>>&);

	std::vector<std::pair<int, int>> GetGroup(int x, int y);
	std::vector<std::pair<int, int>> GetGroup(std::pair<int, int>);
	std::vector<std::pair<int, int>> GetNeighbours(int x, int y) const;
	std::vector<std::pair<int, int>> GetNeighbours(std::pair<int, int>) const;

	void KillGroup(const std::vector<std::pair<int, int>>&);

	Stone StoneAt(int x, int y);
	Stone StoneAt(std::pair<int, int>);

	std::string ToString();
private:
	int x_;
	int y_;
	int bCaptures_ = 0;
	int wCaptures_ = 0;
	std::vector<std::vector<Stone>> stones_;
};